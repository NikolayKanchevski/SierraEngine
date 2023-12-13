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
        // Set up allocate info
        VkCommandBufferAllocateInfo allocateInfo = { };
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = device.GetCommandPool();
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = 1;

        // Allocate command buffer
        const VkResult result = device.GetFunctionTable().vkAllocateCommandBuffers(device.GetLogicalDevice(), &allocateInfo, &commandBuffer);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create command buffer [{0}]! Error code: {1}.", GetName(), result);
    }

    /* --- POLLING METHODS --- */

    void VulkanCommandBuffer::Begin()
    {
        // Reset command buffer
        device.GetFunctionTable().vkResetCommandBuffer(commandBuffer, 0);

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

    /* --- DESTRUCTOR --- */

    void VulkanCommandBuffer::Destroy()
    {
        // Command buffers are deallocated with the command pool
    }

}