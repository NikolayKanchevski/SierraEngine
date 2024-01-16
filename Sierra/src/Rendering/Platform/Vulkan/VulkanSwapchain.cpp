//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "VulkanSwapchain.h"

#if SR_PLATFORM_WINDOWS
    #include "Platform/Windows/VulkanWindowsSurface.h"
    typedef Sierra::VulkanWindowsSurface NativeSurface;
#elif SR_PLATFORM_macOS
    #include "Platform/macOS/VulkanMacOSSurface.h"
    typedef Sierra::VulkanMacOSSurface NativeSurface;
#elif SR_PLATFORM_LINUX
    #include "Platform/Linux/VulkanLinuxSurface.h"
    typedef Sierra::VulkanLinuxSurface NativeSurface;
#elif SR_PLATFORM_ANDROID
    #include "Platform/Android/VulkanAndroidSurface.h"
    typedef Sierra::VulkanAndroidSurface NativeSurface;
#elif SR_PLATFORM_iOS
    #include "Platform/iOS/VulkaniOSSurface.h"
    typedef Sierra::VulkaniOSSurface NativeSurface;
#endif
#include "VulkanCommandBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanSwapchain::VulkanSwapchain(const VulkanInstance &instance, const VulkanDevice &device, const SwapchainCreateInfo &createInfo)
        : Swapchain(createInfo), VulkanResource(createInfo.name), instance(instance), device(device), window(createInfo.window), preferredPresentationMode(createInfo.preferredPresentationMode), preferredImageMemoryType(createInfo.preferredImageMemoryType)
    {
        SR_ERROR_IF(!device.IsExtensionLoaded(VK_KHR_SWAPCHAIN_EXTENSION_NAME), "[Vulkan]: Cannot create swapchain [{0}], as the provided device [{1}] does not support the {2} extension!", GetName(), device.GetName(), VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        // Create surface
        surface = NativeSurface::Create(instance, createInfo.window);

        RetrieveConstantSettings();
        CreateSwapchain();
        CreateImages();
        CreateSynchronization();
    }

    /* --- POLLING METHODS --- */

    void VulkanSwapchain::AcquireNextImage()
    {
        // Acquire next image
        const VkResult result = device.GetFunctionTable().vkAcquireNextImageKHR(device.GetLogicalDevice(), swapchain, std::numeric_limits<uint64>::max(), isImageFreeSemaphores[currentFrame], VK_NULL_HANDLE, &currentImage);

        // Resize swapchain if needed
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            Recreate();
            AcquireNextImage();
        }
    }

    void VulkanSwapchain::SubmitCommandBufferAndPresent(std::unique_ptr<CommandBuffer> &commandBuffer)
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot present swapchain [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(*commandBuffer);

        // Set up submit info
        const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkCommandBuffer vkCommandBuffer = vulkanCommandBuffer.GetVulkanCommandBuffer();
        VkSubmitInfo submitInfo = { };
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &isImageFreeSemaphores[currentFrame];
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vkCommandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &isImageRenderedSemaphores[currentFrame];

        // Submit command buffer
        device.GetFunctionTable().vkResetFences(device.GetLogicalDevice(), 1, &isImageUnderWorkFences[currentFrame]);
        VkResult result = device.GetFunctionTable().vkQueueSubmit(device.GetGeneralQueue(), 1, &submitInfo, isImageUnderWorkFences[currentFrame]);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not submit command buffer [{0}] for swapchain [{1}]! Error code: {2}.", commandBuffer->GetName(), GetName(), result);

        // Set up presentation info
        VkPresentInfoKHR presentInfo = { };
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &isImageRenderedSemaphores[currentFrame];
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

        // Wait until GPU is done with this frame, so user can use their command buffer without any manual sync
        device.GetFunctionTable().vkWaitForFences(device.GetLogicalDevice(), 1, &isImageUnderWorkFences[currentFrame], VK_TRUE, std::numeric_limits<uint64>::max());
    }

    /* --- DESTRUCTOR --- */

    VulkanSwapchain::~VulkanSwapchain()
    {
        device.GetFunctionTable().vkDestroySwapchainKHR(device.GetLogicalDevice(), swapchain, nullptr);

        for (uint32 i = 0; i < concurrentFrameCount; i++)
        {
            device.GetFunctionTable().vkDestroySemaphore(device.GetLogicalDevice(), isImageFreeSemaphores[i], nullptr);
            device.GetFunctionTable().vkDestroySemaphore(device.GetLogicalDevice(), isImageRenderedSemaphores[i], nullptr);
            device.GetFunctionTable().vkDestroyFence(device.GetLogicalDevice(), isImageUnderWorkFences[i], nullptr);
        }

        instance.GetFunctionTable().vkDestroySurfaceKHR(instance.GetVulkanInstance(), surface, nullptr);
    }

    /* --- PRIVATE METHODS --- */

    void VulkanSwapchain::RetrieveConstantSettings()
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
            if (presentationSupported)
            {
                presentationQueueFamily = i;
                presentationFamilyFound = true;
                break;
            }
        }
        SR_ERROR_IF(!presentationFamilyFound, "[Vulkan]: Cannot create swapchain [{0}], as no queue family supports presentation!", GetName());

        // Check if general family differs from presentation family, and configure image sharing mode
        const std::vector<uint32> sharedQueueFamilyIndices = { device.GetGeneralQueueFamily(), presentationQueueFamily };
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

        // Retrieve presentation queue
        device.GetFunctionTable().vkGetDeviceQueue(device.GetLogicalDevice(), presentationQueueFamily, 0, &presentationQueue);
    }

    void VulkanSwapchain::CreateSwapchain()
    {
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
        swapchainCreateInfo.imageFormat = selectedFormat.format;
        swapchainCreateInfo.imageColorSpace = selectedFormat.colorSpace;

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
        const uint32 preferredConcurrentFrameCount = std::clamp(3u, surfaceCapabilities.minImageCount, std::max(surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount));

        // Set up base swapchain creation info
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = surface;
        swapchainCreateInfo.minImageCount = preferredConcurrentFrameCount;
        swapchainCreateInfo.imageExtent.width = std::clamp(window->GetFramebufferSize().x, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        swapchainCreateInfo.imageExtent.height = std::clamp(window->GetFramebufferSize().y, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = selectedPresentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.oldSwapchain = swapchain;

        // NOTE: Image sharing mode and queue families are already set

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
    }

    void VulkanSwapchain::CreateImages()
    {
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
                .name = "Image " + std::to_string(i) + " of [" + GetName() + "]",
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
        isImageFreeSemaphores.resize(concurrentFrameCount);
        isImageRenderedSemaphores.resize(concurrentFrameCount);
        isImageUnderWorkFences.resize(concurrentFrameCount);
        for (uint32 i = concurrentFrameCount; i--;)
        {
            SR_ERROR_IF(device.GetFunctionTable().vkCreateSemaphore(device.GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &isImageFreeSemaphores[i]) != VK_SUCCESS ||
                device.GetFunctionTable().vkCreateSemaphore(device.GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &isImageRenderedSemaphores[i]) != VK_SUCCESS ||
                device.GetFunctionTable().vkCreateFence(device.GetLogicalDevice(), &fenceCreateInfo, nullptr, &isImageUnderWorkFences[i]) != VK_SUCCESS,
            "[Vulkan]: Could not create sync objects of swapchain [{0}]!", GetName());
        }
    }

    void VulkanSwapchain::Recreate()
    {
        while (window->IsMinimized())
        {
            window->OnUpdate();
        }

        device.GetFunctionTable().vkDeviceWaitIdle(device.GetLogicalDevice());
        const VkExtent2D lastExtent = swapchainCreateInfo.imageExtent;

        CreateSwapchain();
        CreateImages();

        if (lastExtent.width != swapchainCreateInfo.imageExtent.width || lastExtent.height != swapchainCreateInfo.imageExtent.height) GetSwapchainResizeDispatcher().DispatchEvent(Vector2UInt(swapchainCreateInfo.imageExtent.width, swapchainCreateInfo.imageExtent.height));
    }

}
