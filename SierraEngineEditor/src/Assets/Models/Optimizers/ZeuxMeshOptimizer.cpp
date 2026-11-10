//
// Created by Nikolay Kanchevski on 22.09.26.
//

#include "ZeuxMeshOptimizer.h"

#include <meshoptimizer.h>

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    std::optional<Mesh> ZeuxMeshOptimizer::Optimize(const MeshOptimizeInfo& optimizeInfo) const noexcept
    {
        const std::span<const Vertex> vertices = optimizeInfo.vertices;
        const std::span<const uint32> indices = optimizeInfo.indices;

        if (vertices.empty() || indices.empty())
        {
            APP_WARNING("Could not optimize mesh, as it has no geometry");
            return std::nullopt;
        }

        if (indices.size() % 3 != 0)
        {
            APP_WARNING("Could not optimize mesh, as its index count [{0}] does not form a triangle list", indices.size());
            return std::nullopt;
        }

        if (std::ranges::any_of(indices, [&vertices](const uint32 index) -> bool { return index >= vertices.size(); }))
        {
            APP_WARNING("Could not optimize mesh, as it references vertices outside of its vertex buffer of size [{0}]", vertices.size());
            return std::nullopt;
        }

        // Weld duplicate vertices and drop unreferenced ones
        std::vector<uint32> remap(vertices.size());
        const size vertexCount = meshopt_generateVertexRemap(remap.data(), indices.data(), indices.size(), vertices.data(), vertices.size(), sizeof(Vertex));

        Mesh optimizedMesh
        {
            .vertices = std::vector<Vertex>(vertexCount),
            .indices = std::vector<uint32>(indices.size())
        };

        meshopt_remapIndexBuffer(optimizedMesh.indices.data(), indices.data(), indices.size(), remap.data());
        meshopt_remapVertexBuffer(optimizedMesh.vertices.data(), vertices.data(), vertices.size(), sizeof(Vertex), remap.data());

        // Reorder triangles to maximize vertex cache hits
        meshopt_optimizeVertexCache(optimizedMesh.indices.data(), optimizedMesh.indices.data(), optimizedMesh.indices.size(), vertexCount);

        // Reduce overdraw
        constexpr float32 OVERDRAW_THRESHOLD = 1.05f;
        meshopt_optimizeOverdraw(optimizedMesh.indices.data(), optimizedMesh.indices.data(), optimizedMesh.indices.size(), reinterpret_cast<const float*>(&optimizedMesh.vertices.data()->position), vertexCount, sizeof(Vertex), OVERDRAW_THRESHOLD);

        // Reorder vertices to match the order they are fetched in
        meshopt_optimizeVertexFetch(optimizedMesh.vertices.data(), optimizedMesh.indices.data(), optimizedMesh.indices.size(), optimizedMesh.vertices.data(), vertexCount, sizeof(Vertex));

        return optimizedMesh;
    }

}
