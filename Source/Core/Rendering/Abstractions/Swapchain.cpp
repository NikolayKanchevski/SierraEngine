//
// Created by Nikolay Kanchevski on 19.12.22.
//

#include "Swapchain.h"

#include "../Bases/VK.h"

namespace Sierra::Rendering
{

    /* --- CONSTRUCTORS --- */

    Swapchain::Swapchain(const SwapchainCreateInfo &createInfo)
        : window(createInfo.window), MAX_CONCURRENT_FRAMES(VK::GetDevice()->GetMaxConcurrentFramesCount())
    {
        GetSurfaceData();
        CreateSwapchain();
        CreateImages();
        CreateRenderPass();
        CreateFramebuffers();
        CreateSynchronization();
    }

    UniquePtr<Swapchain> Swapchain::Create(const SwapchainCreateInfo &createInfo)
    {
        return std::make_unique<Swapchain>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void Swapchain::BeginRenderPass(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        renderPass->Begin(framebuffers[imageIndex], commandBuffer);
    }

    void Swapchain::EndRenderPass(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        renderPass->End(commandBuffer);
    }

    VkResult Swapchain::AcquireNextImage()
    {
        // Wait until no images are being processed
        vkWaitForFences(VK::GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        // Acquire next image
        VkResult result = vkAcquireNextImageKHR(VK::GetLogicalDevice(), vkSwapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        // Resize swapchain if needed
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapchain();
            AcquireNextImage();
        }

        return result;
    }

    VkResult Swapchain::SwapImage(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        // If the current image is being processed
        if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
        {
            vkWaitForFences(VK::GetLogicalDevice(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }

        // Store the current image's fence
        imagesInFlight[imageIndex] = inFlightFences[currentFrame];

        // Set up submit info
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkPipelineStageFlags waitStagesPtr[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkCommandBuffer commandBuffersPtr[] = { commandBuffer->GetVulkanCommandBuffer() };

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];
        submitInfo.commandBufferCount = 1;
        submitInfo.pWaitDstStageMask = waitStagesPtr;
        submitInfo.pCommandBuffers = commandBuffersPtr;

        // Reset the current image's fence and submit graphics queue
        vkResetFences(VK::GetLogicalDevice(), 1, &inFlightFences[currentFrame]);
        VK_ASSERT(vkQueueSubmit(VK::GetDevice()->GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]), "Failed to submit draw command buffer");

        // Set up presentation info
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];

        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &vkSwapchain;
        presentInfo.pImageIndices = &imageIndex;

        // Submit presentation queue and resize the swapchain if needed
        VkResult queuePresentResult = vkQueuePresentKHR(VK::GetDevice()->GetPresentationQueue(), &presentInfo);
        if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR || window->IsResized())
        {
            RecreateSwapchain();
            return queuePresentResult;
        }

        // Increment currentFrame
        currentFrame = (currentFrame + 1) % MAX_CONCURRENT_FRAMES;

        // Update current frame for VMA
        vmaSetCurrentFrameIndex(VK::GetMemoryAllocator(), currentFrame);

        return queuePresentResult;
    }

    void Swapchain::RecreateSwapchain()
    {
        // Wait until window is responsive
        while (window->IsMinimized() || !window->IsFocused() || window->GetWidth() == 0)
        {
            glfwWaitEvents();
        }

        VK::GetDevice()->WaitUntilIdle();

        DestroyTemporaryObjects();
        CreateSwapchain();
        CreateImages();
        CreateFramebuffers();
    }

    /* --- SETTER METHODS --- */

    void Swapchain::GetSurfaceData()
    {
        SwapchainSupportDetails swapchainSupportDetails = GetSwapchainSupportDetails();
        bestSurfaceFormat = ChooseBestSurfaceFormat(swapchainSupportDetails.formats);
        bestPresentMode = ChooseBestSwapchainPresentMode(swapchainSupportDetails.presentModes);
    }

    void Swapchain::CreateSwapchain()
    {
        // Get surface's capabilities
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VK::GetPhysicalDevice(), window->GetSurface(), &surfaceCapabilities);

        // Create the extent and retrieve how many frames ahead can be rendered
        extent = GetSwapchainExtent(surfaceCapabilities);

        // Set up swapchain creation info
        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = window->GetSurface();
        swapchainCreateInfo.minImageCount = MAX_CONCURRENT_FRAMES;
        swapchainCreateInfo.imageFormat = bestSurfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = bestSurfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = extent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = bestPresentMode;
        swapchainCreateInfo.clipped = VK_TRUE;

        // Reuse old swapchain
        VkSwapchainKHR oldSwapchain = vkSwapchain;
        swapchainCreateInfo.oldSwapchain = vkSwapchain;

        // Get the queue indices
        const std::vector<uint32> queueFamilyIndicesCollection  { VK::GetDevice()->GetGraphicsQueueFamily(), VK::GetDevice()->GetPresentationQueueFamily() };

        // Check whether the graphics family is the same as the present one and based on that configure the creation info
        if (VK::GetDevice()->GetGraphicsQueueFamily() != VK::GetDevice()->GetPresentationQueueFamily())
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
            vkCreateSwapchainKHR(VK::GetLogicalDevice(), &swapchainCreateInfo, nullptr, &vkSwapchain),
            "Failed to create swapchain"
        );

        // If old swapchain was reused make sure to destroy it
        if (oldSwapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(VK::GetLogicalDevice(), oldSwapchain, nullptr);
        }
    }

    void Swapchain::CreateImages()
    {
        // Get swapchain images
        uint32 swapchainImageCount;
        vkGetSwapchainImagesKHR(VK::GetLogicalDevice(), vkSwapchain, &swapchainImageCount, nullptr);

        // Get swapchain images
        VkImage* swapchainVkImages = new VkImage[MAX_CONCURRENT_FRAMES];
        vkGetSwapchainImagesKHR(VK::GetLogicalDevice(), vkSwapchain, &swapchainImageCount, swapchainVkImages);

        // Create every image
        swapchainImages.resize(MAX_CONCURRENT_FRAMES);
        for (uint32 i = 0; i < MAX_CONCURRENT_FRAMES; i++)
        {
            // Create swapchain image and view
            swapchainImages[i] = Image::CreateSwapchainImage({
                .image = swapchainVkImages[i],
                .width = extent.width,
                .height = extent.height,
                .format = static_cast<ImageFormat>(bestSurfaceFormat.format),
            });
        }

        delete[](swapchainVkImages);
    }

    void Swapchain::CreateRenderPass()
    {
        renderPass = RenderPass::Create({
            .attachments = {
                {
                    .image = swapchainImages[0],
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::STORE,
                    .type = RenderPassAttachmentType::SWAPCHAIN
                }
            }
        });
    }

    void Swapchain::CreateFramebuffers()
    {
        // Resize framebuffers vectors to store one image for each concurrent frame
        framebuffers.resize(MAX_CONCURRENT_FRAMES);

        // Create a framebuffer for each concurrent frame
        for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
        {
            // Create a framebuffer from the stored attachments
            framebuffers[i] = Framebuffer::Create({
                .renderPass = renderPass,
                .attachments = { { swapchainImages[i] } }
            });
        }
    }

    void Swapchain::CreateSynchronization()
    {
        // Resize the semaphores and fences arrays
        imageAvailableSemaphores.resize(MAX_CONCURRENT_FRAMES);
        renderFinishedSemaphores.resize(MAX_CONCURRENT_FRAMES);
        inFlightFences.resize(MAX_CONCURRENT_FRAMES);
        imagesInFlight.resize(MAX_CONCURRENT_FRAMES, VK_NULL_HANDLE);

        // Define the semaphores creation info (universal for all semaphores)
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        // Define the fences creation info (universal for all fences)
        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        // Create semaphores and fences
        for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
        {
            VK_ASSERT(vkCreateSemaphore(VK::GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]), "Could not create semaphores/fences for swapchain");
            VK_ASSERT(vkCreateSemaphore(VK::GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]), "Could not create semaphores/fences for swapchain");
            VK_ASSERT(vkCreateFence(VK::GetLogicalDevice(), &fenceCreateInfo, nullptr, &inFlightFences[i]), "Could not create semaphores/fences for swapchain");
        }
    }

    /* --- DESTRUCTOR --- */

    void Swapchain::Destroy()
    {
        VK::GetDevice()->WaitUntilIdle();

        DestroyTemporaryObjects();

        vkDestroySwapchainKHR(VK::GetLogicalDevice(), vkSwapchain, nullptr);
        vkSwapchain = VK_NULL_HANDLE;

        renderPass->Destroy();

        for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
        {
            vkDestroyFence(VK::GetDevice()->GetLogicalDevice(), inFlightFences[i], nullptr);
            vkDestroySemaphore(VK::GetDevice()->GetLogicalDevice(), imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(VK::GetDevice()->GetLogicalDevice(), renderFinishedSemaphores[i], nullptr);
        }
    }

    void Swapchain::DestroyTemporaryObjects()
    {
        for (uint32 i = MAX_CONCURRENT_FRAMES; i--;)
        {
            swapchainImages[i]->Destroy();
            framebuffers[i]->Destroy();
        }
    }

    Swapchain::SwapchainSupportDetails Swapchain::GetSwapchainSupportDetails()
    {
        // Get the details of the GPU's supported swapchain
        SwapchainSupportDetails swapchainDetails{};

        // Get how many formats are available
        uint32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(VK::GetPhysicalDevice(), window->GetSurface(), &formatCount, nullptr);

        // Put each of them in an array
        swapchainDetails.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(VK::GetPhysicalDevice(), window->GetSurface(), &formatCount, swapchainDetails.formats.data());

        // Get how many presentation modes are available
        uint32 presentModesCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(VK::GetPhysicalDevice(), window->GetSurface(), &presentModesCount, nullptr);

        // Put each of them in an array
        swapchainDetails.presentModes.resize(presentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(VK::GetPhysicalDevice(), window->GetSurface(), &formatCount, swapchainDetails.presentModes.data());

        return swapchainDetails;
    }

    VkExtent2D Swapchain::GetSwapchainExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities)
    {
        // Get size of window's framebuffer
        int32 width, height;
        glfwGetFramebufferSize(window->GetCoreWindow(), &width, &height);

        // Save the sizes in a struct
        VkExtent2D createdExtent{};
        createdExtent.width = width;
        createdExtent.height = height;

        // Clamp the width and height such that they don't exceed the maximums
        createdExtent.width = std::clamp(createdExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        createdExtent.height = std::clamp(createdExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

        // Return the manually created extent
        return createdExtent;
    }

    VkSurfaceFormatKHR Swapchain::ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &surfaceFormats)
    {
        for (const auto &availableFormat : surfaceFormats)
        {
            if ((availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM || availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM) && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        // Otherwise just return the very first one
        return surfaceFormats[0];
    }

    VkPresentModeKHR Swapchain::ChooseBestSwapchainPresentMode(const std::vector<VkPresentModeKHR> &presentModes)
    {
        // Loop trough each to check if it is VK_PRESENT_MODE_MAILBOX_KHR
        for (const auto &availablePresentMode : presentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) // VK_PRESENT_MODE_MAILBOX_KHR
            {
                return availablePresentMode;
            }
        }

        // Otherwise return VK_PRESENT_MODE_FIFO_KHR, which is guaranteed to be available
        return VK_PRESENT_MODE_FIFO_KHR;
    }

}