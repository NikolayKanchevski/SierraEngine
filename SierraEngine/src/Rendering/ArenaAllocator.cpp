//
// Created by Nikolay Kanchevski on 22.04.24.
//

#include "ArenaAllocator.h"

namespace SierraEngine
{

    constexpr float64 VERTEX_BUFFER_GROWTH_FACTOR = 1.8f;
    constexpr float64 INDEX_BUFFER_GROWTH_FACTOR = VERTEX_BUFFER_GROWTH_FACTOR * 3.6f;

    /* --- CONSTRUCTORS --- */

    ArenaAllocator::ArenaAllocator(const ArenaAllocatorCreateInfo& createInfo)
        : device(createInfo.device)
    {
        vertexBuffer = device.CreateBuffer({
            .name = "Arena Allocator Vertex Buffer",
            .memorySize = createInfo.initialVertexBufferCapacity * sizeof(Vertex),
            .usage = Sierra::BufferUsage::Vertex | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory,
            .memoryLocation = Sierra::BufferMemoryLocation::GPU
        });

        indexBuffer = device.CreateBuffer({
            .name = "Arena Allocator Index Buffer",
            .memorySize = createInfo.initialIndexBufferCapacity * sizeof(uint32),
            .usage = Sierra::BufferUsage::Index | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory,
            .memoryLocation = Sierra::BufferMemoryLocation::GPU
        });
    }

    /* --- POLLING METHODS --- */

    void ArenaAllocator::Bind(Sierra::CommandBuffer& commandBuffer)
    {
        commandBuffer.BindVertexBuffer(*vertexBuffer, 0);
        commandBuffer.BindIndexBuffer(*indexBuffer, 0);
    }

    Mesh ArenaAllocator::CreateMesh(Sierra::CommandBuffer& commandBuffer, const std::span<const Vertex> vertices, const std::span<const uint32> indices)
    {
        Mesh mesh
        {
            .vertexOffset = static_cast<uint32>(currentVertexOffset / sizeof(Vertex)),
            .vertexCount = static_cast<uint32>(vertices.size()),
            .indexOffset = static_cast<uint32>(currentIndexOffset / sizeof(uint32)),
            .indexCount = static_cast<uint32>(indices.size())
        };

        // Check if vertex buffer does not have enough space to store new vertices
        if (const size endVertexOffset = currentVertexOffset + vertices.size_bytes(); endVertexOffset >= vertexBuffer->GetMemorySize())
        {
            // Create temporary buffer to house both old and new vertex data
            std::unique_ptr<Sierra::Buffer> stagingBuffer = device.CreateBuffer({
                .name = "Staging Buffer of Arena Allocator Vertex Data",
                .memorySize = endVertexOffset,
                .usage = Sierra::BufferUsage::Vertex | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::RAM
            });
            stagingBuffer->Write(vertices.data(), 0, currentVertexOffset, vertices.size_bytes());

            // Copy old vertex data to temporary buffer
            commandBuffer.SynchronizeBufferUsage(*vertexBuffer, { .previousUsage = Sierra::BufferCommandUsage::VertexRead, .nextUsage = Sierra::BufferCommandUsage::MemoryRead, .memorySize = currentVertexOffset });
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, { .nextUsage = Sierra::BufferCommandUsage::MemoryWrite, .memorySize = currentVertexOffset });
            commandBuffer.CopyBufferToBuffer(*vertexBuffer, *stagingBuffer, { .memorySize = currentVertexOffset });

            // Save old memory size and query old buffer for destruction
            const uint64 oldVertexBufferSize = vertexBuffer->GetMemorySize();
            commandBuffer.QueueBufferForDestruction(std::move(vertexBuffer));

            // Recreate vertex buffer with more space
            vertexBuffer = device.CreateBuffer({
                .name = "Arena Allocator Vertex Buffer",
                .memorySize = glm::max(endVertexOffset, static_cast<size>(static_cast<float64>(oldVertexBufferSize) * VERTEX_BUFFER_GROWTH_FACTOR)),
                .usage = Sierra::BufferUsage::Vertex | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory
            });

            // Copy vertex data over to GPU buffer
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, { .previousUsage = Sierra::BufferCommandUsage::MemoryWrite, .nextUsage = Sierra::BufferCommandUsage::MemoryRead, .memorySize = stagingBuffer->GetMemorySize() });
            commandBuffer.SynchronizeBufferUsage(*vertexBuffer, { .nextUsage = Sierra::BufferCommandUsage::MemoryWrite, .memorySize = stagingBuffer->GetMemorySize() });
            commandBuffer.CopyBufferToBuffer(*stagingBuffer, *vertexBuffer, { .memorySize = stagingBuffer->GetMemorySize() });

            // Discard temporary buffer
            commandBuffer.QueueBufferForDestruction(std::move(stagingBuffer));
            commandBuffer.SynchronizeBufferUsage(*vertexBuffer, { .previousUsage = Sierra::BufferCommandUsage::MemoryWrite, .nextUsage = Sierra::BufferCommandUsage::VertexRead, .memorySize = endVertexOffset });
        }
        else
        {
            // Create temporary buffer to house new vertex data
            std::unique_ptr<Sierra::Buffer> stagingBuffer = device.CreateBuffer({
                .name = "Staging Buffer of Arena Allocator Vertex Data",
                .memorySize = vertices.size_bytes(),
                .usage = Sierra::BufferUsage::SourceMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::RAM
            });
            stagingBuffer->Write(vertices.data(), 0, 0, stagingBuffer->GetMemorySize());

            // Copy vertex data over to GPU buffer
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, { .nextUsage = Sierra::BufferCommandUsage::MemoryRead, .memorySize = stagingBuffer->GetMemorySize() });
            commandBuffer.SynchronizeBufferUsage(*vertexBuffer, { .nextUsage = Sierra::BufferCommandUsage::MemoryWrite, .offset = currentVertexOffset, .memorySize = stagingBuffer->GetMemorySize() });
            commandBuffer.CopyBufferToBuffer(*stagingBuffer, *vertexBuffer, { .destinationOffset = currentVertexOffset, .memorySize =  stagingBuffer->GetMemorySize() });

            // Discard temporary buffer
            commandBuffer.SynchronizeBufferUsage(*vertexBuffer, { .previousUsage = Sierra::BufferCommandUsage::MemoryWrite, .nextUsage = Sierra::BufferCommandUsage::VertexRead, .offset = currentVertexOffset, .memorySize = stagingBuffer->GetMemorySize() });
            commandBuffer.QueueBufferForDestruction(std::move(stagingBuffer));
        }
        currentVertexOffset += vertices.size_bytes();

        // Check if index buffer does not have enough space to store new vertices
        if (const size endIndexOffset = currentIndexOffset + indices.size_bytes(); endIndexOffset >= indexBuffer->GetMemorySize())
        {
            // Create temporary buffer to house both old and new index data
            std::unique_ptr<Sierra::Buffer> stagingBuffer = device.CreateBuffer({
                .name = "Staging Buffer of Arena Allocator Index Data",
                .memorySize = endIndexOffset,
                .usage = Sierra::BufferUsage::Index | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::RAM
            });
            stagingBuffer->Write(indices.data(), 0, currentIndexOffset, indices.size_bytes());

            // Copy old index data to temporary buffer
            commandBuffer.SynchronizeBufferUsage(*indexBuffer, { .previousUsage = Sierra::BufferCommandUsage::IndexRead, .nextUsage = Sierra::BufferCommandUsage::MemoryRead, .memorySize = currentIndexOffset });
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, { .nextUsage = Sierra::BufferCommandUsage::MemoryWrite, .memorySize = currentIndexOffset });
            commandBuffer.CopyBufferToBuffer(*indexBuffer, *stagingBuffer, { .memorySize = currentIndexOffset });

            // Save old memory size and query old buffer for destruction
            const uint64 oldIndexBufferSize = indexBuffer->GetMemorySize();
            commandBuffer.QueueBufferForDestruction(std::move(indexBuffer));

            // Recreate index buffer with more space
            commandBuffer.QueueBufferForDestruction(std::move(indexBuffer));
            indexBuffer = device.CreateBuffer({
                .name = "Arena Allocator Index Buffer",
                .memorySize = glm::max(endIndexOffset, static_cast<size>(static_cast<float64>(oldIndexBufferSize) * INDEX_BUFFER_GROWTH_FACTOR)),
                .usage = Sierra::BufferUsage::Index | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory
            });

            // Copy index data over to GPU buffer
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, { .previousUsage = Sierra::BufferCommandUsage::MemoryWrite, .nextUsage = Sierra::BufferCommandUsage::MemoryRead, .memorySize = stagingBuffer->GetMemorySize() });
            commandBuffer.SynchronizeBufferUsage(*indexBuffer, { .nextUsage = Sierra::BufferCommandUsage::MemoryWrite, .memorySize = stagingBuffer->GetMemorySize() });
            commandBuffer.CopyBufferToBuffer(*stagingBuffer, *indexBuffer, { .memorySize = stagingBuffer->GetMemorySize() });

            // Discard temporary buffer
            commandBuffer.QueueBufferForDestruction(std::move(stagingBuffer));
            commandBuffer.SynchronizeBufferUsage(*indexBuffer, { .previousUsage = Sierra::BufferCommandUsage::MemoryWrite, .nextUsage = Sierra::BufferCommandUsage::IndexRead, .memorySize = endIndexOffset });
        }
        else
        {
            // Create temporary buffer to house new index data
            std::unique_ptr<Sierra::Buffer> stagingBuffer = device.CreateBuffer({
                .name = "Staging Buffer of Arena Allocator Index Data",
                .memorySize = indices.size_bytes(),
                .usage = Sierra::BufferUsage::SourceMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::RAM
            });
            stagingBuffer->Write(indices.data(), 0, 0, stagingBuffer->GetMemorySize());

            // Copy index data over to GPU buffer
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, { .nextUsage = Sierra::BufferCommandUsage::MemoryRead, .memorySize = stagingBuffer->GetMemorySize() });
            commandBuffer.SynchronizeBufferUsage(*indexBuffer, { .nextUsage = Sierra::BufferCommandUsage::MemoryWrite, .offset = currentIndexOffset, .memorySize = stagingBuffer->GetMemorySize() });
            commandBuffer.CopyBufferToBuffer(*stagingBuffer, *indexBuffer, { .destinationOffset = currentIndexOffset, .memorySize = stagingBuffer->GetMemorySize() });

            // Discard temporary buffer
            commandBuffer.SynchronizeBufferUsage(*indexBuffer, { .previousUsage = Sierra::BufferCommandUsage::MemoryWrite, .nextUsage = Sierra::BufferCommandUsage::IndexRead, .offset = currentIndexOffset, .memorySize = stagingBuffer->GetMemorySize() });
            commandBuffer.QueueBufferForDestruction(std::move(stagingBuffer));
        }
        currentIndexOffset += indices.size_bytes();

        return mesh;
    }

}