//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include "Vertex.h"

namespace Sierra::Engine
{

    struct MeshCreateInfo
    {
        const std::vector<Vertex> &vertices;
        const std::vector<uint32> &indices;
    };

    class Mesh
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Mesh(const MeshCreateInfo &createInfo);
        static UniquePtr<Mesh> Create(const MeshCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetVertexCount() const { return vertexCount; }
        [[nodiscard]] inline uint32 GetIndexCount() const { return indexCount; }
        [[nodiscard]] inline uint64 GetVertexDataOffset() const { return vertexDataOffset; }
        [[nodiscard]] inline uint64 GetIndexDataOffset() const { return indexDataOffset; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Mesh);

    private:
        uint64 vertexDataOffset = 0;
        uint64 indexDataOffset = 0;

        uint32 vertexCount = 0;
        uint32 indexCount = 0;
    };

}