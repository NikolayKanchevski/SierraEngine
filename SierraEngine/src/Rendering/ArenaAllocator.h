//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

#include "Mesh.h"
#include "Vertex.h"

namespace SierraEngine
{

    struct ArenaAllocatorCreateInfo
    {
        const Sierra::RenderingContext &renderingContext;
        uint64 initialVertexBufferCapacity = 4096;
        uint64 initialIndexBufferCapacity = 8192;
    };

    class SIERRA_ENGINE_API ArenaAllocator final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ArenaAllocator(const ArenaAllocatorCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] Mesh RegisterMesh(Sierra::CommandBuffer &commandBuffer, std::span<Vertex> vertices, std::span<uint32> indices);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetVertexCount() const { return static_cast<uint32>(currentVertexByteOffset / sizeof(Vertex)); }
        [[nodiscard]] const Sierra::Buffer& GetVertexBuffer() const { return *vertexBuffer; }

        [[nodiscard]] uint32 GetIndexCount() const { return static_cast<uint32>(currentIndexByteOffset / sizeof(uint32)); }
        [[nodiscard]] const Sierra::Buffer& GetIndexBuffer() const { return *indexBuffer; }

        /* --- OPERATORS --- */
        ArenaAllocator(const ArenaAllocator&) = delete;
        ArenaAllocator& operator=(const ArenaAllocator&) = delete;

        /* --- DESTRUCTOR --- */
        ~ArenaAllocator() = default;

    private:
        const Sierra::RenderingContext &renderingContext;

        uint64 currentVertexByteOffset = 0;
        std::unique_ptr<Sierra::Buffer> vertexBuffer = nullptr;

        uint64 currentIndexByteOffset = 0;
        std::unique_ptr<Sierra::Buffer> indexBuffer = nullptr;

    };

}
