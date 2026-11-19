//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "VulkanSwapchain.h"

#if SR_PLATFORM_WINDOWS
    #include "Windows/VulkanWin32Surface.h"
    constexpr VkSurfaceKHR (*vkCreateSurfaceKHR)(const Sierra::VulkanInstance&, const Sierra::Window&) = &Sierra::CreateVulkanWin32Surface;
#elif SR_PLATFORM_LINUX
    #include "Linux/VulkanX11Surface.h"
    #include "Linux/VulkanWaylandSurface.h"
    namespace Sierra
    {
        namespace
        {
            // Windowing backend is only known at runtime, so the surface is created for whichever one the window belongs to
            VkSurfaceKHR CreateVulkanLinuxSurface(const VulkanInstance& instance, const Window& window)
            {
                if (window.GetBackendType() == WindowingBackendType::Wayland) return CreateVulkanWaylandSurface(instance, window);
                return CreateVulkanX11Surface(instance, window);
            }
        }
    }
    constexpr VkSurfaceKHR (*vkCreateSurfaceKHR)(const Sierra::VulkanInstance&, const Sierra::Window&) = &Sierra::CreateVulkanLinuxSurface;
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
        : Swapchain(createInfo), name(createInfo.name), instance(&givenInstance), device(&givenDevice), window(&createInfo.window), surface(vkCreateSurfaceKHR(givenInstance, createInfo.window)), preferredPresentationMode(createInfo.preferredPresentationMode), preferredBuffering(createInfo.preferredBuffering), preferredImageMemoryType(createInfo.preferredImageMemoryType)
    {
        SR_THROW_IF(!device->IsExtensionLoaded(VK_KHR_SWAPCHAIN_EXTENSION_NAME), UnsupportedFeatureError(SR_FORMAT("Device [{0}] cannot create resource table [{1}]", device->GetName(), createInfo.name)));

        CreateSwapchain();
        CreateSynchronization();
    }

    /* --- POLLING METHODS --- */

    void VulkanSwapchain::AcquireNextImage()
    {
        SR_THROW_IF(window->IsClosed(), InvalidOperationError(SR_FORMAT("Cannot acquire next image of swapchain [{0}], as its corresponding window [{1}] has been closed", GetName(), window->GetTitle())));

        // Drivers do not always report a change of framebuffer (such as when the scale of a Wayland window changes), so the swapchain is checked against it, and recreated if it differs
        if (window->GetFramebufferSize() != requestedFramebufferSize) Recreate();

        // The acquisition semaphore of the frame was last waited for frames ago, which must have happened for it to be signalled again
        if (const VulkanSemaphoreValue& previousAcquisition = imageAcquisitionSemaphoreValues[currentFrame]; previousAcquisition.semaphore != VK_NULL_HANDLE)
        {
            const VkSemaphoreWaitInfo waitInfo
            {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
                .semaphoreCount = 1,
                .pSemaphores = &previousAcquisition.semaphore,
                .pValues = &previousAcquisition.value
            };

            const VkResult result = device->GetFunctionTable().vkWaitSemaphores(device->GetVulkanDevice(), &waitInfo, std::numeric_limits<uint64>::max());
            if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not acquire next image of swapchain [{0}], as waiting for its previous acquisition failed", GetName()));
        }

        // Acquire next image (a suboptimal swapchain can still be presented to, and is recreated after presenting)
        VkResult result = device->GetFunctionTable().vkAcquireNextImageKHR(device->GetVulkanDevice(), swapchain, std::numeric_limits<uint64>::max(), isImageAcquiredSemaphores[currentFrame], VK_NULL_HANDLE, &currentImage);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            Recreate();

            // Try to re-acquire next image
            result = device->GetFunctionTable().vkAcquireNextImageKHR(device->GetVulkanDevice(), swapchain, std::numeric_limits<uint64>::max(), isImageAcquiredSemaphores[currentFrame], VK_NULL_HANDLE, &currentImage);
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) HandleVulkanError(result, SR_FORMAT("Could not acquire next image of swapchain [{0}]", GetName()));

        // The semaphore can only be waited for once, while any number of submissions may use the image, so it is converted into a point on the timeline of the presentation queue, which they wait for instead (see VulkanCommandBuffer::WaitForImageAcquisition())
        const VulkanSemaphoreValue imageAcquiredSemaphoreValue = { .semaphore = isImageAcquiredSemaphores[currentFrame] };
        VulkanSemaphoreValue acquisitionSemaphoreValue = { };

        result = device->SubmitToQueue({ .family = presentationQueueFamily, .semaphoresToWait = { &imageAcquiredSemaphoreValue, 1 } }, acquisitionSemaphoreValue);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not acquire next image of swapchain [{0}], as waiting for image [{1}] to be acquired failed", GetName(), currentImage));

        imageAcquisitionSemaphoreValues[currentFrame] = acquisitionSemaphoreValue;
        swapchainImages[currentImage]->acquisitionSemaphoreValue = acquisitionSemaphoreValue;
    }

    void VulkanSwapchain::Present(CommandBuffer& commandBuffer)
    {
        SR_THROW_IF(window->IsClosed(), InvalidOperationError(SR_FORMAT("Cannot present swapchain [{0}], as its corresponding window [{1}] has been closed", GetName(), window->GetTitle())));

        SR_THROW_IF(commandBuffer.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot present swapchain [{0}] using command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", GetName(), commandBuffer.GetName())));
        const VulkanCommandBuffer& vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(commandBuffer);

        const VulkanSemaphoreValue& completionSemaphoreValue = vulkanCommandBuffer.GetCompletionSemaphoreValue();
        SR_THROW_IF(completionSemaphoreValue.semaphore == VK_NULL_HANDLE, InvalidOperationError(SR_FORMAT("Cannot present swapchain [{0}] using command buffer [{1}], as it has never been submitted", GetName(), vulkanCommandBuffer.GetName())));

        // NOTE: The image needs no transfer of ownership between queue families, as it is shared by all of them (see CreateSwapchain())

        // Presentation can only wait for binary semaphores, so completion of the command buffer is converted into the one of the image
        const VkSemaphore isPresentationCommandBufferFreeSemaphore = isPresentationCommandBufferFreeSemaphores[currentImage];
        VulkanSemaphoreValue unusedSemaphoreValue = { };

        VkResult result = device->SubmitToQueue({ .family = presentationQueueFamily, .semaphoresToWait = { &completionSemaphoreValue, 1 }, .binarySemaphoresToSignal = { &isPresentationCommandBufferFreeSemaphore, 1 } }, unusedSemaphoreValue);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Cannot present swapchain [{0}], as awaiting command buffer [{1}]'s end of execution failed", GetName(), vulkanCommandBuffer.GetName()));

        // Set up presentation info
        const VkPresentInfoKHR presentInfo
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &isPresentationCommandBufferFreeSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &currentImage,
            .pResults = nullptr
        };

        // Submit presentation queue and resize the swapchain if needed
        result = device->PresentToQueue(presentationQueueFamily, presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            Recreate();
            return;
        }

        if (result != VK_SUCCESS)
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
        // Find a family the device has a queue of, which supports presentation to the surface (preferably one that also supports graphics, as that is where rendering to it most likely happens)
        presentationQueueFamily = std::numeric_limits<uint32>::max();
        for (const std::shared_ptr<VulkanQueueDescription>& queueDescription : device->GetQueueDescriptions())
        {
            VkBool32 presentationSupported = VK_FALSE;
            instance->GetFunctionTable().vkGetPhysicalDeviceSurfaceSupportKHR(device->GetVulkanPhysicalDevice(), queueDescription->family, surface, &presentationSupported);
            if (presentationSupported == VK_FALSE) continue;

            if (presentationQueueFamily == std::numeric_limits<uint32>::max() || queueDescription->operations & QueueOperations::Graphics) presentationQueueFamily = queueDescription->family;
            if (queueDescription->operations & QueueOperations::Graphics) break;
        }
        SR_THROW_IF(presentationQueueFamily == std::numeric_limits<uint32>::max(), UnsupportedFeatureError(SR_FORMAT("Device [{0}] cannot create swapchain [{1}], as it does not support presentation", GetName(), device->GetName())));

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
        instance->GetFunctionTable().vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetVulkanPhysicalDevice(), surface, &supportedPresentModeCount, supportedPresentModes.data());

        #if !SR_PLATFORM_MOBILE
            constexpr VkPresentModeKHR VSYNC_PRESENT_MODE = VK_PRESENT_MODE_MAILBOX_KHR; // VSync with no tearing, no GPU idling, low latency, and high energy consumption
        #else
            constexpr VkPresentModeKHR VSYNC_PRESENT_MODE = VK_PRESENT_MODE_FIFO_KHR; // VSync with no tearing, low GPU idling, moderate latency, minimal energy consumption
        #endif

        // Select a present mode in regard to requested
        VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR; // FIFO is guaranteed to be supported
        for (const VkPresentModeKHR presentMode : supportedPresentModes)
        {
            if ((preferredPresentationMode == SwapchainPresentationMode::Immediate && presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) || (preferredPresentationMode == SwapchainPresentationMode::VSync && presentMode == VSYNC_PRESENT_MODE)) // Immediate has possible tearing, no GPU idling, minimal latency, and high energy consumption
            {
                selectedPresentMode = presentMode;
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

        // Remember which framebuffer the swapchain is created for, so that a change of it is noticed
        requestedFramebufferSize = window->GetFramebufferSize();

        // Images are shared by every family the device has queues of, as rendering to them may happen on a different one than presenting them does, which would otherwise require transferring their ownership in between
        std::vector<uint32> queueFamilies = { };
        for (const std::shared_ptr<VulkanQueueDescription>& queueDescription : device->GetQueueDescriptions())
        {
            if (std::ranges::find(queueFamilies, queueDescription->family) == queueFamilies.end()) queueFamilies.emplace_back(queueDescription->family);
        }

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
            .imageSharingMode = queueFamilies.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = queueFamilies.size() > 1 ? static_cast<uint32>(queueFamilies.size()) : 0,
            .pQueueFamilyIndices = queueFamilies.size() > 1 ? queueFamilies.data() : nullptr,
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

        // Create acquisition semaphores (one per frame)
        isImageAcquiredSemaphores.resize(concurrentFrameCount);
        imageAcquisitionSemaphoreValues.assign(concurrentFrameCount, { });
        for (size i = 0; i < isImageAcquiredSemaphores.size(); i++)
        {
            const VkResult result = device->GetFunctionTable().vkCreateSemaphore(device->GetVulkanDevice(), &semaphoreCreateInfo, nullptr, &isImageAcquiredSemaphores[i]);
            if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Cannot create swapchain [{0}], as creation of semaphore indicating whether corresponding swapchain image is ready to be used failed", GetName()));
            device->SetResourceName(isImageAcquiredSemaphores[i], VK_OBJECT_TYPE_SEMAPHORE, SR_FORMAT("Image free semaphore [{0}] of swapchain [{1}]", i, GetName()));
        }

        // Create presentation semaphores (one per image)
        isPresentationCommandBufferFreeSemaphores.resize(swapchainImages.size());
        for (size i = 0; i < isPresentationCommandBufferFreeSemaphores.size(); i++)
        {
            const VkResult result = device->GetFunctionTable().vkCreateSemaphore(device->GetVulkanDevice(), &semaphoreCreateInfo, nullptr, &isPresentationCommandBufferFreeSemaphores[i]);
            if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Cannot create swapchain [{0}], as creation of semaphore indicating whether drawing command buffer has finished failed", GetName()));
            device->SetResourceName(isPresentationCommandBufferFreeSemaphores[i], VK_OBJECT_TYPE_SEMAPHORE, SR_FORMAT("Presentation command buffer ready semaphore of image [{0}] of swapchain [{1}]", i, GetName()));
        }
    }

    void VulkanSwapchain::DestroySynchronization()
    {
        for (const VkSemaphore semaphore : isImageAcquiredSemaphores) device->GetFunctionTable().vkDestroySemaphore(device->GetVulkanDevice(), semaphore, nullptr);
        isImageAcquiredSemaphores.clear();
        imageAcquisitionSemaphoreValues.clear();

        for (const VkSemaphore semaphore : isPresentationCommandBufferFreeSemaphores) device->GetFunctionTable().vkDestroySemaphore(device->GetVulkanDevice(), semaphore, nullptr);
        isPresentationCommandBufferFreeSemaphores.clear();
    }

    /* --- DESTRUCTOR --- */
    void VulkanSwapchain::Recreate()
    {
        while (window->IsMinimized() && !window->IsClosed())
        {
            window->Update();
        }

        // Window may have been closed while it was being waited on (such as from the taskbar, while minimized), after which there is nothing left to present to
        if (window->IsClosed()) return;

        // Rendering to, and presenting, the images (as well as converting semaphores of them) may happen on any queue, all of which must be done using them, and their semaphores, before they are replaced
        device->WaitForQueuesIdle();
        DestroySynchronization();

        // The image count may differ, which the semaphores of every image depend on
        CreateSwapchain();
        CreateSynchronization();
        currentFrame = 0;

        const uint32 width = swapchainImages[0]->GetWidth();
        const uint32 height = swapchainImages[0]->GetHeight();
        GetSwapchainResizeDispatcher().DispatchEvent(width, height, GetScaling());
    }

    VulkanSwapchain::~VulkanSwapchain() noexcept
    {
        // Presentations, and conversions of semaphores, are not part of any command buffer, so they are not covered by waiting for those
        device->WaitForQueuesIdle();

        device->GetFunctionTable().vkDestroySwapchainKHR(device->GetVulkanDevice(), swapchain, nullptr);
        DestroySynchronization();

        instance->GetFunctionTable().vkDestroySurfaceKHR(instance->GetVulkanInstance(), surface, nullptr);
    }

}
