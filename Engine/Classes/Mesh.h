//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include "Vertex.h"
#include "../../Core/Rendering/RenderingSettings.h"

namespace Sierra::Engine
{

    struct MeshCreateInfo
    {
        std::vector<Vertex> &vertices;
        std::vector<VERTEX_INDEX_TYPE> &indices;
    };

    class Mesh
    {
    public:
        /* --- CONSTRUCTORS --- */
        Mesh(const MeshCreateInfo &createInfo);
        static SharedPtr<Mesh> Create(const MeshCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint GetVertexCount() const { return vertexCount; }
        [[nodiscard]] inline uint GetIndexCount() const { return indexCount; }
        [[nodiscard]] inline uint64 GetVertexDataOffset() const { return vertexDataOffset; }
        [[nodiscard]] inline uint64 GetIndexDataOffset() const { return indexDataOffset; }

        [[nodiscard]] static inline uint GetTotalMeshCount()  { return totalMeshCount; }
        [[nodiscard]] static inline uint GetTotalVertexCount() { return totalVertexCount; }

        /* --- SETTER METHODS --- */
        static inline void IncreaseTotalMeshCount(const uint count = 1) { totalMeshCount += count; }
        static inline void IncreaseTotalVertexCount(const uint count) { totalVertexCount += count; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Mesh);

    private:
        uint64 vertexDataOffset = 0;
        uint64 indexDataOffset = 0;

        uint vertexCount = 0;
        uint indexCount = 0;

        static inline uint totalVertexCount = 0;
        static inline uint totalMeshCount = 0;
    };

}