//
// Created by Nikolay Kanchevski on 11.12.23.
//

#include "VulkanCommandBuffer.h"

#include "VulkanImage.h"
#include "VulkanBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanErrorHandler.h"
#include "VulkanResourceTable.h"
#include "VulkanComputePipeline.h"
#include "VulkanGraphicsPipeline.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */

    VkAccessFlags BufferCommandUsageToVkAccessFlags(const BufferCommandUsage bufferCommandUsage) noexcept
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

    VkPipelineStageFlags BufferCommandUsageToVkPipelineStageFlags(const BufferCommandUsage bufferCommandUsage) noexcept
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

    VkImageLayout ImageCommandUsageToVkLayout(const ImageCommandUsage imageCommandUsage) noexcept
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

    VkAccessFlags ImageCommandUsageToVkAccessFlags(const ImageCommandUsage imageCommandUsage) noexcept
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

    VkPipelineStageFlags ImageCommandUsageToVkPipelineStageFlags(const ImageCommandUsage imageCommandUsage) noexcept
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

    /* --- CONSTRUCTORS --- */

    VulkanCommandBuffer::VulkanCommandBuffer(const VulkanQueue& givenQueue, const CommandBufferCreateInfo& createInfo)
        : CommandBuffer(createInfo), queue(&givenQueue), name(createInfo.name)
    {
        // Set up pool create info
        const VkCommandPoolCreateInfo commandPoolCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queue->GetFamily()
        };

        // Create command pool
        VkResult result = queue->GetDevice().GetFunctionTable().vkCreateCommandPool(queue->GetDevice().GetVulkanDevice(), &commandPoolCreateInfo, nullptr, &commandPool);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create command buffer [{0}], as creation of command pool failed", name));
        queue->GetDevice().SetResourceName(commandPool, VK_OBJECT_TYPE_COMMAND_POOL, SR_FORMAT("Command pool of command buffer [{0}]", name));

        // Set up allocate info
        const VkCommandBufferAllocateInfo allocateInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        // Allocate command buffer
        result = queue->GetDevice().GetFunctionTable().vkAllocateCommandBuffers(queue->GetDevice().GetVulkanDevice(), &allocateInfo, &commandBuffer);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create command buffer [{0}]", name));
        queue->GetDevice().SetResourceName(commandBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER, name);
    }

    /* --- POLLING METHODS --- */

    void VulkanCommandBuffer::Begin()
    {
        // Free queued resources
        queuedBuffersForDestruction = { };
        queuedImagesForDestruction = { };

        // Reset command buffer
        queue->GetDevice().GetFunctionTable().vkResetCommandPool(queue->GetDevice().GetVulkanDevice(), commandPool, 0);

        // Set up begin info
        constexpr VkCommandBufferBeginInfo beginInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        };

        // Begin command buffer
        const VkResult result = queue->GetDevice().GetFunctionTable().vkBeginCommandBuffer(commandBuffer, &beginInfo);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not begin command buffer [{0}]", name));

        // Get new code
        completionSemaphoreSignalValue = queue->GetDevice().GetNewSemaphoreSignalValue();
        queue->GetDevice().SetResourceName(commandBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER, SR_FORMAT("{0} - {1}", name, completionSemaphoreSignalValue));

        operations = QueueOperations::None;
    }

    void VulkanCommandBuffer::End()
    {
        SR_THROW_IF(currentRenderPass != nullptr, InvalidOperationError(SR_FORMAT("Cannot end command buffer [{0}], as current render pass [{1}] has not been ended", name, currentRenderPass->GetName())));

        // End command buffer
        const VkResult result = queue->GetDevice().GetFunctionTable().vkEndCommandBuffer(commandBuffer);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not end command buffer [{0}]", name));

        currentResourceTable = nullptr;

        currentVertexBuffer = nullptr;
        currentIndexBuffer = nullptr;
    }

    void VulkanCommandBuffer::SynchronizeBufferUsage(const Buffer& buffer, const BufferSynchronizeInfo& synchronizeInfo)
    {
        SR_THROW_IF(buffer.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot synchronize usage of buffer [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", buffer.GetName(), name)));
        const VulkanBuffer& vulkanBuffer = static_cast<const VulkanBuffer&>(buffer);

        CommandBuffer::SynchronizeBufferUsage(vulkanBuffer, synchronizeInfo);

        // Set up pipeline barrier
        const VkBufferMemoryBarrier pipelineBarrier
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .srcAccessMask = BufferCommandUsageToVkAccessFlags(synchronizeInfo.previousUsage),
            .dstAccessMask = BufferCommandUsageToVkAccessFlags(synchronizeInfo.nextUsage),
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = vulkanBuffer.GetVulkanBuffer(),
            .offset = synchronizeInfo.offset,
            .size = synchronizeInfo.memorySize
        };

        // Bind barrier
        queue->GetDevice().GetFunctionTable().vkCmdPipelineBarrier(commandBuffer, BufferCommandUsageToVkPipelineStageFlags(synchronizeInfo.previousUsage), BufferCommandUsageToVkPipelineStageFlags(synchronizeInfo.nextUsage), 0, 0, nullptr, 1, &pipelineBarrier, 0, nullptr);
    }

    void VulkanCommandBuffer::SynchronizeImageUsage(const Image& image, const ImageSynchronizeInfo& synchronizeInfo)
    {
        SR_THROW_IF(image.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot synchronize usage of image [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", image.GetName(), name)));
        const VulkanImage& vulkanImage = static_cast<const VulkanImage&>(image);

        CommandBuffer::SynchronizeImageUsage(vulkanImage, synchronizeInfo);

        // Set up pipeline barrier
        const VkImageMemoryBarrier pipelineBarrier
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = ImageCommandUsageToVkAccessFlags(synchronizeInfo.previousUsage),
            .dstAccessMask = ImageCommandUsageToVkAccessFlags(synchronizeInfo.nextUsage),
            .oldLayout = ImageCommandUsageToVkLayout(synchronizeInfo.previousUsage),
            .newLayout = ImageCommandUsageToVkLayout(synchronizeInfo.nextUsage),
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = vulkanImage.GetVulkanImage(),
            .subresourceRange = {
                .aspectMask = vulkanImage.GetVulkanAspectFlags(),
                .baseMipLevel = synchronizeInfo.baseLevel,
                .levelCount = synchronizeInfo.levelCount,
                .baseArrayLayer = synchronizeInfo.baseLayer,
                .layerCount = synchronizeInfo.layerCount
            }
        };

        // Bind barrier
        queue->GetDevice().GetFunctionTable().vkCmdPipelineBarrier(commandBuffer, ImageCommandUsageToVkPipelineStageFlags(synchronizeInfo.previousUsage), ImageCommandUsageToVkPipelineStageFlags(synchronizeInfo.nextUsage), 0, 0, nullptr, 0, nullptr, 1, &pipelineBarrier);
    }

    void VulkanCommandBuffer::CopyBufferToBuffer(const Buffer& sourceBuffer, const Buffer& destinationBuffer, const BufferToBufferCopyInfo& copyInfo)
    {
        SR_THROW_IF(sourceBuffer.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot copy from buffer [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", sourceBuffer.GetName(), name)));
        const VulkanBuffer& vulkanSourceBuffer = static_cast<const VulkanBuffer&>(sourceBuffer);

        SR_THROW_IF(destinationBuffer.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot copy to buffer [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", destinationBuffer.GetName(), name)));
        const VulkanBuffer& vulkanDestinationBuffer = static_cast<const VulkanBuffer&>(destinationBuffer);

        CommandBuffer::CopyBufferToBuffer(vulkanSourceBuffer, vulkanDestinationBuffer, copyInfo);

        const VkBufferCopy copyRegion
        {
            .srcOffset = copyInfo.sourceOffset,
            .dstOffset = copyInfo.destinationOffset,
            .size = copyInfo.memorySize
        };

        // Record copy
        queue->GetDevice().GetFunctionTable().vkCmdCopyBuffer(commandBuffer, vulkanSourceBuffer.GetVulkanBuffer(), vulkanDestinationBuffer.GetVulkanBuffer(), 1, &copyRegion);
        operations |= QueueOperations::Transfer;
    }

    void VulkanCommandBuffer::CopyBufferToImage(const Buffer& sourceBuffer, const Image& destinationImage, const BufferToImageCopyInfo& copyInfo)
    {
        SR_THROW_IF(sourceBuffer.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot copy from buffer [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", sourceBuffer.GetName(), name)));
        const VulkanBuffer& vulkanSourceBuffer = static_cast<const VulkanBuffer&>(sourceBuffer);

        SR_THROW_IF(destinationImage.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot copy to image [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", destinationImage.GetName(), name)));
        const VulkanImage& vulkanDestinationImage = static_cast<const VulkanImage&>(destinationImage);

        CommandBuffer::CopyBufferToImage(vulkanSourceBuffer, vulkanDestinationImage, copyInfo);

        // Set copy region
        const VkBufferImageCopy copyRegion
        {
            .bufferOffset = copyInfo.sourceOffset,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {
                .aspectMask = vulkanDestinationImage.GetVulkanAspectFlags(),
                .mipLevel = copyInfo.level,
                .baseArrayLayer = copyInfo.layer,
                .layerCount = 1
            },
            .imageOffset = {
                .x = static_cast<int32>(copyInfo.destinationPixelOffset.x),
                .y = static_cast<int32>(copyInfo.destinationPixelOffset.y),
                .z = static_cast<int32>(copyInfo.destinationPixelOffset.z)
            },
            .imageExtent = {
                .width = copyInfo.pixelRange.x,
                .height = copyInfo.pixelRange.y,
                .depth = copyInfo.pixelRange.z
            }
        };

        // Copy data and change layout
        queue->GetDevice().GetFunctionTable().vkCmdCopyBufferToImage(commandBuffer, vulkanSourceBuffer.GetVulkanBuffer(), vulkanDestinationImage.GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
        operations |= QueueOperations::Transfer;
    }

    void VulkanCommandBuffer::GenerateMipMapsForImage(const Image& image)
    {
        SR_THROW_IF(image.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot generate mip maps for image [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", image.GetName(), name)));
        const VulkanImage& vulkanImage = static_cast<const VulkanImage&>(image);

        CommandBuffer::GenerateMipMapsForImage(vulkanImage);

        if (vulkanImage.GetLevelCount() == 1)
        {
            SR_WARNING("Did not generate any mip maps for image [{0}], as it only contains a single level.", vulkanImage.GetName());
            return;
        }

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
                .layerCount = image.GetLayerCount()
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

        for (uint32 i = 1; i < image.GetLevelCount(); i++)
        {
            // Prepare former level for blitting from it
            pipelineBarrier.subresourceRange.baseMipLevel = i - 1;
            queue->GetDevice().GetFunctionTable().vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &pipelineBarrier);

            // Set offsets for source level
            blit.srcOffsets[1] = { static_cast<int32>(image.GetWidth() >> (i - 1)), static_cast<int32>(image.GetHeight() >> (i - 1)), 1 };
            blit.srcSubresource.mipLevel = i - 1;

            // Set offsets for destination level
            blit.dstOffsets[1] = { static_cast<int32>(image.GetWidth() >> i), static_cast<int32>(image.GetHeight() >> i), 1 };
            blit.dstSubresource.mipLevel = i;

            // Enqueue blit
            queue->GetDevice().GetFunctionTable().vkCmdBlitImage(commandBuffer, vulkanImage.GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vulkanImage.GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
        }
        operations |= QueueOperations::Transfer;
    }

    void VulkanCommandBuffer::BindResourceTable(const ResourceTable& resourceTable)
    {
        SR_THROW_IF(resourceTable.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot bind resource table [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", resourceTable.GetName(), name)));
        const VulkanResourceTable& vulkanResourceTable = static_cast<const VulkanResourceTable&>(resourceTable);

        CommandBuffer::BindResourceTable(vulkanResourceTable);
        SR_THROW_IF(currentRenderPass != nullptr, InvalidOperationError(SR_FORMAT("Cannot bind resource table [{0}], as current render pass [{1}] has not been ended, and resource table binding must be done prior to beginning a render pass", vulkanResourceTable.GetName(), currentRenderPass->GetName())));
        SR_THROW_IF(currentComputePipeline != nullptr, InvalidOperationError(SR_FORMAT("Cannot bind resource table [{0}], as current compute pipeline [{1}] has not been ended, and resource table binding must be done prior to beginning a compute pipeline", vulkanResourceTable.GetName(), currentComputePipeline->GetName())));

        currentResourceTable = &vulkanResourceTable;
    }

    void VulkanCommandBuffer::PushConstants(const void* memory, const uint8 sourceOffset, const uint8 memorySize)
    {
        CommandBuffer::PushConstants(memory, sourceOffset, memorySize);
        SR_THROW_IF(currentGraphicsPipeline == nullptr && currentComputePipeline == nullptr, InvalidOperationError(SR_FORMAT("Cannot push constants within command buffer [{0}] if no pipeline has been begun", name)));

        if (currentGraphicsPipeline != nullptr) queue->GetDevice().GetFunctionTable().vkCmdPushConstants(commandBuffer, currentGraphicsPipeline->GetVulkanPipelineLayout(), VK_SHADER_STAGE_ALL, 0, memorySize, reinterpret_cast<const uint8*>(memory) + sourceOffset);
        if (currentComputePipeline != nullptr) queue->GetDevice().GetFunctionTable().vkCmdPushConstants(commandBuffer, currentComputePipeline->GetVulkanPipelineLayout(), VK_SHADER_STAGE_ALL, 0, memorySize, reinterpret_cast<const uint8*>(memory) + sourceOffset);
    }

    void VulkanCommandBuffer::BeginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer)
    {
        SR_THROW_IF(renderPass.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot begin render pass [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", renderPass.GetName(), name)));
        const VulkanRenderPass& vulkanRenderPass = static_cast<const VulkanRenderPass&>(renderPass);

        SR_THROW_IF(framebuffer.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot begin render pass [{0}] within command buffer [{1}], as specified specified framebuffer [{2}]'s backend differs from [RenderingBackendType::Vulkan]", vulkanRenderPass.GetName(), framebuffer.GetName(), name)));
        const VulkanFramebuffer& vulkanFramebuffer = static_cast<const VulkanFramebuffer&>(framebuffer);

        CommandBuffer::BeginRenderPass(vulkanRenderPass, framebuffer);
        SR_THROW_IF(currentRenderPass != nullptr, InvalidOperationError(SR_FORMAT("Cannot begin render pass [{0}] within command buffer [{1}], as current render pass [{2}] has not been ended", vulkanRenderPass.GetName(), name, currentRenderPass->GetName())));

        // Set up begin info
        const VkRenderPassBeginInfo renderPassBeginInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = vulkanRenderPass.GetVulkanRenderPass(),
            .framebuffer = vulkanFramebuffer.GetVulkanFramebuffer(),
            .renderArea = {
                .offset = {
                    .x = 0,
                    .y = 0
                },
                .extent = {
                    .width = vulkanFramebuffer.GetWidth(),
                    .height = vulkanFramebuffer.GetHeight()
                }
            },
            .clearValueCount = static_cast<uint32>(vulkanFramebuffer.GetVulkanClearValues().size()),
            .pClearValues = vulkanFramebuffer.GetVulkanClearValues().data()
        };

        // Begin render pass
        queue->GetDevice().GetFunctionTable().vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Define viewport
        const VkViewport viewport
        {
            .x = 0,
            .y = static_cast<float32>(renderPassBeginInfo.renderArea.extent.height),
            .width = static_cast<float32>(renderPassBeginInfo.renderArea.extent.width),
            .height = -static_cast<float32>(renderPassBeginInfo.renderArea.extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        // Set viewport
        queue->GetDevice().GetFunctionTable().vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        // Define scissor
        const VkRect2D scissor
        {
            .offset = {
                .x = 0,
                .y = 0
            },
            .extent = {
                .width = renderPassBeginInfo.renderArea.extent.width,
                .height = renderPassBeginInfo.renderArea.extent.height
            }
        };

        // Set scissor
        queue->GetDevice().GetFunctionTable().vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        currentSubpass = 0;
        currentRenderPass = &vulkanRenderPass;

        operations |= QueueOperations::Graphics;
    }

    void VulkanCommandBuffer::BeginNextSubpass()
    {
        CommandBuffer::BeginNextSubpass();
        SR_THROW_IF(currentRenderPass == nullptr, InvalidOperationError(SR_FORMAT("Cannot begin next subpass of render pass within command buffer [{0}], as none has been begun", name)));
        SR_THROW_IF(currentSubpass + 1 >= currentRenderPass->GetSubpassCount(), ValueOutOfRangeError(SR_FORMAT("Cannot begin next subpass [{0}] of render pass [{1}] within command buffer [{2}]", currentSubpass + 1, currentRenderPass->GetName(), name), currentSubpass, uint32(0), currentRenderPass->GetSubpassCount() - 1));

        queue->GetDevice().GetFunctionTable().vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
        currentRenderPass++;
    }

    void VulkanCommandBuffer::EndRenderPass()
    {
        CommandBuffer::EndRenderPass();
        SR_THROW_IF(currentRenderPass == nullptr, InvalidOperationError(SR_FORMAT("Cannot end render pass within command buffer [{0}], as none has been begun yet", name)));

        queue->GetDevice().GetFunctionTable().vkCmdEndRenderPass(commandBuffer);

        currentRenderPass = nullptr;
        currentSubpass = 0;
    }

    void VulkanCommandBuffer::BeginGraphicsPipeline(const GraphicsPipeline& graphicsPipeline)
    {
        SR_THROW_IF(graphicsPipeline.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot begin graphics pipeline [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", graphicsPipeline.GetName(), name)));
        const VulkanGraphicsPipeline& vulkanGraphicsPipeline = static_cast<const VulkanGraphicsPipeline&>(graphicsPipeline);

        CommandBuffer::BeginGraphicsPipeline(graphicsPipeline);
        SR_THROW_IF(currentRenderPass == nullptr, InvalidOperationError(SR_FORMAT("Cannot begin graphics pipeline [{0}] within command buffer [{1}], as no render pass has been begun", vulkanGraphicsPipeline.GetName(), name)));
        SR_THROW_IF(currentGraphicsPipeline != nullptr && currentGraphicsPipeline != &vulkanGraphicsPipeline, InvalidOperationError(SR_FORMAT("Cannot begin graphics pipeline [{0}] within command buffer [{1}], as current graphics pipeline [{2}] has not been ended", vulkanGraphicsPipeline.GetName(), name, currentGraphicsPipeline->GetName())));

        if (currentResourceTable != nullptr)
        {
            VkDescriptorSet descriptorSet = currentResourceTable->GetDescriptorSet();
            queue->GetDevice().GetFunctionTable().vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanGraphicsPipeline.GetVulkanPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);
        }

        queue->GetDevice().GetFunctionTable().vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanGraphicsPipeline.GetVulkanPipeline());
        currentGraphicsPipeline = &vulkanGraphicsPipeline;
    }

    void VulkanCommandBuffer::EndGraphicsPipeline()
    {
        CommandBuffer::EndGraphicsPipeline();

        SR_THROW_IF(currentGraphicsPipeline == nullptr, InvalidOperationError(SR_FORMAT("Cannot end graphics pipeline within command buffer [{0}], as none been begun yet", name)));
        currentGraphicsPipeline = nullptr;
    }

    void VulkanCommandBuffer::BindVertexBuffer(const Buffer& vertexBuffer, const uint64 offset)
    {
        SR_THROW_IF(vertexBuffer.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot bind vertex buffer [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", vertexBuffer.GetName(), name)));
        const VulkanBuffer& vulkanVertexBuffer = static_cast<const VulkanBuffer&>(vertexBuffer);

        CommandBuffer::BindVertexBuffer(vulkanVertexBuffer, offset);

        const std::array<VkDeviceSize, 1> offsets = { offset };
        VkBuffer vkBuffer = vulkanVertexBuffer.GetVulkanBuffer();
        queue->GetDevice().GetFunctionTable().vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vkBuffer, offsets.data());

        currentVertexBuffer = &vulkanVertexBuffer;
    }

    void VulkanCommandBuffer::BindIndexBuffer(const Buffer& indexBuffer, const uint64 offset)
    {
        SR_THROW_IF(indexBuffer.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot bind index buffer [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", indexBuffer.GetName(), name)));
        const VulkanBuffer& vulkanIndexBuffer = static_cast<const VulkanBuffer&>(indexBuffer);

        CommandBuffer::BindIndexBuffer(vulkanIndexBuffer, offset);
        queue->GetDevice().GetFunctionTable().vkCmdBindIndexBuffer(commandBuffer, vulkanIndexBuffer.GetVulkanBuffer(), offset, VK_INDEX_TYPE_UINT32); // 32-bit indices are required, due to Vulkan shaders enforcing it

        currentIndexBuffer = &vulkanIndexBuffer;
    }

    void VulkanCommandBuffer::SetScissor(const Vector4UInt scissor)
    {
        CommandBuffer::SetScissor(scissor);
        SR_THROW_IF(currentRenderPass == nullptr, InvalidOperationError(SR_FORMAT("Cannot set scissor within command buffer [{0}], as no render pass has been begun", name)));

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
        queue->GetDevice().GetFunctionTable().vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
    }

    void VulkanCommandBuffer::Draw(const uint32 vertexCount, const uint64 vertexOffset)
    {
        CommandBuffer::Draw(vertexCount, vertexOffset);
        SR_THROW_IF(currentGraphicsPipeline == nullptr, InvalidOperationError(SR_FORMAT("Cannot draw within command buffer [{0}], as no graphics pipeline has been begun", name)));

        const uint64 vertexBufferOffset = static_cast<uint64>(vertexOffset) * currentGraphicsPipeline->GetVertexStride();
        SR_THROW_IF(currentVertexBuffer != nullptr && vertexBufferOffset >= currentVertexBuffer->GetMemorySize(), ValueOutOfRangeError(SR_FORMAT("Cannot draw from invalid vertex index in vertex buffer [{0}] within command buffer [{1}]", currentVertexBuffer->GetName(), name), vertexBufferOffset, uint64(0), currentVertexBuffer->GetMemorySize()));

        queue->GetDevice().GetFunctionTable().vkCmdDraw(commandBuffer, vertexCount, 1, vertexOffset, 0);
    }

    void VulkanCommandBuffer::DrawIndexed(const uint32 indexCount, const uint64 indexOffset, const uint64 vertexOffset)
    {
        CommandBuffer::DrawIndexed(indexCount, indexOffset, vertexOffset);
        SR_THROW_IF(currentRenderPass == nullptr, InvalidOperationError(SR_FORMAT("Cannot draw indexed within command buffer [{0}], as no graphics pipeline has been begun", name)));
        SR_THROW_IF(currentIndexBuffer == nullptr, InvalidOperationError(SR_FORMAT("Cannot draw indexed within command buffer [{0}], as no index buffer has been bound", name)));

        const uint64 indexBufferOffset = indexOffset * sizeof(uint32);
        SR_THROW_IF(currentIndexBuffer != nullptr && indexBufferOffset >= currentIndexBuffer->GetMemorySize(), ValueOutOfRangeError(SR_FORMAT("Cannot draw from invalid index offset in index buffer [{0}] within command buffer [{1}]", currentIndexBuffer->GetName(), name), indexBufferOffset, uint64(0), currentVertexBuffer->GetMemorySize()));

        const uint64 vertexBufferOffset = static_cast<uint64>(vertexOffset) * currentGraphicsPipeline->GetVertexStride();
        SR_THROW_IF(currentVertexBuffer != nullptr && vertexBufferOffset >= currentVertexBuffer->GetMemorySize(), ValueOutOfRangeError(SR_FORMAT("Cannot draw indexed from invalid vertex offset in vertex buffer [{0}] within command buffer [{1}]", currentVertexBuffer->GetName(), name), vertexBufferOffset, uint64(0), currentVertexBuffer->GetMemorySize()));

        queue->GetDevice().GetFunctionTable().vkCmdDrawIndexed(commandBuffer, indexCount, 1, indexOffset, static_cast<int32>(vertexOffset), 0);
    }

    void VulkanCommandBuffer::BeginComputePipeline(const ComputePipeline& computePipeline)
    {
        SR_THROW_IF(computePipeline.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot begin compute pipeline [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", computePipeline.GetName(), name)));
        const VulkanComputePipeline& vulkanComputePipeline = static_cast<const VulkanComputePipeline&>(computePipeline);

        CommandBuffer::BeginComputePipeline(vulkanComputePipeline);
        SR_THROW_IF(currentComputePipeline == nullptr, InvalidOperationError(SR_FORMAT("Cannot begin compute pipeline [{0}] within command buffer [{1}], as no render pass has been begun", vulkanComputePipeline.GetName(), name)));
        SR_THROW_IF(currentComputePipeline != nullptr && currentComputePipeline != &vulkanComputePipeline, InvalidOperationError(SR_FORMAT("Cannot begin compute pipeline [{0}] within command buffer [{1}], as current compute pipeline [{2}] has not been ended", vulkanComputePipeline.GetName(), name, currentComputePipeline->GetName())));

        if (currentResourceTable != nullptr)
        {
            VkDescriptorSet descriptorSet = currentResourceTable->GetDescriptorSet();
            queue->GetDevice().GetFunctionTable().vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vulkanComputePipeline.GetVulkanPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);
        }

        queue->GetDevice().GetFunctionTable().vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vulkanComputePipeline.GetVulkanPipeline());
        currentComputePipeline = &vulkanComputePipeline;

        operations |= QueueOperations::Compute;
    }

    void VulkanCommandBuffer::EndComputePipeline()
    {
        CommandBuffer::EndComputePipeline();

        SR_THROW_IF(currentComputePipeline == nullptr, InvalidOperationError(SR_FORMAT("Cannot end compute pipeline within command buffer [{0}], as none has been begun yet", name)));
        currentComputePipeline = nullptr;
    }

    void VulkanCommandBuffer::Dispatch(const Vector3UInt workGroupSize)
    {
        CommandBuffer::Dispatch(workGroupSize);

        SR_THROW_IF(currentComputePipeline == nullptr, InvalidOperationError(SR_FORMAT("Cannot dispatch within command buffer [{0}], as no compute pipeline has been begun", name)));
        SR_THROW_IF(workGroupSize.x > queue->GetDevice().GetLimits().maxWorkGroupSize.x, ValueOutOfRangeError(SR_FORMAT("Cannot dispatch command buffer [{0}], as specified work group's horizontal axis is greater than the max work group horizontal axis of device [{1}] - use Device::GetLimits() to query limits", name, queue->GetDevice().GetName()), workGroupSize.x, 1U, queue->GetDevice().GetLimits().maxWorkGroupSize.x));
        SR_THROW_IF(workGroupSize.y > queue->GetDevice().GetLimits().maxWorkGroupSize.y, ValueOutOfRangeError(SR_FORMAT("Cannot dispatch command buffer [{0}], as specified work group's horizontal axis is greater than the max work group vertical axis of device [{1}] - use Device::GetLimits() to query limits", name, queue->GetDevice().GetName()), workGroupSize.x, 1U, queue->GetDevice().GetLimits().maxWorkGroupSize.y));
        SR_THROW_IF(workGroupSize.z > queue->GetDevice().GetLimits().maxWorkGroupSize.z, ValueOutOfRangeError(SR_FORMAT("Cannot dispatch command buffer [{0}], as specified work group's horizontal axis is greater than the max work group depth axis of device [{1}] - use Device::GetLimits() to query limits", name, queue->GetDevice().GetName()), workGroupSize.x, 1U, queue->GetDevice().GetLimits().maxWorkGroupSize.z));

        queue->GetDevice().GetFunctionTable().vkCmdDispatch(commandBuffer, workGroupSize.x, workGroupSize.y, workGroupSize.z);
    }

    void VulkanCommandBuffer::BeginDebugRegion(const std::string_view regionName, const Color32 color)
    {
        CommandBuffer::BeginDebugRegion(regionName, color);
        SR_THROW_IF(debugRegionBegan, InvalidOperationError(SR_FORMAT("Cannot begin debug region [{0}] within command buffer [{1}], as current debug region has not been ended", regionName, name)));

        if (!queue->GetDevice().IsExtensionLoaded(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
        {
            SR_WARNING("Cannot begin debug region [{0}] within command buffer [{1}], as device [{2}] does not support this feature.", regionName, name, queue->GetDevice().GetName());
            return;
        }

        // Set up marker info
        const VkDebugMarkerMarkerInfoEXT markerInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
            .pMarkerName = regionName.data(),
            .color = { color.r, color.g, color.b, color.a }
        };

        debugRegionBegan = true;
        queue->GetDevice().GetFunctionTable().vkCmdDebugMarkerBeginEXT(commandBuffer, &markerInfo);
    }

    void VulkanCommandBuffer::InsertDebugMarker(const std::string_view markerName, const Color32 color)
    {
        CommandBuffer::InsertDebugMarker(markerName, color);

        if (!queue->GetDevice().IsExtensionLoaded(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
        {
            SR_WARNING("Cannot insert debug marker [{0}] within command buffer [{1}], as device [{2}] does not support this feature.", markerName, name, queue->GetDevice().GetName());
            return;
        }

        // Set up marker info
        const VkDebugMarkerMarkerInfoEXT markerInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
            .pMarkerName = markerName.data(),
            .color = { color.r, color.g, color.b, color.a }
        };

        // Bind marker
        queue->GetDevice().GetFunctionTable().vkCmdDebugMarkerInsertEXT(commandBuffer, &markerInfo);
    }

    void VulkanCommandBuffer::EndDebugRegion()
    {
        CommandBuffer::EndDebugRegion();
        SR_THROW_IF(debugRegionBegan, InvalidOperationError(SR_FORMAT("Cannot end debug region within command buffer [{0}], as one must have been begun first", name)));

        if (!queue->GetDevice().IsExtensionLoaded(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
        {
            SR_WARNING("Cannot end debug region within command buffer [{0}], as device [{1}] does not support this feature.", name, queue->GetDevice().GetName());
            return;
        }

        queue->GetDevice().GetFunctionTable().vkCmdDebugMarkerEndEXT(commandBuffer);
        debugRegionBegan = false;
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

    VulkanCommandBuffer::~VulkanCommandBuffer() noexcept
    {
        queue->GetDevice().GetFunctionTable().vkFreeCommandBuffers(queue->GetDevice().GetVulkanDevice(), commandPool, 1, &commandBuffer);
        queue->GetDevice().GetFunctionTable().vkDestroyCommandPool(queue->GetDevice().GetVulkanDevice(), commandPool, nullptr);

        queuedBuffersForDestruction = { };
        queuedImagesForDestruction = { };
    }

}