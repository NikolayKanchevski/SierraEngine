//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "VulkanUtilities.h"

#include "../../../Engine/Classes/File.h"

using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering::Vulkan
{

    /* --- GETTER METHODS --- */
    uint32_t VulkanUtilities::FindMemoryTypeIndex(const uint32_t typeFilter, const MemoryFlags givenMemoryFlags)
    {
        VkPhysicalDeviceMemoryProperties memoryProperties{};
        vkGetPhysicalDeviceMemoryProperties(VulkanCore::GetPhysicalDevice(), &memoryProperties);

        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << (int) i)) != 0 && (memoryProperties.memoryTypes[i].propertyFlags & (int) givenMemoryFlags) == (int) givenMemoryFlags)
            {
                return i;
            }
        }

        return 0;
    }

    VkCommandBuffer VulkanUtilities::BeginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandPool = VulkanCore::GetCommandPool();
        commandBufferAllocateInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        VK_ASSERT(
                vkAllocateCommandBuffers(VulkanCore::GetLogicalDevice(), &commandBufferAllocateInfo, &commandBuffer),
                "Failed to allocate single time command buffer"
        );

        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

        return commandBuffer;
    }

    void VulkanUtilities::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
    {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(VulkanCore::GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(VulkanCore::GetGraphicsQueue());

        vkFreeCommandBuffers(VulkanCore::GetLogicalDevice(), VulkanCore::GetCommandPool(), 1, &commandBuffer);
    }

    VkShaderModule VulkanUtilities::CreateShaderModule(const std::string &fileName)
    {
        // Read bytes from the given file
        const std::vector<char> shaderCode = File::ReadFile(fileName);

        // Set module creation info
        VkShaderModuleCreateInfo moduleCreateInfo{};
        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.codeSize = shaderCode.size();
        moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

        // Create shader module
        VkShaderModule shaderModule;
        VK_ASSERT(
            vkCreateShaderModule(VulkanCore::GetLogicalDevice(), &moduleCreateInfo, nullptr, &shaderModule),
            "Failed to create shader module for [" + fileName + "]"
        );

        return shaderModule;
    }

}