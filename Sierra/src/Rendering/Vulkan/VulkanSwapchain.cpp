//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "VulkanSwapchain.h"

#if SR_PLATFORM_WINDOWS
    #include "Windows/VulkanWin32Surface.h"
    constexpr VkSurfaceKHR (*vkCreateSurfaceKHR)(const Sierra::VulkanInstance&, const Sierra::Window&) = &Sierra::CreateVulkanWin32Surface;
#elif SR_PLATFORM_LINUX
    #include "Linux/VulkanX11Surface.h"
    constexpr VkSurfaceKHR (*vkCreateSurfaceKHR)(const Sierra::VulkanInstance&, const Sierra::Window&) = &Sierra::CreateVulkanX11Surface;
#elif SR_PLATFORM_APPLE
    #include "Apple/VulkanMetalSurface.h"
    constexpr VkSurfaceKHR (*vkCreateSurfaceKHR)(const Sierra::VulkanInstance&, const Sierra::Window&) = &Sierra::CreateVulkanMetalSurface;
#elif SR_PLATFORM_ANDROID
    #include "Android/VulkanGameKitSurface.h"
    constexpr VkSurfaceKHR (*vkCreateSurfaceKHR)(const Sierra::VulkanInstance&, const Sierra::Window&) = &Sierra::CreateVulkanGameKitSurface;
#endif

#include "VulkanCommandBuffer.h"
#include "VulkanErrorHandler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanSwapchain::VulkanSwapchain(const VulkanInstance& givenInstance, const VulkanDevice& givenDevice, const SwapchainCreateInfo& createInfo)
        : VulkanResource(createInfo.name), Swapchain(createInfo), instance(&givenInstance), device(&givenDevice), window(&createInfo.window), surface(vkCreateSurfaceKHR(givenInstance, createInfo.window)), preferredPresentationMode(createInfo.preferredPresentationMode), preferredBuffering(createInfo.preferredBuffering), preferredImageMemoryType(createInfo.preferredImageMemoryType)
    {
        SR_THROW_IF(!device->IsExtensionLoaded(VK_KHR_SWAPCHAIN_EXTENSION_NAME), UnsupportedFeatureError(SR_FORMAT("Device [{0}] cannot create resource table [{1}]", device->GetName(), createInfo.name)));

        CreateSwapchain();
        CreateSynchronization();
    }

    /* --- POLLING METHODS --- */

    void VulkanSwapchain::AcquireNextImage()
    {
        SR_THROW_IF(window->IsClosed(), InvalidOperationError(SR_FORMAT("Cannot acquire next image of swapchain [{0}], as its corresponding window [{1}] has been closed", GetName(), window->GetTitle())));

        // Acquire next image
        VkResult result = device->GetFunctionTable().vkAcquireNextImageKHR(device->GetVulkanDevice(), swapchain, std::numeric_limits<uint64>::max(), isImageAcquiredSemaphores[currentFrame], VK_NULL_HANDLE, &currentImage);

        // Resize swapchain if needed
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            Recreate();

            // Try to re-acquire next image
            result = device->GetFunctionTable().vkAcquireNextImageKHR(device->GetVulkanDevice(), swapchain, std::numeric_limits<uint64>::max(), isImageAcquiredSemaphores[currentFrame], VK_NULL_HANDLE, &currentImage);
            if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not acquire image [{0}] of swapchain [{1}]", currentImage, GetName()));
        }

        constexpr VkPipelineStageFlags IMAGE_ACQUIRE_WAIT_STAGE = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        const VkSubmitInfo imageAcquireWaitSubmitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &isImageAcquiredSemaphores[currentFrame],
            .pWaitDstStageMask = &IMAGE_ACQUIRE_WAIT_STAGE
        };

        // Wait until swapchain image has been acquired
        result = device->GetFunctionTable().vkQueueSubmit(presentationQueue, 1, &imageAcquireWaitSubmitInfo, VK_NULL_HANDLE);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Cannot present swapchain [{0}], as awaiting image [{1}]'s swap-out failed", GetName(), currentImage));
    }

    void VulkanSwapchain::Present(CommandBuffer& commandBuffer)
    {
        SR_THROW_IF(window->IsClosed(), InvalidOperationError(SR_FORMAT("Cannot present swapchain [{0}], as its corresponding window [{1}] has been closed", GetName(), window->GetTitle())));

        SR_THROW_IF(commandBuffer.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot present swapchain [{0}] using command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", GetName(), commandBuffer.GetName())));
        const VulkanCommandBuffer& vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(commandBuffer);

        const uint64 commandBufferWaitValue = vulkanCommandBuffer.GetCompletionSemaphoreSignalValue();
        constexpr uint64 BINARY_SEMAPHORE_SIGNAL_VALUE = 1; // Simply using 1, as we are signalling a binary semaphore

        // Set up semaphore submit info
        const VkTimelineSemaphoreSubmitInfoKHR commandBufferSignalSubmitInfo
        {
            .sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
            .waitSemaphoreValueCount = 1,
            .pWaitSemaphoreValues = &commandBufferWaitValue,
            .signalSemaphoreValueCount = 1,
            .pSignalSemaphoreValues = &BINARY_SEMAPHORE_SIGNAL_VALUE
        };

        // Set up submit info
        constexpr VkPipelineStageFlags COMMAND_BUFFER_WAIT_STAGE = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSemaphore waitSemaphore = device->GetSemaphore();

        const VkSubmitInfo commandBufferWaitSubmitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = &commandBufferSignalSubmitInfo,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &waitSemaphore,
            .pWaitDstStageMask = &COMMAND_BUFFER_WAIT_STAGE,
            .commandBufferCount = 0,
            .pCommandBuffers = nullptr,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &isPresentationCommandBufferFreeSemaphores[currentFrame]
        };

        // Wait for command buffer to signal timeline semaphore, and immediately signal binary one
        VkResult result = device->GetFunctionTable().vkQueueSubmit(presentationQueue, 1, &commandBufferWaitSubmitInfo, VK_NULL_HANDLE);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Cannot present swapchain [{0}], as awaiting command buffer [{1}]'s end of execution failed", GetName(), vulkanCommandBuffer.GetName()));

        if (presentationQueueFamily != vulkanCommandBuffer.GetQueueFamily())
        {
            const VulkanImage& swapchainImage = *swapchainImages[currentImage];
            const VkImageMemoryBarrier pipelineBarrier
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_NONE,
                .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .srcQueueFamilyIndex = vulkanCommandBuffer.GetQueueFamily(),
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
            device->GetFunctionTable().vkCmdPipelineBarrier(vulkanCommandBuffer.GetVulkanCommandBuffer(), VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &pipelineBarrier);
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
        result = device->GetFunctionTable().vkQueuePresentKHR(presentationQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            Recreate();
            return;
        }
        else if (result != VK_SUCCESS)
        {
            HandleVulkanError(result, SR_FORMAT("Could not present swapchain [{0}]! Error: {1}", GetName(), static_cast<int32>(result)));
        }

        // Increment currentFrame
        currentFrame = (currentFrame + 1) % concurrentFrameCount;
    }

    /* --- GETTER METHODS --- */

    float32 VulkanSwapchain::GetScaling() const noexcept
    {
        return glm::max(1.0f, static_cast<float32>(swapchainImages[currentImage]->GetWidth()) / static_cast<float32>(window->GetWidth()));
    }

    const Image& VulkanSwapchain::GetImage(const uint32 frameIndex) const
    {
        SR_THROW_IF(frameIndex >= concurrentFrameCount, ValueOutOfRangeError(SR_FORMAT("Cannot get image [{0}] of swapchain [{1}]! Use Swapchain::GetConcurrentFrameCount() to query count", frameIndex, GetName()), frameIndex, static_cast<uint32>(0), GetConcurrentFrameCount() - 1));
        return *swapchainImages[frameIndex];
    }

    /* --- POLLING METHODS --- */

    void VulkanSwapchain::CreateSwapchain()
    {
        // Retrieve queue family properties count
        uint32 queueFamilyPropertiesCount = 0;
        instance->GetFunctionTable().vkGetPhysicalDeviceQueueFamilyProperties(device->GetVulkanPhysicalDevice(), &queueFamilyPropertiesCount, nullptr);

        // Retrieve queue family properties
        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        instance->GetFunctionTable().vkGetPhysicalDeviceQueueFamilyProperties(device->GetVulkanPhysicalDevice(), &queueFamilyPropertiesCount, queueFamilyProperties.data());

        // Try to find a queue family, which supports presentation for the surface
        for (uint32 i = 0; i < queueFamilyPropertiesCount; i++)
        {
            // Check if current family supports presentation
            VkBool32 presentationSupported = VK_FALSE;
            instance->GetFunctionTable().vkGetPhysicalDeviceSurfaceSupportKHR(device->GetVulkanPhysicalDevice(), i, surface, &presentationSupported);

            // Save family
            if (presentationSupported == VK_TRUE)
            {
                presentationQueueFamily = i;
                break;
            }
        }
        SR_THROW_IF(presentationQueueFamily == std::numeric_limits<uint32>::max(), UnsupportedFeatureError(SR_FORMAT("Device [{0}] cannot create swapchain [{1}], as it does not support presentation", GetName(), device->GetName())));

        // Retrieve presentation queue
        device->GetFunctionTable().vkGetDeviceQueue(device->GetVulkanDevice(), presentationQueueFamily, 0, &presentationQueue);

        // Retrieve supported format count
        uint32 supportedFormatCount = 0;
        instance->GetFunctionTable().vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetVulkanPhysicalDevice(), surface, &supportedFormatCount, nullptr);
        SR_THROW_IF(supportedFormatCount == 0, UnsupportedFeatureError(SR_FORMAT("Cannot create swapchain [{0}], as it does not support any valid surface formats", GetName())));

        // Retrieve supported formats
        std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatCount);
        instance->GetFunctionTable().vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetVulkanPhysicalDevice(), surface, &supportedFormatCount, supportedFormats.data());

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
            const auto iterator = std::find_if(supportedFormats.begin(), supportedFormats.end(), [format](const VkSurfaceFormatKHR surfaceFormat) -> bool { return surfaceFormat.format == format && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; });
            if (iterator != supportedFormats.end())
            {
                selectedFormat.format = format;
                break;
            }
        }

        // Retrieve supported present mode count
        uint32 supportedPresentModeCount = 0;
        instance->GetFunctionTable().vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetVulkanPhysicalDevice(), surface, &supportedPresentModeCount, nullptr);

        // Retrieve supported present modes
        std::vector<VkPresentModeKHR> supportedPresentModes(supportedPresentModeCount);
        instance->GetFunctionTable().vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetVulkanPhysicalDevice(), surface, &supportedFormatCount, supportedPresentModes.data());

        // Select a present mode in regard to requested
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
        instance->GetFunctionTable().vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetVulkanPhysicalDevice(), surface, &surfaceCapabilities);

        // Determine concurrent image count in regard to requested
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
                .width = glm::clamp(window->GetFramebufferWidth(), surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width),
                .height = glm::clamp(window->GetFramebufferHeight(), surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height)
            },
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .preTransform = surfaceCapabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = selectedPresentMode,
            .clipped = VK_TRUE,
            .oldSwapchain = swapchain
        };

        // Create swapchain
        const VkResult result = device->GetFunctionTable().vkCreateSwapchainKHR(device->GetVulkanDevice(), &swapchainCreateInfo, nullptr, &swapchain);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create swapchain [{0}]", GetName()));

        // Set object name
        device->SetResourceName(swapchain, VK_OBJECT_TYPE_SWAPCHAIN_KHR, GetName());

        // If an old swapchain was reused, destroy that
        if (swapchainCreateInfo.oldSwapchain != VK_NULL_HANDLE)
        {
            device->GetFunctionTable().vkDestroySwapchainKHR(device->GetVulkanDevice(), swapchainCreateInfo.oldSwapchain, nullptr);
        }

        // Get actual concurrent image count
        concurrentFrameCount = 0;
        device->GetFunctionTable().vkGetSwapchainImagesKHR(device->GetVulkanDevice(), swapchain, &concurrentFrameCount, nullptr);

        // Get swapchain images
        std::vector<VkImage> vulkanSwapchainImages(concurrentFrameCount);
        device->GetFunctionTable().vkGetSwapchainImagesKHR(device->GetVulkanDevice(), swapchain, &concurrentFrameCount, vulkanSwapchainImages.data());

        // Create image implementations
        swapchainImages.resize(concurrentFrameCount);
        for (size i = 0; i < concurrentFrameCount; i++)
        {
            swapchainImages[i] = std::unique_ptr<VulkanImage>(new VulkanImage(*device, VulkanImage::SwapchainImageCreateInfo {
                .name = SR_FORMAT("Image [{0}] of swapchain [{1}]", i, GetName()),
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

        for (size i = 0; i < concurrentFrameCount; i++)
        {
            VkResult result = device->GetFunctionTable().vkCreateSemaphore(device->GetVulkanDevice(), &semaphoreCreateInfo, nullptr, &isImageAcquiredSemaphores[i]);
            if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Cannot create swapchain [{0}], as creation of semaphore indicating whether corresponding swapchain image is ready to be used failed", GetName()));
            device->SetResourceName(isImageAcquiredSemaphores[i], VK_OBJECT_TYPE_SEMAPHORE, SR_FORMAT("Image free semaphore [{0}] of swapchain [{1}]", i, GetName()));

            result = device->GetFunctionTable().vkCreateSemaphore(device->GetVulkanDevice(), &semaphoreCreateInfo, nullptr, &isPresentationCommandBufferFreeSemaphores[i]);
            if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Cannot create swapchain [{0}], as creation of semaphore indicating whether drawing command buffer has finished failed", GetName()));
            device->SetResourceName(isPresentationCommandBufferFreeSemaphores[i], VK_OBJECT_TYPE_SEMAPHORE, SR_FORMAT("Presentation command buffer ready semaphore [{0}] of swapchain [{1}]", i, GetName()));
        }
    }

    void VulkanSwapchain::Recreate()
    {
        while (window->IsMinimized())
        {
            window->Update();
        }

        device->GetFunctionTable().vkQueueWaitIdle(presentationQueue);
        const Vector2UInt lastSize = { swapchainImages[0]->GetWidth(), swapchainImages[0]->GetHeight() };

        CreateSwapchain();
        currentFrame = 0;

        if (const Vector2UInt newSize = { swapchainImages[0]->GetWidth(), swapchainImages[0]->GetHeight() }; lastSize != newSize) GetSwapchainResizeDispatcher().DispatchEvent(newSize.x, newSize.y, GetScaling());
    }

    /* --- DESTRUCTOR --- */

    VulkanSwapchain::~VulkanSwapchain() noexcept
    {
        device->GetFunctionTable().vkDestroySwapchainKHR(device->GetVulkanDevice(), swapchain, nullptr);

        for (size i = 0; i < concurrentFrameCount; i++)
        {
            device->GetFunctionTable().vkDestroySemaphore(device->GetVulkanDevice(), isImageAcquiredSemaphores[i], nullptr);
            device->GetFunctionTable().vkDestroySemaphore(device->GetVulkanDevice(), isPresentationCommandBufferFreeSemaphores[i], nullptr);
        }

        instance->GetFunctionTable().vkDestroySurfaceKHR(instance->GetVulkanInstance(), surface, nullptr);
    }

}
