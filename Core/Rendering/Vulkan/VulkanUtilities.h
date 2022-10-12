//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace Sierra::Core::Rendering::Vulkan
{

    class VulkanUtilities
    {
    public:
        /* --- GETTER METHODS --- */
        static uint32_t FindMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags givenMemoryFlags);

        static VkCommandBuffer BeginSingleTimeCommands();
        static void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

        static VkShaderModule CreateShaderModule(const std::string &fileName);
    };

}
