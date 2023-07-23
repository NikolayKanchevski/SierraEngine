//
// Created by Nikolay Kanchevski on 23.07.23.
//

#include "ArenaAllocator.h"

#include "../Vulkan/VK.h"

namespace Sierra::Rendering::Modules
{

    /* --- CONSTANT PROPERTIES --- */

    constexpr uint64 INITIAL_VERTEX_BUFFER_SIZE = 4096 * 1 * sizeof(Engine::Vertex);
    constexpr uint64 INITIAL_INDEX_BUFFER_SIZE  = 4096 * 4 * sizeof(VERTEX_INDEX_TYPE);

    constexpr float VERTEX_BUFFER_GROWTH_MULTIPLIER = 2.4f;
    constexpr float INDEX_BUFFER_GROWTH_MULTIPLIER  = VERTEX_BUFFER_GROWTH_MULTIPLIER * 4.0f;

    /* --- CONSTRUCTORS --- */

    ArenaAllocator::ArenaAllocator(const ArenaAllocatorCreateInfo &createInfo)
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

    UniquePtr<ArenaAllocator> ArenaAllocator::Create(const ArenaAllocatorCreateInfo &createInfo)
    {
        return std::make_unique<ArenaAllocator>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void ArenaAllocator::RegisterMesh(const std::vector<Engine::Vertex> &vertices, const std::vector<VERTEX_INDEX_TYPE> &indices, uint64 &vertexDataOffset, uint64 &indexDataOffset)
    {
        VK::GetDevice()->WaitUntilIdle(); // TODO: Replace WaitUntilIdle() with proper barriers

        /* === Vertex Data Handling === */
        {
            // Calculate offsets
            vertexDataOffset = currentVertexDataOffset / sizeof(Engine::Vertex);
            uint64 endVertexDataOffset = currentVertexDataOffset + vertices.size() * sizeof(Engine::Vertex);

            // If vertex data cannot fit in the buffer, it is swapped for a new and bigger one
            UniquePtr<Buffer> stagingVertexBuffer;
            if (endVertexDataOffset > vertexBuffer->GetMemorySize())
            {
                ASSERT_INFO("Insufficient global vertex buffer memory - reallocating");

                // In some rare cases a single expansion may not be enough, so we increase the size until it is enough
                uint64 newVertexBufferMemorySize = vertexBuffer->GetMemorySize();
                while (newVertexBufferMemorySize < endVertexDataOffset)
                {
                    newVertexBufferMemorySize = static_cast<uint64>(newVertexBufferMemorySize * VERTEX_BUFFER_GROWTH_MULTIPLIER);
                }

                // Create staging buffer to hold old + new vertex data
                stagingVertexBuffer = Buffer::Create({
                    .memorySize = newVertexBufferMemorySize,
                    .bufferUsage = BufferUsage::TRANSFER_SRC | BufferUsage::TRANSFER_DST
                });

                // Copy old vertex data to the staging buffer
                vertexBuffer->CopyToBuffer(stagingVertexBuffer);

                // Copy new vertex data to staging buffer
                stagingVertexBuffer->CopyFromPointer(vertices.data(), vertices.size() * sizeof(Engine::Vertex), currentVertexDataOffset);

                // Reallocate old buffer
                vertexBuffer->Destroy();
                vertexBuffer = Buffer::Create({
                    .memorySize = newVertexBufferMemorySize,
                    .bufferUsage = BufferUsage::VERTEX | BufferUsage::TRANSFER_SRC | BufferUsage::TRANSFER_DST
                });

                // Copy all data from staging buffer to the newly-made vertex buffer
                stagingVertexBuffer->CopyToBuffer(vertexBuffer);
                stagingVertexBuffer->Destroy();
            }
            else
            {
                // Create staging buffer of the same size as the current vertex buffer
                stagingVertexBuffer = Buffer::Create({
                    .memorySize = endVertexDataOffset - currentVertexDataOffset,
                    .bufferUsage = BufferUsage::TRANSFER_SRC | BufferUsage::TRANSFER_DST
                });

                // Copy new vertex data to staging buffer
                stagingVertexBuffer->CopyFromPointer(vertices.data());

                // Insert staging buffer data to the main vertex buffer
                stagingVertexBuffer->CopyToBuffer(vertexBuffer, stagingVertexBuffer->GetMemorySize(), 0, currentVertexDataOffset);
                stagingVertexBuffer->Destroy();
            }

            // Save new vertex offset
            currentVertexDataOffset = endVertexDataOffset;
        }

        /* === Index Data Handling === */
        {
            // Calculate offsets
            indexDataOffset = currentIndexDataOffset / sizeof(VERTEX_INDEX_TYPE);
            uint64 endIndexDataOffset = currentIndexDataOffset + indices.size() * sizeof(VERTEX_INDEX_TYPE);

            // If index data cannot fit in the buffer, it is swapped for a new and bigger one
            UniquePtr<Buffer> stagingIndexBuffer;
            if (endIndexDataOffset > indexBuffer->GetMemorySize())
            {
                ASSERT_INFO("Insufficient global index buffer memory - reallocating");

                // Calculate new memory size
                uint64 newIndexBufferMemorySize = indexBuffer->GetMemorySize();

                // In some rare cases a single expansion may not be enough, so we increase the size until it is enough
                while (newIndexBufferMemorySize < endIndexDataOffset)
                {
                    newIndexBufferMemorySize = static_cast<uint64>(newIndexBufferMemorySize * INDEX_BUFFER_GROWTH_MULTIPLIER);
                }

                // Create staging buffer to hold old + new index data
                stagingIndexBuffer = Buffer::Create({
                    .memorySize = newIndexBufferMemorySize,
                    .bufferUsage = BufferUsage::TRANSFER_SRC | BufferUsage::TRANSFER_DST
                });

                // Copy old index data to the staging buffer
                indexBuffer->CopyToBuffer(stagingIndexBuffer);

                // Copy new index data to staging buffer
                stagingIndexBuffer->CopyFromPointer(indices.data(), indices.size() * sizeof(VERTEX_INDEX_TYPE), currentIndexDataOffset);

                // Reallocate old buffer
                indexBuffer->Destroy();
                indexBuffer = Buffer::Create({
                    .memorySize = newIndexBufferMemorySize,
                    .bufferUsage = BufferUsage::INDEX | BufferUsage::TRANSFER_SRC | BufferUsage::TRANSFER_DST
                });

                // Copy all data from staging buffer to the newly-made index buffer
                stagingIndexBuffer->CopyToBuffer(indexBuffer);
                stagingIndexBuffer->Destroy();
            }
            else
            {
                // Create staging buffer of the same size as the current index buffer
                stagingIndexBuffer = Buffer::Create({
                    .memorySize = endIndexDataOffset - currentIndexDataOffset,
                    .bufferUsage = BufferUsage::TRANSFER_SRC | BufferUsage::TRANSFER_DST
                });

                // Copy new index data to staging buffer
                stagingIndexBuffer->CopyFromPointer(indices.data());

                // Insert staging buffer data to the main index buffer
                stagingIndexBuffer->CopyToBuffer(indexBuffer, stagingIndexBuffer->GetMemorySize(), 0, currentIndexDataOffset);
                stagingIndexBuffer->Destroy();

                // Save new index offset
                currentIndexDataOffset = endIndexDataOffset;
            }
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
        vkCmdBindIndexBuffer(commandBuffer->GetVulkanCommandBuffer(), vkIndexBuffer, 0, VK_VERTEX_INDEX_BUFFER_TYPE);
    }

    /* --- DESTRUCTOR --- */

    void ArenaAllocator::Destroy()
    {
        indexBuffer->Destroy();
        vertexBuffer->Destroy();
    }

}