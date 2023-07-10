//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include "../../Core/Rendering/RenderingSettings.h"
#include "../../Core/Rendering/Vulkan/Abstractions/Buffer.h"
#include "../../Core/Rendering/Vulkan/Abstractions/CommandBuffer.h"

namespace Sierra::Engine
{
    struct Vertex
    {
        Vector3 position;
        Vector3 normal;
        Vector2 UV;
    };

    struct Material
    {
        Vector3 diffuse = Vector3(1.0f, 1.0f, 1.0f);
        float specular = 1.0f;

        float shininess = 0.001953125f;
        float vertexExaggeration = 0.0f;

        Vector2 _align1_;
    };

    struct MeshPushConstant
    {
        Material material;

        uint meshID = 0;
        uint entityID = 0;
        uint meshTexturesPresence = 0; // Bools encoded as binary indicating whether texture types are bound
        uint directionalLightID = 0;
    };

    struct MeshCreateInfo
    {
        std::vector<Vertex> &vertices;
        std::vector<INDEX_BUFFER_TYPE> &indices;
    };

    class Mesh
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Mesh(const MeshCreateInfo &createInfo);
        static SharedPtr<Mesh> Create(MeshCreateInfo createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint GetVertexCount() const { return vertexCount; }
        [[nodiscard]] inline uint GetIndexCount() const { return indexCount; }

        [[nodiscard]] inline UniquePtr<Rendering::Buffer>& GetVertexBuffer() { return vertexBuffer; }
        [[nodiscard]] inline UniquePtr<Rendering::Buffer>& GetIndexBuffer() { return indexBuffer; }

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

        UniquePtr<Rendering::Buffer> vertexBuffer;
        UniquePtr<Rendering::Buffer> indexBuffer;

        void CreateVertexBuffer(std::vector<Vertex> &givenVertices);
        void CreateIndexBuffer(std::vector<uint> &givenIndices);

        static inline uint totalMeshCount;
        static inline uint totalVertexCount;
    };

}