//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <cstdint>
#include <memory>
#include <vector>

#include "../Structures/Vertex.h"
#include "../Structures/Material.h"
#include "../../Core/Rendering/Vulkan/Abstractions/Buffer.h"

using namespace Sierra::Core::Rendering::Vulkan::Abstractions;

namespace Sierra::Engine::Classes
{
    struct alignas(16) PushConstant
    {
        Material material;

        uint32_t meshID;
        uint32_t meshTexturesPresence; // Bools encoded as binary indicating whether texture types are bound
    };

    class Mesh
    {
    public:
        /* --- CONSTRUCTORS --- */
        Mesh(std::vector<Vertex> &givenVertices, std::vector<uint32_t> &givenIndices);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32_t GetVertexCount() const { return vertexCount; }
        [[nodiscard]] inline uint32_t GetIndexCount() const { return indexCount; }
        [[nodiscard]] inline std::shared_ptr<Buffer> GetVertexBuffer() { return vertexBuffer; }
        [[nodiscard]] inline std::shared_ptr<Buffer> GetIndexBuffer() { return indexBuffer; }

        [[nodiscard]] static inline uint32_t GetTotalMeshCount()  { return totalMeshCount; }
        [[nodiscard]] static inline uint32_t GetTotalVertexCount() { return totalVertexCount; }

        /* --- SETTER METHODS --- */
        static inline void IncreaseTotalMeshCount(const uint32_t count = 1) { totalMeshCount += count; }
        static inline void IncreaseTotalVertexCount(const uint32_t count) { totalVertexCount += count; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        Mesh(const Mesh &) = delete;
        Mesh &operator=(const Mesh &) = delete;

    private:
        uint32_t vertexCount;
        uint32_t indexCount;

        std::shared_ptr<Buffer> vertexBuffer;
        std::shared_ptr<Buffer> indexBuffer;

        void CreateVertexBuffer(std::vector<Vertex> &givenVertices);
        void CreateIndexBuffer(std::vector<uint32_t> &givenIndices);

        static inline uint32_t totalMeshCount;
        static inline uint32_t totalVertexCount;
    };

}