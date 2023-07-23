//
// Created by Nikolay Kanchevski on 22.12.22.
//

#include "CommandBuffer.h"

#include "../VK.h"

namespace Sierra::Rendering
{

    /* --- CONSTRUCTORS --- */

    CommandBuffer::CommandBuffer(const UniquePtr<CommandBufferCreateInfo> &createInfo)
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

    UniquePtr<CommandBuffer> CommandBuffer::Create(const UniquePtr<CommandBufferCreateInfo> &createInfo)
    {
        return std::make_unique<CommandBuffer>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void CommandBuffer::Begin(const CommandBufferUsage usage) const
    {
        VkCommandBufferBeginInfo bufferBeginInfo{};
        bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        bufferBeginInfo.flags = static_cast<VkCommandBufferUsageFlags>(usage);
        bufferBeginInfo.pInheritanceInfo = nullptr;

        VK_ASSERT(
            vkBeginCommandBuffer(vkCommandBuffer, &bufferBeginInfo),
            "Failed to begin command buffer"
        );
    }

    void CommandBuffer::End() const
    {
        vkEndCommandBuffer(vkCommandBuffer);

        // Revert all images' initial layouts, as the updated ones are discarded after drawing
        for (auto pair : initialImageLayouts)
        {
            pair.second.firstTime = true;
            pair.first->layout = pair.second.initialLayout;
        }
    }

    void CommandBuffer::Reset() const
    {
        vkResetCommandBuffer(vkCommandBuffer, 0);
    }

    void CommandBuffer::Free() const
    {
        vkFreeCommandBuffers(VK::GetLogicalDevice(), VK::GetCommandPool(), 1, &vkCommandBuffer);
    }

    void CommandBuffer::TransitionImageLayout(const UniquePtr<Image> &image, const ImageLayout newLayout)
    {
        if (image->GetLayout() == newLayout) return;

        // If first time transitioning the image for frame save its original layout
        auto &imageReference = initialImageLayouts[image.get()];
        if (imageReference.firstTime)
        {
            imageReference.firstTime = false;
            imageReference.initialLayout = image->GetLayout();
        }

        // Transition layout
        TransitionImageLayout(image.get(), newLayout);
    }

    void CommandBuffer::TransitionImageLayouts(const std::vector<ReferenceWrapper<UniquePtr<Image>>> &images, const ImageLayout newLayout)
    {
        for (auto &imageReference : images)
        {
            TransitionImageLayout(imageReference.get(), newLayout);
        }
    }

    void CommandBuffer::SetViewport(const uint width, const uint height) const
    {
        VkViewport viewport{};
        viewport.x = 0;
        viewport.y = height;
        viewport.width = static_cast<float>(width);
        viewport.height = -static_cast<float>(height);
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

    void CommandBuffer::SynchronizeBufferUsage(const UniquePtr<Buffer> &buffer, VkPipelineStageFlags srcStage, VkAccessFlagBits srcAccess, VkPipelineStageFlags dstStage, VkAccessFlagBits dstAccess)
    {
        // Set up barrier info
        VkBufferMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        barrier.srcAccessMask = srcAccess;
        barrier.dstAccessMask = dstAccess;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.buffer = buffer->GetVulkanBuffer();
        barrier.offset = 0;
        barrier.size = buffer->GetMemorySize();

        // Execute barrier
        vkCmdPipelineBarrier(vkCommandBuffer, static_cast<VkShaderStageFlagBits>(srcStage), static_cast<VkShaderStageFlagBits>(dstStage), 0, 0, nullptr, 1, &barrier, 0, nullptr);
    }

    void CommandBuffer::SynchronizeImageUsage(const UniquePtr<Image> &image, const VkPipelineStageFlags srcStage, const VkAccessFlagBits srcAccess, const VkPipelineStageFlags dstStage, const VkAccessFlagBits dstAccess, const ImageLayout newLayout)
    {
        // Set up barrier info
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.srcAccessMask = srcAccess;
        barrier.dstAccessMask = dstAccess;
        barrier.oldLayout = static_cast<VkImageLayout>(image->GetLayout());
        barrier.newLayout = static_cast<VkImageLayout>(newLayout != ImageLayout::UNDEFINED ? newLayout : image->GetLayout());
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image->GetVulkanImage();
        barrier.subresourceRange = {
            .aspectMask = static_cast<VkImageAspectFlags>(image->GetAspectFlags()),
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = image->GetLayerCount()
        };

        // Execute barrier
        vkCmdPipelineBarrier(vkCommandBuffer, static_cast<VkShaderStageFlagBits>(srcStage), static_cast<VkShaderStageFlagBits>(dstStage), 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    /* --- PRIVATE METHODS --- */

    void CommandBuffer::TransitionImageLayout(Image *image, const ImageLayout newLayout)
    {
        // Initialize barrier info
        VkPipelineStageFlags srcStage = 0;
        VkPipelineStageFlags dstStage = 0;

        VkImageMemoryBarrier imageBarrier{};
        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

        switch (image->GetLayout())
        {
            case ImageLayout::UNDEFINED:
                imageBarrier.srcAccessMask = 0;
                srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                break;
            case ImageLayout::PRESENT_SRC:
                imageBarrier.srcAccessMask = 0;
                srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                break;
            case ImageLayout::PREINITIALIZED:
                imageBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                break;
            case ImageLayout::COLOR_ATTACHMENT_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                break;
            case ImageLayout::DEPTH_ATTACHMENT_OPTIMAL:
            case ImageLayout::STENCIL_ATTACHMENT_OPTIMAL:
            case ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                srcStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                break;
            case ImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
            case ImageLayout::DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                srcStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                break;
            case ImageLayout::TRANSFER_SRC_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            case ImageLayout::TRANSFER_DST_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            case ImageLayout::SHADER_READ_ONLY_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                break;
            case ImageLayout::DEPTH_READ_ONLY_OPTIMAL:
            case ImageLayout::STENCIL_READ_ONLY_OPTIMAL:
            case ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
                srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                break;
            default:
                ASSERT_ERROR_FORMATTED("Cannot transition image's layout from [{0}] to [{1}] on the fly", VK_TO_STRING(image->GetLayout(), ImageLayout), VK_TO_STRING(newLayout, ImageLayout));
                break;
        }

        bool transitioningForDepth = false;
        switch (newLayout)
        {
            case ImageLayout::COLOR_ATTACHMENT_OPTIMAL:
                imageBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                break;
            case ImageLayout::DEPTH_ATTACHMENT_OPTIMAL:
            case ImageLayout::STENCIL_ATTACHMENT_OPTIMAL:
            case ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                transitioningForDepth = true;
                imageBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                break;
            case ImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
            case ImageLayout::DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
                transitioningForDepth = true;
                imageBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                break;
            case ImageLayout::TRANSFER_SRC_OPTIMAL:
                imageBarrier.srcAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
                imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            case ImageLayout::TRANSFER_DST_OPTIMAL:
                imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            case ImageLayout::SHADER_READ_ONLY_OPTIMAL:
                imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                dstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                break;
            case ImageLayout::PRESENT_SRC:
                imageBarrier.dstAccessMask = 0;
                dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                break;
            default:
                ASSERT_ERROR_FORMATTED("Cannot transition image's layout from [{0}] to [{1}] on the fly", VK_TO_STRING(image->GetLayout(), ImageLayout), VK_TO_STRING(newLayout, ImageLayout));
                break;
        }

        // Suit special Vulkan requirements for depth transitions
        if (transitioningForDepth)
        {
            if (image->GetFormat() == ImageFormat::D32_SFLOAT_S8_UINT || image->GetFormat() == ImageFormat::D24_UNORM_S8_UINT)
                imageBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
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

        // Apply new layout
        image->layout = newLayout;

        // Record barrier
        vkCmdPipelineBarrier(
            vkCommandBuffer, srcStage, dstStage, 0, 0,
            nullptr, 0, nullptr, 1, &imageBarrier
        );
    }

    void CommandBuffer::TransitionImageLayoutFromRenderPass(Image *image, const ImageLayout newLayout)
    {
        if (image->GetLayout() == newLayout) return;

        // If first time transitioning the image for frame save its original layout
        auto &imageReference = initialImageLayouts[image];
        if (imageReference.firstTime)
        {
            imageReference.firstTime = false;
            imageReference.initialLayout = image->GetLayout();
        }

        // Apply new layout
        image->layout = newLayout;
    }

}
