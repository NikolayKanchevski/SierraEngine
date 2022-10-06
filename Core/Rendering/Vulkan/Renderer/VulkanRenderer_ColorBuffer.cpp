//
// Created by Nikolay Kanchevski on 6.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    VkSampleCountFlagBits VulkanRenderer::GetHighestSupportedMsaaCount()
    {
        VkSampleCountFlags countFlags = VulkanCore::GetPhysicalDeviceProperties().limits.framebufferColorSampleCounts & VulkanCore::GetPhysicalDeviceProperties().limits.framebufferDepthSampleCounts;

        if ((countFlags & VK_SAMPLE_COUNT_64_BIT) != 0) return VK_SAMPLE_COUNT_64_BIT;
        if ((countFlags & VK_SAMPLE_COUNT_32_BIT) != 0) return VK_SAMPLE_COUNT_32_BIT;
        if ((countFlags & VK_SAMPLE_COUNT_16_BIT) != 0) return VK_SAMPLE_COUNT_16_BIT;
        if ((countFlags & VK_SAMPLE_COUNT_8_BIT) != 0) return VK_SAMPLE_COUNT_8_BIT;
        if ((countFlags & VK_SAMPLE_COUNT_4_BIT) != 0) return VK_SAMPLE_COUNT_4_BIT;
        if ((countFlags & VK_SAMPLE_COUNT_2_BIT) != 0) return VK_SAMPLE_COUNT_2_BIT;

        return VK_SAMPLE_COUNT_1_BIT;
    }

}