//
// Created by Nikolay Kanchevski on 22.12.22.
//

#include "CommandBuffer.h"

#include "../VK.h"
#include "../../RenderingSettings.h"

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

    void CommandBuffer::Begin(const VkCommandBufferUsageFlagBits flags) const
    {
        VkCommandBufferBeginInfo bufferBeginInfo{};
        bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        bufferBeginInfo.flags = flags;
        bufferBeginInfo.pInheritanceInfo = nullptr;

        VK_ASSERT(
            vkBeginCommandBuffer(vkCommandBuffer, &bufferBeginInfo),
            "Failed to begin command buffer"
        );
    }

    void CommandBuffer::End() const
    {
        vkEndCommandBuffer(vkCommandBuffer);

        // Revert all images' initial layouts, as the updated ones are discared after drawing
        for (const auto pair : initialImageLayouts)
        {
            pair.first->layout = pair.second.initialLayout;
        }
    }

    void CommandBuffer::Reset() const
    {
        vkResetCommandBuffer(vkCommandBuffer, 0);
    }

    // TODO: TRY AND AUTO PICK STAGE FLAGS

    void CommandBuffer::TransitionImageLayout(const UniquePtr<Image> &image, const ImageLayout newLayout, const VkPipelineStageFlags lastUsageStage, const VkPipelineStageFlags expectedUsageStage)
    {
        // Initialize barrier info
        VkImageMemoryBarrier imageBarrier{};
        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

        // Check if image's layout has changed during command execution
        switch (image->GetLayout())
        {
            case ImageLayout::UNDEFINED:
            case ImageLayout::PRESENT_SRC:
                imageBarrier.srcAccessMask = 0;
                break;
            case ImageLayout::PREINITIALIZED:
                imageBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                break;
            case ImageLayout::COLOR_ATTACHMENT_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                break;
            case ImageLayout::DEPTH_ATTACHMENT_OPTIMAL:
            case ImageLayout::STENCIL_ATTACHMENT_OPTIMAL:
            case ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;
            case ImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
            case ImageLayout::DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;
            case ImageLayout::TRANSFER_SRC_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                break;
            case ImageLayout::TRANSFER_DST_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                break;
            case ImageLayout::SHADER_READ_ONLY_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                break;
            case ImageLayout::DEPTH_READ_ONLY_OPTIMAL:
            case ImageLayout::STENCIL_READ_ONLY_OPTIMAL:
            case ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
                break;
            default:
                ASSERT_ERROR_FORMATTED("Cannot transition image's layout from [{0}] to [{1}] on the fly", string_VkImageLayout((VkImageLayout) image->GetLayout()), string_VkImageLayout((VkImageLayout) newLayout));
                break;
        }

        switch (newLayout)
        {
            case ImageLayout::COLOR_ATTACHMENT_OPTIMAL:
                imageBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                break;
            case ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                imageBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;
            case ImageLayout::TRANSFER_SRC_OPTIMAL:
                imageBarrier.srcAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
                imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                break;
            case ImageLayout::TRANSFER_DST_OPTIMAL:
                imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                break;
            case ImageLayout::SHADER_READ_ONLY_OPTIMAL:
                imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                break;
            case ImageLayout::PRESENT_SRC:
                imageBarrier.dstAccessMask = 0;
                break;
            default:
                ASSERT_ERROR_FORMATTED("Cannot transition image's layout from [{0}] to [{1}] on the fly", string_VkImageLayout((VkImageLayout) image->GetLayout()), string_VkImageLayout((VkImageLayout) newLayout));
                break;
        }

        // Populate rest of barrier info
        imageBarrier.oldLayout = (VkImageLayout) image->GetLayout();
        imageBarrier.newLayout = (VkImageLayout) newLayout;
        imageBarrier.image = image->GetVulkanImage();
        imageBarrier.subresourceRange.aspectMask = (VkImageAspectFlags) image->GetAspectFlags();
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.subresourceRange.layerCount = image->GetLayerCount();
        imageBarrier.subresourceRange.baseMipLevel = 0;
        imageBarrier.subresourceRange.levelCount = image->GetMipMapLevels();

        // If first time transitioning the image for frame save its original layout
        auto &imageReference = initialImageLayouts[image.get()];
        if (imageReference.firstTime)
        {
            imageReference.firstTime = false;
            imageReference.initialLayout = image->GetLayout();
        }

        // Apply new layout
        image->layout = newLayout;

        // Record barrier
        vkCmdPipelineBarrier(
            vkCommandBuffer, lastUsageStage, expectedUsageStage, 0, 0,
            nullptr, 0, nullptr, 1, &imageBarrier
        );
    }

    void CommandBuffer::TransitionImageLayouts(std::vector<ImageReference> images, const ImageLayout newLayout, const VkPipelineStageFlags lastUsageStage, const VkPipelineStageFlags expectedUsageStage)
    {
        for (auto &imageReference : images)
        {
            TransitionImageLayout(imageReference.image, newLayout, lastUsageStage, expectedUsageStage);
        }
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

    void CommandBuffer::DrawIndexed(const uint indexCount) const
    {
        vkCmdDrawIndexed(vkCommandBuffer, indexCount, 1, 0, 0, 0);
    }

    void CommandBuffer::Draw(const uint vertexCount) const
    {
        vkCmdDraw(vkCommandBuffer, vertexCount, 1, 0, 0);
    }

    void CommandBuffer::Dispatch(const uint xCount, const uint yCount, const uint zCount)
    {
        vkCmdDispatch(vkCommandBuffer, xCount, yCount, zCount);
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
