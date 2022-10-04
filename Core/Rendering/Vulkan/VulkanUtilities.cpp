//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "VulkanUtilities.h"
#include "VulkanCore.h"
#include "VulkanDebugger.h"

namespace Sierra::Core::Rendering::Vulkan
{

    /* --- GETTER METHODS --- */
    uint32_t VulkanUtilities::FindMemoryTypeIndex(const uint32_t typeFilter, const VkMemoryPropertyFlags givenMemoryFlags)
    {
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(VulkanCore::GetPhysicalDevice(), &memoryProperties);

        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << (int) i)) != 0 && (memoryProperties.memoryTypes[i].propertyFlags & givenMemoryFlags) == givenMemoryFlags)
            {
                return i;
            }
        }

        return 0;
    }

    VkCommandBuffer VulkanUtilities::BeginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo commandBufferAllocateInfo;
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandPool = VulkanCore::GetCommandPool();
        commandBufferAllocateInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        VulkanDebugger::CheckResults(
                vkAllocateCommandBuffers(VulkanCore::GetLogicalDevice(), &commandBufferAllocateInfo, &commandBuffer),
                "Failed to allocate single time command buffer"
        );

        VkCommandBufferBeginInfo commandBufferBeginInfo;
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

        return commandBuffer;
    }

    void VulkanUtilities::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
    {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo;
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(VulkanCore::GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(VulkanCore::GetGraphicsQueue());

        vkFreeCommandBuffers(VulkanCore::GetLogicalDevice(), VulkanCore::GetCommandPool(), 1, &commandBuffer);
    }

}