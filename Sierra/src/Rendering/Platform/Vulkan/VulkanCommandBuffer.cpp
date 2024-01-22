//
// Created by Nikolay Kanchevski on 11.12.23.
//

#include "VulkanCommandBuffer.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice &device, const CommandBufferCreateInfo &createInfo)
        : CommandBuffer(createInfo), VulkanResource(createInfo.name), device(device)
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
        device.SetObjectName(commandPool, VK_OBJECT_TYPE_COMMAND_POOL, "Command pool of [" + GetName() + "]");
    }

    /* --- POLLING METHODS --- */

    void VulkanCommandBuffer::Begin()
    {
        // Reset command buffer
        device.GetFunctionTable().vkResetCommandPool(device.GetLogicalDevice(), commandPool, 0);

        // Set up begin info
        VkCommandBufferBeginInfo beginInfo = { };
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        // Begin command buffer
        device.GetFunctionTable().vkBeginCommandBuffer(commandBuffer, &beginInfo);

        // Get new code
        signalValue = device.GetNewSignalValue();
    }

    void VulkanCommandBuffer::End()
    {
        // End command buffer
        const VkResult result = device.GetFunctionTable().vkEndCommandBuffer(commandBuffer);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not end command buffer [{0}]! Error code: {1}.", GetName(), result);
    }

    void VulkanCommandBuffer::SynchronizeBufferUsage(const std::unique_ptr<Buffer> &buffer, const BufferCommandUsage previousUsage, const BufferCommandUsage nextUsage, const uint64 memorySize, const uint64 offset)
    {
        SR_ERROR_IF(buffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not synchronize usage of buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), buffer->GetName());
        const VulkanBuffer &vulkanBuffer = static_cast<VulkanBuffer&>(*buffer);

        // Set up pipeline barrier
        VkBufferMemoryBarrier pipelineBarrier = { };
        pipelineBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        pipelineBarrier.srcAccessMask = BufferCommandUsageToVkAccessFlags(previousUsage);
        pipelineBarrier.dstAccessMask = BufferCommandUsageToVkAccessFlags(nextUsage);
        pipelineBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        pipelineBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        pipelineBarrier.buffer = vulkanBuffer.GetVulkanBuffer();
        pipelineBarrier.offset = offset;
        pipelineBarrier.size = memorySize != 0 ? memorySize : buffer->GetMemorySize();

        // Bind barrier
        device.GetFunctionTable().vkCmdPipelineBarrier(commandBuffer, BufferCommandUsageToVkPipelineStageFlags(previousUsage), BufferCommandUsageToVkPipelineStageFlags(nextUsage), 0, 0, nullptr, 1, &pipelineBarrier, 0, nullptr);
    }

    void VulkanCommandBuffer::SynchronizeImageUsage(const std::unique_ptr<Image> &image, const ImageCommandUsage previousUsage, const ImageCommandUsage nextUsage, const uint32 baseMipLevel, const uint32 mipLevelCount, const uint32 baseLayer, const uint32 layerCount)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not synchronize usage of image [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), image->GetName());
        const VulkanImage &vulkanImage = static_cast<VulkanImage&>(*image);

        // If layout has not been changed yet, insert it as UNDEFINED
        auto iterator = imageLayouts.find(vulkanImage.GetVulkanImage());
        if (iterator == imageLayouts.end()) iterator = imageLayouts.emplace(vulkanImage.GetVulkanImage(), VK_IMAGE_LAYOUT_UNDEFINED).first;

        // Set up pipeline barrier
        VkImageMemoryBarrier pipelineBarrier = { };
        pipelineBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        pipelineBarrier.srcAccessMask = ImageCommandUsageToVkAccessFlags(previousUsage);
        pipelineBarrier.dstAccessMask = ImageCommandUsageToVkAccessFlags(nextUsage);
        pipelineBarrier.oldLayout = iterator->second;
        pipelineBarrier.newLayout = ImageCommandUsageToVkLayout(nextUsage);
        pipelineBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        pipelineBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        pipelineBarrier.image = vulkanImage.GetVulkanImage();
        pipelineBarrier.subresourceRange.aspectMask = vulkanImage.GetVulkanAspectFlags();
        pipelineBarrier.subresourceRange.baseMipLevel = baseMipLevel;
        pipelineBarrier.subresourceRange.levelCount = mipLevelCount != 0 ? mipLevelCount : vulkanImage.GetMipLevelCount();
        pipelineBarrier.subresourceRange.baseArrayLayer = baseLayer;
        pipelineBarrier.subresourceRange.layerCount = layerCount != 0 ? layerCount : vulkanImage.GetLayerCount();

        // Bind barrier
        device.GetFunctionTable().vkCmdPipelineBarrier(commandBuffer, ImageCommandUsageToVkPipelineStageFlags(previousUsage), ImageCommandUsageToVkPipelineStageFlags(nextUsage), 0, 0, nullptr, 0, nullptr, 1, &pipelineBarrier);
        imageLayouts[vulkanImage.GetVulkanImage()] = pipelineBarrier.newLayout;
    }

    void VulkanCommandBuffer::BeginDebugRegion(const std::string &regionName, const Color &color) const
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

    void VulkanCommandBuffer::InsertDebugMarker(const std::string &markerName, const Color &color) const
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

    void VulkanCommandBuffer::EndDebugRegion() const
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
    }

    /* --- CONVERSIONS --- */

    VkAccessFlags VulkanCommandBuffer::BufferCommandUsageToVkAccessFlags(const BufferCommandUsage bufferCommandUsage)
    {
        switch (bufferCommandUsage)
        {
            case BufferCommandUsage::MemoryRead:        return VK_ACCESS_MEMORY_READ_BIT;
            case BufferCommandUsage::VertexRead:
            case BufferCommandUsage::IndexRead:         return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
            case BufferCommandUsage::GraphicsRead:
            case BufferCommandUsage::ComputeRead:       return VK_ACCESS_SHADER_READ_BIT;

            case BufferCommandUsage::MemoryWrite:       return VK_ACCESS_MEMORY_WRITE_BIT;
            case BufferCommandUsage::GraphicsWrite:
            case BufferCommandUsage::ComputeWrite:      return VK_ACCESS_SHADER_WRITE_BIT;
        }

        return VK_ACCESS_NONE;
    }

    VkPipelineStageFlags VulkanCommandBuffer::BufferCommandUsageToVkPipelineStageFlags(const BufferCommandUsage bufferCommandUsage)
    {
        switch (bufferCommandUsage)
        {
            case BufferCommandUsage::VertexRead:
            case BufferCommandUsage::IndexRead:        return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
            case BufferCommandUsage::MemoryRead:
            case BufferCommandUsage::MemoryWrite:      return VK_PIPELINE_STAGE_TRANSFER_BIT;
            case BufferCommandUsage::GraphicsRead:

            case BufferCommandUsage::GraphicsWrite:    return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
            case BufferCommandUsage::ComputeRead:
            case BufferCommandUsage::ComputeWrite:     return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        }

        return VK_PIPELINE_STAGE_NONE;
    }

    VkImageLayout VulkanCommandBuffer::ImageCommandUsageToVkLayout(const ImageCommandUsage imageCommandUsage)
    {
        switch (imageCommandUsage)
        {
            case ImageCommandUsage::MemoryRead:         return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            case ImageCommandUsage::MemoryWrite:        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case ImageCommandUsage::AttachmentRead:
            case ImageCommandUsage::AttachmentWrite:    return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case ImageCommandUsage::DepthRead:          return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            case ImageCommandUsage::DepthWrite:         return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            case ImageCommandUsage::GraphicsRead:
            case ImageCommandUsage::ComputeRead:        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            case ImageCommandUsage::GraphicsWrite:
            case ImageCommandUsage::ComputeWrite:       return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case ImageCommandUsage::Present:            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }

        return VK_IMAGE_LAYOUT_UNDEFINED;
    }

    VkAccessFlags VulkanCommandBuffer::ImageCommandUsageToVkAccessFlags(const ImageCommandUsage imageCommandUsage)
    {
        switch (imageCommandUsage)
        {
            case ImageCommandUsage::MemoryRead:        return VK_ACCESS_MEMORY_READ_BIT;
            case ImageCommandUsage::AttachmentRead:    return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
            case ImageCommandUsage::DepthRead:         return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            case ImageCommandUsage::GraphicsRead:
            case ImageCommandUsage::ComputeRead:       return VK_ACCESS_SHADER_READ_BIT;

            case ImageCommandUsage::MemoryWrite:       return VK_ACCESS_MEMORY_WRITE_BIT;
            case ImageCommandUsage::AttachmentWrite:   return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
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
            case ImageCommandUsage::MemoryRead:
            case ImageCommandUsage::MemoryWrite:       return VK_PIPELINE_STAGE_TRANSFER_BIT;

            case ImageCommandUsage::AttachmentRead:
            case ImageCommandUsage::AttachmentWrite:   return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

            case ImageCommandUsage::DepthRead:         return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            case ImageCommandUsage::DepthWrite:        return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

            case ImageCommandUsage::GraphicsRead:
            case ImageCommandUsage::GraphicsWrite:     return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;

            case ImageCommandUsage::ComputeRead:
            case ImageCommandUsage::ComputeWrite:      return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

            case ImageCommandUsage::Present:           return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        }

        return VK_PIPELINE_STAGE_NONE;
    }

}