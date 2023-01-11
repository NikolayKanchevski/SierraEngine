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

    Mesh::Mesh(std::vector<VertexP> &givenVertices, std::vector<uint32_t> &givenIndices)
        : vertexCount(givenVertices.size()), indexCount(givenIndices.size())
    {
        CreateVertexBuffer(givenVertices);
        CreateIndexBuffer(givenIndices);

        totalMeshCount++;
        totalVertexCount += vertexCount;
    }

    Mesh::Mesh(std::vector<VertexPNT> &givenVertices, std::vector<uint32_t> &givenIndices)
            : vertexCount(givenVertices.size()), indexCount(givenIndices.size())
    {
        CreateVertexBuffer(givenVertices);
        CreateIndexBuffer(givenIndices);

        totalMeshCount++;
        totalVertexCount += vertexCount;
    }

    /* --- SETTER METHODS --- */

    void Mesh::CreateVertexBuffer(std::vector<VertexP> &givenVertices)
    {
        // Calculate the buffer size
        uint64_t bufferSize = sizeof(VertexP) * givenVertices.size();

        auto stagingBuffer = Buffer::Create({
            .memorySize = bufferSize,
            .memoryFlags = MEMORY_FLAGS_HOST_VISIBLE | MEMORY_FLAGS_HOST_COHERENT,
            .bufferUsage = TRANSFER_SRC_BUFFER
        });

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenVertices.data());

        vertexBuffer = Buffer::CreateShared({
           .memorySize = bufferSize,
           .memoryFlags = MEMORY_FLAGS_HOST_VISIBLE | MEMORY_FLAGS_HOST_COHERENT,
           .bufferUsage = TRANSFER_DST_BUFFER | VERTEX_BUFFER
       });

        stagingBuffer->CopyToBuffer(vertexBuffer.get());

        stagingBuffer->Destroy();
    }


    void Mesh::CreateVertexBuffer(std::vector<VertexPNT> &givenVertices)
    {
        // Calculate the buffer size
        uint64_t bufferSize = sizeof(VertexPNT) * givenVertices.size();

        auto stagingBuffer = Buffer::Create({
            .memorySize = bufferSize,
            .memoryFlags = MEMORY_FLAGS_HOST_VISIBLE | MEMORY_FLAGS_HOST_COHERENT,
            .bufferUsage = TRANSFER_SRC_BUFFER
        });

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenVertices.data());

        vertexBuffer = Buffer::CreateShared({
            .memorySize = bufferSize,
            .memoryFlags = MEMORY_FLAGS_HOST_VISIBLE | MEMORY_FLAGS_HOST_COHERENT,
            .bufferUsage = TRANSFER_DST_BUFFER | VERTEX_BUFFER
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
            .memoryFlags = MEMORY_FLAGS_HOST_VISIBLE | MEMORY_FLAGS_HOST_COHERENT,
            .bufferUsage = TRANSFER_SRC_BUFFER
        });

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenIndices.data());

        indexBuffer = Buffer::CreateShared({
             .memorySize = bufferSize,
             .memoryFlags = MEMORY_FLAGS_HOST_VISIBLE | MEMORY_FLAGS_HOST_COHERENT,
             .bufferUsage = TRANSFER_DST_BUFFER | INDEX_BUFFER
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