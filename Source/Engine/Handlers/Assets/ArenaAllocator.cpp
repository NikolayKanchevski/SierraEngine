//
// Created by Nikolay Kanchevski on 23.07.23.
//

#include "ArenaAllocator.h"

namespace Sierra::Engine
{

    /* --- CONSTANT PROPERTIES --- */

    using namespace Rendering;
    constexpr uint64 INITIAL_VERTEX_BUFFER_SIZE = 24 * sizeof(Vertex);
    constexpr uint64 INITIAL_INDEX_BUFFER_SIZE  = 36 * sizeof(uint32);

    constexpr float VERTEX_BUFFER_GROWTH_MULTIPLIER = 2.4f;
    constexpr float INDEX_BUFFER_GROWTH_MULTIPLIER  = VERTEX_BUFFER_GROWTH_MULTIPLIER * 4.0f;

    /* --- CONSTRUCTORS --- */

    ArenaAllocator::ArenaAllocator()
    {
        // Create vertex buffer
        vertexBuffer = Buffer::Create({
            .memorySize = INITIAL_VERTEX_BUFFER_SIZE,
            .bufferUsage = BufferUsage::VERTEX | BufferUsage::TRANSFER_SRC | BufferUsage::TRANSFER_DST
        });

        // Create index buffer
        indexBuffer = Buffer::Create({
            .memorySize = INITIAL_INDEX_BUFFER_SIZE,
            .bufferUsage = BufferUsage::INDEX | BufferUsage::TRANSFER_SRC | BufferUsage::TRANSFER_DST
        });
    }

    UniquePtr<ArenaAllocator> ArenaAllocator::Create()
    {
        return std::make_unique<ArenaAllocator>();
    }

    /* --- POLLING METHODS --- */

    void ArenaAllocator::RegisterMesh(const UniquePtr<CommandBuffer>& commandBuffer, const std::vector<Vertex> &vertices, const std::vector<uint32> &indices, uint64 &vertexDataOffset, uint64 &indexDataOffset)
    {
        // Get asset manager's command buffer
        commandBuffer->SynchronizeBufferUsage(vertexBuffer, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_WRITE_BIT);
        commandBuffer->SynchronizeBufferUsage(indexBuffer, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_INDEX_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_WRITE_BIT);

        // === Vertex Data Handling === //
        {
            // Return last set vertex offset (it is in bytes, so a division by vertex size is required)
            vertexDataOffset = currentVertexDataOffset / sizeof(Vertex);

            // Calculate last index that vertex data will occupy and check if it is within current buffer size
            uint64 endVertexDataOffset = currentVertexDataOffset + vertices.size() * sizeof(Vertex);
            if (endVertexDataOffset > vertexBuffer->GetMemorySize())
            {
                ASSERT_INFO("Insufficient global vertex buffer memory - reallocating");

                // Increase buffer size until it is enough to hold requested data
                uint64 newVertexBufferSize = vertexBuffer->GetMemorySize();
                while (newVertexBufferSize < endVertexDataOffset)
                {
                    newVertexBufferSize = static_cast<uint64>(newVertexBufferSize * VERTEX_BUFFER_GROWTH_MULTIPLIER);
                }

                // Create staging buffer
                auto stagingBuffer = Buffer::Create({
                    .memorySize = newVertexBufferSize,
                    .bufferUsage = BufferUsage::TRANSFER_SRC | BufferUsage::TRANSFER_DST
                });

                // Copy old vertex data (not the whole buffer, but only the occupied part of it) to the staging buffer
                commandBuffer->SynchronizeBufferUsage(vertexBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_READ_BIT);
                commandBuffer->CopyBufferToBuffer(vertexBuffer, stagingBuffer, currentVertexDataOffset, 0, 0);

                // Copy requested vertex data to staging buffer
                stagingBuffer->CopyFromPointer(vertices.data(), endVertexDataOffset - currentVertexDataOffset, currentVertexDataOffset);

                // Reallocate vertex buffer with new memory size
                commandBuffer->DestroyBufferAfterExecution(vertexBuffer);
                vertexBuffer = Buffer::Create({
                    .memorySize = newVertexBufferSize,
                    .bufferUsage = BufferUsage::VERTEX | BufferUsage::TRANSFER_SRC | BufferUsage::TRANSFER_DST
                });

                // Copy all data inside the staging buffer to the new vertex buffer
                commandBuffer->SynchronizeBufferUsage(stagingBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_READ_BIT);
                commandBuffer->CopyBufferToBuffer(stagingBuffer, vertexBuffer, endVertexDataOffset, 0, 0);

                // Query staging buffer for destruction
                commandBuffer->DestroyBufferAfterExecution(stagingBuffer);
            }
            else
            {
                // Create staging buffer
                auto stagingBuffer = Buffer::Create({
                    .memorySize = endVertexDataOffset - currentVertexDataOffset,
                    .bufferUsage = BufferUsage::TRANSFER_SRC | BufferUsage::TRANSFER_DST
                });

                // Copy requested vertex data to staging buffer
                stagingBuffer->CopyFromPointer(vertices.data(), stagingBuffer->GetMemorySize(), 0);

                // Copy all data from staging buffer to vertex buffer
                commandBuffer->SynchronizeBufferUsage(vertexBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_WRITE_BIT);
                commandBuffer->CopyBufferToBuffer(stagingBuffer, vertexBuffer, stagingBuffer->GetMemorySize(), 0, currentVertexDataOffset);

                // Query staging buffer for destruction
                commandBuffer->DestroyBufferAfterExecution(stagingBuffer);
            }

            // Save last offset written to
            currentVertexDataOffset = endVertexDataOffset;
        }

        // === Index Data Handling === //
        {
            // Return last set index offset (it is in bytes, so a division by the size of index type is required)
            indexDataOffset = currentIndexDataOffset / sizeof(uint32);

            // Calculate last index that index data will occupy and check if it is within current buffer size
            uint64 endIndexDataOffset = currentIndexDataOffset + indices.size() * sizeof(uint32);
            if (endIndexDataOffset > indexBuffer->GetMemorySize())
            {
                ASSERT_INFO("Insufficient global index buffer memory - reallocating");

                // Increase buffer size until it is enough to hold requested data
                uint64 newIndexBufferSize = indexBuffer->GetMemorySize();
                while (newIndexBufferSize < endIndexDataOffset)
                {
                    newIndexBufferSize = static_cast<uint64>(newIndexBufferSize * INDEX_BUFFER_GROWTH_MULTIPLIER);
                }

                // Create staging buffer
                auto stagingBuffer = Buffer::Create({
                    .memorySize = newIndexBufferSize,
                    .bufferUsage = BufferUsage::TRANSFER_SRC | BufferUsage::TRANSFER_DST
                });

                // Copy old index data (not the whole buffer, but only the occupied part of it) to the staging buffer
                commandBuffer->SynchronizeBufferUsage(indexBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_READ_BIT);
                commandBuffer->CopyBufferToBuffer(indexBuffer, stagingBuffer, currentIndexDataOffset, 0, 0);

                // Copy requested index data to staging buffer
                stagingBuffer->CopyFromPointer(indices.data(), endIndexDataOffset - currentIndexDataOffset, currentIndexDataOffset);

                // Reallocate index buffer with new memory size
                commandBuffer->DestroyBufferAfterExecution(indexBuffer);
                indexBuffer = Buffer::Create({
                    .memorySize = newIndexBufferSize,
                    .bufferUsage = BufferUsage::INDEX | BufferUsage::TRANSFER_SRC | BufferUsage::TRANSFER_DST
                });

                // Copy all data inside the staging buffer to the new index buffer
                commandBuffer->SynchronizeBufferUsage(stagingBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_READ_BIT);
                commandBuffer->CopyBufferToBuffer(stagingBuffer, indexBuffer, endIndexDataOffset, 0, 0);

                // Query staging buffer for destruction
                commandBuffer->DestroyBufferAfterExecution(stagingBuffer);
            }
            else
            {
                // Create staging buffer
                auto stagingBuffer = Buffer::Create({
                    .memorySize = endIndexDataOffset - currentIndexDataOffset,
                    .bufferUsage = BufferUsage::TRANSFER_SRC | BufferUsage::TRANSFER_DST
                });

                // Copy requested vertex data to staging buffer
                stagingBuffer->CopyFromPointer(indices.data(), stagingBuffer->GetMemorySize(), 0);

                // Copy all data from staging buffer to index buffer
                commandBuffer->SynchronizeBufferUsage(indexBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_WRITE_BIT);
                commandBuffer->CopyBufferToBuffer(stagingBuffer, indexBuffer, stagingBuffer->GetMemorySize(), 0, currentIndexDataOffset);

                // Query staging buffer for destruction
                commandBuffer->DestroyBufferAfterExecution(stagingBuffer);
            }

            // Save last offset written to
            currentIndexDataOffset = endIndexDataOffset;
        }
    }

    void ArenaAllocator::Bind(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        constexpr static VkDeviceSize offsets[] { 0 };

        // Bind vertex data
        VkBuffer vkVertexBuffer = vertexBuffer->GetVulkanBuffer();
        vkCmdBindVertexBuffers(commandBuffer->GetVulkanCommandBuffer(), 0, 1, &vkVertexBuffer, offsets);

        // Bind index data
        VkBuffer vkIndexBuffer = indexBuffer->GetVulkanBuffer();
        vkCmdBindIndexBuffer(commandBuffer->GetVulkanCommandBuffer(), vkIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    /* --- DESTRUCTOR --- */

    void ArenaAllocator::Destroy()
    {
        indexBuffer->Destroy();
        vertexBuffer->Destroy();
    }

}