//
// Created by Nikolay Kanchevski on 6.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateColorBufferImage()
    {
        // Create the sampled color image
        colorImage = Image::Builder()
            .SetWidth(swapchainExtent.width)
            .SetHeight(swapchainExtent.height)
            .SetSampling(msaaSampleCount)
            .SetFormat(swapchainImageFormat)
            .SetUsageFlags(VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            .SetMemoryFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        .Build();

        // Create an image view off the sampled color image
        colorImage->CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
    }

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