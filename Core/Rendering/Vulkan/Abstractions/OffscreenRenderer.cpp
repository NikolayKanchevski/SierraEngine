//
// Created by Nikolay Kanchevski on 10.12.22.
//

#include "OffscreenRenderer.h"

#include "../VulkanCore.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- CONSTRUCTORS --- */

    OffscreenRenderer::OffscreenRenderer(const OffscreenRendererCreateInfo &createInfo)
        : width(createInfo.width), height(createInfo.height), maxConcurrentFrames(createInfo.maxConcurrentFrames), msaaSampleCount(createInfo.msaaSampling), hasDepthAttachment(createInfo.createDepthAttachment)
    {
        // Create images
        CreateImages();

        // Create render pass
        CreateRenderPass();

        // Create framebuffers
        CreateFramebuffers();

        // Toggle the object as already created
        alreadyCreated = true;
    }

    std::unique_ptr<OffscreenRenderer> OffscreenRenderer::Create(const OffscreenRendererCreateInfo createInfo)
    {
        return std::make_unique<OffscreenRenderer>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void OffscreenRenderer::Begin(const VkCommandBuffer &givenCommandBuffer, const uint32_t frameIndex) const
    {
        // Begin render pass
        renderPass->Begin(framebuffers[frameIndex], givenCommandBuffer);
    }

    void OffscreenRenderer::End(const VkCommandBuffer &givenCommandBuffer) const
    {
        // End render pass
        renderPass->End(givenCommandBuffer);
    }

    /* --- SETTER METHODS --- */

    void OffscreenRenderer::Resize(const uint32_t newWidth, const uint32_t newHeight)
    {
        // Save new dimensions
        width = newWidth;
        height = newHeight;

        // Recreate resources
        CreateImages();
        CreateFramebuffers();
    }

    void OffscreenRenderer::CreateImages()
    {
        // Check if being created for the first time or resized/recreated
        if (alreadyCreated)
        {
            // Wait for device to be idle
            vkDeviceWaitIdle(VulkanCore::GetLogicalDevice());

            // Destroy resources
            sampler->Destroy();
            for (const auto &colorImage : colorImages) colorImage->Destroy();
            if (hasDepthAttachment) depthImage->Destroy();
        }
        else
        {
            colorImages.resize(maxConcurrentFrames);
        }

        // Create color images and their views
        for (uint32_t i = maxConcurrentFrames; i--;)
        {
            colorImages[i] = Image::Create({
                .dimensions = { width, height, 1 },
                .format = MAIN_IMAGE_FORMAT,
                .usageFlags = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
            });

            colorImages[i]->CreateImageView(ImageAspectFlags::COLOR);
        }

        // Check if depth attachment is required
        if (hasDepthAttachment)
        {
            // Create depth image
            depthImage = Image::Create({
                .dimensions = { VulkanCore::GetSwapchainExtent().width, VulkanCore::GetSwapchainExtent().height, 1 },
                .format = VulkanCore::GetDevice()->GetBestDepthImageFormat(),
                .sampling = msaaSampleCount,
                .usageFlags = ImageUsage::DEPTH_STENCIL_ATTACHMENT,
                .memoryFlags = MemoryFlags::DEVICE_LOCAL,
            });

            depthImage->CreateImageView(ImageAspectFlags::DEPTH);

            depthImage->TransitionLayout(ImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL);
        }

        // Check if MSAA is enabled
        if (IsMSAAEnabled())
        {
            // Create the sampled color image
            msaaImage = Image::Create({
                .dimensions = { VulkanCore::GetSwapchainExtent().width, VulkanCore::GetSwapchainExtent().height, 1 },
                .format = MAIN_IMAGE_FORMAT,
                .sampling = msaaSampleCount,
                .usageFlags = ImageUsage::TRANSIENT_ATTACHMENT | ImageUsage::COLOR_ATTACHMENT,
                .memoryFlags = MemoryFlags::DEVICE_LOCAL
            });

            // Create an image view off the sampled color image
            msaaImage->CreateImageView(ImageAspectFlags::COLOR);
        }

        // Create sampler
        sampler = Sampler::Create({
            .maxAnisotropy = 1.0f
        });
    }

    void OffscreenRenderer::CreateRenderPass()
    {
        // Set up attachments
        std::vector<RenderPassAttachment> renderPassAttachments;
        renderPassAttachments.reserve(1 + (int) hasDepthAttachment + (int) IsMSAAEnabled());

        // Populate attachments vector
        if (IsMSAAEnabled())
        {
            renderPassAttachments.push_back({
                .imageAttachment = msaaImage, .loadOp = LoadOp::CLEAR, .storeOp = StoreOp::STORE,
                .finalLayout = ImageLayout::GENERAL
            });

            renderPassAttachments.push_back({
                .imageAttachment = colorImages[0], .loadOp = LoadOp::DONT_CARE, .storeOp = StoreOp::STORE,
                .finalLayout = ImageLayout::GENERAL, .isResolve = true
            });
        }
        else
        {
            renderPassAttachments.push_back({
                .imageAttachment = colorImages[0], .loadOp = LoadOp::CLEAR, .storeOp = StoreOp::STORE,
                .finalLayout = ImageLayout::GENERAL
            });
        }

        if (hasDepthAttachment) renderPassAttachments.push_back({
            .imageAttachment = depthImage, .loadOp = LoadOp::CLEAR, .storeOp = StoreOp::DONT_CARE,
            .finalLayout = ImageLayout::DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL
        });

        // Set up render targets
        std::vector<uint32_t> renderTargets(renderPassAttachments.size());
        for (uint32_t i = renderPassAttachments.size(); i--;)
        {
            renderTargets[i] = i;
        }

        // Create render pass
        renderPass = RenderPass::Create({
            renderPassAttachments, { { .renderTargets = renderTargets } }
        });
    }

    void OffscreenRenderer::CreateFramebuffers()
    {
        // Check if being created for the first time or resized/recreated
        if (alreadyCreated)
        {
            // Destroy the framebuffers
            for (const auto &framebuffer : framebuffers)
            {
                framebuffer->Destroy();
            }
        }
        else
        {
            framebuffers.resize(maxConcurrentFrames);
        }

        // Set up attachments
        uint32_t colorImageIndex = 0;
        std::vector<VkImageView> framebufferAttachments(1 + (int) hasDepthAttachment + (int) IsMSAAEnabled());

        // Populate attachments
        if (IsMSAAEnabled())
        {
            colorImageIndex = 1;
            framebufferAttachments[0] = msaaImage->GetVulkanImageView();
        }

        if (hasDepthAttachment) framebufferAttachments[framebufferAttachments.size() - 1] = depthImage->GetVulkanImageView();

        // Create a framebuffer for each concurrent frame
        for (uint32_t i = maxConcurrentFrames; i--;)
        {
            // Assign corresponding color image
            framebufferAttachments[colorImageIndex] = colorImages[i]->GetVulkanImageView();

            // Create framebuffer
            framebuffers[i] = Framebuffer::Create({
                .width = width,
                .height = height,
                .attachments = framebufferAttachments,
                .renderPass = renderPass->GetVulkanRenderPass()
            });
        }
    }

    /* --- DESTRUCTOR --- */

    void OffscreenRenderer::Destroy()
    {
        // Free resources
        sampler->Destroy();
        for (const auto &colorImage : colorImages) colorImage->Destroy();
        if (hasDepthAttachment) depthImage->Destroy();
        if (IsMSAAEnabled()) msaaImage->Destroy();

        for (const auto &framebuffer : framebuffers) framebuffer->Destroy();
        renderPass->Destroy();
    }

}