//
// Created by Nikolay Kanchevski on 5.10.22.
//

#include "VulkanRenderer.h"

#include "../VulkanCore.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateSwapchain()
    {
        // Get most suitable properties - format, present mode and extent
        VkExtent2D extent = ChooseSwapchainExtent(device->GetSwapchainCapabilites());

        // Save the properties locally to be used later on
        swapchainExtent = extent;

        // Get swapchain image count and make sure it is between the min and max allowed
        if (device->GetSwapchainCapabilites().minImageCount > maxConcurrentFrames || device->GetSwapchainCapabilites().maxImageCount < maxConcurrentFrames)
        {
            ASSERT_WARNING("Amount of total concurrent frames requested [" + std::to_string(maxConcurrentFrames) + "] is not supported! Setting was automatically changed to [" + std::to_string(device->GetSwapchainCapabilites().maxImageCount) +"]");
            maxConcurrentFrames = device->GetSwapchainCapabilites().maxImageCount;
        }

        // Set up swapchain creation info
        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = device->GetSurface();
        swapchainCreateInfo.minImageCount = maxConcurrentFrames;
        swapchainCreateInfo.imageFormat = device->GetBestSwapchainImageFormat().format;
        swapchainCreateInfo.imageColorSpace = device->GetBestSwapchainImageFormat().colorSpace;
        swapchainCreateInfo.imageExtent = extent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.preTransform = device->GetSwapchainCapabilites().currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = device->GetBestPresentationMode();
        swapchainCreateInfo.clipped = VK_TRUE;

        // Get the queue indices
        const std::vector<uint32_t> queueFamilyIndicesCollection  { static_cast<uint32_t>(device->GetGraphicsQueueFamily()), static_cast<uint32_t>(device->GetPresentationQueueFamily()) };

        // Check whether the graphics family is the same as the present one and based on that configure the creation info
        if (device->GetGraphicsQueueFamily() != device->GetPresentationQueueFamily())
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
            vkCreateSwapchainKHR(device->GetLogicalDevice(), &swapchainCreateInfo, nullptr, &swapchain),
            "Failed to create swapchain"
        );

        // Get swapchain images
        vkGetSwapchainImagesKHR(device->GetLogicalDevice(), this->swapchain, &maxConcurrentFrames, nullptr);
        std::vector<VkImage> swapchainVkImages(maxConcurrentFrames);

        // Resize image arrays
        swapchainImages.resize(maxConcurrentFrames);

        // Resize the swapchain images array and extract every swapchain image
        vkGetSwapchainImagesKHR(device->GetLogicalDevice(), this->swapchain, &maxConcurrentFrames, swapchainVkImages.data());

        for (uint32_t i = 0; i < maxConcurrentFrames; i++)
        {
            // Create swapchain image and view
            swapchainImages[i] = Image::CreateSwapchainImage({
                .image = swapchainVkImages[i],
                .format = device->GetBestSwapchainImageFormat().format,
                .sampling = VK_SAMPLE_COUNT_1_BIT,
                .dimensions = { swapchainExtent.width, swapchainExtent.height, 1 }
            });

            swapchainImages[i]->CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        }

        // Assign the EngineCore's swapchain extent
        VulkanCore::SetSwapchainExtent(swapchainExtent);
    }

    void VulkanRenderer::RecreateSwapchainObjects()
    {
        // Wait until window is responsive
        while (window.IsMinimized() || !window.IsFocused() || window.GetWidth() == 0)
        {
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device->GetLogicalDevice());

        DestroySwapchainObjects();
        CreateSwapchain();
        CreateFramebuffers();

        offscreenRenderer->Resize(swapchainExtent.width, swapchainExtent.height);
        CreateOffscreenImageDescriptorSets();
    }

    void VulkanRenderer::DestroySwapchainObjects()
    {
        // Destroy swapchain images
        for (uint32_t i = maxConcurrentFrames; i--;)
        {
            swapchainImages[i]->Destroy();
            swapchainFramebuffers[i]->Destroy();
        }

        // Destroy swapchain
        vkDestroySwapchainKHR(device->GetLogicalDevice(), this->swapchain, nullptr);
    }

    VkExtent2D VulkanRenderer::ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR &givenCapabilities)
    {
        // Get size of window's framebuffer
        int width, height;
        glfwGetFramebufferSize(window.GetCoreWindow(), &width, &height);

        // Save the sizes in a struct
        VkExtent2D createdExtent{};
        createdExtent.width = width;
        createdExtent.height = height;

        // Clamp the width and height such that they don't exceed the maximums
        createdExtent.width = std::clamp(createdExtent.width, givenCapabilities.minImageExtent.width, givenCapabilities.maxImageExtent.width);
        createdExtent.height = std::clamp(createdExtent.height, givenCapabilities.minImageExtent.height, givenCapabilities.maxImageExtent.height);

        // Return the manually created extent
        return createdExtent;
    }

}