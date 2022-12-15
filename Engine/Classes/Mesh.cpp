//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "Mesh.h"

#include <vector>

using Sierra::Core::Debugger;
using namespace Sierra::Core::Rendering::Vulkan;

namespace Sierra::Engine::Classes
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
            .memoryFlags = Core::Rendering::Vulkan::MemoryFlags::HOST_VISIBLE | Core::Rendering::Vulkan::MemoryFlags::HOST_COHERENT,
            .bufferUsage = Core::Rendering::Vulkan::BufferUsage::TRANSFER_SRC
        });

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenVertices.data());

        vertexBuffer = Buffer::CreateShared({
           .memorySize = bufferSize,
           .memoryFlags = Core::Rendering::Vulkan::MemoryFlags::HOST_VISIBLE | Core::Rendering::Vulkan::MemoryFlags::HOST_COHERENT,
           .bufferUsage = Core::Rendering::Vulkan::BufferUsage::TRANSFER_DST | BufferUsage::VERTEX_BUFFER
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
              .memoryFlags = Core::Rendering::Vulkan::MemoryFlags::HOST_VISIBLE | Core::Rendering::Vulkan::MemoryFlags::HOST_COHERENT,
              .bufferUsage = BufferUsage::TRANSFER_SRC
        });

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenIndices.data());

        indexBuffer = Buffer::CreateShared({
             .memorySize = bufferSize,
             .memoryFlags = Core::Rendering::Vulkan::MemoryFlags::HOST_VISIBLE | Core::Rendering::Vulkan::MemoryFlags::HOST_COHERENT,
             .bufferUsage = Core::Rendering::Vulkan::BufferUsage::TRANSFER_DST | BufferUsage::INDEX_BUFFER
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