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

    ArenaMesh ArenaAllocator::RegisterMesh(Sierra::CommandBuffer& commandBuffer, const std::span<const Vertex> vertices, const std::span<const uint32> indices)
    {
        const ArenaMesh mesh
        {
            .vertexOffset = currentVertexOffset,
            .vertexCount = static_cast<uint32>(vertices.size()),
            .indexOffset = currentIndexOffset,
            .indexCount = static_cast<uint32>(indices.size())
        };

        // Check if vertex buffer does not have enough space to store new vertices
        if (const size endVertexOffset = currentVertexOffset + vertices.size_bytes(); endVertexOffset > vertexBuffer->GetMemorySize())
        {
            // Create temporary buffer to house both old and new vertex data
            std::unique_ptr<Sierra::Buffer> stagingBuffer = device.CreateBuffer({
                .name = "Staging Buffer of Arena Allocator Vertex Data",
                .memorySize = endVertexOffset,
                .usage = Sierra::BufferUsage::Vertex | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::CPU
            });
            stagingBuffer->Write(vertices.data(), 0, currentVertexOffset, vertices.size_bytes());

            // Copy old vertex data to temporary buffer
            commandBuffer.SynchronizeBufferUsage(*vertexBuffer, Sierra::BufferCommandUsage::VertexRead, Sierra::BufferCommandUsage::MemoryRead, 0, currentVertexOffset);
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, Sierra::BufferCommandUsage::None, Sierra::BufferCommandUsage::MemoryWrite, 0, currentVertexOffset);
            commandBuffer.CopyBufferToBuffer(*vertexBuffer, *stagingBuffer, 0, 0, currentVertexOffset);

            // Recreate vertex buffer with more space
            commandBuffer.QueueBufferForDestruction(std::move(vertexBuffer));
            vertexBuffer = device.CreateBuffer({
                .name = "Arena Allocator Vertex Buffer",
                .memorySize = glm::max(endVertexOffset, static_cast<size>(static_cast<float64>(vertexBuffer->GetMemorySize()) * VERTEX_BUFFER_GROWTH_FACTOR)),
                .usage = Sierra::BufferUsage::Vertex | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::GPU
            });

            // Copy vertex data over to GPU buffer
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, Sierra::BufferCommandUsage::MemoryWrite, Sierra::BufferCommandUsage::MemoryRead, 0, stagingBuffer->GetMemorySize());
            commandBuffer.SynchronizeBufferUsage(*vertexBuffer, Sierra::BufferCommandUsage::None, Sierra::BufferCommandUsage::MemoryWrite, 0, stagingBuffer->GetMemorySize());
            commandBuffer.CopyBufferToBuffer(*stagingBuffer, *vertexBuffer, 0, 0, stagingBuffer->GetMemorySize());

            // Discard temporary buffer
            commandBuffer.QueueBufferForDestruction(std::move(stagingBuffer));
            commandBuffer.SynchronizeBufferUsage(*vertexBuffer, Sierra::BufferCommandUsage::MemoryWrite, Sierra::BufferCommandUsage::VertexRead, 0, endVertexOffset);
        }
        else
        {
            // Create temporary buffer to house new vertex data
            std::unique_ptr<Sierra::Buffer> stagingBuffer = device.CreateBuffer({
                .name = "Staging Buffer of Arena Allocator Vertex Data",
                .memorySize = vertices.size_bytes(),
                .usage = Sierra::BufferUsage::SourceMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::CPU
            });
            stagingBuffer->Write(vertices.data(), 0, 0, stagingBuffer->GetMemorySize());

            // Copy vertex data over to GPU buffer
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, Sierra::BufferCommandUsage::None, Sierra::BufferCommandUsage::MemoryRead, 0, stagingBuffer->GetMemorySize());
            commandBuffer.SynchronizeBufferUsage(*vertexBuffer, Sierra::BufferCommandUsage::None, Sierra::BufferCommandUsage::MemoryWrite, currentVertexOffset, stagingBuffer->GetMemorySize());
            commandBuffer.CopyBufferToBuffer(*stagingBuffer, *vertexBuffer, 0, currentVertexOffset, stagingBuffer->GetMemorySize());

            // Discard temporary buffer
            commandBuffer.QueueBufferForDestruction(std::move(stagingBuffer));
            commandBuffer.SynchronizeBufferUsage(*vertexBuffer, Sierra::BufferCommandUsage::MemoryWrite, Sierra::BufferCommandUsage::VertexRead, currentVertexOffset, stagingBuffer->GetMemorySize());
        }
        currentVertexOffset += vertices.size_bytes();

        // Check if index buffer does not have enough space to store new vertices
        if (const size endIndexOffset = currentIndexOffset + indices.size_bytes(); endIndexOffset > indexBuffer->GetMemorySize())
        {
            // Create temporary buffer to house both old and new index data
            std::unique_ptr<Sierra::Buffer> stagingBuffer = device.CreateBuffer({
                .name = "Staging Buffer of Arena Allocator Index Data",
                .memorySize = endIndexOffset,
                .usage = Sierra::BufferUsage::Index | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::CPU
            });
            stagingBuffer->Write(indices.data(), 0, currentIndexOffset, indices.size_bytes());

            // Copy old index data to temporary buffer
            commandBuffer.SynchronizeBufferUsage(*indexBuffer, Sierra::BufferCommandUsage::IndexRead, Sierra::BufferCommandUsage::MemoryRead, 0, currentIndexOffset);
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, Sierra::BufferCommandUsage::None, Sierra::BufferCommandUsage::MemoryWrite, 0, currentIndexOffset);
            commandBuffer.CopyBufferToBuffer(*indexBuffer, *stagingBuffer, 0, 0, currentIndexOffset);

            // Recreate index buffer with more space
            commandBuffer.QueueBufferForDestruction(std::move(indexBuffer));
            indexBuffer = device.CreateBuffer({
                .name = "Arena Allocator Index Buffer",
                .memorySize = glm::max(endIndexOffset, static_cast<size>(static_cast<float64>(indexBuffer->GetMemorySize()) * INDEX_BUFFER_GROWTH_FACTOR)),
                .usage = Sierra::BufferUsage::Index | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::GPU
            });

            // Copy index data over to GPU buffer
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, Sierra::BufferCommandUsage::MemoryWrite, Sierra::BufferCommandUsage::MemoryRead, 0, stagingBuffer->GetMemorySize());
            commandBuffer.SynchronizeBufferUsage(*indexBuffer, Sierra::BufferCommandUsage::None, Sierra::BufferCommandUsage::MemoryWrite, 0, stagingBuffer->GetMemorySize());
            commandBuffer.CopyBufferToBuffer(*stagingBuffer, *indexBuffer, 0, 0, stagingBuffer->GetMemorySize());

            // Discard temporary buffer
            commandBuffer.QueueBufferForDestruction(std::move(stagingBuffer));
            commandBuffer.SynchronizeBufferUsage(*indexBuffer, Sierra::BufferCommandUsage::MemoryWrite, Sierra::BufferCommandUsage::IndexRead, 0, endIndexOffset);
        }
        else
        {
            // Create temporary buffer to house new index data
            std::unique_ptr<Sierra::Buffer> stagingBuffer = device.CreateBuffer({
                .name = "Staging Buffer of Arena Allocator Index Data",
                .memorySize = indices.size_bytes(),
                .usage = Sierra::BufferUsage::SourceMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::CPU
            });
            stagingBuffer->Write(indices.data(), 0, 0, indices.size_bytes());

            // Copy index data over to GPU buffer
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, Sierra::BufferCommandUsage::None, Sierra::BufferCommandUsage::MemoryRead, 0, stagingBuffer->GetMemorySize());
            commandBuffer.SynchronizeBufferUsage(*indexBuffer, Sierra::BufferCommandUsage::None, Sierra::BufferCommandUsage::MemoryWrite, currentIndexOffset, stagingBuffer->GetMemorySize());
            commandBuffer.CopyBufferToBuffer(*stagingBuffer, *indexBuffer, 0, currentIndexOffset, stagingBuffer->GetMemorySize());

            // Discard temporary buffer
            commandBuffer.QueueBufferForDestruction(std::move(stagingBuffer));
            commandBuffer.SynchronizeBufferUsage(*indexBuffer, Sierra::BufferCommandUsage::MemoryWrite, Sierra::BufferCommandUsage::IndexRead, currentIndexOffset, stagingBuffer->GetMemorySize());
        }
        currentIndexOffset += indices.size_bytes();

        return mesh;
    }

}