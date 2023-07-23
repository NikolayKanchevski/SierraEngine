//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "Mesh.h"

#include "../../Core/Rendering/Vulkan/VK.h"

namespace Sierra::Engine
{

    /* --- CONSTRUCTORS --- */

    Mesh::Mesh(const MeshCreateInfo &createInfo)
        : vertexCount(createInfo.vertices.size()), indexCount(createInfo.indices.size())
    {
        // Register mesh and load returned offsets
        Rendering::VK::GetArenaAllocator()->RegisterMesh(createInfo.vertices, createInfo.indices, vertexDataOffset, indexDataOffset);

        // Increment vertex & mesh count
        totalVertexCount += vertexCount;
        totalMeshCount++;
    }

    SharedPtr<Mesh> Mesh::Create(const MeshCreateInfo &createInfo)
    {
        return std::make_shared<Mesh>(createInfo);
    }

    /* --- DESTRUCTOR --- */

    void Mesh::Destroy()
    {
        totalVertexCount -= vertexCount;
        totalMeshCount--;
    }
}