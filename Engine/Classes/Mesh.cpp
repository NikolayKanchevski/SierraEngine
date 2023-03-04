//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "Mesh.h"

using Sierra::Core::Debugger;
using namespace Sierra::Core::Rendering::Vulkan;

namespace Sierra::Engine::Classes
{

    /* --- CONSTRUCTORS --- */

    Mesh::Mesh(std::vector<VertexP> &givenVertices, std::vector<uint> &givenIndices)
        : vertexCount(givenVertices.size()), indexCount(givenIndices.size())
    {
        CreateVertexBuffer(givenVertices);
        CreateIndexBuffer(givenIndices);

        totalMeshCount++;
        totalVertexCount += vertexCount;
    }

    Mesh::Mesh(std::vector<VertexPNU> &givenVertices, std::vector<uint> &givenIndices)
            : vertexCount(givenVertices.size()), indexCount(givenIndices.size())
    {
        CreateVertexBuffer(givenVertices);
        CreateIndexBuffer(givenIndices);

        totalMeshCount++;
        totalVertexCount += vertexCount;
    }

    /* --- SETTER METHODS --- */

    // TEMPLATE FOLLOWING 2 FUNCTIONS

    void Mesh::CreateVertexBuffer(std::vector<VertexP> &givenVertices)
    {
        // Calculate the buffer size
        uint64 bufferSize = sizeof(VertexP) * givenVertices.size();

        auto stagingBuffer = Buffer::Create({
            .memorySize = bufferSize,
            .memoryFlags = MemoryFlags::HOST_VISIBLE | MemoryFlags::HOST_COHERENT,
            .bufferUsage = BufferUsage::TRANSFER_SRC
        });

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenVertices.data());

        vertexBuffer = Buffer::CreateShared({
           .memorySize = bufferSize,
           .memoryFlags = MemoryFlags::HOST_VISIBLE | MemoryFlags::HOST_COHERENT,
           .bufferUsage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX
       });

        stagingBuffer->CopyToBuffer(vertexBuffer.get());

        stagingBuffer->Destroy();
    }


    void Mesh::CreateVertexBuffer(std::vector<VertexPNU> &givenVertices)
    {
        // Calculate the buffer size
        uint64 bufferSize = sizeof(VertexPNU) * givenVertices.size();

        auto stagingBuffer = Buffer::Create({
            .memorySize = bufferSize,
            .memoryFlags = MemoryFlags::HOST_VISIBLE | MemoryFlags::HOST_COHERENT,
            .bufferUsage = BufferUsage::TRANSFER_SRC
        });

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenVertices.data());

        vertexBuffer = Buffer::CreateShared({
            .memorySize = bufferSize,
            .memoryFlags = MemoryFlags::HOST_VISIBLE | MemoryFlags::HOST_COHERENT,
            .bufferUsage = BufferUsage::TRANSFER_DST | BufferUsage::VERTEX
        });

        stagingBuffer->CopyToBuffer(vertexBuffer.get());

        stagingBuffer->Destroy();
    }

    void Mesh::CreateIndexBuffer(std::vector<uint> &givenIndices)
    {
        // Calculate the buffer size
        uint64 bufferSize = UINT_SIZE * givenIndices.size();

        auto stagingBuffer = Buffer::Create({
            .memorySize = bufferSize,
            .memoryFlags = MemoryFlags::HOST_VISIBLE | MemoryFlags::HOST_COHERENT,
            .bufferUsage = BufferUsage::TRANSFER_SRC
        });

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenIndices.data());

        indexBuffer = Buffer::CreateShared({
             .memorySize = bufferSize,
             .memoryFlags = MemoryFlags::HOST_VISIBLE | MemoryFlags::HOST_COHERENT,
             .bufferUsage = BufferUsage::TRANSFER_DST | BufferUsage::INDEX
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