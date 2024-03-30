//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "VulkanSwapchain.h"

#if SR_PLATFORM_WINDOWS
    #include "Platform/Windows/VulkanWindowsSurface.h"
    using NativeSurface = Sierra::VulkanWindowsSurface;
#elif SR_PLATFORM_macOS
    #include "Platform/macOS/VulkanMacOSSurface.h"
    using NativeSurface = Sierra::VulkanMacOSSurface;
#elif SR_PLATFORM_LINUX
    #include "Platform/Linux/VulkanLinuxSurface.h"
    using NativeSurface = Sierra::VulkanLinuxSurface;
#elif SR_PLATFORM_ANDROID
    #include "Platform/Android/VulkanAndroidSurface.h"
    using NativeSurface = Sierra::VulkanAndroidSurface;
#elif SR_PLATFORM_iOS
    #include "Platform/iOS/VulkaniOSSurface.h"
    using NativeSurface = Sierra::VulkaniOSSurface;
#endif
#include "VulkanCommandBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanSwapchain::VulkanSwapchain(const VulkanInstance &instance, const VulkanDevice &device, const SwapchainCreateInfo &createInfo)
        : Swapchain(createInfo), VulkanResource(createInfo.name), instance(instance), device(device), window(createInfo.window), surface(NativeSurface::Create(instance, createInfo.window)), preferredPresentationMode(createInfo.preferredPresentationMode), preferredImageMemoryType(createInfo.preferredImageMemoryType)
    {
        SR_ERROR_IF(!device.IsExtensionLoaded(VK_KHR_SWAPCHAIN_EXTENSION_NAME), "[Vulkan]: Cannot create swapchain [{0}], as the provided device [{1}] does not support the {2} extension!", GetName(), device.GetName(), VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        CreateSwapchain();
        CreateSynchronization();
    }

    /* --- POLLING METHODS --- */

    void VulkanSwapchain::AcquireNextImage()
    {
        // Acquire next image
        VkResult result = device.GetFunctionTable().vkAcquireNextImageKHR(device.GetLogicalDevice(), swapchain, std::numeric_limits<uint64>::max(), isImageAcquiredSemaphores[currentFrame], VK_NULL_HANDLE, &currentImage);

        // Resize swapchain if needed
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            Recreate();
            AcquireNextImage();
        }

        // Set up submit info
        const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkSubmitInfo submitInfo = { };
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &isImageAcquiredSemaphores[currentFrame];
        submitInfo.pWaitDstStageMask = &waitStage;

        // Wait until swapchain image has been acquired and is ready to be worked on
        result = device.GetFunctionTable().vkQueueSubmit(device.GetGeneralQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not wait for swapchain image [{0}] on swapchain [{1}] to get swapped out! Error code: {2}.", currentFrame, GetName(), result);
    }

    void VulkanSwapchain::Present(std::unique_ptr<CommandBuffer> &commandBuffer)
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot present swapchain [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<VulkanCommandBuffer&>(*commandBuffer);

        // Set up semaphore submit info
        const uint64 waitValue = vulkanCommandBuffer.GetCompletionSignalValue();
        constexpr uint64 binarySignalValue = 1;
        VkTimelineSemaphoreSubmitInfoKHR semaphoreSubmitInfo = { };
        semaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
        semaphoreSubmitInfo.waitSemaphoreValueCount = 1;
        semaphoreSubmitInfo.pWaitSemaphoreValues = &waitValue;
        semaphoreSubmitInfo.signalSemaphoreValueCount = 1;
        semaphoreSubmitInfo.pSignalSemaphoreValues = &binarySignalValue; // Simply using 1, as we are signalling a binary semaphore

        // Set up submit info
        const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkSemaphore waitSemaphore = device.GetGeneralTimelineSemaphore();
        VkSubmitInfo submitInfo = { };
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &waitSemaphore;
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.commandBufferCount = 0;
        submitInfo.pCommandBuffers = nullptr;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &isImagePresentedSemaphores[currentFrame];
        submitInfo.pNext = &semaphoreSubmitInfo;

        // Wait for timeline semaphore to signal, and signal the binary one as well, as VkPresentInfoKHR forbids passing timeline one to it
        VkResult result = device.GetFunctionTable().vkQueueSubmit(device.GetGeneralQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not wait for timeline semaphore on swapchain [{1}]! Error code: {2}.", commandBuffer->GetName(), GetName(), result);

        // Set up presentation info
        VkPresentInfoKHR presentInfo = { };
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &isImagePresentedSemaphores[currentFrame];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.pImageIndices = &currentImage;
        presentInfo.pResults = nullptr;

        // Submit presentation queue and resize the swapchain if needed
        result = device.GetFunctionTable().vkQueuePresentKHR(presentationQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            Recreate();
        }

        // Increment currentFrame
        currentFrame = (currentFrame + 1) % concurrentFrameCount;
    }

    /* --- DESTRUCTOR --- */

    VulkanSwapchain::~VulkanSwapchain()
    {
        device.GetFunctionTable().vkDestroySwapchainKHR(device.GetLogicalDevice(), swapchain, nullptr);

        for (uint32 i = 0; i < concurrentFrameCount; i++)
        {
            device.GetFunctionTable().vkDestroySemaphore(device.GetLogicalDevice(), isImageAcquiredSemaphores[i], nullptr);
            device.GetFunctionTable().vkDestroySemaphore(device.GetLogicalDevice(), isImagePresentedSemaphores[i], nullptr);
        }

        instance.GetFunctionTable().vkDestroySurfaceKHR(instance.GetVulkanInstance(), surface, nullptr);
    }

    /* --- PRIVATE METHODS --- */

    void VulkanSwapchain::CreateSwapchain()
    {
        // Get count of all present queues
        uint32 queueFamilyPropertiesCount = 0;
        instance.GetFunctionTable().vkGetPhysicalDeviceQueueFamilyProperties(device.GetPhysicalDevice(), &queueFamilyPropertiesCount, nullptr);

        // Retrieve queue properties
        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        instance.GetFunctionTable().vkGetPhysicalDeviceQueueFamilyProperties(device.GetPhysicalDevice(), &queueFamilyPropertiesCount, queueFamilyProperties.data());

        // Try to find a queue family, which supports presentation
        bool presentationFamilyFound = false;
        uint32 presentationQueueFamily = 0;
        for (uint32 i = 0; i < queueFamilyPropertiesCount; i++)
        {
            // Check if current family supports presentation
            VkBool32 presentationSupported = VK_FALSE;
            instance.GetFunctionTable().vkGetPhysicalDeviceSurfaceSupportKHR(device.GetPhysicalDevice(), i, surface, &presentationSupported);

            // Save family
            if (presentationSupported == VK_TRUE)
            {
                presentationQueueFamily = i;
                presentationFamilyFound = true;
                break;
            }
        }
        SR_ERROR_IF(!presentationFamilyFound, "[Vulkan]: Cannot create swapchain [{0}], as no queue family supports presentation!", GetName());
        const std::vector<uint32> sharedQueueFamilyIndices = { device.GetGeneralQueueFamily(), presentationQueueFamily };

        // Retrieve presentation queue
        device.GetFunctionTable().vkGetDeviceQueue(device.GetLogicalDevice(), presentationQueueFamily, 0, &presentationQueue);

        // Retrieve supported formats
        uint32 supportedFormatCount = 0;
        instance.GetFunctionTable().vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysicalDevice(), surface, &supportedFormatCount, nullptr);

        std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatCount);
        instance.GetFunctionTable().vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysicalDevice(), surface, &supportedFormatCount, supportedFormats.data());

        // Depending on user's preference, see which formats work
        std::vector<VkFormat> formatsToTry;
        switch (preferredImageMemoryType)
        {
            case SwapchainImageMemoryType::UNorm8:      { formatsToTry = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R16G16B16_UNORM, VK_FORMAT_R16G16B16_SFLOAT }; break; }
            case SwapchainImageMemoryType::SRGB8:       { formatsToTry = { VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_R8G8B8A8_SRGB };                                                          break; }
            case SwapchainImageMemoryType::UNorm16:     { formatsToTry = { VK_FORMAT_R16G16B16_UNORM, VK_FORMAT_R16G16B16_SFLOAT };                                                     break; }
            default:                                    break;
        }

        // Try to find a format that satisfies user's needs
        VkSurfaceFormatKHR selectedFormat = { .format = VK_FORMAT_UNDEFINED, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        for (const auto format : formatsToTry)
        {
            auto iterator = std::find_if(supportedFormats.begin(), supportedFormats.end(), [format](const VkSurfaceFormatKHR item) { return item.format == format && item.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; });
            if (iterator != supportedFormats.end())
            {
                selectedFormat.format = format;
                break;
            }
        }

        // Assign best format, if one was found
        SR_ERROR_IF(selectedFormat.format == VK_FORMAT_UNDEFINED, "[Vulkan] Could not create swapchain [{0}], as it does not support any allowed swapchain formats!", GetName());

        // Retrieve supported present modes
        uint32 supportedPresentModeCount = 0;
        instance.GetFunctionTable().vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysicalDevice(), surface, &supportedPresentModeCount, nullptr);

        std::vector<VkPresentModeKHR> supportedPresentModes(supportedPresentModeCount);
        instance.GetFunctionTable().vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysicalDevice(), surface, &supportedFormatCount, supportedPresentModes.data());

        // Select a present mode according to preferred one in create info
        VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR; // FIFO is guaranteed to be supported
        for (const auto presentMode : supportedPresentModes)
        {
            if (preferredPresentationMode == SwapchainPresentationMode::Immediate && presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)   // Possible tearing, no GPU idling, minimal latency, and high energy consumption
            {
                selectedPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
                break;
            }
            #if !SR_PLATFORM_MOBILE
                if (preferredPresentationMode == SwapchainPresentationMode::VSync && presentMode == VK_PRESENT_MODE_MAILBOX_KHR)     // VSync with no tearing, no GPU idling, low latency, and high energy consumption
            #else
                if (preferredPresentationMode == SwapchainPresentationMode::VSync && presentMode == VK_PRESENT_MODE_FIFO_KHR)        // VSync with no tearing, low GPU idling, moderate latency, minimal energy consuption
            #endif
            {
                selectedPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                break;
            }
        }

        // Get surface capabilities
        VkSurfaceCapabilitiesKHR surfaceCapabilities = { };
        instance.GetFunctionTable().vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.GetPhysicalDevice(), surface, &surfaceCapabilities);

        // Use at least 3 images per frame (for triple-buffering) if supported, otherwise the max allowed
        const uint32 preferredConcurrentFrameCount = glm::clamp(3U, surfaceCapabilities.minImageCount, glm::max(surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount));

        // Set up swapchain creation info
        VkSwapchainCreateInfoKHR swapchainCreateInfo = { };
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = surface;
        swapchainCreateInfo.imageFormat = selectedFormat.format;
        swapchainCreateInfo.imageColorSpace = selectedFormat.colorSpace;
        swapchainCreateInfo.minImageCount = preferredConcurrentFrameCount;
        swapchainCreateInfo.imageExtent.width = glm::clamp(window->GetFramebufferSize().x, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        swapchainCreateInfo.imageExtent.height = glm::clamp(window->GetFramebufferSize().y, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = selectedPresentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.oldSwapchain = swapchain;
        if (device.GetGeneralQueueFamily() != presentationQueueFamily)
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = sharedQueueFamilyIndices.data();
        }
        else
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0;
            swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        }

        // Create swapchain
        VkResult result = device.GetFunctionTable().vkCreateSwapchainKHR(device.GetLogicalDevice(), &swapchainCreateInfo, nullptr, &swapchain);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create swapchain [{0}]! Error code: {1}.", GetName(), result);

        // Set object name
        device.SetObjectName(swapchain, VK_OBJECT_TYPE_SWAPCHAIN_KHR, GetName());

        // If an old swapchain was reused, destroy that
        if (swapchainCreateInfo.oldSwapchain != VK_NULL_HANDLE)
        {
            device.GetFunctionTable().vkDestroySwapchainKHR(device.GetLogicalDevice(), swapchainCreateInfo.oldSwapchain, nullptr);
            swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
        }

        // Get actual concurrent image count
        concurrentFrameCount = 0;
        device.GetFunctionTable().vkGetSwapchainImagesKHR(device.GetLogicalDevice(), swapchain, &concurrentFrameCount, nullptr);

        // Get swapchain images
        std::vector<VkImage> vulkanSwapchainImages(concurrentFrameCount);
        device.GetFunctionTable().vkGetSwapchainImagesKHR(device.GetLogicalDevice(), swapchain, &concurrentFrameCount, vulkanSwapchainImages.data());

        // Create image implementations
        swapchainImages.resize(concurrentFrameCount);
        for (uint32 i = 0; i < concurrentFrameCount; i++)
        {
            swapchainImages[i] = std::unique_ptr<VulkanImage>(new VulkanImage(device, VulkanImage::SwapchainImageCreateInfo {
                .name = "Image " + std::to_string(i) + " of swapchain [" + std::string(GetName()) + "]",
                .image = vulkanSwapchainImages[i],
                .width = swapchainCreateInfo.imageExtent.width,
                .height = swapchainCreateInfo.imageExtent.height,
                .format = swapchainCreateInfo.imageFormat
            }));
        }
    }

    void VulkanSwapchain::CreateSynchronization()
    {
        // Set up semaphore create info
        VkSemaphoreCreateInfo semaphoreCreateInfo = { };
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        // Set up fence create info
        VkFenceCreateInfo fenceCreateInfo = { };
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        // Create sync objects
        isImageAcquiredSemaphores.resize(concurrentFrameCount);
        isImagePresentedSemaphores.resize(concurrentFrameCount);

        VkResult result;
        for (uint32 i = 0; i < concurrentFrameCount; i++)
        {
            result = device.GetFunctionTable().vkCreateSemaphore(device.GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &isImageAcquiredSemaphores[i]);
            SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create semaphore [{0}], indicating whether corresponding swapchain image of swapchain [{1}] is free for use!", i, GetName());
            device.SetObjectName(isImageAcquiredSemaphores[i], VK_OBJECT_TYPE_SEMAPHORE, "Image free semaphore [" + std::to_string(i) + "] of swapchain [" + std::string(GetName()) + "]");

            result = device.GetFunctionTable().vkCreateSemaphore(device.GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &isImagePresentedSemaphores[i]);
            SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create semaphore [{0}], indicating whether corresponding swapchain image of swapchain [{1}] is free for presenting!", i, GetName());
            device.SetObjectName(isImagePresentedSemaphores[i], VK_OBJECT_TYPE_SEMAPHORE, "Image rendered semaphore [" + std::to_string(i) + "] of swapchain [" + std::string(GetName()) + "]");
        }
    }

    void VulkanSwapchain::Recreate()
    {
        while (window->IsMinimized())
        {
            window->Update();
        }

        device.GetFunctionTable().vkDeviceWaitIdle(device.GetLogicalDevice());
        const Vector2UInt lastSize = { swapchainImages[0]->GetWidth(), swapchainImages[0]->GetHeight() };

        CreateSwapchain();
        const Vector2UInt newSize = { swapchainImages[0]->GetWidth(), swapchainImages[0]->GetHeight() };

        if (lastSize != newSize) GetSwapchainResizeDispatcher().DispatchEvent(Vector2UInt(newSize));
    }

}
