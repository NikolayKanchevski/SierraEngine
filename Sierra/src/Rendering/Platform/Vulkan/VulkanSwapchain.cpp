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
        : Swapchain(createInfo), VulkanResource(createInfo.name), instance(instance), device(device), window(createInfo.window), surface(NativeSurface::Create(instance, createInfo.window)), preferredPresentationMode(createInfo.preferredPresentationMode), preferredBuffering(createInfo.preferredBuffering), preferredImageMemoryType(createInfo.preferredImageMemoryType)
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
            return;
        }

        // Set up submit info
        constexpr VkPipelineStageFlags WAIT_STAGE = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        const VkSubmitInfo submitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &isImageAcquiredSemaphores[currentFrame],
            .pWaitDstStageMask = &WAIT_STAGE
        };

        // Wait until swapchain image has been acquired and is ready to be worked on
        result = device.GetFunctionTable().vkQueueSubmit(device.GetGeneralQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not wait for swapchain image [{0}] on swapchain [{1}] to get swapped out! Error code: {2}.", currentFrame, GetName(), static_cast<int32>(result));
    }

    void VulkanSwapchain::Present(CommandBuffer &commandBuffer)
    {
        SR_ERROR_IF(commandBuffer.GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot present swapchain [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer.GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(commandBuffer);

        const uint64 waitValue = vulkanCommandBuffer.GetCompletionSignalValue();
        constexpr uint64 BINARY_SEMAPHORE_SIGNAL_VALUE = 1; // Simply using 1, as we are signalling a binary semaphore

        // Set up semaphore submit info
        const VkTimelineSemaphoreSubmitInfoKHR semaphoreSubmitInfo
        {
            .sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
            .waitSemaphoreValueCount = 1,
            .pWaitSemaphoreValues = &waitValue,
            .signalSemaphoreValueCount = 1,
            .pSignalSemaphoreValues = &BINARY_SEMAPHORE_SIGNAL_VALUE
        };

        // Set up submit info
        constexpr VkPipelineStageFlags WAIT_STAGE = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkSemaphore waitSemaphore = device.GetGeneralTimelineSemaphore();

        const VkSubmitInfo submitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = &semaphoreSubmitInfo,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &waitSemaphore,
            .pWaitDstStageMask = &WAIT_STAGE,
            .commandBufferCount = 0,
            .pCommandBuffers = nullptr,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &isPresentationCommandBufferFreeSemaphores[currentFrame]
        };

        // Wait for timeline semaphore to signal, and signal the binary one as well, as VkPresentInfoKHR forbids passing timeline one to it
        VkResult result = device.GetFunctionTable().vkQueueSubmit(device.GetGeneralQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not wait for timeline semaphore on swapchain [{1}]! Error code: {2}.", commandBuffer.GetName(), GetName(), static_cast<int32>(result));

        if (presentationQueueFamily != device.GetGeneralQueueFamily())
        {
            const VulkanImage &swapchainImage = static_cast<const VulkanImage&>(*swapchainImages[currentImage]);
            const VkImageMemoryBarrier pipelineBarrier
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_NONE,
                .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .srcQueueFamilyIndex = device.GetGeneralQueueFamily(),
                .dstQueueFamilyIndex = presentationQueueFamily,
                .image = swapchainImage.GetVulkanImage(),
                .subresourceRange = {
                    .aspectMask = swapchainImage.GetVulkanAspectFlags(),
                    .baseMipLevel = 0,
                    .levelCount = swapchainImage.GetLevelCount(),
                    .baseArrayLayer = 0,
                    .layerCount = swapchainImage.GetLayerCount()
                }
            };

            // Transfer ownership of image over to presentation queue
            device.GetFunctionTable().vkCmdPipelineBarrier(vulkanCommandBuffer.GetVulkanCommandBuffer(), VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &pipelineBarrier);
        }

        // Set up presentation info
        const VkPresentInfoKHR presentInfo
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &isPresentationCommandBufferFreeSemaphores[currentFrame],
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &currentImage,
            .pResults = nullptr
        };

        // Submit presentation queue and resize the swapchain if needed
        result = device.GetFunctionTable().vkQueuePresentKHR(presentationQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            Recreate();
            return;
        }

        // Increment currentFrame
        currentFrame = (currentFrame + 1) % concurrentFrameCount;
    }

    /* --- DESTRUCTOR --- */

    VulkanSwapchain::~VulkanSwapchain()
    {
        device.GetFunctionTable().vkDestroySwapchainKHR(device.GetLogicalDevice(), swapchain, nullptr);

        for (size i = 0; i < concurrentFrameCount; i++)
        {
            device.GetFunctionTable().vkDestroySemaphore(device.GetLogicalDevice(), isImageAcquiredSemaphores[i], nullptr);
            device.GetFunctionTable().vkDestroySemaphore(device.GetLogicalDevice(), isPresentationCommandBufferFreeSemaphores[i], nullptr);
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
        for (uint32 i = 0; i < queueFamilyPropertiesCount; i++)
        {
            // Check if current family supports presentation
            VkBool32 presentationSupported = VK_FALSE;
            instance.GetFunctionTable().vkGetPhysicalDeviceSurfaceSupportKHR(device.GetPhysicalDevice(), i, surface, &presentationSupported);

            // Save family
            if (presentationSupported == VK_TRUE)
            {
                presentationQueueFamily = i;
                break;
            }
        }
        SR_ERROR_IF(presentationQueueFamily == std::numeric_limits<uint32>::max(), "[Vulkan]: Cannot create swapchain [{0}], as no queue family supports presentation!", GetName());
        const std::vector<uint32> sharedQueueFamilyIndices = { device.GetGeneralQueueFamily(), presentationQueueFamily };

        // Retrieve presentation queue
        device.GetFunctionTable().vkGetDeviceQueue(device.GetLogicalDevice(), presentationQueueFamily, 0, &presentationQueue);

        // Retrieve supported formats
        uint32 supportedFormatCount = 0;
        instance.GetFunctionTable().vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysicalDevice(), surface, &supportedFormatCount, nullptr);

        std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatCount);
        instance.GetFunctionTable().vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysicalDevice(), surface, &supportedFormatCount, supportedFormats.data());
        SR_ERROR_IF(supportedFormats.empty(), "[Vulkan] Could not create swapchain [{0}], as it does not support any surface formats!", GetName());

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
        VkSurfaceFormatKHR selectedFormat = { .format = supportedFormats[0].format, .colorSpace = supportedFormats[0].colorSpace };
        for (const VkFormat format : formatsToTry)
        {
            if (auto iterator = std::ranges::find_if(supportedFormats, [format](const VkSurfaceFormatKHR item) -> bool { return item.format == format && item.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; }); iterator != supportedFormats.end())
            {
                selectedFormat.format = format;
                break;
            }
        }

        // Retrieve supported present modes
        uint32 supportedPresentModeCount = 0;
        instance.GetFunctionTable().vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysicalDevice(), surface, &supportedPresentModeCount, nullptr);

        std::vector<VkPresentModeKHR> supportedPresentModes(supportedPresentModeCount);
        instance.GetFunctionTable().vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysicalDevice(), surface, &supportedFormatCount, supportedPresentModes.data());

        // Select a present mode according to preferred one in create info
        VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR; // FIFO is guaranteed to be supported
        for (const VkPresentModeKHR presentMode : supportedPresentModes)
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
        uint32 preferredConcurrentFrameCount = 0;
        switch (preferredBuffering)
        {
            case SwapchainBuffering::DoubleBuffering:       { preferredConcurrentFrameCount = 2; break; }
            case SwapchainBuffering::TripleBuffering:       { preferredConcurrentFrameCount = 3; break; }
        }
        preferredConcurrentFrameCount = glm::clamp(preferredConcurrentFrameCount, surfaceCapabilities.minImageCount, glm::max(surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount));

        // Set up swapchain creation info
        const VkSwapchainCreateInfoKHR swapchainCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = surface,
            .minImageCount = preferredConcurrentFrameCount,
            .imageFormat = selectedFormat.format,
            .imageColorSpace = selectedFormat.colorSpace,
            .imageExtent = {
                .width = glm::clamp(window.GetFramebufferWidth(), surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width),
                .height = glm::clamp(window.GetFramebufferHeight(), surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height)
            },
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = static_cast<uint32_t>(static_cast<uint32>(device.GetGeneralQueueFamily() != presentationQueueFamily) * sharedQueueFamilyIndices.size()),
            .pQueueFamilyIndices = sharedQueueFamilyIndices.data(),
            .preTransform = surfaceCapabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = selectedPresentMode,
            .clipped = VK_TRUE,
            .oldSwapchain = swapchain
        };

        // Create swapchain
        const VkResult result = device.GetFunctionTable().vkCreateSwapchainKHR(device.GetLogicalDevice(), &swapchainCreateInfo, nullptr, &swapchain);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create swapchain [{0}]! Error code: {1}.", GetName(), static_cast<int32>(result));

        // Set object name
        device.SetResourceName(swapchain, VK_OBJECT_TYPE_SWAPCHAIN_KHR, GetName());

        // If an old swapchain was reused, destroy that
        if (swapchainCreateInfo.oldSwapchain != VK_NULL_HANDLE)
        {
            device.GetFunctionTable().vkDestroySwapchainKHR(device.GetLogicalDevice(), swapchainCreateInfo.oldSwapchain, nullptr);
        }

        // Get actual concurrent image count
        concurrentFrameCount = 0;
        device.GetFunctionTable().vkGetSwapchainImagesKHR(device.GetLogicalDevice(), swapchain, &concurrentFrameCount, nullptr);

        // Get swapchain images
        std::vector<VkImage> vulkanSwapchainImages(concurrentFrameCount);
        device.GetFunctionTable().vkGetSwapchainImagesKHR(device.GetLogicalDevice(), swapchain, &concurrentFrameCount, vulkanSwapchainImages.data());

        // Create image implementations
        swapchainImages.resize(concurrentFrameCount);
        for (size i = 0; i < concurrentFrameCount; i++)
        {
            swapchainImages[i] = std::unique_ptr<VulkanImage>(new VulkanImage(device, VulkanImage::SwapchainImageCreateInfo {
                .name = fmt::format("Image [{0}] of swapchain [{1}]", i, GetName()),
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
        constexpr VkSemaphoreCreateInfo semaphoreCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };

        // Create sync objects
        isImageAcquiredSemaphores.resize(concurrentFrameCount);
        isPresentationCommandBufferFreeSemaphores.resize(concurrentFrameCount);

        VkResult result;
        for (size i = 0; i < concurrentFrameCount; i++)
        {
            result = device.GetFunctionTable().vkCreateSemaphore(device.GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &isImageAcquiredSemaphores[i]);
            SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create semaphore [{0}], indicating whether corresponding swapchain image of swapchain [{1}] is ready to be used!", i, GetName());
            device.SetResourceName(isImageAcquiredSemaphores[i], VK_OBJECT_TYPE_SEMAPHORE, fmt::format("Image free semaphore [{0}] of swapchain [{1}]", i, GetName()));

            result = device.GetFunctionTable().vkCreateSemaphore(device.GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &isPresentationCommandBufferFreeSemaphores[i]);
            SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create semaphore [{0}], indicating whether presentation command buffer is ready to present swapchain [{1}]!", i, GetName());
            device.SetResourceName(isPresentationCommandBufferFreeSemaphores[i], VK_OBJECT_TYPE_SEMAPHORE, fmt::format("Presentation command buffer ready semaphore [{0}] of swapchain [{1}]", i, GetName()));
        }
    }

    void VulkanSwapchain::Recreate()
    {
        while (window.IsMinimized())
        {
            window.Update();
        }

        device.GetFunctionTable().vkQueueWaitIdle(presentationQueue);
        const Vector2UInt lastSize = { swapchainImages[0]->GetWidth(), swapchainImages[0]->GetHeight() };

        CreateSwapchain();
        currentFrame = 0;

        if (const Vector2UInt newSize = { swapchainImages[0]->GetWidth(), swapchainImages[0]->GetHeight() }; lastSize != newSize) GetSwapchainResizeDispatcher().DispatchEvent(newSize.x, newSize.y, GetScaling());
    }

}
