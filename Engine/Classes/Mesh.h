//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include "../../Core/Rendering/RenderingTemplates.h"
#include "../../Core/Rendering/Vulkan/Abstractions/Buffer.h"
#include "../../Core/Rendering/Vulkan/Abstractions/CommandBuffer.h"

using namespace Sierra::Core::Rendering;
using namespace Sierra::Core::Rendering::Vulkan::Abstractions;

namespace Sierra::Engine::Classes
{
    struct Material
    {
        Vector3 diffuse = Vector3(1.0f, 1.0f, 1.0f);
        float specular = 1.0f;

        float shininess = 0.001953125f;
        float ambient = 0.1f;

        float vertexExaggeration = 0.0f;
        float _align1_;
    };

    struct MeshPushConstant
    {
        Material material;

        uint entityID;
        uint meshID;
        uint meshTexturesPresence; // Bools encoded as binary indicating whether texture types are bound
        float _align1_;
    };

    class Mesh
    {
    public:
        /* --- CONSTRUCTORS --- */
        Mesh(std::vector<VertexP> &givenVertices, std::vector<uint> &givenIndices);
        Mesh(std::vector<VertexPNU> &givenVertices, std::vector<uint> &givenIndices);

        /* --- POLLING METHODS --- */
        void Bind(UniquePtr<CommandBuffer> &commandBuffer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint GetVertexCount() const { return vertexCount; }
        [[nodiscard]] inline uint GetIndexCount() const { return indexCount; }

        [[nodiscard]] inline UniquePtr<Buffer>& GetVertexBuffer() { return vertexBuffer; }
        [[nodiscard]] inline UniquePtr<Buffer>& GetIndexBuffer() { return indexBuffer; }

        [[nodiscard]] static inline uint GetTotalMeshCount()  { return totalMeshCount; }
        [[nodiscard]] static inline uint GetTotalVertexCount() { return totalVertexCount; }

        /* --- SETTER METHODS --- */
        static inline void IncreaseTotalMeshCount(const uint count = 1) { totalMeshCount += count; }
        static inline void IncreaseTotalVertexCount(const uint count) { totalVertexCount += count; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Mesh);

    private:
        uint vertexCount;
        uint indexCount;

        UniquePtr<Buffer> vertexBuffer;
        UniquePtr<Buffer> indexBuffer;

        void CreateVertexBuffer(std::vector<VertexP> &givenVertices);
        void CreateVertexBuffer(std::vector<VertexPNU> &givenVertices);
        void CreateIndexBuffer(std::vector<uint> &givenIndices);

        static inline uint totalMeshCount;
        static inline uint totalVertexCount;
    };

}