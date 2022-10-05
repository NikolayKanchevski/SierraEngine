//
// Created by Nikolay Kanchevski on 5.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    VulkanRenderer::SwapchainSupportDetails VulkanRenderer::GetSwapchainSupportDetails(VkPhysicalDevice &givenPhysicalDevice)
    {
        // Get the details of the GPU's supported swapchain
        SwapchainSupportDetails swapchainDetails{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(givenPhysicalDevice, surface, &swapchainDetails.capabilities);

        // Get how many formats are available
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(givenPhysicalDevice, this->surface, &formatCount, nullptr);

        // Put each of them in an array
        swapchainDetails.formats = std::vector<VkSurfaceFormatKHR>(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(givenPhysicalDevice, this->surface, &formatCount, swapchainDetails.formats.data());

        // Get how many presentation modes are available
        uint32_t presentModesCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(givenPhysicalDevice, this->surface, &presentModesCount, nullptr);

        // Put each of them in an array
        swapchainDetails.presentModes = std::vector<VkPresentModeKHR>(presentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(givenPhysicalDevice, this->surface, &formatCount, swapchainDetails.presentModes.data());

        return swapchainDetails;
    }

}