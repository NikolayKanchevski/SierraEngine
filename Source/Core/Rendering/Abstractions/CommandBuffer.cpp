//
// Created by Nikolay Kanchevski on 22.12.22.
//

#include "CommandBuffer.h"

#include "../Bases/VK.h"

namespace Sierra::Rendering
{

    /* --- CONSTRUCTORS --- */

    CommandBuffer::CommandBuffer(const CommandBufferCreateInfo &createInfo)
        : usage(createInfo.usage)
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
    }

    UniquePtr<CommandBuffer> CommandBuffer::Create(const CommandBufferCreateInfo &createInfo)
    {
        return std::make_unique<CommandBuffer>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void CommandBuffer::Begin()
    {
        // Reset command buffer
        vkResetCommandBuffer(vkCommandBuffer, 0);

        // Set up begin info
        VkCommandBufferBeginInfo bufferBeginInfo{};
        bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        bufferBeginInfo.flags = static_cast<VkCommandBufferUsageFlags>(usage);
        bufferBeginInfo.pInheritanceInfo = nullptr;

        VK_ASSERT(
            vkBeginCommandBuffer(vkCommandBuffer, &bufferBeginInfo),
            "Failed to begin command buffer"
        );
        isDirty = false;
    }

    void CommandBuffer::End()
    {
        vkEndCommandBuffer(vkCommandBuffer);
    }

    void CommandBuffer::SetViewport(const uint32 width, const uint32 height)
    {
        VkViewport viewport{};
        viewport.x = 0;
        viewport.y = height;
        viewport.width = static_cast<float>(width);
        viewport.height = -static_cast<float>(height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
        isDirty = true;
    }

    void CommandBuffer::SetScissor(const uint32 width, const uint32 height, const int32 xOffset, const int32 yOffset)
    {
        VkRect2D scissor{};
        scissor.offset = { xOffset, yOffset };
        scissor.extent = { width, height };

        vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
        isDirty = true;
    }

    void CommandBuffer::SetViewportAndScissor(const uint32 width, const uint32 height, const int32 xOffset, const int32 yOffset)
    {
        SetViewport(width, height);
        SetScissor(width, height, xOffset, yOffset);
    }

    void CommandBuffer::CopyBufferToBuffer(const UniquePtr<Buffer> &srcBuffer, const UniquePtr<Buffer> &dstBuffer, const uint64 size, const uint64 srcOffset, const uint64 dstOffset)
    {
        // Set up the buffer's copy region
        VkBufferCopy copyRegion{};
        copyRegion.size = size != 0 ? size : srcBuffer->GetMemorySize();
        copyRegion.srcOffset = srcOffset;
        copyRegion.dstOffset = dstOffset;

        // Copy the buffer
        vkCmdCopyBuffer(vkCommandBuffer, srcBuffer->GetVulkanBuffer(), dstBuffer->GetVulkanBuffer(), 1, &copyRegion);
        isDirty = true;
    }

    void CommandBuffer::CopyBufferToImage(const UniquePtr<Buffer> &buffer, const UniquePtr<Image> &image, const Vector3 &offsets)
    {
        // Set up image copy region
        VkBufferImageCopy copyRegion{};
        copyRegion.bufferOffset = 0;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;
        copyRegion.imageSubresource.aspectMask = static_cast<VkImageAspectFlags>(image->GetAspectFlags());
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.layerCount = image->GetLayerCount();
        copyRegion.imageOffset.x = offsets.x;
        copyRegion.imageOffset.y = offsets.y;
        copyRegion.imageOffset.z = offsets.z;
        copyRegion.imageExtent.width = image->GetWidth();
        copyRegion.imageExtent.height = image->GetHeight();
        copyRegion.imageExtent.depth = 1;

        // Copy the image to the buffer
        vkCmdCopyBufferToImage(vkCommandBuffer, buffer->GetVulkanBuffer(), image->GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
        isDirty = true;
    }

    void CommandBuffer::BlitImage(const UniquePtr<Image> &image, const Vector2 &srcOffset, const Vector2 &dstOffset, const uint32 srcMipLevel)
    {
        // Set up blit info
        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { static_cast<int32>(srcOffset.x), static_cast<int32>(srcOffset.y), 1 };
        blit.srcSubresource.aspectMask = static_cast<VkImageAspectFlags>(image->GetAspectFlags());
        blit.srcSubresource.mipLevel = srcMipLevel;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { static_cast<int32>(dstOffset.x), static_cast<int32>(dstOffset.y), 1 };
        blit.dstSubresource.aspectMask = static_cast<VkImageAspectFlags>(image->GetAspectFlags());
        blit.dstSubresource.mipLevel = srcMipLevel + 1;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        // Blit image
        vkCmdBlitImage(vkCommandBuffer, image->GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image->GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_NEAREST);
        isDirty = true;
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
        isDirty = true;
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
            .levelCount = image->GetMipLevels(),
            .baseArrayLayer = 0,
            .layerCount = image->GetLayerCount()
        };

        // Execute barrier
        vkCmdPipelineBarrier(vkCommandBuffer, static_cast<VkShaderStageFlagBits>(srcStage), static_cast<VkShaderStageFlagBits>(dstStage), 0, 0, nullptr, 0, nullptr, 1, &barrier);
        isDirty = true;
    }

    void CommandBuffer::SynchronizeImageLevelUsage(const UniquePtr<Image> &image, const VkPipelineStageFlags srcStage, const VkAccessFlagBits srcAccess, const VkPipelineStageFlags dstStage, const VkAccessFlagBits dstAccess, const uint32 targetLevel, const ImageLayout newLayout)
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
            .baseMipLevel = targetLevel,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = image->GetLayerCount()
        };

        // Execute barrier
        vkCmdPipelineBarrier(vkCommandBuffer, static_cast<VkShaderStageFlagBits>(srcStage), static_cast<VkShaderStageFlagBits>(dstStage), 0, 0, nullptr, 0, nullptr, 1, &barrier);
        isDirty = true;
    }

    void CommandBuffer::DestroyBufferAfterExecution(UniquePtr<Buffer> &buffer)
    {
        buffersToDestroy.push_back(std::move(buffer));
    }

    void CommandBuffer::PushSubmissionCompletionCallback(const std::function<void()> &Callback)
    {
        submissionCallbacks.push_back(Callback);
    }

    /* --- DESTRUCTOR --- */

    void CommandBuffer::Destroy() const
    {
        vkFreeCommandBuffers(VK::GetLogicalDevice(), VK::GetCommandPool(), 1, &vkCommandBuffer);
    }

    /* --- PRIVATE METHODS --- */

    void CommandBuffer::OnSubmissionCompletion()
    {
        for (const auto &buffer : buffersToDestroy) buffer->Destroy();
        buffersToDestroy.clear();

        for (const auto &Callback : submissionCallbacks) Callback();
        submissionCallbacks.clear();
    }

}
