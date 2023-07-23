//
// Created by Nikolay Kanchevski on 23.07.23.
//

#pragma once

#include "../RenderingSettings.h"
#include "../../Engine/Classes/Vertex.h"
#include "../Vulkan/Abstractions/Buffer.h"
#include "../Vulkan/Abstractions/CommandBuffer.h"

namespace Sierra::Engine { class Mesh; }

namespace Sierra::Rendering::Modules
{

    struct ArenaAllocatorCreateInfo
    {

    };

    class ArenaAllocator
    {
    public:
        /* --- CONSTRUCTORS --- */
        ArenaAllocator([[maybe_unused]] const ArenaAllocatorCreateInfo &createInfo);
        static UniquePtr<ArenaAllocator> Create([[maybe_unused]] const ArenaAllocatorCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void RegisterMesh(const std::vector<Engine::Vertex> &vertices, const std::vector<VERTEX_INDEX_TYPE> &indices, uint64 &vertexDataOffset, uint64 &indexDataOffset);
        void Bind(const UniquePtr<CommandBuffer> &commandBuffer);

        /* --- DESTRUCTOR --- */
        DELETE_COPY(ArenaAllocator);
        void Destroy();

    private:
        UniquePtr<Buffer> vertexBuffer;
        uint64 currentVertexDataOffset = 0;

        UniquePtr<Buffer> indexBuffer;
        uint64 currentIndexDataOffset = 0;

        void RegisterMesh(entt::registry &enttRegistry, const entt::entity enttEntity);

    };

}