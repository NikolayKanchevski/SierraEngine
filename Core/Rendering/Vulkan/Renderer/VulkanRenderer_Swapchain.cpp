//
// Created by Nikolay Kanchevski on 5.10.22.
//

#include "VulkanRenderer.h"

#include "../VulkanCore.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateSwapchain()
    {
        // Lower the sample count if it is not supported
        if (msaaSamplingEnabled)
        {
            VkSampleCountFlagBits highestSupportedSampleCount = this->GetHighestSupportedMsaaCount();
            if (this->msaaSampleCount > highestSupportedSampleCount)
            {
                ASSERT_WARNING("Sampling MSAA level [" + std::string(string_VkSampleCountFlagBits(this->msaaSampleCount)) + "] requested but is not supported by the system. It is automatically lowered to [" + std::string(string_VkSampleCountFlagBits(highestSupportedSampleCount)) + "] which is the highest supported setting");
                this->msaaSampleCount = highestSupportedSampleCount;
            }
        }

        // Get most suitable properties - format, present mode and extent
        SwapchainSupportDetails swapchainSupportDetails = GetSwapchainSupportDetails(physicalDevice);
        VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainFormat(swapchainSupportDetails.formats);
        VkPresentModeKHR presentMode = ChooseSwapchainPresentMode(swapchainSupportDetails.presentModes);
        VkExtent2D extent = ChooseSwapchainExtent(swapchainSupportDetails.capabilities);

        // Save the properties locally to be used later on
        swapchainImageFormat = surfaceFormat.format;
        swapchainExtent = extent;

        // Get swapchain image count and make sure it is between the min and max allowed
        uint32_t imageCount = swapchainSupportDetails.capabilities.minImageCount + 1;
        if (swapchainSupportDetails.capabilities.maxImageCount != 0 && imageCount > swapchainSupportDetails.capabilities.maxImageCount)
        {
            imageCount = swapchainSupportDetails.capabilities.maxImageCount;
        }

        // Set up swapchain creation info
        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = this->surface;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = extent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.preTransform = swapchainSupportDetails.capabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
//        swapchainCreateInfo.oldSwapchain = swapchain;

        // Get the queue indices
        const std::vector<uint32_t> queueFamilyIndicesCollection  { static_cast<uint32_t>(queueFamilyIndices.graphicsFamily), static_cast<uint32_t>(queueFamilyIndices.presentFamily) };

        // Check whether the graphics family is the same as the present one and based on that configure the creation info
        if (queueFamilyIndices.graphicsFamily != queueFamilyIndices.presentFamily)
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndicesCollection.data();
        }
        else
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0;
            swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        }

        // Create the swapchain
        VK_ASSERT(
            vkCreateSwapchainKHR(this->logicalDevice, &swapchainCreateInfo, nullptr, &swapchain),
            "Failed to create swapchain"
        );

        // Get swapchain images
        vkGetSwapchainImagesKHR(this->logicalDevice, this->swapchain, &imageCount, nullptr);

        std::vector<VkImage> swapchainVkImages(imageCount);
        swapchainImages.resize(imageCount);

        // Resize the swapchain images array and extract every swapchain image
        vkGetSwapchainImagesKHR(this->logicalDevice, this->swapchain, &imageCount, swapchainVkImages.data());

        for (int i = imageCount; i--;)
        {
            swapchainImages[i] = Image::CreateSwapchainImage({.image = swapchainVkImages[i], .format = swapchainImageFormat, .sampling = msaaSampleCount, .dimensions = {swapchainExtent.width, swapchainExtent.height, 0},});

            swapchainImages[i]->CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        }

        // Assign the EngineCore's swapchain extent
        VulkanCore::SetSwapchainExtent(swapchainExtent);
    }

    void VulkanRenderer::RecreateSwapchainObjects()
    {
        while (window.IsMinimized() || !window.IsFocused() || window.GetWidth() == 0)
        {
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(this->logicalDevice);

        DestroySwapchainObjects();

        CreateSwapchain();
        CreateRenderPass();
        CreateDepthBufferImage();
        CreateColorBufferImage();
        CreateFramebuffers();
    }

    void VulkanRenderer::DestroySwapchainObjects()
    {
        colorImage->Destroy();

        for (const auto &swapchainFramebuffer : this->swapchainFramebuffers)
        {
            swapchainFramebuffer->Destroy();
        }

        depthImage->Destroy();

        renderPass->Destroy();

        for (const auto &image : swapchainImages)
        {
            image->DestroyImageView();
        }

        vkDestroySwapchainKHR(this->logicalDevice, this->swapchain, nullptr);
    }

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

    VkExtent2D VulkanRenderer::ChooseSwapchainExtent(VkSurfaceCapabilitiesKHR &givenCapabilities)
    {
        // Check to see if the extent is already configured
        if (givenCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            // If so just return it
            return givenCapabilities.currentExtent;
        }

        // Otherwise get the settings for it manually by finding out the width and height for it

        // Save the sizes in a struct
        VkExtent2D createdExtent{};
        createdExtent.width = static_cast<uint32_t>(window.GetWidth());
        createdExtent.height = static_cast<uint32_t>(window.GetHeight());

        // Clamp the width and height such that they don't exceed the maximums
        createdExtent.width = std::clamp(createdExtent.width, givenCapabilities.minImageExtent.width, givenCapabilities.maxImageExtent.width);
        createdExtent.height = std::clamp(createdExtent.height, givenCapabilities.minImageExtent.height, givenCapabilities.maxImageExtent.height);

        // Return the manually created extent
        return createdExtent;
    }

    VkSurfaceFormatKHR VulkanRenderer::ChooseSwapchainFormat(std::vector<VkSurfaceFormatKHR> &givenFormats)
    {
        for (const auto &availableFormat : givenFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        // Otherwise just return the very first one
        return givenFormats[0];
    }

    VkPresentModeKHR VulkanRenderer::ChooseSwapchainPresentMode(std::vector<VkPresentModeKHR> &givenPresentModes)
    {
        // Loop trough each to check if it is VK_PRESENT_MODE_MAILBOX_KHR
        for (const auto &availablePresentMode : givenPresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }

        // Otherwise return VK_PRESENT_MODE_FIFO_KHR which is guaranteed to be available
        return VK_PRESENT_MODE_FIFO_KHR;
    }

}