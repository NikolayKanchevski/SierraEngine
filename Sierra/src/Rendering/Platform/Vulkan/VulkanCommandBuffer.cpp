//
// Created by Nikolay Kanchevski on 11.12.23.
//

#include "VulkanCommandBuffer.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanSampler.h"

#include "VulkanRenderPass.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice &device, const CommandBufferCreateInfo &createInfo)
        : CommandBuffer(createInfo), VulkanResource(createInfo.name), device(device), pushDescriptorSet({ })
    {
        // Set up pool create info
        VkCommandPoolCreateInfo commandPoolCreateInfo = { };
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = device.GetGeneralQueueFamily();

        // Create command pool
        VkResult result = device.GetFunctionTable().vkCreateCommandPool(device.GetLogicalDevice(), &commandPoolCreateInfo, nullptr, &commandPool);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create command pool for command buffer [{0}]! Error code: {1}.", GetName(), result);

        // Set up allocate info
        VkCommandBufferAllocateInfo allocateInfo = { };
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = commandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = 1;

        // Allocate command buffer
        result = device.GetFunctionTable().vkAllocateCommandBuffers(device.GetLogicalDevice(), &allocateInfo, &commandBuffer);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create command buffer [{0}]! Error code: {1}.", GetName(), result);

        // Set object names
        device.SetObjectName(commandBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER, GetName());
        device.SetObjectName(commandPool, VK_OBJECT_TYPE_COMMAND_POOL, "Command pool of command buffer [" + GetName() + "]");
    }

    /* --- POLLING METHODS --- */

    void VulkanCommandBuffer::Begin()
    {
        // Free queued resources
        queuedBuffers = std::queue<std::unique_ptr<Buffer>>();
        queuedImages = std::queue<std::unique_ptr<Image>>();

        // Reset command buffer
        device.GetFunctionTable().vkResetCommandPool(device.GetLogicalDevice(), commandPool, 0);

        // Set up begin info
        VkCommandBufferBeginInfo beginInfo = { };
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        // Begin command buffer
        const VkResult result = device.GetFunctionTable().vkBeginCommandBuffer(commandBuffer, &beginInfo);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not begin command buffer [{0}]! Error code: {1}.", GetName(), result);

        // Get new code
        completionSignalValue = device.GetNewSignalValue();
    }

    void VulkanCommandBuffer::End()
    {
        // End command buffer
        const VkResult result = device.GetFunctionTable().vkEndCommandBuffer(commandBuffer);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not end command buffer [{0}]! Error code: {1}.", GetName(), result);
    }

    void VulkanCommandBuffer::SynchronizeBufferUsage(const std::unique_ptr<Buffer> &buffer, const BufferCommandUsage previousUsage, const BufferCommandUsage nextUsage, const uint64 memorySize, const uint64 byteOffset)
    {
        SR_ERROR_IF(buffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not synchronize usage of buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", buffer->GetName(), GetName());
        const VulkanBuffer &vulkanBuffer = static_cast<VulkanBuffer&>(*buffer);

        SR_ERROR_IF(nextUsage == BufferCommandUsage::None, "[Vulkan]: Cannot synchronize buffer [{0}], as specified next usage must not be BufferCommandUsage::None!", buffer->GetName());

        // Set up pipeline barrier
        VkBufferMemoryBarrier pipelineBarrier = { };
        pipelineBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        pipelineBarrier.srcAccessMask = BufferCommandUsageToVkAccessFlags(previousUsage);
        pipelineBarrier.dstAccessMask = BufferCommandUsageToVkAccessFlags(nextUsage);
        pipelineBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        pipelineBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        pipelineBarrier.buffer = vulkanBuffer.GetVulkanBuffer();
        pipelineBarrier.offset = byteOffset;
        pipelineBarrier.size = memorySize != 0 ? memorySize : buffer->GetMemorySize();

        SR_ERROR_IF(byteOffset + pipelineBarrier.size > buffer->GetMemorySize(), "[Vulkan]: Cannot synchronize [{0}] bytes, which are offset by another [{1}], within buffer [{2}] from command buffer [{3}], as specified memory range exceeds that of the buffer - [{4}]!", pipelineBarrier.size, byteOffset, buffer->GetMemorySize(), GetName(), buffer->GetMemorySize());

        // Bind barrier
        device.GetFunctionTable().vkCmdPipelineBarrier(commandBuffer, BufferCommandUsageToVkPipelineStageFlags(previousUsage), BufferCommandUsageToVkPipelineStageFlags(nextUsage), 0, 0, nullptr, 1, &pipelineBarrier, 0, nullptr);
    }

    void VulkanCommandBuffer::SynchronizeImageUsage(const std::unique_ptr<Image> &image, const ImageCommandUsage previousUsage, const ImageCommandUsage nextUsage, const uint32 baseMipLevel, const uint32 mipLevelCount, const uint32 baseLayer, uint32 layerCount)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not synchronize usage of image [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", image->GetName(), GetName());
        const VulkanImage &vulkanImage = static_cast<VulkanImage&>(*image);

        SR_ERROR_IF(nextUsage == ImageCommandUsage::None, "[Vulkan]: Cannot synchronize image [{0}], as specified next usage must not be ImageCommandUsage::None!", image->GetName());

        SR_ERROR_IF(baseMipLevel >= image->GetMipLevelCount(), "[Vulkan]: Cannot synchronize mip level [{0}] of image [{1}] within command buffer [{2}], as it does not have it!", baseMipLevel, image->GetName(), GetName());
        SR_ERROR_IF(baseLayer >= image->GetLayerCount(), "[Vulkan]: Cannot synchronize layer [{0}] of image [{1}] within command buffer [{2}], as it does not have it!", baseLayer, image->GetName(), GetName());

        // Set up pipeline barrier
        VkImageMemoryBarrier pipelineBarrier = { };
        pipelineBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        pipelineBarrier.srcAccessMask = ImageCommandUsageToVkAccessFlags(previousUsage);
        pipelineBarrier.dstAccessMask = ImageCommandUsageToVkAccessFlags(nextUsage);
        pipelineBarrier.oldLayout = ImageCommandUsageToVkLayout(previousUsage);
        pipelineBarrier.newLayout = ImageCommandUsageToVkLayout(nextUsage);
        pipelineBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        pipelineBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        pipelineBarrier.image = vulkanImage.GetVulkanImage();
        pipelineBarrier.subresourceRange.aspectMask = vulkanImage.GetVulkanAspectFlags();
        pipelineBarrier.subresourceRange.baseMipLevel = baseMipLevel;
        pipelineBarrier.subresourceRange.levelCount = mipLevelCount != 0 ? mipLevelCount : image->GetMipLevelCount() - baseMipLevel;
        pipelineBarrier.subresourceRange.baseArrayLayer = baseLayer;
        pipelineBarrier.subresourceRange.layerCount = layerCount != 0 ? layerCount : image->GetLayerCount() - baseLayer;

        SR_ERROR_IF(baseMipLevel + pipelineBarrier.subresourceRange.levelCount > image->GetMipLevelCount(), "[Vulkan]: Cannot synchronize mip levels [{0}-{1}] of image [{2}] within command buffer [{3}], as they exceed image's mip level count - [{4}]!", baseMipLevel, baseMipLevel + mipLevelCount - 1, image->GetName(), GetName(), image->GetMipLevelCount());
        SR_ERROR_IF(baseLayer + pipelineBarrier.subresourceRange.layerCount > image->GetLayerCount(), "[Vulkan]: Cannot synchronize layers [{0}-{1}] of image [{2}] within command buffer [{3}], as they exceed image's layer count - [{4}]!", baseLayer, baseLayer + layerCount - 1, image->GetName(), GetName(), image->GetLayerCount());

        // Bind barrier
        device.GetFunctionTable().vkCmdPipelineBarrier(commandBuffer, ImageCommandUsageToVkPipelineStageFlags(previousUsage), ImageCommandUsageToVkPipelineStageFlags(nextUsage), 0, 0, nullptr, 0, nullptr, 1, &pipelineBarrier);
    }

    void VulkanCommandBuffer::CopyBufferToBuffer(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Buffer> &destinationBuffer, const uint64 memoryRange, const uint64 sourceByteOffset, const uint64 destinationByteOffset)
    {
        SR_ERROR_IF(sourceBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not copy from buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", sourceBuffer->GetName(), GetName());
        const VulkanBuffer &vulkanSourceBuffer = static_cast<VulkanBuffer&>(*sourceBuffer);

        SR_ERROR_IF(destinationBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not copy to buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", destinationBuffer->GetName(), GetName());
        const VulkanBuffer &vulkanDestinationBuffer = static_cast<VulkanBuffer&>(*destinationBuffer);

        VkBufferCopy copyRegion = { };
        copyRegion.srcOffset = sourceByteOffset;
        copyRegion.dstOffset = destinationByteOffset;
        copyRegion.size = memoryRange != 0 ? memoryRange : sourceBuffer->GetMemorySize();

        SR_ERROR_IF(sourceByteOffset + copyRegion.size > sourceBuffer->GetMemorySize(), "[Vulkan]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, from buffer [{2}] within command buffer [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryRange, sourceByteOffset, sourceBuffer->GetName(), GetName(), sourceByteOffset + memoryRange, sourceBuffer->GetMemorySize());
        SR_ERROR_IF(destinationByteOffset + copyRegion.size > destinationBuffer->GetMemorySize(), "[Vulkan]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, to buffer [{2}] within command buffer [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryRange, destinationByteOffset, destinationBuffer->GetName(), GetName(), destinationByteOffset + memoryRange, destinationBuffer->GetMemorySize());

        // Record copy
        device.GetFunctionTable().vkCmdCopyBuffer(commandBuffer, vulkanSourceBuffer.GetVulkanBuffer(), vulkanDestinationBuffer.GetVulkanBuffer(), 1, &copyRegion);
    }

    void VulkanCommandBuffer::CopyBufferToImage(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Image> &destinationImage, const uint32 mipLevel, const Vector2UInt &pixelRange, const uint32 layer, const uint64 sourceByteOffset, const Vector2UInt &destinationPixelOffset)
    {
        SR_ERROR_IF(sourceBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not copy from buffer [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], to image [{1}] within command buffer [{2}]!", sourceBuffer->GetName(), destinationImage->GetName(), GetName());
        const VulkanBuffer &vulkanSourceBuffer = static_cast<VulkanBuffer&>(*sourceBuffer);

        SR_ERROR_IF(destinationImage->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not from buffer [{0}] to image [{1}], graphics API differs from [GraphicsAPI::Vulkan], within command buffer [{2}]!", sourceBuffer->GetName(), destinationImage->GetName(), GetName());
        const VulkanImage &vulkanDestinationImage = static_cast<VulkanImage&>(*destinationImage);

        SR_ERROR_IF(mipLevel >= destinationImage->GetMipLevelCount(), "[Vulkan]: Cannot copy from buffer [{0}] to mip level [{1}] of image [{2}] within command buffer [{3}], as image does not contain it!", sourceBuffer->GetName(), mipLevel, destinationImage->GetName(), GetName());
        SR_ERROR_IF(layer >= destinationImage->GetLayerCount(), "[Vulkan]: Cannot copy from buffer [{0}] to layer [{1}] of image [{2}] within command buffer [{3}], as image does not contain it!", sourceBuffer->GetName(), layer, destinationImage->GetName(), GetName());

        // Set copy region
        VkBufferImageCopy copyRegion = { };
        copyRegion.bufferOffset = sourceByteOffset;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;
        copyRegion.imageSubresource.aspectMask = vulkanDestinationImage.GetVulkanAspectFlags();
        copyRegion.imageSubresource.mipLevel = mipLevel;
        copyRegion.imageSubresource.baseArrayLayer = layer;
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageOffset.x = static_cast<int32>(destinationPixelOffset.x);
        copyRegion.imageOffset.y = static_cast<int32>(destinationPixelOffset.y);
        copyRegion.imageOffset.z = 0;
        copyRegion.imageExtent.width = pixelRange.x != 0 ? pixelRange.x : destinationImage->GetWidth() >> mipLevel;
        copyRegion.imageExtent.height = pixelRange.y != 0 ? pixelRange.y : destinationImage->GetHeight() >> mipLevel;
        copyRegion.imageExtent.depth = 1;

        SR_ERROR_IF(destinationPixelOffset.x + copyRegion.imageExtent.width > destinationImage->GetWidth() || destinationPixelOffset.y + copyRegion.imageExtent.height > destinationImage->GetHeight(), "[Vulkan]: Cannot copy from buffer [{0}] pixel range [{1}x{2}], which is offset by another [{3}x{4}] pixels to image [{5}] within command buffer [{6}], as resulting pixel range of a total of [{7}x{8}] pixels exceeds the image's dimensions - [{9}x{10}]!", sourceBuffer->GetName(), copyRegion.imageExtent.width, copyRegion.imageExtent.height, destinationPixelOffset.x, destinationPixelOffset.y, destinationImage->GetName(), GetName(), destinationPixelOffset.x + copyRegion.imageExtent.width, destinationPixelOffset.y + copyRegion.imageExtent.height, destinationImage->GetWidth(), destinationImage->GetHeight());

        // Copy data and change layout
        device.GetFunctionTable().vkCmdCopyBufferToImage(commandBuffer, vulkanSourceBuffer.GetVulkanBuffer(), vulkanDestinationImage.GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
    }

    void VulkanCommandBuffer::GenerateMipMapsForImage(const std::unique_ptr<Image> &image)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot generate mip maps for image [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], within command buffer [{1}]!", image->GetName(), GetName());
        const VulkanImage &vulkanImage = static_cast<VulkanImage&>(*image);

        SR_ERROR_IF(vulkanImage.GetMipLevelCount() <= 1, "[Vulkan]: Cannot generate mip maps for image [{0}], as it has a single mip level only!", vulkanImage.GetName());

        // Set up base pipeline barrier
        VkImageMemoryBarrier pipelineBarrier = { };
        pipelineBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        pipelineBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        pipelineBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        pipelineBarrier.image = vulkanImage.GetVulkanImage();
        pipelineBarrier.subresourceRange.aspectMask = vulkanImage.GetVulkanAspectFlags();
        pipelineBarrier.subresourceRange.levelCount = 1;
        pipelineBarrier.subresourceRange.baseArrayLayer = 0;
        pipelineBarrier.subresourceRange.layerCount = image->GetLayerCount();

        // Set up base blit command
        VkImageBlit blit = { };
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcSubresource.aspectMask = vulkanImage.GetVulkanAspectFlags();
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = vulkanImage.GetLayerCount();
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstSubresource.aspectMask = vulkanImage.GetVulkanAspectFlags();
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = vulkanImage.GetLayerCount();

        for (uint32 i = 1; i < image->GetMipLevelCount(); i++)
        {
            // Prepare former mip level for blitting from it
            pipelineBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            pipelineBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            pipelineBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            pipelineBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            pipelineBarrier.subresourceRange.baseMipLevel = i - 1;
            device.GetFunctionTable().vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &pipelineBarrier);

            // Set offsets for source mip level
            blit.srcOffsets[1] = { static_cast<int32>(image->GetWidth() >> (i - 1)), static_cast<int32>(image->GetHeight() >> (i - 1)), 1 };
            blit.srcSubresource.mipLevel = i - 1;

            // Set offsets for destination mip level
            blit.dstOffsets[1] = { static_cast<int32>(image->GetWidth() >> i), static_cast<int32>(image->GetHeight() >> i), 1 };
            blit.dstSubresource.mipLevel = i;

            // Enqueue blit
            device.GetFunctionTable().vkCmdBlitImage(commandBuffer, vulkanImage.GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vulkanImage.GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
        }
    }

    void VulkanCommandBuffer::BeginRenderPass(const std::unique_ptr<RenderPass> &renderPass, const std::initializer_list<RenderPassBeginAttachment> &attachments)
    {
        SR_ERROR_IF(renderPass->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot begin render pass [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], from command buffer [{1}]!", renderPass->GetName(), GetName());
        const VulkanRenderPass &vulkanRenderPass = static_cast<VulkanRenderPass&>(*renderPass);

        SR_ERROR_IF(attachments.size() != vulkanRenderPass.GetAttachmentCount(), "[Vulkan]: Cannot begin render pass [{0}] from command buffer [{1}] with [{2}] attachments, as it was created to hold [{3}]!", renderPass->GetName(), GetName(), attachments.size(), vulkanRenderPass.GetAttachmentCount());

        // Collect attachment views
        std::vector<VkClearValue> clearValues(attachments.size());
        std::vector<VkImageView> attachmentViews(attachments.size());
        for (uint32 i = 0; i < attachments.size(); i++)
        {
            const RenderPassBeginAttachment &attachment = *(attachments.begin() + i);
            SR_ERROR_IF(attachment.image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot begin render pass [{0}] using image [{1}] as attachment [{2}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), attachment.image->GetName(), i);
            const VulkanImage &vulkanImage = static_cast<VulkanImage&>(*attachment.image);

            // Configure attachment info
            attachmentViews[i] = vulkanImage.GetVulkanImageView();
            if (attachment.image->GetFormat() == ImageFormat::D16_UNorm || attachment.image->GetFormat() == ImageFormat::D32_Float)
            {
                clearValues[i].depthStencil = { 1.0f, 0 };
            }
            else
            {
                clearValues[i].color = { attachment.clearColor.r, attachment.clearColor.g, attachment.clearColor.b, attachment.clearColor.a };
            }
        }

        // Set up dynamic attachments
        VkRenderPassAttachmentBeginInfo attachmentBeginInfo = { };
        attachmentBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO;
        attachmentBeginInfo.attachmentCount = static_cast<uint32>(attachmentViews.size());
        attachmentBeginInfo.pAttachments = attachmentViews.data();

        // Set up begin info
        VkRenderPassBeginInfo beginInfo = { };
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.renderPass = vulkanRenderPass.GetVulkanRenderPass();
        beginInfo.framebuffer = vulkanRenderPass.GetVulkanFramebuffer();
        beginInfo.renderArea.extent.width = attachments.begin()->image->GetWidth();
        beginInfo.renderArea.extent.height = attachments.begin()->image->GetHeight();
        beginInfo.renderArea.offset.x = 0;
        beginInfo.renderArea.offset.y = 0;
        beginInfo.clearValueCount = static_cast<uint32>(clearValues.size());
        beginInfo.pClearValues = clearValues.data();
        beginInfo.pNext = &attachmentBeginInfo;

        // Begin render pass
        device.GetFunctionTable().vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Define viewport
        VkViewport viewport = { };
        viewport.x = 0;
        viewport.y = static_cast<float32>(beginInfo.renderArea.extent.height);
        viewport.width = static_cast<float32>(beginInfo.renderArea.extent.width);
        viewport.height = -static_cast<float32>(beginInfo.renderArea.extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        // Set viewport
        device.GetFunctionTable().vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        // Define scissor
        VkRect2D scissor = { };
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = beginInfo.renderArea.extent.width;
        scissor.extent.height = beginInfo.renderArea.extent.height;

        // Set scissor
        device.GetFunctionTable().vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void VulkanCommandBuffer::BeginNextSubpass(const std::unique_ptr<RenderPass> &renderPass)
    {
        SR_ERROR_IF(renderPass->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot begin next subpass of render pass [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], from command buffer [{1}]!", renderPass->GetName(), GetName());
        device.GetFunctionTable().vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanCommandBuffer::EndRenderPass(const std::unique_ptr<RenderPass> &renderPass)
    {
        SR_ERROR_IF(renderPass->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot end render pass [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], from command buffer [{1}]!", renderPass->GetName(), GetName());
        device.GetFunctionTable().vkCmdEndRenderPass(commandBuffer);
    }

    void VulkanCommandBuffer::BeginGraphicsPipeline(const std::unique_ptr<GraphicsPipeline> &graphicsPipeline)
    {
        SR_ERROR_IF(graphicsPipeline->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot begin graphics graphicsPipeline [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], from command buffer [{1}]!", graphicsPipeline->GetName(), GetName());
        const VulkanGraphicsPipeline &vulkanGraphicsPipeline = static_cast<VulkanGraphicsPipeline&>(*graphicsPipeline);

        device.GetFunctionTable().vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanGraphicsPipeline.GetVulkanPipeline());
        currentGraphicsPipeline = &vulkanGraphicsPipeline;
    }

    void VulkanCommandBuffer::EndGraphicsPipeline(const std::unique_ptr<GraphicsPipeline> &graphicsPipeline)
    {
        SR_ERROR_IF(graphicsPipeline->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot end graphics pipeline [{0}], from command buffer [{1}]!", graphicsPipeline->GetName(), GetName());
        currentGraphicsPipeline = nullptr;
    }

    void VulkanCommandBuffer::BindVertexBuffer(const std::unique_ptr<Buffer> &vertexBuffer, const uint64 byteOffset)
    {
        SR_ERROR_IF(vertexBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind vertex buffer [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], within command buffer [{1}]!", vertexBuffer->GetName(), GetName());
        const VulkanBuffer &vulkanVertexBuffer = static_cast<VulkanBuffer&>(*vertexBuffer);

        SR_ERROR_IF(byteOffset > vertexBuffer->GetMemorySize(), "[Vulkan]: Cannot bind vertex buffer [{0}] within command buffer [{1}] using specified offset of [{2}] bytes, which is outside of the [{3}] bytes size of the size of the buffer!", vertexBuffer->GetName(), GetName(), byteOffset, vertexBuffer->GetMemorySize());

        VkBuffer vkBuffer = vulkanVertexBuffer.GetVulkanBuffer();
        device.GetFunctionTable().vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vkBuffer, &byteOffset);
    }

    void VulkanCommandBuffer::BindIndexBuffer(const std::unique_ptr<Buffer> &indexBuffer, const uint64 byteOffset)
    {
        SR_ERROR_IF(indexBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind index buffer [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], within command buffer [{1}]!", indexBuffer->GetName(), GetName());
        const VulkanBuffer &vulkanIndexBuffer = static_cast<VulkanBuffer&>(*indexBuffer);

        SR_ERROR_IF(byteOffset > indexBuffer->GetMemorySize(), "[Vulkan]: Cannot bind index buffer [{0}] within command buffer [{1}] using specified offset of [{2}] bytes, which is outside of the [{3}] bytes size of the size of the buffer!", indexBuffer->GetName(), GetName(), byteOffset, indexBuffer->GetMemorySize());
        device.GetFunctionTable().vkCmdBindIndexBuffer(commandBuffer, vulkanIndexBuffer.GetVulkanBuffer(), byteOffset, VK_INDEX_TYPE_UINT32); // 32-bit indices are required, due to Metal shaders enforcing it
    }

    void VulkanCommandBuffer::SetScissor(const Vector4UInt &scissor)
    {
        // Set up scissor rect
        VkRect2D scissorRect = { };
        scissorRect.offset.x = static_cast<int32>(scissor.x);
        scissorRect.offset.y = static_cast<int32>(scissor.y);
        scissorRect.extent.width = scissor.z;
        scissorRect.extent.height = scissor.w;

        // Apply scissor
        device.GetFunctionTable().vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
    }

    void VulkanCommandBuffer::Draw(const uint32 vertexCount, const uint32 vertexOffset)
    {
        SR_ERROR_IF(currentGraphicsPipeline == nullptr, "[Vulkan]: Cannot draw if no graphics pipeline is active within command buffer [{0}]!", GetName());

        BindResources();
        device.GetFunctionTable().vkCmdDraw(commandBuffer, vertexCount, 1, vertexOffset, 0);
    }

    void VulkanCommandBuffer::DrawIndexed(const uint32 indexCount, const uint32 indexOffset, const uint32 vertexOffset)
    {
        SR_ERROR_IF(currentGraphicsPipeline == nullptr, "[Vulkan]: Cannot draw indexed if no graphics pipeline is active within command buffer [{0}]!", GetName());

        BindResources();
        device.GetFunctionTable().vkCmdDrawIndexed(commandBuffer, indexCount, 1, indexOffset, static_cast<int32>(vertexOffset), 0);
    }

    void VulkanCommandBuffer::BeginComputePipeline(const std::unique_ptr<ComputePipeline> &computePipeline)
    {
        SR_ERROR_IF(computePipeline->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot begin compute graphicsPipeline [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], from command buffer [{1}]!", computePipeline->GetName(), GetName());
        const VulkanComputePipeline &vulkanComputePipeline = static_cast<VulkanComputePipeline&>(*computePipeline);

        device.GetFunctionTable().vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vulkanComputePipeline.GetVulkanPipeline());
        currentComputePipeline = &vulkanComputePipeline;
    }

    void VulkanCommandBuffer::EndComputePipeline(const std::unique_ptr<ComputePipeline> &computePipeline)
    {
        SR_ERROR_IF(computePipeline->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot end compute pipeline [{0}], from command buffer [{1}]!", computePipeline->GetName(), GetName());
        currentComputePipeline = nullptr;
    }

    void VulkanCommandBuffer::Dispatch(const uint32 xWorkGroupCount, const uint32 yWorkGroupCount, const uint32 zWorkGroupCount)
    {
        SR_ERROR_IF(currentComputePipeline == nullptr, "[Vulkan]: Cannot dispatch if no compute pipeline is active within command buffer [{0}]!", GetName());

        BindResources();
        device.GetFunctionTable().vkCmdDispatch(commandBuffer, xWorkGroupCount, yWorkGroupCount, zWorkGroupCount);
    }

    void VulkanCommandBuffer::PushConstants(const void* data, const uint16 memoryRange, const uint16 byteOffset)
    {
        SR_ERROR_IF(currentGraphicsPipeline == nullptr && currentComputePipeline == nullptr, "[Vulkan]: Cannot push constants if no pipeline is active within command buffer [{0}]!", GetName());
        const VulkanPipelineLayout &currentPipelineLayout = currentComputePipeline != nullptr ? currentComputePipeline->GetLayout() : currentGraphicsPipeline->GetLayout();

        SR_ERROR_IF(memoryRange > currentPipelineLayout.GetPushConstantSize(), "[Vulkan]: Cannot push [{0}] bytes of push constant data within command buffer [{1}], as specified memory range is bigger than specified in the current pipeline's layout, which is [{2}] bytes!", memoryRange, GetName(), currentPipelineLayout.GetPushConstantSize());
        device.GetFunctionTable().vkCmdPushConstants(commandBuffer, currentPipelineLayout.GetVulkanPipelineLayout(), VK_SHADER_STAGE_ALL, byteOffset, memoryRange, data);
    }

    void VulkanCommandBuffer::BindBuffer(const uint32 binding, const std::unique_ptr<Buffer> &buffer, const uint32 arrayIndex, uint64 memoryRange, const uint64 byteOffset)
    {
        SR_ERROR_IF(buffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind buffer [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], to binding [{1}] within command buffer [{2}]!", buffer->GetName(), binding, GetName());
        const VulkanBuffer &vulkanBuffer = static_cast<VulkanBuffer&>(*buffer);

        memoryRange = memoryRange != 0 ? memoryRange : buffer->GetMemorySize();
        SR_ERROR_IF(byteOffset + memoryRange > buffer->GetMemorySize(), "[Vulkan]: Cannot bind [{0}] bytes (offset by another [{1}] bytes) from buffer [{2}] within command buffer [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryRange, byteOffset, buffer->GetName(), GetName(), byteOffset + memoryRange, buffer->GetMemorySize());

        pushDescriptorSet.BindBuffer(binding, vulkanBuffer, arrayIndex, memoryRange, byteOffset);
        resourcesBound = false;
    }

    void VulkanCommandBuffer::BindImage(const uint32 binding, const std::unique_ptr<Image> &image, const uint32 arrayIndex)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind image [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], to binding [{1}] within command buffer [{2}]!", image->GetName(), binding, GetName());
        const VulkanImage &vulkanImage = static_cast<VulkanImage&>(*image);

        pushDescriptorSet.BindImage(binding, vulkanImage, nullptr, VK_IMAGE_LAYOUT_GENERAL, arrayIndex);
        resourcesBound = false;
    }

    void VulkanCommandBuffer::BindImage(const uint32 binding, const std::unique_ptr<Image> &image, const std::unique_ptr<Sampler> &sampler, const uint32 arrayIndex)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind image [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], to binding [{1}] within command buffer [{2}]!", image->GetName(), binding, GetName());
        const VulkanImage &vulkanImage = static_cast<VulkanImage&>(*image);

        SR_ERROR_IF(sampler->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind image [{0}] using sampler [{1}], whose graphics API differs from [GraphicsAPI::Vulkan], to binding [{2}] within command buffer [{3}]!", image->GetName(), sampler->GetName(), binding, GetName());
        const VulkanSampler &vulkanSampler = static_cast<VulkanSampler&>(*sampler);

        pushDescriptorSet.BindImage(binding, vulkanImage, &vulkanSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, arrayIndex);
        resourcesBound = false;
    }

    void VulkanCommandBuffer::BeginDebugRegion(const std::string &regionName, const ColorRGBA32 &color)
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif
        if (!device.IsExtensionLoaded(VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) return;

        // Set up marker info
        VkDebugMarkerMarkerInfoEXT markerInfo = { };
        markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
        markerInfo.pMarkerName = regionName.c_str();
        std::memcpy(markerInfo.color, &color, sizeof(VkDebugMarkerMarkerInfoEXT::color));

        // Bind marker
        device.GetFunctionTable().vkCmdDebugMarkerBeginEXT(commandBuffer, &markerInfo);
    }

    void VulkanCommandBuffer::InsertDebugMarker(const std::string &markerName, const ColorRGBA32 &color)
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif
        if (!device.IsExtensionLoaded(VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) return;

        // Set up marker info
        VkDebugMarkerMarkerInfoEXT markerInfo = { };
        markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
        markerInfo.pMarkerName = markerName.c_str();
        std::memcpy(markerInfo.color, &color, sizeof(VkDebugMarkerMarkerInfoEXT::color));

        // Bind marker
        device.GetFunctionTable().vkCmdDebugMarkerInsertEXT(commandBuffer, &markerInfo);
    }

    void VulkanCommandBuffer::EndDebugRegion()
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif
        device.GetFunctionTable().vkCmdDebugMarkerEndEXT(commandBuffer);
    }

    /* --- DESTRUCTOR --- */

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        device.GetFunctionTable().vkFreeCommandBuffers(device.GetLogicalDevice(), commandPool, 1, &commandBuffer);
        device.GetFunctionTable().vkDestroyCommandPool(device.GetLogicalDevice(), commandPool, nullptr);

        queuedBuffers = std::queue<std::unique_ptr<Buffer>>();
        queuedImages = std::queue<std::unique_ptr<Image>>();
    }

    /* --- PRIVATE METHODS --- */

    void VulkanCommandBuffer::BindResources()
    {
        if (resourcesBound) return;

        const VkPipelineBindPoint bindPoint = currentComputePipeline != nullptr ? VK_PIPELINE_BIND_POINT_COMPUTE : VK_PIPELINE_BIND_POINT_GRAPHICS;
        const VulkanPipelineLayout &currentPipelineLayout = currentComputePipeline != nullptr ? currentComputePipeline->GetLayout() : currentGraphicsPipeline->GetLayout();

        if (!pushDescriptorSet.GetWriteDescriptorSets().empty()) device.GetFunctionTable().vkCmdPushDescriptorSetKHR(commandBuffer, bindPoint, currentPipelineLayout.GetVulkanPipelineLayout(), 0, pushDescriptorSet.GetWriteDescriptorSets().size(), pushDescriptorSet.GetWriteDescriptorSets().data());
        resourcesBound = true;
    }

    /* --- CONVERSIONS --- */

    VkAccessFlags VulkanCommandBuffer::BufferCommandUsageToVkAccessFlags(const BufferCommandUsage bufferCommandUsage)
    {
        switch (bufferCommandUsage)
        {
            case BufferCommandUsage::None:              return VK_ACCESS_NONE;
            case BufferCommandUsage::MemoryRead:        return VK_ACCESS_TRANSFER_READ_BIT;
            case BufferCommandUsage::MemoryWrite:       return VK_ACCESS_TRANSFER_WRITE_BIT;
            case BufferCommandUsage::VertexRead:        return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
            case BufferCommandUsage::IndexRead:         return VK_ACCESS_INDEX_READ_BIT;
            case BufferCommandUsage::GraphicsRead:
            case BufferCommandUsage::ComputeRead:       return VK_ACCESS_SHADER_READ_BIT;
            case BufferCommandUsage::GraphicsWrite:
            case BufferCommandUsage::ComputeWrite:      return VK_ACCESS_SHADER_WRITE_BIT;
        }

        return VK_ACCESS_NONE;
    }

    VkPipelineStageFlags VulkanCommandBuffer::BufferCommandUsageToVkPipelineStageFlags(const BufferCommandUsage bufferCommandUsage)
    {
        switch (bufferCommandUsage)
        {
            case BufferCommandUsage::None:             return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            case BufferCommandUsage::VertexRead:
            case BufferCommandUsage::IndexRead:        return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
            case BufferCommandUsage::MemoryRead:
            case BufferCommandUsage::MemoryWrite:      return VK_PIPELINE_STAGE_TRANSFER_BIT;
            case BufferCommandUsage::GraphicsRead:
            case BufferCommandUsage::GraphicsWrite:    return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            case BufferCommandUsage::ComputeRead:
            case BufferCommandUsage::ComputeWrite:     return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        }

        return VK_PIPELINE_STAGE_NONE;
    }

    VkImageLayout VulkanCommandBuffer::ImageCommandUsageToVkLayout(const ImageCommandUsage imageCommandUsage)
    {
        switch (imageCommandUsage)
        {
            case ImageCommandUsage::None:               return VK_IMAGE_LAYOUT_UNDEFINED;
            case ImageCommandUsage::MemoryRead:         return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            case ImageCommandUsage::MemoryWrite:        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case ImageCommandUsage::ColorRead:
            case ImageCommandUsage::ColorWrite:         return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case ImageCommandUsage::DepthRead:          return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            case ImageCommandUsage::DepthWrite:         return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            case ImageCommandUsage::GraphicsRead:
            case ImageCommandUsage::ComputeRead:        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            case ImageCommandUsage::GraphicsWrite:
            case ImageCommandUsage::ComputeWrite:       return VK_IMAGE_LAYOUT_GENERAL;
            case ImageCommandUsage::Present:            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }

        return VK_IMAGE_LAYOUT_UNDEFINED;
    }

    VkAccessFlags VulkanCommandBuffer::ImageCommandUsageToVkAccessFlags(const ImageCommandUsage imageCommandUsage)
    {
        switch (imageCommandUsage)
        {
            case ImageCommandUsage::None:              return VK_ACCESS_NONE;
            case ImageCommandUsage::MemoryRead:        return VK_ACCESS_MEMORY_READ_BIT;
            case ImageCommandUsage::ColorRead:         return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
            case ImageCommandUsage::DepthRead:         return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            case ImageCommandUsage::GraphicsRead:
            case ImageCommandUsage::ComputeRead:       return VK_ACCESS_SHADER_READ_BIT;

            case ImageCommandUsage::MemoryWrite:       return VK_ACCESS_MEMORY_WRITE_BIT;
            case ImageCommandUsage::ColorWrite:        return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            case ImageCommandUsage::DepthWrite:        return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            case ImageCommandUsage::GraphicsWrite:
            case ImageCommandUsage::ComputeWrite:      return VK_ACCESS_SHADER_WRITE_BIT;

            case ImageCommandUsage::Present:           return VK_ACCESS_NONE;
        }

        return VK_ACCESS_NONE;
    }

    VkPipelineStageFlags VulkanCommandBuffer::ImageCommandUsageToVkPipelineStageFlags(const ImageCommandUsage imageCommandUsage)
    {
        switch (imageCommandUsage)
        {
            case ImageCommandUsage::None:              return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

            case ImageCommandUsage::MemoryRead:
            case ImageCommandUsage::MemoryWrite:       return VK_PIPELINE_STAGE_TRANSFER_BIT;

            case ImageCommandUsage::ColorRead:
            case ImageCommandUsage::ColorWrite:        return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

            case ImageCommandUsage::DepthRead:         return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            case ImageCommandUsage::DepthWrite:        return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

            case ImageCommandUsage::GraphicsRead:
            case ImageCommandUsage::GraphicsWrite:     return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

            case ImageCommandUsage::ComputeRead:
            case ImageCommandUsage::ComputeWrite:      return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

            case ImageCommandUsage::Present:           return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        }

        return VK_PIPELINE_STAGE_NONE;
    }

}