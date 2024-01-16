//
// Created by Nikolay Kanchevski on 11.12.23.
//

#include "VulkanCommandBuffer.h"

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
    }

    void VulkanCommandBuffer::End()
    {
        // End command buffer
        const VkResult result = device.GetFunctionTable().vkEndCommandBuffer(commandBuffer);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not end command buffer [{0}]! Error code: {1}.", GetName(), result);
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
        SR_ERROR_IF(sizeof(Color) != sizeof(VkDebugMarkerMarkerInfoEXT::color), "[Vulkan]: Memory size of Color does not match that of VkDebugMarkerMarkerInfoEXT::color, so a memcpy operation is forbidden!");
        memcpy(markerInfo.color, &color, sizeof(Color));

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
        SR_ERROR_IF(sizeof(Color) != sizeof(VkDebugMarkerMarkerInfoEXT::color), "[Vulkan]: Memory size of Color does not match that of VkDebugMarkerMarkerInfoEXT::color, so a memcpy operation is forbidden!");
        memcpy(markerInfo.color, &color, sizeof(Color));

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

}