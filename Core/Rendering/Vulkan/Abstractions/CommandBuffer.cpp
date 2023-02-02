//
// Created by Nikolay Kanchevski on 22.12.22.
//

#include "CommandBuffer.h"

#define INDEX_BUFFER_TYPE VK_INDEX_TYPE_UINT32

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    /* --- CONSTRUCTORS --- */

    CommandBuffer::CommandBuffer()
    {
        // Set up allocation info
        VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandPool = VK::GetCommandPool();
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = 1;

        VK_ASSERT(
            vkAllocateCommandBuffers(VK::GetDevice()->GetLogicalDevice(), &commandBufferAllocateInfo, &vkCommandBuffer),
            "Failed to allocate command buffer"
        );

        Reset();
    }

    UniquePtr<CommandBuffer> CommandBuffer::Create()
    {
        return std::make_unique<CommandBuffer>();
    }

    /* --- POLLING METHODS --- */

    void CommandBuffer::Begin() const
    {
        VkCommandBufferBeginInfo bufferBeginInfo{};
        bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        bufferBeginInfo.flags = 0;
        bufferBeginInfo.pInheritanceInfo = nullptr;

        VK_ASSERT(
            vkBeginCommandBuffer(vkCommandBuffer, &bufferBeginInfo),
            "Failed to begin command buffer"
        );
    }

    void CommandBuffer::End() const
    {
        vkEndCommandBuffer(vkCommandBuffer);
    }

    void CommandBuffer::Reset() const
    {
        vkResetCommandBuffer(vkCommandBuffer, 0);
    }

    void CommandBuffer::BindVertexBuffers(const std::vector<VkBuffer> &vertexBuffers) const
    {
        VkDeviceSize offsets[] { 0 };
        vkCmdBindVertexBuffers(vkCommandBuffer, 0, vertexBuffers.size(), vertexBuffers.data(), offsets);
    }

    void CommandBuffer::BindIndexBuffer(const VkBuffer &indexBuffer) const
    {
        vkCmdBindIndexBuffer(vkCommandBuffer, indexBuffer, 0, INDEX_BUFFER_TYPE);
    }

    void CommandBuffer::Draw(const uint indexCount) const
    {
        vkCmdDrawIndexed(vkCommandBuffer, indexCount, 1, 0, 0, 0);
    }

    void CommandBuffer::DrawUnindexed(const uint vertexCount) const
    {
        vkCmdDraw(vkCommandBuffer, vertexCount, 1, 0, 0);
    }

    void CommandBuffer::SetViewport(const uint width, const uint height) const
    {
        VkViewport viewport{};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = width;
        viewport.height = height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
    }

    void CommandBuffer::SetScissor(const uint width, const uint height, const int xOffset, const int yOffset) const
    {
        VkRect2D scissor{};
        scissor.offset = { xOffset, yOffset };
        scissor.extent = { width, height };

        vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
    }

    void CommandBuffer::SetViewportAndScissor(const uint width, const uint height, const int xOffset, const int yOffset) const
    {
        SetViewport(width, height);
        SetScissor(width, height, xOffset, yOffset);
    }

}
