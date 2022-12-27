//
// Created by Nikolay Kanchevski on 22.12.22.
//

#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

#include "Pipeline.h"
#include "../../../Debugger.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    class CommandBuffer
    {
    public:
        /* --- CONSTRUCTORS --- */
        CommandBuffer();
        static std::unique_ptr<CommandBuffer> Create();

        /* --- POLLING METHODS --- */
        void Begin() const;
        void End() const;
        void Reset() const;
//        void ResetQueryPool(this->drawTimeQueryPool, imageIndex * 2, 2) const;
//        void WriteTimestamp(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, drawTimeQueryPool, imageIndex * 2) const;
        void BindVertexBuffers(const std::vector<VkBuffer> &vertexBuffers) const;
        void BindIndexBuffer(const VkBuffer &indexBuffer) const;
        void Draw(const uint32_t indexCount) const;
        void SetViewport(uint32_t width, uint32_t height) const;
        void SetScissor(uint32_t width, uint32_t height, int xOffset = 0, int yOffset = 0) const;
        void SetViewportAndScissor(uint32_t width, uint32_t height, int xOffset = 0, int yOffset = 0) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkCommandBuffer GetVulkanCommandBuffer() const { return vkCommandBuffer; };

        /* --- OPERATIONS --- */
        CommandBuffer(const CommandBuffer &) = delete;
        CommandBuffer &operator=(const CommandBuffer &) = delete;

    private:
        VkCommandBuffer vkCommandBuffer;

    };

}