//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "VulkanSwapchain.h"

#include "VulkanCommandBuffer.h"
#if SR_PLATFORM_WINDOWS

#elif SR_PLATFORM_macOS
    #include "Platform/macOS/VulkanMacOSSurface.h"
    typedef Sierra::VulkanMacOSSurface NativeSurface;
#elif SR_PLATFORM_LINUX

#elif SR_PLATFORM_ANDROID

#elif SR_PLATFORM_iOS
    #include "Platform/iOS/VulkaniOSSurface.h"
    typedef Sierra::VulkaniOSSurface NativeSurface;
#endif

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanSwapchain::VulkanSwapchain(const VulkanInstance &instance, const VulkanDevice &device, const SwapchainCreateInfo &createInfo)
        : Swapchain(createInfo), VulkanResource(createInfo.name), instance(instance), device(device)
    {
        // Create surface
        surface = NativeSurface::Create(instance, createInfo.window);

        // Retrieve supported formats
        uint32 supportedFormatCount = 0;
        instance.GetFunctionTable().vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysicalDevice(), surface, &supportedFormatCount, nullptr);

        std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatCount);
        instance.GetFunctionTable().vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysicalDevice(), surface, &supportedFormatCount, supportedFormats.data());

        // Retrieve supported present modes
        uint32 supportedPresentModeCount = 0;
        instance.GetFunctionTable().vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysicalDevice(), surface, &supportedPresentModeCount, nullptr);

        std::vector<VkPresentModeKHR> supportedPresentModes(supportedPresentModeCount);
        instance.GetFunctionTable().vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysicalDevice(), surface, &supportedFormatCount, supportedPresentModes.data());

        // Select a better format if such is supported
        VkSurfaceFormatKHR selectedFormat = supportedFormats[0];
        for (const auto format : supportedFormats)
        {
            if ((format.format == VK_FORMAT_R8G8B8A8_UNORM || format.format == VK_FORMAT_B8G8R8A8_UNORM) && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                selectedFormat = format;
                break;
            }
        }

        // Select a present mode according to preferred one in create info
        VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;
        for (const auto presentMode : supportedPresentModes)
        {
            if (createInfo.preferredPresentationMode == SwapchainPresentationMode::Immediate && presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)   // Possible tearing, no GPU idling, minimal latency, and high energy consumption
            {
                selectedPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
                break;
            }
            #if !SR_PLATFORM_MOBILE
                if (createInfo.preferredPresentationMode == SwapchainPresentationMode::VSync && presentMode == VK_PRESENT_MODE_MAILBOX_KHR)     // VSync with no tearing, no GPU idling, low latency, and high energy consumption
            #else
                if (createInfo.preferredPresentationMode == SwapchainPresentationMode::VSync && presentMode == VK_PRESENT_MODE_FIFO_KHR)        // VSync with no tearing, low GPU idling, moderate latency, minimal energy consuption
            #endif
            {
                selectedPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                break;
            }
        }

        // Get surface capabilities
        VkSurfaceCapabilitiesKHR surfaceCapabilities = { };
        instance.GetFunctionTable().vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.GetPhysicalDevice(), surface, &surfaceCapabilities);

        // Get extents and frame count
        VkExtent2D extents = { };
        extents.width = std::clamp(createInfo.window->GetFramebufferSize().x, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        extents.height = std::clamp(createInfo.window->GetFramebufferSize().y, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
        concurrentFrameCount = std::clamp(3u, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);

        // Set up swapchain creation info
        swapchainCreateInfo = { };
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = surface;
        swapchainCreateInfo.minImageCount = concurrentFrameCount;
        swapchainCreateInfo.imageFormat = selectedFormat.format;
        swapchainCreateInfo.imageColorSpace = selectedFormat.colorSpace;
        swapchainCreateInfo.imageExtent = extents;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = selectedPresentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = swapchain;

        // Get count of all present queues
        uint queueFamilyPropertiesCount = 0;
        instance.GetFunctionTable().vkGetPhysicalDeviceQueueFamilyProperties(device.GetPhysicalDevice(), &queueFamilyPropertiesCount, nullptr);

        // Retrieve queue properties
        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        instance.GetFunctionTable().vkGetPhysicalDeviceQueueFamilyProperties(device.GetPhysicalDevice(), &queueFamilyPropertiesCount, queueFamilyProperties.data());

        // Try to find a queue family, which supports presentation
        bool presentationFamilyFound = false;
        uint32 presentationQueueFamily = 0;
        for (uint i = 0; i < queueFamilyPropertiesCount; i++)
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
        SR_ERROR_IF(!presentationFamilyFound, "[Vulkan]: Cannot create swapchain [{0}], as no queue family supports presentation!");

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

        // Set up semaphore create info
        VkSemaphoreCreateInfo semaphoreCreateInfo = { };
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        // Set up fence create info
        VkFenceCreateInfo fenceCreateInfo = { };
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        // Create sync objects
        isImageAvailableSemaphores.resize(concurrentFrameCount);
        isImageRenderedSemaphores.resize(concurrentFrameCount);
        isImageUnderWorkFences.resize(concurrentFrameCount);
        for (uint32 i = concurrentFrameCount; i--;)
        {
            SR_ERROR_IF(
                    device.GetFunctionTable().vkCreateSemaphore(device.GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &isImageAvailableSemaphores[i]) ||
                    device.GetFunctionTable().vkCreateSemaphore(device.GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &isImageRenderedSemaphores[i]) ||
                    device.GetFunctionTable().vkCreateFence(device.GetLogicalDevice(), &fenceCreateInfo, nullptr, &isImageUnderWorkFences[i]),
            "[Vulkan]: Could not create sync objects for swapchain [{0}]!", GetName());
        }

        CreateTemporaryObjects();
    }

    /* --- POLLING METHODS --- */

    void VulkanSwapchain::Begin(const std::unique_ptr<CommandBuffer> &commandBuffer)
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot begin swapchain [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(*commandBuffer);

        // Acquire next image
        const VkResult result = device.GetFunctionTable().vkAcquireNextImageKHR(device.GetLogicalDevice(), swapchain, std::numeric_limits<uint64>::max(), isImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &currentImage);

        // Begin render pass
        VkClearValue clearValue = { .color = { 1.0f, 0.0f, 0.0f, 1.0f } };
        VkRenderPassBeginInfo renderPassBeginInfo = { };
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = framebuffers[currentImage];
        renderPassBeginInfo.renderArea.offset = { .x = 0, .y = 0 };
        renderPassBeginInfo.renderArea.extent = swapchainCreateInfo.imageExtent;
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearValue;
        device.GetFunctionTable().vkCmdBeginRenderPass(vulkanCommandBuffer.GetVulkanCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanSwapchain::End(const std::unique_ptr<CommandBuffer> &commandBuffer)
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot end swapchain [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(*commandBuffer);

        // End render pass
        device.GetFunctionTable().vkCmdEndRenderPass(vulkanCommandBuffer.GetVulkanCommandBuffer());
    }

    void VulkanSwapchain::SubmitCommandBufferAndPresent(const std::unique_ptr<CommandBuffer> &commandBuffer)
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot present swapchain [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(*commandBuffer);

        // Set up submit info
        VkCommandBuffer vkCommandBuffer = vulkanCommandBuffer.GetVulkanCommandBuffer();
        const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo = { };
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &isImageAvailableSemaphores[currentFrame];
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vkCommandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &isImageRenderedSemaphores[currentFrame];

        // Submit command buffer
        device.GetFunctionTable().vkResetFences(device.GetLogicalDevice(), 1, &isImageUnderWorkFences[currentImage]);
        const VkResult result = device.GetFunctionTable().vkQueueSubmit(device.GetGeneralQueue(), 1, &submitInfo, isImageUnderWorkFences[currentFrame]);
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
        const VkResult queuePresentResult = device.GetFunctionTable().vkQueuePresentKHR(presentationQueue, &presentInfo);

        // Increment currentFrame
        currentFrame = (currentFrame + 1) % concurrentFrameCount;

        // Wait until GPU is done with the frame, so user can reuse command buffer with no issues
        device.GetFunctionTable().vkWaitForFences(device.GetLogicalDevice(), 1, &isImageUnderWorkFences[currentFrame], VK_TRUE, std::numeric_limits<uint64>::max());
    }

    /* --- DESTRUCTOR --- */

    void VulkanSwapchain::Destroy()
    {
        device.GetFunctionTable().vkDeviceWaitIdle(device.GetLogicalDevice());

        DestroyTemporaryObjects();

        framebuffers.clear();
        swapchainImageViews.clear();
        swapchainImages.clear();

        for (uint32 i = concurrentFrameCount; i--;)
        {
            device.GetFunctionTable().vkDestroySemaphore(device.GetLogicalDevice(), isImageAvailableSemaphores[i], nullptr);
            device.GetFunctionTable().vkDestroySemaphore(device.GetLogicalDevice(), isImageRenderedSemaphores[i], nullptr);
            device.GetFunctionTable().vkDestroyFence(device.GetLogicalDevice(), isImageUnderWorkFences[i], nullptr);
        }

        isImageAvailableSemaphores.clear();
        isImageRenderedSemaphores.clear();
        isImageUnderWorkFences.clear();

        instance.GetFunctionTable().vkDestroySurfaceKHR(instance.GetVulkanInstance(), surface, nullptr);
    }

    /* --- PRIVATE METHODS --- */

    void VulkanSwapchain::CreateTemporaryObjects()
    {
        // Create swapchain
        VkResult result = device.GetFunctionTable().vkCreateSwapchainKHR(device.GetLogicalDevice(), &swapchainCreateInfo, nullptr, &swapchain);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create swapchain [{0}]! Error code: {1}.", GetName(), result);

        // If an old swapchain was reused, destroy that
        if (swapchainCreateInfo.oldSwapchain != VK_NULL_HANDLE) device.GetFunctionTable().vkDestroySwapchainKHR(device.GetLogicalDevice(), swapchainCreateInfo.oldSwapchain, nullptr);

        // Get swapchain images
        swapchainImages.resize(concurrentFrameCount);
        swapchainImageViews.resize(concurrentFrameCount);
        device.GetFunctionTable().vkGetSwapchainImagesKHR(device.GetLogicalDevice(), swapchain, &concurrentFrameCount, swapchainImages.data());

        // Set up shared swapchain image view create info
        VkImageViewCreateInfo swapchainImageViewCreateInfo = { };
        swapchainImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        swapchainImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        swapchainImageViewCreateInfo.format = swapchainCreateInfo.imageFormat;
        swapchainImageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
        swapchainImageViewCreateInfo.subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        // Create swapchain image views
        result = VK_SUCCESS;
        for (uint i = 0; i < concurrentFrameCount; i++)
        {
            // Create image view
            swapchainImageViewCreateInfo.image = swapchainImages[i];
            result = device.GetFunctionTable().vkCreateImageView(device.GetLogicalDevice(), &swapchainImageViewCreateInfo, nullptr, &swapchainImageViews[i]);
            SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create swapchain image [{0}] of swapchain [{1}]! Error code: {2}.", i, GetName(), result);
        }

        // Set up color attachment for render pass
        VkAttachmentDescription colorAttachment = { };
        colorAttachment.format = swapchainCreateInfo.imageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // Make a reference to the attachment
        VkAttachmentReference colorAttachmentReference = { };
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // Set up the only subpass
        VkSubpassDescription subpass = { };
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;

        // Specify entry subpass dependency
        VkSubpassDependency dependency = { };
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        // Set up render pass create info
        VkRenderPassCreateInfo renderPassCreateInfo = { };
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &colorAttachment;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &dependency;

        // Create render pass
        result = device.GetFunctionTable().vkCreateRenderPass(device.GetLogicalDevice(), &renderPassCreateInfo, nullptr, &renderPass);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create render pass of swapchain [{0}]! Error code: {1}.", result);

        // Set up shared framebuffer create info
        VkFramebufferCreateInfo framebufferCreateInfo = { };
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.width = swapchainCreateInfo.imageExtent.width;
        framebufferCreateInfo.height = swapchainCreateInfo.imageExtent.height;
        framebufferCreateInfo.layers = 1;

        // Create framebuffers
        framebuffers.resize(concurrentFrameCount);
        for (uint32 i = concurrentFrameCount; i--;)
        {
            framebufferCreateInfo.pAttachments = &swapchainImageViews[i];
            result = device.GetFunctionTable().vkCreateFramebuffer(device.GetLogicalDevice(), &framebufferCreateInfo, nullptr, &framebuffers[i]);
            SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create framebuffer for swapchain image [{0}] of swapchain [{1}]! Error code: {2}.", i, GetName(), result);
        }
    }

    void VulkanSwapchain::DestroyTemporaryObjects()
    {
        for (uint32 i = concurrentFrameCount; i--;)
        {
            device.GetFunctionTable().vkDestroyFramebuffer(device.GetLogicalDevice(), framebuffers[i], nullptr);
            device.GetFunctionTable().vkDestroyImageView(device.GetLogicalDevice(), swapchainImageViews[i], nullptr);
        }

        device.GetFunctionTable().vkDestroyRenderPass(device.GetLogicalDevice(), renderPass, nullptr);
        device.GetFunctionTable().vkDestroySwapchainKHR(device.GetLogicalDevice(), swapchain, nullptr);
    }

}