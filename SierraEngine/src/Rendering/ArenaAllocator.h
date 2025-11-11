//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

#include "Primitives/Mesh.h"
#include "RenderingContext.h"
#include "Primitives/Vertex.h"

namespace SierraEngine
{

    struct ArenaAllocatorCreateInfo
    {
        const RenderingContext& renderingContext;
        uint32 initialVertexBufferCapacity = 4096;
        uint32 initialIndexBufferCapacity = 8192;
    };

    class SIERRA_ENGINE_API ArenaAllocator final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ArenaAllocator(const ArenaAllocatorCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Bind(Sierra::CommandBuffer& commandBuffer) const;
        [[nodiscard]] Mesh CreateMesh(Sierra::CommandBuffer& commandBuffer, std::span<const Vertex> vertices, std::span<const uint32> indices);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetVertexCount() const noexcept { return static_cast<uint32>(currentVertexOffset / sizeof(Vertex)); }
        [[nodiscard]] const Sierra::Buffer& GetVertexBuffer() const noexcept { return *vertexBuffer; }

        [[nodiscard]] uint32 GetIndexCount() const noexcept { return static_cast<uint32>(currentIndexOffset / sizeof(uint32)); }
        [[nodiscard]] const Sierra::Buffer& GetIndexBuffer() const noexcept { return *indexBuffer; }

        /* --- COPY SEMANTICS --- */
        ArenaAllocator(const ArenaAllocator&) = delete;
        ArenaAllocator& operator=(const ArenaAllocator&) = delete;

        /* --- MOVE SEMANTICS --- */
        ArenaAllocator(ArenaAllocator&&) noexcept = default;
        ArenaAllocator& operator=(ArenaAllocator&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~ArenaAllocator() noexcept = default;

    private:
        const RenderingContext* renderingContext;

        uint64 currentVertexOffset = 0;
        std::unique_ptr<Sierra::Buffer> vertexBuffer = nullptr;

        uint64 currentIndexOffset = 0;
        std::unique_ptr<Sierra::Buffer> indexBuffer = nullptr;

    };

}
