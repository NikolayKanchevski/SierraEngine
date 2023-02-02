//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include "../Structures/Vertex.h"
#include "../Structures/Material.h"
#include "../../Core/Rendering/Vulkan/Abstractions/Buffer.h"

using namespace Sierra::Core::Rendering::Vulkan::Abstractions;

namespace Sierra::Engine::Classes
{
    struct alignas(16) MeshPushConstant
    {
        Material material;

        uint meshID;
        uint meshTexturesPresence; // Bools encoded as binary indicating whether texture types are bound
    };

    class Mesh
    {
    public:
        /* --- CONSTRUCTORS --- */
        Mesh(std::vector<VertexP> &givenVertices, std::vector<uint> &givenIndices);
        Mesh(std::vector<VertexPNT> &givenVertices, std::vector<uint> &givenIndices);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint GetVertexCount() const { return vertexCount; }
        [[nodiscard]] inline uint GetIndexCount() const { return indexCount; }

        [[nodiscard]] inline SharedPtr<Buffer> GetVertexBuffer() { return vertexBuffer; }
        [[nodiscard]] inline SharedPtr<Buffer> GetIndexBuffer() { return indexBuffer; }

        [[nodiscard]] static inline uint GetTotalMeshCount()  { return totalMeshCount; }
        [[nodiscard]] static inline uint GetTotalVertexCount() { return totalVertexCount; }

        /* --- SETTER METHODS --- */
        static inline void IncreaseTotalMeshCount(const uint count = 1) { totalMeshCount += count; }
        static inline void IncreaseTotalVertexCount(const uint count) { totalVertexCount += count; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        Mesh(const Mesh &) = delete;
        Mesh &operator=(const Mesh &) = delete;

    private:
        uint vertexCount;
        uint indexCount;

        SharedPtr<Buffer> vertexBuffer;
        SharedPtr<Buffer> indexBuffer;

        void CreateVertexBuffer(std::vector<VertexP> &givenVertices);
        void CreateVertexBuffer(std::vector<VertexPNT> &givenVertices);
        void CreateIndexBuffer(std::vector<uint> &givenIndices);

        static inline uint totalMeshCount;
        static inline uint totalVertexCount;
    };

}