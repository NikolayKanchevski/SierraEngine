//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "Mesh.h"
#include "../Components/InternalComponents.h"
#include "../../Core/Rendering/Vulkan/VulkanCore.h"

using Sierra::Core::Debugger;
using namespace Sierra::Core::Rendering::Vulkan;

namespace Sierra::Engine::Components
{

    /* --- CONSTRUCTORS --- */

    Mesh::Mesh(std::vector<Vertex> &givenVertices, std::vector<uint32_t> &givenIndices)
        : vertexCount(givenVertices.size()), indexCount(givenIndices.size())
    {
        CreateVertexBuffer(givenVertices);
        CreateIndexBuffer(givenIndices);

        totalMeshCount++;
        totalVertexCount += vertexCount;
    }

    /* --- SETTER METHODS --- */

    void Mesh::CreateVertexBuffer(std::vector<Vertex> &givenVertices)
    {
        // Calculate the buffer size
        uint64_t bufferSize = sizeof(Vertex) * givenVertices.size();

        auto stagingBuffer = Buffer::Create({
            .memorySize = bufferSize,
            .memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            .bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        });

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenVertices.data());

        vertexBuffer = Buffer::CreateShared({
           .memorySize = bufferSize,
           .memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
           .bufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
       });

        stagingBuffer->CopyToBuffer(vertexBuffer.get());

        stagingBuffer->Destroy();
    }

    void Mesh::CreateIndexBuffer(std::vector<uint32_t> &givenIndices)
    {
        // Calculate the buffer size
        uint64_t bufferSize = sizeof(uint32_t) * givenIndices.size();

        auto stagingBuffer = Buffer::Create({
              .memorySize = bufferSize,
              .memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
              .bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        });

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenIndices.data());

        indexBuffer = Buffer::CreateShared({
             .memorySize = bufferSize,
             .memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
             .bufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
         });

        stagingBuffer->CopyToBuffer(indexBuffer.get());

        stagingBuffer->Destroy();
    }

    /* --- DESTRUCTOR --- */

    void Mesh::Destroy()
    {
        vertexBuffer->Destroy();
        indexBuffer->Destroy();

        totalMeshCount--;
        totalVertexCount -= vertexCount;
    }
}