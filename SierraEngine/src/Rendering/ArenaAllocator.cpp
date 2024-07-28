//
// Created by Nikolay Kanchevski on 22.04.24.
//

#include "ArenaAllocator.h"

namespace SierraEngine
{

    constexpr float64 VERTEX_BUFFER_GROWTH_FACTOR = 1.8f;
    constexpr float64 INDEX_BUFFER_GROWTH_FACTOR = VERTEX_BUFFER_GROWTH_FACTOR * 3.6f;

    /* --- CONSTRUCTORS --- */

    ArenaAllocator::ArenaAllocator(const ArenaAllocatorCreateInfo &createInfo)
        : renderingContext(createInfo.renderingContext)
    {
        vertexBuffer = renderingContext.CreateBuffer({
            .name = "Arena Allocator Vertex Buffer",
            .memorySize = createInfo.initialVertexBufferCapacity * sizeof(Vertex),
            .usage = Sierra::BufferUsage::Vertex | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory,
            .memoryLocation = Sierra::BufferMemoryLocation::GPU
        });

        indexBuffer = renderingContext.CreateBuffer({
            .name = "Arena Allocator Index Buffer",
            .memorySize = createInfo.initialIndexBufferCapacity * sizeof(uint32),
            .usage = Sierra::BufferUsage::Index | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory,
            .memoryLocation = Sierra::BufferMemoryLocation::GPU
        });
    }

    /* --- POLLING METHODS --- */

    ArenaMesh ArenaAllocator::RegisterMesh(Sierra::CommandBuffer &commandBuffer, const std::span<const Vertex> vertices, const std::span<const uint32> indices)
    {
        const ArenaMesh mesh
        {
            .vertexByteOffset = currentVertexByteOffset,
            .vertexCount = static_cast<uint32>(vertices.size()),
            .indexByteOffset = currentIndexByteOffset,
            .indexCount = static_cast<uint32>(indices.size())
        };

        // Check if vertex buffer has enough space to store new vertices
        if (const uint64 endVertexByteOffset = currentVertexByteOffset + vertices.size_bytes(); endVertexByteOffset > vertexBuffer->GetMemorySize())
        {
            // Create temporary buffer to house both old and new vertex data
            std::unique_ptr<Sierra::Buffer> stagingBuffer = renderingContext.CreateBuffer({
                .name = "Staging Buffer of Arena Allocator Vertex Data",
                .memorySize = endVertexByteOffset,
                .usage = Sierra::BufferUsage::Vertex | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::CPU
            });
            stagingBuffer->CopyFromMemory(vertices.data(), vertices.size_bytes(), 0, currentVertexByteOffset);

            // Copy old vertex data to temporary buffer
            commandBuffer.SynchronizeBufferUsage(*vertexBuffer, Sierra::BufferCommandUsage::VertexRead, Sierra::BufferCommandUsage::MemoryRead);
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, Sierra::BufferCommandUsage::None, Sierra::BufferCommandUsage::MemoryWrite);
            commandBuffer.CopyBufferToBuffer(*vertexBuffer, *stagingBuffer);

            // Recreate vertex buffer with more space
            commandBuffer.QueueBufferForDestruction(std::move(vertexBuffer));
            vertexBuffer = renderingContext.CreateBuffer({
                .name = "Arena Allocator Vertex Buffer",
                .memorySize = glm::max(endVertexByteOffset, static_cast<uint64>(static_cast<float64>(vertexBuffer->GetMemorySize()) * VERTEX_BUFFER_GROWTH_FACTOR)),
                .usage = Sierra::BufferUsage::Vertex | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::GPU
            });

            // Copy vertex data over to GPU buffer
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, Sierra::BufferCommandUsage::MemoryWrite, Sierra::BufferCommandUsage::MemoryRead);
            commandBuffer.SynchronizeBufferUsage(*vertexBuffer, Sierra::BufferCommandUsage::None, Sierra::BufferCommandUsage::MemoryWrite);
            commandBuffer.CopyBufferToBuffer(*stagingBuffer, *vertexBuffer);

            // Discard temporary buffer
            commandBuffer.QueueBufferForDestruction(std::move(stagingBuffer));
            commandBuffer.SynchronizeBufferUsage(*vertexBuffer, Sierra::BufferCommandUsage::MemoryWrite, Sierra::BufferCommandUsage::VertexRead);
        }
        else
        {
            // Create temporary buffer to house new vertex data
            std::unique_ptr<Sierra::Buffer> stagingBuffer = renderingContext.CreateBuffer({
                .name = "Staging Buffer of Arena Allocator Vertex Data",
                .memorySize = vertices.size_bytes(),
                .usage = Sierra::BufferUsage::SourceMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::CPU
            });
            stagingBuffer->CopyFromMemory(vertices.data());

            // Copy vertex data over to GPU buffer
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, Sierra::BufferCommandUsage::None, Sierra::BufferCommandUsage::MemoryRead);
            commandBuffer.SynchronizeBufferUsage(*vertexBuffer, Sierra::BufferCommandUsage::VertexRead, Sierra::BufferCommandUsage::MemoryWrite);
            commandBuffer.CopyBufferToBuffer(*stagingBuffer, *vertexBuffer, stagingBuffer->GetMemorySize(), 0, currentVertexByteOffset);

            // Discard temporary buffer
            commandBuffer.QueueBufferForDestruction(std::move(stagingBuffer));
            commandBuffer.SynchronizeBufferUsage(*vertexBuffer, Sierra::BufferCommandUsage::MemoryWrite, Sierra::BufferCommandUsage::VertexRead);
        }
        currentVertexByteOffset += vertices.size_bytes();

        // Check if index buffer has enough space to store new vertices
        if (const uint64 endIndexByteOffset = currentIndexByteOffset + indices.size_bytes(); endIndexByteOffset > indexBuffer->GetMemorySize())
        {
            // Create temporary buffer to house both old and new index data
            std::unique_ptr<Sierra::Buffer> stagingBuffer = renderingContext.CreateBuffer({
                .name = "Staging Buffer of Arena Allocator Index Data",
                .memorySize = endIndexByteOffset,
                .usage = Sierra::BufferUsage::Index | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::CPU
            });
            stagingBuffer->CopyFromMemory(indices.data(), indices.size_bytes(), 0, currentIndexByteOffset);

            // Copy old index data to temporary buffer
            commandBuffer.SynchronizeBufferUsage(*indexBuffer, Sierra::BufferCommandUsage::IndexRead, Sierra::BufferCommandUsage::MemoryRead);
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, Sierra::BufferCommandUsage::None, Sierra::BufferCommandUsage::MemoryWrite);
            commandBuffer.CopyBufferToBuffer(*indexBuffer, *stagingBuffer);

            // Recreate index buffer with more space
            commandBuffer.QueueBufferForDestruction(std::move(indexBuffer));
            indexBuffer = renderingContext.CreateBuffer({
                .name = "Arena Allocator Index Buffer",
                .memorySize = glm::max(endIndexByteOffset, static_cast<uint64>(static_cast<float64>(indexBuffer->GetMemorySize()) * INDEX_BUFFER_GROWTH_FACTOR)),
                .usage = Sierra::BufferUsage::Index | Sierra::BufferUsage::SourceMemory | Sierra::BufferUsage::DestinationMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::GPU
            });

            // Copy index data over to GPU buffer
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, Sierra::BufferCommandUsage::MemoryWrite, Sierra::BufferCommandUsage::MemoryRead);
            commandBuffer.SynchronizeBufferUsage(*indexBuffer, Sierra::BufferCommandUsage::None, Sierra::BufferCommandUsage::MemoryWrite);
            commandBuffer.CopyBufferToBuffer(*stagingBuffer, *indexBuffer);

            // Discard temporary buffer
            commandBuffer.QueueBufferForDestruction(std::move(stagingBuffer));
            commandBuffer.SynchronizeBufferUsage(*indexBuffer, Sierra::BufferCommandUsage::MemoryWrite, Sierra::BufferCommandUsage::IndexRead);
        }
        else
        {
            // Create temporary buffer to house new index data
            std::unique_ptr<Sierra::Buffer> stagingBuffer = renderingContext.CreateBuffer({
                .name = "Staging Buffer of Arena Allocator Index Data",
                .memorySize = indices.size_bytes(),
                .usage = Sierra::BufferUsage::SourceMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::CPU
            });
            stagingBuffer->CopyFromMemory(indices.data());

            // Copy index data over to GPU buffer
            commandBuffer.SynchronizeBufferUsage(*stagingBuffer, Sierra::BufferCommandUsage::None, Sierra::BufferCommandUsage::MemoryRead);
            commandBuffer.SynchronizeBufferUsage(*indexBuffer, Sierra::BufferCommandUsage::IndexRead, Sierra::BufferCommandUsage::MemoryWrite);
            commandBuffer.CopyBufferToBuffer(*stagingBuffer, *indexBuffer, stagingBuffer->GetMemorySize(), 0, currentIndexByteOffset);

            // Discard temporary buffer
            commandBuffer.QueueBufferForDestruction(std::move(stagingBuffer));
            commandBuffer.SynchronizeBufferUsage(*indexBuffer, Sierra::BufferCommandUsage::MemoryWrite, Sierra::BufferCommandUsage::IndexRead);
        }
        currentIndexByteOffset += indices.size_bytes();

        return mesh;
    }

}