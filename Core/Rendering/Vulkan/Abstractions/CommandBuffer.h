//
// Created by Nikolay Kanchevski on 22.12.22.
//

#pragma once

#include "Pipeline.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    class CommandBuffer
    {
    public:
        /* --- CONSTRUCTORS --- */
        CommandBuffer();
        static UniquePtr<CommandBuffer> Create();

        /* --- POLLING METHODS --- */
        void Begin() const;
        void End() const;
        void Reset() const;
//        void ResetQueryPool(this->drawTimeQueryPool, imageIndex * 2, 2) const;
//        void WriteTimestamp(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, drawTimeQueryPool, imageIndex * 2) const;
        void BindVertexBuffers(const std::vector<VkBuffer> &vertexBuffers) const;
        void BindIndexBuffer(const VkBuffer &indexBuffer) const;
        void Draw(const uint indexCount) const;
        void DrawUnindexed(const uint vertexCount) const;
        void SetViewport(uint width, uint height) const;
        void SetScissor(uint width, uint height, int xOffset = 0, int yOffset = 0) const;
        void SetViewportAndScissor(uint width, uint height, int xOffset = 0, int yOffset = 0) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkCommandBuffer GetVulkanCommandBuffer() const { return vkCommandBuffer; };

        /* --- OPERATIONS --- */
        DELETE_COPY(CommandBuffer);

    private:
        VkCommandBuffer vkCommandBuffer;

    };

}