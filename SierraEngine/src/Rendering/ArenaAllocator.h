//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

#include "Vertex.h"

namespace SierraEngine
{

    struct ArenaAllocatorCreateInfo
    {
        const Sierra::Device& device;
        uint64 initialVertexBufferCapacity = 4096;
        uint64 initialIndexBufferCapacity = 8192;
    };

    struct ArenaMesh
    {
        uint64 vertexOffset = 0;
        uint32 vertexCount = 0;

        uint64 indexOffset = 0;
        uint32 indexCount = 0;
    };

    class SIERRA_ENGINE_API ArenaAllocator final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ArenaAllocator(const ArenaAllocatorCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] ArenaMesh RegisterMesh(Sierra::CommandBuffer& commandBuffer, std::span<const Vertex> vertices, std::span<const uint32> indices);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetVertexCount() const noexcept { return static_cast<uint32>(currentVertexOffset / sizeof(Vertex)); }
        [[nodiscard]] const Sierra::Buffer& GetVertexBuffer() const noexcept { return *vertexBuffer; }

        [[nodiscard]] uint32 GetIndexCount() const noexcept { return static_cast<uint32>(currentIndexOffset / sizeof(uint32)); }
        [[nodiscard]] const Sierra::Buffer& GetIndexBuffer() const noexcept { return *indexBuffer; }

        /* --- COPY SEMANTICS --- */
        ArenaAllocator(const ArenaAllocator&) = delete;
        ArenaAllocator& operator=(const ArenaAllocator&) = delete;

        /* --- MOVE SEMANTICS --- */
        ArenaAllocator(ArenaAllocator&&) = delete;
        ArenaAllocator& operator=(ArenaAllocator&&) = delete;

        /* --- DESTRUCTOR --- */
        ~ArenaAllocator() noexcept = default;

    private:
        const Sierra::Device& device;

        size currentVertexOffset = 0;
        std::unique_ptr<Sierra::Buffer> vertexBuffer = nullptr;

        size currentIndexOffset = 0;
        std::unique_ptr<Sierra::Buffer> indexBuffer = nullptr;

    };

}
