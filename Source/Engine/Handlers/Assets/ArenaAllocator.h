//
// Created by Nikolay Kanchevski on 23.07.23.
//

#pragma once

#include "../../Classes/Vertex.h"
#include "../../../Core/Rendering/Abstractions/Buffer.h"
#include "../../../Core/Rendering/Abstractions/CommandBuffer.h"

namespace Sierra::Engine
{

    class ArenaAllocator
    {
    public:
        /* --- CONSTRUCTORS --- */
        ArenaAllocator();
        static UniquePtr<ArenaAllocator> Create();

        /* --- POLLING METHODS --- */
        void RegisterMesh(const UniquePtr<Rendering::CommandBuffer>& commandBuffer, const std::vector<Vertex> &vertices, const std::vector<uint32> &indices, uint64 &vertexDataOffset, uint64 &indexDataOffset);
        void Bind(const UniquePtr<Rendering::CommandBuffer> &commandBuffer);

        /* --- DESTRUCTOR --- */
        DELETE_COPY(ArenaAllocator);
        void Destroy();

    private:
        UniquePtr<Rendering::Buffer> vertexBuffer;
        uint64 currentVertexDataOffset = 0;

        UniquePtr<Rendering::Buffer> indexBuffer;
        uint64 currentIndexDataOffset = 0;

    };

}