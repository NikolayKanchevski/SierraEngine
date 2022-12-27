//
// Created by Nikolay Kanchevski on 10.12.22.
//

#include "OffscreenRenderer.h"

#include "../VK.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- CONSTRUCTORS --- */

    OffscreenRenderer::OffscreenRenderer(const OffscreenRendererCreateInfo &createInfo)
        : width(createInfo.width), height(createInfo.height), maxConcurrentFrames(createInfo.maxConcurrentFrames), attachmentTypes(createInfo.attachmentTypes), msaaSampling(createInfo.msaaSampling)
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

    void OffscreenRenderer::SetMultisampling(const Sampling newMultisampling)
    {
        this->msaaSampling = newMultisampling;

        CreateImages();
        CreateRenderPass();
        CreateFramebuffers();
    }

    void OffscreenRenderer::Resize(const uint32_t newWidth, const uint32_t newHeight)
    {
        if (newWidth == width && newHeight == height) return;

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
            vkDeviceWaitIdle(VK::GetLogicalDevice());

            // Destroy resources
            if (HasColorAttachment()) for (const auto &colorImage : colorImages) colorImage->Destroy();
            if (msaaImage != nullptr) msaaImage->Destroy();
            if (HasDepthAttachment()) depthImage->Destroy();
        }
        else
        {
            if (HasColorAttachment()) colorImages.resize(maxConcurrentFrames);

            // Create sampler
            sampler = Sampler::Create({
                .maxAnisotropy = 1.0f
            });
        }

        // Create color images and their views
        if (HasColorAttachment())
        {
            for (uint32_t i = maxConcurrentFrames; i--;)
            {
                colorImages[i] = Image::Create({
                    .dimensions = { width, height, 1 },
                    .format = VK::GetDevice()->GetBestColorImageFormat(),
                    .usageFlags = COLOR_ATTACHMENT_IMAGE | SAMPLED_IMAGE
                });

                colorImages[i]->CreateImageView(ASPECT_COLOR);
            }
        }

        // Check if depth attachment is required
        if (HasDepthAttachment())
        {
            // Create depth image
            depthImage = Image::Create({
                .dimensions = { width, height, 1 },
                .format = VK::GetDevice()->GetBestDepthImageFormat(),
                .sampling = (Sampling) msaaSampling,
                .usageFlags = DEPTH_STENCIL_ATTACHMENT_IMAGE,
                .memoryFlags = MEMORY_FLAGS_DEVICE_LOCAL,
            });

            depthImage->CreateImageView(ASPECT_DEPTH);

            depthImage->TransitionLayout(LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL);
        }

        // Check if MSAA is enabled
        if (HasMSAAEnabled())
        {
            // Create the sampled color image
            msaaImage = Image::Create({
                .dimensions = { width, height, 1 },
                .format = VK::GetDevice()->GetBestColorImageFormat(),
                .sampling = (Sampling) msaaSampling,
                .usageFlags = TRANSIENT_ATTACHMENT_IMAGE | COLOR_ATTACHMENT_IMAGE,
                .memoryFlags = MEMORY_FLAGS_DEVICE_LOCAL
            });

            // Create an image view off the sampled color image
            msaaImage->CreateImageView(ASPECT_COLOR);
        }
    }

    void OffscreenRenderer::CreateRenderPass()
    {
        if (alreadyCreated)
        {
            renderPass->Destroy();
        }

        // Set up attachments
        std::vector<RenderPassAttachment> renderPassAttachments;
        renderPassAttachments.reserve((int) HasColorAttachment() + (int) HasDepthAttachment() + (int) HasMSAAEnabled());

        // Populate attachments vector
        if (HasMSAAEnabled())
        {
            renderPassAttachments.push_back({
                .imageAttachment = msaaImage, .loadOp = LOAD_OP_CLEAR, .storeOp = STORE_OP_STORE,
                .finalLayout = LAYOUT_GENERAL
            });

            if (HasColorAttachment()) renderPassAttachments.push_back({
                .imageAttachment = colorImages[0], .loadOp = LOAD_OP_DONT_CARE, .storeOp = STORE_OP_STORE,
                .finalLayout = LAYOUT_GENERAL, .isResolve = true
            });
        }
        else
        {
            if (HasColorAttachment()) renderPassAttachments.push_back({
                .imageAttachment = colorImages[0], .loadOp = LOAD_OP_CLEAR, .storeOp = STORE_OP_STORE,
                .finalLayout = LAYOUT_GENERAL
            });
        }

        if (HasDepthAttachment()) renderPassAttachments.push_back({
            .imageAttachment = depthImage, .loadOp = LOAD_OP_CLEAR, .storeOp = STORE_OP_DONT_CARE,
            .finalLayout = LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL
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
        std::vector<VkImageView> framebufferAttachments((int) HasColorAttachment() + (int) HasDepthAttachment() + (int) HasMSAAEnabled());

        // Populate attachments
        if (HasMSAAEnabled())
        {
            colorImageIndex = 1;
            framebufferAttachments[0] = msaaImage->GetVulkanImageView();
        }

        if (HasDepthAttachment()) framebufferAttachments[framebufferAttachments.size() - 1] = depthImage->GetVulkanImageView();

        // Create a framebuffer for each concurrent frame
        for (uint32_t i = maxConcurrentFrames; i--;)
        {
            // Assign corresponding color image
            if (HasColorAttachment()) framebufferAttachments[colorImageIndex] = colorImages[i]->GetVulkanImageView();

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
        if (HasColorAttachment()) for (const auto &colorImage : colorImages) colorImage->Destroy();
        if (HasDepthAttachment()) depthImage->Destroy();
        if (HasMSAAEnabled()) msaaImage->Destroy();

        for (const auto &framebuffer : framebuffers) framebuffer->Destroy();
        renderPass->Destroy();
    }

}