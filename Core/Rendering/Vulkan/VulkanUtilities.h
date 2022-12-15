//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "VulkanCore.h"
#include "VulkanTypes.h"

namespace Sierra::Core::Rendering::Vulkan
{

    // TODO: MOVE TO DEVICE

    class VulkanUtilities
    {
    public:
        /* --- GETTER METHODS --- */
        static uint32_t FindMemoryTypeIndex(uint32_t typeFilter, MemoryFlags givenMemoryFlags);

        static VkCommandBuffer BeginSingleTimeCommands();
        static void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

        static VkShaderModule CreateShaderModule(const std::string &fileName);
    };

}
