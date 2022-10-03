//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <vulkan/vulkan.h>

namespace Sierra::Core::Rendering::Vulkan
{

    class VulkanUtilities
    {
    public:
        /* --- GETTER METHODS --- */
        static uint32_t FindMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags givenMemoryFlags);

        static VkCommandBuffer BeginSingleTimeCommands();
        static void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
    };

}
