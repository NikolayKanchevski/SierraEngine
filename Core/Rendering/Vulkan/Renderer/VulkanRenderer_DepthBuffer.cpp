//
// Created by Nikolay Kanchevski on 7.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateDepthBufferImage()
    {
        // Retrieve the best depth buffer image format
        std::vector<VkFormat> allowedDepthFormats { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT };
        depthImageFormat = GetBestDepthBufferFormat(
                allowedDepthFormats,
                VK_IMAGE_TILING_OPTIMAL,
                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );

        depthImage = Image::Builder()
            .SetWidth(swapchainExtent.width)
            .SetHeight(swapchainExtent.height)
            .SetSampling(this->msaaSampleCount)
            .SetUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
            .SetMemoryFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
            .SetFormat(depthImageFormat)
        .Build();

        depthImage->CreateImageView(VK_IMAGE_ASPECT_DEPTH_BIT);

        depthImage->TransitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    }

    VkFormat VulkanRenderer::GetBestDepthBufferFormat(std::vector<VkFormat> &givenFormats, VkImageTiling imageTiling, VkFormatFeatureFlagBits formatFeatureFlags)
    {
        for (const auto &givenFormat : givenFormats)
        {
            // Get the properties for the current format
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(this->physicalDevice, givenFormat, &formatProperties);

            // Check if the required format properties are supported
            if (imageTiling == VK_IMAGE_TILING_LINEAR && (formatProperties.linearTilingFeatures & formatFeatureFlags) == formatFeatureFlags)
            {
                return givenFormat;
            }
            else if (imageTiling == VK_IMAGE_TILING_OPTIMAL && (formatProperties.optimalTilingFeatures & formatFeatureFlags) == formatFeatureFlags)
            {
                return givenFormat;
            }
        }

        // Otherwise throw an error
        VulkanDebugger::ThrowError("No depth buffer formats supported");

        return VK_FORMAT_UNDEFINED;
    }

}