//
// Created by Nikolay Kanchevski on 19.12.22.
//

#include "Swapchain.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    /* --- CONSTRUCTORS --- */

    Swapchain::Swapchain(UniquePtr<Window> &givenWindow)
            : window(givenWindow)
    {
        GetSurfaceData();
        CreateSwapchain();
        CreateCommandBuffers();
        CreateImages();
        CreateRenderPass();
        CreateFramebuffers();
        CreateSynchronization();
    }

    UniquePtr<Swapchain> Swapchain::Create(UniquePtr<Window> &givenWindow)
    {
        return std::make_unique<Swapchain>(givenWindow);
    }

    /* --- POLLING METHODS --- */

    void Swapchain::BeginRenderPass(const VkCommandBuffer givenCommandBuffer)
    {
        renderPass->Begin(swapchainFramebuffers[imageIndex], givenCommandBuffer);
    }

    void Swapchain::EndRenderPass(const VkCommandBuffer givenCommandBuffer)
    {
        renderPass->End(givenCommandBuffer);
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

    VkResult Swapchain::SubmitCommandBuffers()
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
        VkCommandBuffer commandBuffersPtr[] = { commandBuffers[currentFrame]->GetVulkanCommandBuffer() };

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
        currentFrame = (currentFrame + 1) % maxConcurrentFrames;

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

        for (const auto &commandBuffer : commandBuffers)
        {
            commandBuffer->Reset();
        }

        if (resizeCallback != nullptr) resizeCallback();
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
        maxConcurrentFrames = surfaceCapabilities.maxImageCount > 3 ? 3 : surfaceCapabilities.maxImageCount;

        // Set up swapchain creation info
        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = window->GetSurface();
        swapchainCreateInfo.minImageCount = maxConcurrentFrames;
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
        const std::vector<uint> queueFamilyIndicesCollection  { static_cast<uint>(VK::GetDevice()->GetGraphicsQueueFamily()), static_cast<uint>(VK::GetDevice()->GetPresentationQueueFamily()) };

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

    void Swapchain::CreateCommandBuffers()
    {
        // Resize the command buffers array
        commandBuffers.resize(maxConcurrentFrames);

        // Create command buffers
        for (uint i = maxConcurrentFrames; i--;)
        {
            commandBuffers[i] = CommandBuffer::Create();
        }
    }

    void Swapchain::CreateImages()
    {
        // Get swapchain images
        vkGetSwapchainImagesKHR(VK::GetLogicalDevice(), vkSwapchain, &maxConcurrentFrames, nullptr);

        // Create array for images
        VkImage* swapchainVkImages = new VkImage[maxConcurrentFrames];

        // Resize the swapchain images array and extract every swapchain image
        vkGetSwapchainImagesKHR(VK::GetLogicalDevice(), vkSwapchain, &maxConcurrentFrames, swapchainVkImages);

        swapchainImages.resize(maxConcurrentFrames);

        // Create every image
        for (uint i = 0; i < maxConcurrentFrames; i++)
        {
            // Create swapchain image and view
            swapchainImages[i] = Image::CreateSwapchainImage({
                .image = swapchainVkImages[i],
                .format = (ImageFormat) bestSurfaceFormat.format,
                .sampling = Sampling::MSAAx1,
                .dimensions = { extent.width, extent.height, 1 }
            });

            swapchainImages[i]->CreateImageView(ImageAspectFlags::COLOR);
        }
    }

    void Swapchain::CreateRenderPass()
    {
        renderPass = RenderPass::Create({
            {
                {
                    .imageAttachment = swapchainImages[0],
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::STORE,
                    .finalLayout = ImageLayout::PRESENT_SRC
                }
                },
                { { .renderTargets = { 0 } } }
        });
    }

    void Swapchain::CreateFramebuffers()
    {
        // Resize framebuffers vectors to store one image for each concurrent frame
        swapchainFramebuffers.resize(maxConcurrentFrames);

        // Create a framebuffer for each concurrent frame
        for (uint i = maxConcurrentFrames; i--;)
        {
            // Create a framebuffer from the stored attachments
            swapchainFramebuffers[i] = Framebuffer::Create({
                .width = extent.width,
                .height = extent.height,
                .attachments = { &this->swapchainImages[i] },
                .renderPass = renderPass->GetVulkanRenderPass()
            });
        }
    }

    void Swapchain::CreateSynchronization()
    {
        // Resize the semaphores and fences arrays
        imageAvailableSemaphores.resize(maxConcurrentFrames);
        renderFinishedSemaphores.resize(maxConcurrentFrames);
        inFlightFences.resize(maxConcurrentFrames);
        imagesInFlight.resize(maxConcurrentFrames, VK_NULL_HANDLE);

        // Define the semaphores creation info (universal for all semaphores)
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        // Define the fences creation info (universal for all fences)
        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        // Create semaphores and fences
        for (uint i = maxConcurrentFrames; i--;)
        {
            VK_ASSERT(vkCreateSemaphore(VK::GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]), "Could not create semaphores/fences for swapcahin");
            VK_ASSERT(vkCreateSemaphore(VK::GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]), "Could not create semaphores/fences for swapcahin");
            VK_ASSERT(vkCreateFence(VK::GetLogicalDevice(), &fenceCreateInfo, nullptr, &inFlightFences[i]), "Could not create semaphores/fences for swapcahin");
        }
    }

    /* --- DESTRUCTOR --- */

    void Swapchain::Destroy()
    {
        VK::GetDevice()->WaitUntilIdle();

        DestroyTemporaryObjects();

        vkDestroySwapchainKHR(VK::GetLogicalDevice(), vkSwapchain, nullptr);

        renderPass->Destroy();

        for (uint i = maxConcurrentFrames; i--;)
        {
            vkDestroyFence(VK::GetDevice()->GetLogicalDevice(), inFlightFences[i], nullptr);
            vkDestroySemaphore(VK::GetDevice()->GetLogicalDevice(), imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(VK::GetDevice()->GetLogicalDevice(), renderFinishedSemaphores[i], nullptr);
        }
    }

    void Swapchain::DestroyTemporaryObjects()
    {
        for (uint i = maxConcurrentFrames; i--;)
        {
            swapchainImages[i]->Destroy();
            swapchainFramebuffers[i]->Destroy();
        }
    }

    Swapchain::SwapchainSupportDetails Swapchain::GetSwapchainSupportDetails()
    {
        // Get the details of the GPU's supported swapchain
        SwapchainSupportDetails swapchainDetails{};

        // Get how many formats are available
        uint formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(VK::GetPhysicalDevice(), window->GetSurface(), &formatCount, nullptr);

        // Put each of them in an array
        swapchainDetails.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(VK::GetPhysicalDevice(), window->GetSurface(), &formatCount, swapchainDetails.formats.data());

        // Get how many presentation modes are available
        uint presentModesCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(VK::GetPhysicalDevice(), window->GetSurface(), &presentModesCount, nullptr);

        // Put each of them in an array
        swapchainDetails.presentModes.resize(presentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(VK::GetPhysicalDevice(), window->GetSurface(), &formatCount, swapchainDetails.presentModes.data());

        return swapchainDetails;
    }

    VkExtent2D Swapchain::GetSwapchainExtent(const VkSurfaceCapabilitiesKHR givenSurfaceCapabilities)
    {
        // Get size of window's framebuffer
        int width, height;
        glfwGetFramebufferSize(window->GetCoreWindow(), &width, &height);

        // Save the sizes in a struct
        VkExtent2D createdExtent{};
        createdExtent.width = width;
        createdExtent.height = height;

        // Clamp the width and height such that they don't exceed the maximums
        createdExtent.width = std::clamp(createdExtent.width, givenSurfaceCapabilities.minImageExtent.width, givenSurfaceCapabilities.maxImageExtent.width);
        createdExtent.height = std::clamp(createdExtent.height, givenSurfaceCapabilities.minImageExtent.height, givenSurfaceCapabilities.maxImageExtent.height);

        // Return the manually created extent
        return createdExtent;
    }

    VkSurfaceFormatKHR Swapchain::ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &givenFormats)
    {
        for (const auto &availableFormat : givenFormats)
        {
            if ((availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM || availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM) && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        // Otherwise just return the very first one
        return givenFormats[0];
    }

    VkPresentModeKHR Swapchain::ChooseBestSwapchainPresentMode(const std::vector<VkPresentModeKHR> &givenPresentModes)
    {
        // Loop trough each to check if it is VK_PRESENT_MODE_MAILBOX_KHR
        for (const auto &availablePresentMode : givenPresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) // VK_PRESENT_MODE_MAILBOX_KHR
            {
                return availablePresentMode;
            }
        }

        // Otherwise return VK_PRESENT_MODE_FIFO_KHR which is guaranteed to be available
        return VK_PRESENT_MODE_FIFO_KHR;
    }

}