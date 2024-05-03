//
// Created by Nikolay Kanchevski on 11.12.23.
//

#include "VulkanCommandBuffer.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanResourceTable.h"

#include "VulkanRenderPass.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanComputePipeline.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice &device, const CommandBufferCreateInfo &createInfo)
        : CommandBuffer(createInfo), VulkanResource(createInfo.name), device(device)
    {
        // Set up pool create info
        const VkCommandPoolCreateInfo commandPoolCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = device.GetGeneralQueueFamily()
        };

        // Create command pool
        VkResult result = device.GetFunctionTable().vkCreateCommandPool(device.GetLogicalDevice(), &commandPoolCreateInfo, nullptr, &commandPool);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create command pool for command buffer [{0}]! Error code: {1}.", GetName(), static_cast<int32>(result));
        device.SetObjectName(commandPool, VK_OBJECT_TYPE_COMMAND_POOL, "Command pool of command buffer [" + std::string(GetName()) + "]");

        // Set up allocate info
        const VkCommandBufferAllocateInfo allocateInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        // Allocate command buffer
        result = device.GetFunctionTable().vkAllocateCommandBuffers(device.GetLogicalDevice(), &allocateInfo, &commandBuffer);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create command buffer [{0}]! Error code: {1}.", GetName(), static_cast<int32>(result));
        device.SetObjectName(commandBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER, GetName());
    }

    /* --- POLLING METHODS --- */

    void VulkanCommandBuffer::Begin()
    {
        // Free queued resources
        queuedBuffersForDestruction = { };
        queuedImagesForDestruction = { };

        // Reset command buffer
        device.GetFunctionTable().vkResetCommandPool(device.GetLogicalDevice(), commandPool, 0);

        // Set up begin info
        const VkCommandBufferBeginInfo beginInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        };

        // Begin command buffer
        const VkResult result = device.GetFunctionTable().vkBeginCommandBuffer(commandBuffer, &beginInfo);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not begin command buffer [{0}]! Error code: {1}.", GetName(), static_cast<int32>(result));

        // Get new code
        completionSignalValue = device.GetNewSignalValue();
    }

    void VulkanCommandBuffer::End()
    {
        // End command buffer
        const VkResult result = device.GetFunctionTable().vkEndCommandBuffer(commandBuffer);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not end command buffer [{0}]! Error code: {1}.", GetName(), static_cast<int32>(result));
        currentResourceTableDescriptorSet = VK_NULL_HANDLE;
    }

    void VulkanCommandBuffer::SynchronizeBufferUsage(const std::unique_ptr<Buffer> &buffer, const BufferCommandUsage previousUsage, const BufferCommandUsage nextUsage, const uint64 memorySize, const uint64 byteOffset)
    {
        SR_ERROR_IF(buffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not synchronize usage of buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", buffer->GetName(), GetName());
        const VulkanBuffer &vulkanBuffer = static_cast<const VulkanBuffer&>(*buffer);

        SR_ERROR_IF(nextUsage == BufferCommandUsage::None, "[Vulkan]: Cannot synchronize buffer [{0}], as specified next usage must not be BufferCommandUsage::None!", buffer->GetName());

        // Set up pipeline barrier
        const VkBufferMemoryBarrier pipelineBarrier
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .srcAccessMask = BufferCommandUsageToVkAccessFlags(previousUsage),
            .dstAccessMask = BufferCommandUsageToVkAccessFlags(nextUsage),
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = vulkanBuffer.GetVulkanBuffer(),
            .offset = byteOffset,
            .size = memorySize != 0 ? memorySize : buffer->GetMemorySize()
        };

        SR_ERROR_IF(byteOffset + pipelineBarrier.size > buffer->GetMemorySize(), "[Vulkan]: Cannot synchronize [{0}] bytes, which are offset by another [{1}], within buffer [{2}] from command buffer [{3}], as specified memory range exceeds that of the buffer - [{4}]!", pipelineBarrier.size, byteOffset, buffer->GetMemorySize(), GetName(), buffer->GetMemorySize());

        // Bind barrier
        device.GetFunctionTable().vkCmdPipelineBarrier(commandBuffer, BufferCommandUsageToVkPipelineStageFlags(previousUsage), BufferCommandUsageToVkPipelineStageFlags(nextUsage), 0, 0, nullptr, 1, &pipelineBarrier, 0, nullptr);
    }

    void VulkanCommandBuffer::SynchronizeImageUsage(const std::unique_ptr<Image> &image, const ImageCommandUsage previousUsage, const ImageCommandUsage nextUsage, const uint32 baseLevel, const uint32 levelCount, const uint32 baseLayer, uint32 layerCount)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not synchronize usage of image [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", image->GetName(), GetName());
        const VulkanImage &vulkanImage = static_cast<const VulkanImage&>(*image);

        SR_ERROR_IF(nextUsage == ImageCommandUsage::None, "[Vulkan]: Cannot synchronize image [{0}], as specified next usage must not be ImageCommandUsage::None!", image->GetName());

        SR_ERROR_IF(baseLevel >= image->GetLevelCount(), "[Vulkan]: Cannot synchronize level [{0}] of image [{1}] within command buffer [{2}], as it does not have it!", baseLevel, image->GetName(), GetName());
        SR_ERROR_IF(baseLayer >= image->GetLayerCount(), "[Vulkan]: Cannot synchronize layer [{0}] of image [{1}] within command buffer [{2}], as it does not have it!", baseLayer, image->GetName(), GetName());

        // Set up pipeline barrier
        const VkImageMemoryBarrier pipelineBarrier
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = ImageCommandUsageToVkAccessFlags(previousUsage),
            .dstAccessMask = ImageCommandUsageToVkAccessFlags(nextUsage),
            .oldLayout = ImageCommandUsageToVkLayout(previousUsage),
            .newLayout = ImageCommandUsageToVkLayout(nextUsage),
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = vulkanImage.GetVulkanImage(),
            .subresourceRange = {
                .aspectMask = vulkanImage.GetVulkanAspectFlags(),
                .baseMipLevel = baseLevel,
                .levelCount = levelCount != 0 ? levelCount : image->GetLevelCount() - baseLevel,
                .baseArrayLayer = baseLayer,
                .layerCount = layerCount != 0 ? layerCount : image->GetLayerCount() - baseLayer
            }
        };

        SR_ERROR_IF(baseLevel + pipelineBarrier.subresourceRange.levelCount > image->GetLevelCount(), "[Vulkan]: Cannot synchronize levels [{0}-{1}] of image [{2}] within command buffer [{3}], as they exceed image's level count - [{4}]!", baseLevel, baseLevel + levelCount - 1, image->GetName(), GetName(), image->GetLevelCount());
        SR_ERROR_IF(baseLayer + pipelineBarrier.subresourceRange.layerCount > image->GetLayerCount(), "[Vulkan]: Cannot synchronize layers [{0}-{1}] of image [{2}] within command buffer [{3}], as they exceed image's layer count - [{4}]!", baseLayer, baseLayer + layerCount - 1, image->GetName(), GetName(), image->GetLayerCount());

        // Bind barrier
        device.GetFunctionTable().vkCmdPipelineBarrier(commandBuffer, ImageCommandUsageToVkPipelineStageFlags(previousUsage), ImageCommandUsageToVkPipelineStageFlags(nextUsage), 0, 0, nullptr, 0, nullptr, 1, &pipelineBarrier);
    }

    void VulkanCommandBuffer::CopyBufferToBuffer(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Buffer> &destinationBuffer, const uint64 memoryRange, const uint64 sourceByteOffset, const uint64 destinationByteOffset)
    {
        SR_ERROR_IF(sourceBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not copy from buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", sourceBuffer->GetName(), GetName());
        const VulkanBuffer &vulkanSourceBuffer = static_cast<const VulkanBuffer&>(*sourceBuffer);

        SR_ERROR_IF(destinationBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not copy to buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", destinationBuffer->GetName(), GetName());
        const VulkanBuffer &vulkanDestinationBuffer = static_cast<const VulkanBuffer&>(*destinationBuffer);

        const VkBufferCopy copyRegion
        {
            .srcOffset = sourceByteOffset,
            .dstOffset = destinationByteOffset,
            .size = memoryRange != 0 ? memoryRange : sourceBuffer->GetMemorySize(),
        };

        SR_ERROR_IF(sourceByteOffset + copyRegion.size > sourceBuffer->GetMemorySize(), "[Vulkan]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, from buffer [{2}] within command buffer [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryRange, sourceByteOffset, sourceBuffer->GetName(), GetName(), sourceByteOffset + memoryRange, sourceBuffer->GetMemorySize());
        SR_ERROR_IF(destinationByteOffset + copyRegion.size > destinationBuffer->GetMemorySize(), "[Vulkan]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, to buffer [{2}] within command buffer [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryRange, destinationByteOffset, destinationBuffer->GetName(), GetName(), destinationByteOffset + memoryRange, destinationBuffer->GetMemorySize());

        // Record copy
        device.GetFunctionTable().vkCmdCopyBuffer(commandBuffer, vulkanSourceBuffer.GetVulkanBuffer(), vulkanDestinationBuffer.GetVulkanBuffer(), 1, &copyRegion);
    }

    void VulkanCommandBuffer::CopyBufferToImage(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Image> &destinationImage, const uint32 level, const uint32 layer, const Vector2UInt &pixelRange, const uint64 sourceByteOffset, const Vector2UInt &destinationPixelOffset)
    {
        SR_ERROR_IF(sourceBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not copy from buffer [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], to image [{1}] within command buffer [{2}]!", sourceBuffer->GetName(), destinationImage->GetName(), GetName());
        const VulkanBuffer &vulkanSourceBuffer = static_cast<const VulkanBuffer&>(*sourceBuffer);

        SR_ERROR_IF(destinationImage->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not from buffer [{0}] to image [{1}], graphics API differs from [GraphicsAPI::Vulkan], within command buffer [{2}]!", sourceBuffer->GetName(), destinationImage->GetName(), GetName());
        const VulkanImage &vulkanDestinationImage = static_cast<const VulkanImage&>(*destinationImage);

        SR_ERROR_IF(level >= destinationImage->GetLevelCount(), "[Vulkan]: Cannot copy from buffer [{0}] to level [{1}] of image [{2}] within command buffer [{3}], as image does not contain it!", sourceBuffer->GetName(), level, destinationImage->GetName(), GetName());
        SR_ERROR_IF(layer >= destinationImage->GetLayerCount(), "[Vulkan]: Cannot copy from buffer [{0}] to layer [{1}] of image [{2}] within command buffer [{3}], as image does not contain it!", sourceBuffer->GetName(), layer, destinationImage->GetName(), GetName());

        // Set copy region
        const VkBufferImageCopy copyRegion
        {
            .bufferOffset = sourceByteOffset,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {
                .aspectMask = vulkanDestinationImage.GetVulkanAspectFlags(),
                .mipLevel = level,
                .baseArrayLayer = layer,
                .layerCount = 1
            },
            .imageOffset = {
                .x = static_cast<int32>(destinationPixelOffset.x),
                .y = static_cast<int32>(destinationPixelOffset.y),
                .z = 0
            },
            .imageExtent = {
                .width = pixelRange.x != 0 ? pixelRange.x : destinationImage->GetWidth() >> level,
                .height = pixelRange.y != 0 ? pixelRange.y : destinationImage->GetHeight() >> level,
                .depth = 1
            }
        };
        SR_ERROR_IF(destinationPixelOffset.x + copyRegion.imageExtent.width > destinationImage->GetWidth() || destinationPixelOffset.y + copyRegion.imageExtent.height > destinationImage->GetHeight(), "[Vulkan]: Cannot copy from buffer [{0}] pixel range [{1}x{2}], which is offset by another [{3}x{4}] pixels to image [{5}] within command buffer [{6}], as resulting pixel range of a total of [{7}x{8}] pixels exceeds the image's dimensions - [{9}x{10}]!", sourceBuffer->GetName(), copyRegion.imageExtent.width, copyRegion.imageExtent.height, destinationPixelOffset.x, destinationPixelOffset.y, destinationImage->GetName(), GetName(), destinationPixelOffset.x + copyRegion.imageExtent.width, destinationPixelOffset.y + copyRegion.imageExtent.height, destinationImage->GetWidth(), destinationImage->GetHeight());

        // Copy data and change layout
        device.GetFunctionTable().vkCmdCopyBufferToImage(commandBuffer, vulkanSourceBuffer.GetVulkanBuffer(), vulkanDestinationImage.GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
    }

    void VulkanCommandBuffer::GenerateMipMapsForImage(const std::unique_ptr<Image> &image)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot generate mip maps for image [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], within command buffer [{1}]!", image->GetName(), GetName());
        const VulkanImage &vulkanImage = static_cast<const VulkanImage&>(*image);

        SR_ERROR_IF(vulkanImage.GetLevelCount() <= 1, "[Vulkan]: Cannot generate mip maps for image [{0}], as it has a single mip level only!", vulkanImage.GetName());

        // Set up base pipeline barrier
        VkImageMemoryBarrier pipelineBarrier
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = vulkanImage.GetVulkanImage(),
            .subresourceRange = {
                .aspectMask = vulkanImage.GetVulkanAspectFlags(),
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = image->GetLayerCount()
            },
        };

        // Set up base blit command
        VkImageBlit blit
        {
            .srcSubresource = {
                .aspectMask = vulkanImage.GetVulkanAspectFlags(),
                .baseArrayLayer = 0,
                .layerCount = vulkanImage.GetLayerCount()
            },
            .srcOffsets = { { .x = 0, .y = 0, .z = 0 } },
            .dstSubresource = {
                .aspectMask = vulkanImage.GetVulkanAspectFlags(),
                .baseArrayLayer = 0,
                .layerCount = vulkanImage.GetLayerCount()
            },
            .dstOffsets = { { .x = 0, .y = 0, .z = 0 } }
        };

        for (uint32 i = 1; i < image->GetLevelCount(); i++)
        {
            // Prepare former level for blitting from it
            pipelineBarrier.subresourceRange.baseMipLevel = i - 1;
            device.GetFunctionTable().vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &pipelineBarrier);

            // Set offsets for source level
            blit.srcOffsets[1] = { static_cast<int32>(image->GetWidth() >> (i - 1)), static_cast<int32>(image->GetHeight() >> (i - 1)), 1 };
            blit.srcSubresource.mipLevel = i - 1;

            // Set offsets for destination level
            blit.dstOffsets[1] = { static_cast<int32>(image->GetWidth() >> i), static_cast<int32>(image->GetHeight() >> i), 1 };
            blit.dstSubresource.mipLevel = i;

            // Enqueue blit
            device.GetFunctionTable().vkCmdBlitImage(commandBuffer, vulkanImage.GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vulkanImage.GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
        }
    }

    void VulkanCommandBuffer::BindResourceTable(const std::unique_ptr<ResourceTable> &resourceTable)
    {
        SR_ERROR_IF(resourceTable->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind resource table [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], within command buffer [{1}]!", resourceTable->GetName(), GetName());
        const VulkanResourceTable &vulkanResourceTable = static_cast<const VulkanResourceTable&>(*resourceTable);
        currentResourceTableDescriptorSet = vulkanResourceTable.GetDescriptorSet();
    }

    void VulkanCommandBuffer::PushConstants(const void* data, const uint16 memoryRange, uint16 byteOffset)
    {
        SR_ERROR_IF(currentBindPoint == VK_PIPELINE_BIND_POINT_MAX_ENUM, "[Vulkan]: Cannot push constants within command buffer [{0}] if no pipelines are bound!", GetName());
        SR_ERROR_IF(memoryRange > 128, "[Vulkan]: Cannot push constants of size bigger than 128 bytes within command buffer [{0}]!", GetName());
        device.GetFunctionTable().vkCmdPushConstants(commandBuffer, currentPipelineLayout, VK_SHADER_STAGE_ALL, 0, memoryRange, reinterpret_cast<const char*>(data) + byteOffset);
    }

    void VulkanCommandBuffer::BeginRenderPass(const std::unique_ptr<RenderPass> &renderPass, const std::span<const RenderPassBeginAttachment> &attachments)
    {
        SR_ERROR_IF(renderPass->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot begin render pass [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], from command buffer [{1}]!", renderPass->GetName(), GetName());
        const VulkanRenderPass &vulkanRenderPass = static_cast<const VulkanRenderPass&>(*renderPass);

        SR_ERROR_IF(attachments.size() != vulkanRenderPass.GetAttachmentCount(), "[Vulkan]: Cannot begin render pass [{0}] from command buffer [{1}] with [{2}] attachments, as it was created to hold [{3}]!", renderPass->GetName(), GetName(), attachments.size(), vulkanRenderPass.GetAttachmentCount());

        std::vector<VkClearValue> clearValues(attachments.size());
        std::vector<VkImageView> attachmentViews(attachments.size());

        // Collect attachment views
        for (uint32 i = 0; i < attachments.size(); i++)
        {
            const RenderPassBeginAttachment &attachment = attachments[i];

            SR_ERROR_IF(attachment.outputImage == nullptr, "[Vulkan]: Cannot begin render pass [{0}], as referenced output image must not be a null pointer!", GetName());
            SR_ERROR_IF(attachment.outputImage->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot begin render pass [{0}] using image [{1}] as attachment [{2}]'s output image, as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), attachment.outputImage->GetName(), i);
            const VulkanImage &vulkanImage = static_cast<const VulkanImage&>(*attachment.outputImage);

            attachmentViews[i] = vulkanImage.GetVulkanImageView();
            switch (vulkanImage.GetFormat())
            {
                case ImageFormat::D16_UNorm:
                case ImageFormat::D32_Float:
                {
                    clearValues[i].depthStencil = { attachment.clearValue.r, 0 };
                    break;
                }
                default:
                {
                    clearValues[i].color = { attachment.clearValue.r, attachment.clearValue.g, attachment.clearValue.b, attachment.clearValue.a };
                    break;
                }
            }

            if (attachment.resolverImage != nullptr)
            {
                SR_ERROR_IF(attachment.resolverImage->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot begin render pass [{0}] using image [{1}] as attachment [{2}]'s resolver image, as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), attachment.resolverImage->GetName(), i);
                const VulkanImage &vulkanResolverImage = static_cast<const VulkanImage&>(*attachment.resolverImage);

                attachmentViews.emplace_back(vulkanResolverImage.GetVulkanImageView());
                switch (vulkanResolverImage.GetFormat())
                {
                    case ImageFormat::D16_UNorm:
                    case ImageFormat::D32_Float:
                    {
                        clearValues.emplace_back().depthStencil = { attachment.clearValue.r, 0 };
                        break;
                    }
                    default:
                    {
                        clearValues.emplace_back().color = { attachment.clearValue.r, attachment.clearValue.g, attachment.clearValue.b, attachment.clearValue.a };
                        break;
                    }
                }
            }
        }

        // Set up dynamic attachments
        const VkRenderPassAttachmentBeginInfo attachmentBeginInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO,
            .attachmentCount = static_cast<uint32>(attachmentViews.size()),
            .pAttachments = attachmentViews.data()
        };

        // Set up begin info
        const VkRenderPassBeginInfo beginInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = &attachmentBeginInfo,
            .renderPass = vulkanRenderPass.GetVulkanRenderPass(),
            .framebuffer = vulkanRenderPass.GetVulkanFramebuffer(),
            .renderArea = {
                .offset = {
                    .x = 0,
                    .y = 0
                },
                .extent = {
                    .width = attachments.begin()->outputImage->GetWidth(),
                    .height = attachments.begin()->outputImage->GetHeight()
                }
            },
            .clearValueCount = static_cast<uint32>(clearValues.size()),
            .pClearValues = clearValues.data()
        };

        // Begin render pass
        device.GetFunctionTable().vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Define viewport
        const VkViewport viewport
        {
            .x = 0,
            .y = static_cast<float32>(beginInfo.renderArea.extent.height),
            .width = static_cast<float32>(beginInfo.renderArea.extent.width),
            .height = -static_cast<float32>(beginInfo.renderArea.extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        // Set viewport
        device.GetFunctionTable().vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        // Define scissor
        const VkRect2D scissor
        {
            .offset = {
                .x = 0,
                .y = 0
            },
            .extent = {
                .width = beginInfo.renderArea.extent.width,
                .height = beginInfo.renderArea.extent.height
            }
        };

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
        const VulkanGraphicsPipeline &vulkanGraphicsPipeline = static_cast<const VulkanGraphicsPipeline&>(*graphicsPipeline);

        currentBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        currentPipelineLayout = vulkanGraphicsPipeline.GetVulkanPipelineLayout();

        if (currentResourceTableDescriptorSet != VK_NULL_HANDLE) device.GetFunctionTable().vkCmdBindDescriptorSets(commandBuffer, currentBindPoint, currentPipelineLayout, 0, 1, &currentResourceTableDescriptorSet, 0, nullptr);
        device.GetFunctionTable().vkCmdBindPipeline(commandBuffer, currentBindPoint, vulkanGraphicsPipeline.GetVulkanPipeline());
    }

    void VulkanCommandBuffer::EndGraphicsPipeline(const std::unique_ptr<GraphicsPipeline> &graphicsPipeline)
    {
        SR_ERROR_IF(graphicsPipeline->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot end graphics pipeline [{0}], from command buffer [{1}]!", graphicsPipeline->GetName(), GetName());
        currentBindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
    }

    void VulkanCommandBuffer::BindVertexBuffer(const std::unique_ptr<Buffer> &vertexBuffer, const uint64 byteOffset)
    {
        SR_ERROR_IF(vertexBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind vertex buffer [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], within command buffer [{1}]!", vertexBuffer->GetName(), GetName());
        const VulkanBuffer &vulkanVertexBuffer = static_cast<const VulkanBuffer&>(*vertexBuffer);

        SR_ERROR_IF(byteOffset > vertexBuffer->GetMemorySize(), "[Vulkan]: Cannot bind vertex buffer [{0}] within command buffer [{1}] using specified offset of [{2}] bytes, which is not within a valid range of the [{3}] bytes the buffer holds!", vertexBuffer->GetName(), GetName(), byteOffset, vertexBuffer->GetMemorySize());

        VkBuffer vkBuffer = vulkanVertexBuffer.GetVulkanBuffer();
        device.GetFunctionTable().vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vkBuffer, &byteOffset);
    }

    void VulkanCommandBuffer::BindIndexBuffer(const std::unique_ptr<Buffer> &indexBuffer, const uint64 byteOffset)
    {
        SR_ERROR_IF(indexBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind index buffer [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], within command buffer [{1}]!", indexBuffer->GetName(), GetName());
        const VulkanBuffer &vulkanIndexBuffer = static_cast<const VulkanBuffer&>(*indexBuffer);

        SR_ERROR_IF(byteOffset > indexBuffer->GetMemorySize(), "[Vulkan]: Cannot bind index buffer [{0}] within command buffer [{1}] using specified offset of [{2}] bytes, which is not within a valid range of the [{3}] bytes the buffer holds!", indexBuffer->GetName(), GetName(), byteOffset, indexBuffer->GetMemorySize());
        device.GetFunctionTable().vkCmdBindIndexBuffer(commandBuffer, vulkanIndexBuffer.GetVulkanBuffer(), byteOffset, VK_INDEX_TYPE_UINT32); // 32-bit indices are required, due to Metal shaders enforcing it
    }

    void VulkanCommandBuffer::SetScissor(const Vector4UInt &scissor)
    {
        // Set up scissor rect
        const VkRect2D scissorRect
        {
            .offset = {
                .x = static_cast<int32>(scissor.x),
                .y = static_cast<int32>(scissor.y)
            },
            .extent = {
                .width = scissor.z,
                .height = scissor.w
            }
        };

        // Apply scissor
        device.GetFunctionTable().vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
    }

    void VulkanCommandBuffer::Draw(const uint32 vertexCount, const uint32 vertexOffset)
    {
        SR_ERROR_IF(currentBindPoint != VK_PIPELINE_BIND_POINT_GRAPHICS, "[Vulkan]: Cannot draw if no graphics pipeline is active within command buffer [{0}]!", GetName());
        device.GetFunctionTable().vkCmdDraw(commandBuffer, vertexCount, 1, vertexOffset, 0);
    }

    void VulkanCommandBuffer::DrawIndexed(const uint32 indexCount, const uint32 indexOffset, const uint32 vertexOffset)
    {
        SR_ERROR_IF(currentBindPoint != VK_PIPELINE_BIND_POINT_GRAPHICS, "[Vulkan]: Cannot draw indexed if no graphics pipeline is active within command buffer [{0}]!", GetName());
        device.GetFunctionTable().vkCmdDrawIndexed(commandBuffer, indexCount, 1, indexOffset, static_cast<int32>(vertexOffset), 0);
    }

    void VulkanCommandBuffer::BeginComputePipeline(const std::unique_ptr<ComputePipeline> &computePipeline)
    {
        SR_ERROR_IF(computePipeline->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot begin compute graphicsPipeline [{0}], whose graphics API differs from [GraphicsAPI::Vulkan], from command buffer [{1}]!", computePipeline->GetName(), GetName());
        const VulkanComputePipeline &vulkanComputePipeline = static_cast<const VulkanComputePipeline&>(*computePipeline);

        currentBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
        currentPipelineLayout = vulkanComputePipeline.GetVulkanPipelineLayout();

        if (currentResourceTableDescriptorSet != VK_NULL_HANDLE) device.GetFunctionTable().vkCmdBindDescriptorSets(commandBuffer, currentBindPoint, currentPipelineLayout, 0, 1, &currentResourceTableDescriptorSet, 0, nullptr);
        device.GetFunctionTable().vkCmdBindPipeline(commandBuffer, currentBindPoint, vulkanComputePipeline.GetVulkanPipeline());
    }

    void VulkanCommandBuffer::EndComputePipeline(const std::unique_ptr<ComputePipeline> &computePipeline)
    {
        SR_ERROR_IF(computePipeline->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot end compute pipeline [{0}], from command buffer [{1}]!", computePipeline->GetName(), GetName());
        currentBindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
    }

    void VulkanCommandBuffer::Dispatch(const uint32 xWorkGroupCount, const uint32 yWorkGroupCount, const uint32 zWorkGroupCount)
    {
        SR_ERROR_IF(currentBindPoint != VK_PIPELINE_BIND_POINT_COMPUTE, "[Vulkan]: Cannot dispatch if no compute pipeline is active within command buffer [{0}]!", GetName());
        device.GetFunctionTable().vkCmdDispatch(commandBuffer, xWorkGroupCount, yWorkGroupCount, zWorkGroupCount);
    }

    void VulkanCommandBuffer::BeginDebugRegion(const std::string_view regionName, const ColorRGBA32 &color)
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif
        if (!device.IsExtensionLoaded(VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) return;

        // Set up marker info
        const VkDebugMarkerMarkerInfoEXT markerInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
            .pMarkerName = regionName.data(),
            .color = { color.r, color.g, color.b, color.a }
        };

        // Bind marker
        device.GetFunctionTable().vkCmdDebugMarkerBeginEXT(commandBuffer, &markerInfo);
    }

    void VulkanCommandBuffer::InsertDebugMarker(const std::string_view markerName, const ColorRGBA32 &color)
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif
        if (!device.IsExtensionLoaded(VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) return;

        // Set up marker info
        const VkDebugMarkerMarkerInfoEXT markerInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
            .pMarkerName = markerName.data(),
            .color = { color.r, color.g, color.b, color.a }
        };

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

    std::unique_ptr<Buffer>& VulkanCommandBuffer::QueueBufferForDestruction(std::unique_ptr<Buffer> &&buffer)
    {
        return queuedBuffersForDestruction.emplace(std::move(buffer));
    }

    std::unique_ptr<Image>& VulkanCommandBuffer::QueueImageForDestruction(std::unique_ptr<Image> &&image)
    {
        return queuedImagesForDestruction.emplace(std::move(image));
    }

    /* --- DESTRUCTOR --- */

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        device.GetFunctionTable().vkFreeCommandBuffers(device.GetLogicalDevice(), commandPool, 1, &commandBuffer);
        device.GetFunctionTable().vkDestroyCommandPool(device.GetLogicalDevice(), commandPool, nullptr);

        queuedBuffersForDestruction = { };
        queuedImagesForDestruction = { };
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