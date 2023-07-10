//
// Created by Nikolay Kanchevski on 1.03.23.
//

#include "DynamicRenderer.h"

namespace Sierra::Rendering
{
    /* --- CONSTRUCTORS --- */

    DynamicRenderer::DynamicRenderer(const DynamicRendererCreateInfo &createInfo)
    {
        // Search for a depth attachment first
        bool hasDepthAttachment = false;
        for (const auto &attachment : createInfo.attachments)
        {
            if (attachment.IsDepth())
            {
                depthStencilAttachment = new VkRenderingAttachmentInfoKHR();
                hasDepthAttachment = true;
                break;
            }
        }

        // Store biggest attachment dimensions to use as viewport size and create color attachments array
        uint biggestWidth = 0, biggestHeight = 0, colorAttachmentCount = createInfo.attachments.size() - (uint)(hasDepthAttachment);

        // Allocate memory for attachment data
        colorAttachments = new VkRenderingAttachmentInfoKHR[colorAttachmentCount];
        colorAttachmentImages = new UniquePtr<Image>*[colorAttachmentCount];
        colorAttachmentFormats = new VkFormat[colorAttachmentCount];

        // Populate color and depth attachments
        uint i = 0;
        for (const auto &attachmentInfo : createInfo.attachments)
        {
            if (attachmentInfo.IsDepth())
            {
                depthStencilAttachmentImage = &attachmentInfo.image;
                depthStencilAttachmentFormat = static_cast<VkFormat>(depthStencilAttachmentImage->get()->GetFormat());

                depthStencilAttachment->sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
                depthStencilAttachment->imageView = attachmentInfo.image->GetVulkanImageView();
                depthStencilAttachment->imageLayout = attachmentInfo.image->GetLayout() != ImageLayout::UNDEFINED ? (VkImageLayout) attachmentInfo.image->GetLayout() : (VkImageLayout) ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                depthStencilAttachment->loadOp = (VkAttachmentLoadOp) attachmentInfo.loadOp;
                depthStencilAttachment->storeOp = (VkAttachmentStoreOp) attachmentInfo.storeOp;
                depthStencilAttachment->clearValue.color = { attachmentInfo.clearColor.r, attachmentInfo.clearColor.g, attachmentInfo.clearColor.b, attachmentInfo.clearColor.a };
                depthStencilAttachment->clearValue.depthStencil = { 1.0f, 0 };
                depthStencilAttachment->resolveMode = (VkResolveModeFlagBitsKHR) attachmentInfo.resolveMode;
                depthStencilAttachment->resolveImageView = attachmentInfo.resolveImage != nullptr ? attachmentInfo.resolveImage->get()->GetVulkanImageView() : VK_NULL_HANDLE;
                depthStencilAttachment->resolveImageLayout = attachmentInfo.resolveImage != nullptr ? (VkImageLayout) attachmentInfo.resolveImage->get()->GetLayout() : VK_IMAGE_LAYOUT_UNDEFINED;
                depthStencilAttachment->pNext = nullptr;
            }
            else
            {
                colorAttachmentImages[i] = &attachmentInfo.image;
                colorAttachmentFormats[i] = static_cast<VkFormat>(attachmentInfo.image.get()->GetFormat());

                VkRenderingAttachmentInfoKHR &attachment = colorAttachments[i];
                attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
                attachment.imageView = attachmentInfo.image->GetVulkanImageView();
                attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachment.loadOp = (VkAttachmentLoadOp) attachmentInfo.loadOp;
                attachment.storeOp = (VkAttachmentStoreOp) attachmentInfo.storeOp;
                attachment.clearValue.color = { attachmentInfo.clearColor.r, attachmentInfo.clearColor.g, attachmentInfo.clearColor.b, attachmentInfo.clearColor.a };
                attachment.resolveMode = (VkResolveModeFlagBitsKHR) attachmentInfo.resolveMode;
                attachment.resolveImageView = attachmentInfo.resolveImage != nullptr ? attachmentInfo.resolveImage->get()->GetVulkanImageView() : VK_NULL_HANDLE;
                attachment.resolveImageLayout = attachmentInfo.resolveImage != nullptr ? (VkImageLayout) attachmentInfo.resolveImage->get()->GetLayout() : VK_IMAGE_LAYOUT_UNDEFINED;
                attachment.pNext = nullptr;

                i++;
            }

            // Check if current dimensions are bigger than saved
            if (attachmentInfo.image->GetWidth() > biggestWidth) biggestWidth = static_cast<uint>(attachmentInfo.image->GetWidth());
            if (attachmentInfo.image->GetHeight() > biggestHeight) biggestHeight = static_cast<uint>(attachmentInfo.image->GetHeight());
        }

        // Set up rendering info
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea = { 0, 0, biggestWidth, biggestHeight };
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = createInfo.attachments.size() - (uint)(hasDepthAttachment);
        renderingInfo.pColorAttachments = colorAttachments;
        renderingInfo.pDepthAttachment = depthStencilAttachment;
        renderingInfo.pStencilAttachment = nullptr;
    }

    UniquePtr<DynamicRenderer> DynamicRenderer::Create(const DynamicRendererCreateInfo createInfo)
    {
        return std::make_unique<DynamicRenderer>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void DynamicRenderer::Begin(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        for (uint i = renderingInfo.colorAttachmentCount; i--;)
        {
            commandBuffer->TransitionImageLayout(*colorAttachmentImages[i], ImageLayout::COLOR_ATTACHMENT_OPTIMAL);
        }

        if (depthStencilAttachment != nullptr)
        {
            commandBuffer->TransitionImageLayout(*depthStencilAttachmentImage, ImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL);
        }


        vkCmdBeginRenderingKHR(commandBuffer->GetVulkanCommandBuffer(), &renderingInfo);
        commandBuffer->SetViewportAndScissor(renderingInfo.renderArea.extent.width, renderingInfo.renderArea.extent.height);
    }

    void DynamicRenderer::End(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        vkCmdEndRenderingKHR(commandBuffer->GetVulkanCommandBuffer());
    }

    /* --- SETTER METHODS --- */

    void DynamicRenderer::OverloadColorAttachment(const uint index, UniquePtr<Image> &image)
    {
        colorAttachmentImages[index] = &image;
        colorAttachments[index].imageView = image->GetVulkanImageView();
    }

    void DynamicRenderer::OverloadDepthAttachment(UniquePtr<Image> &image)
    {
        depthStencilAttachmentImage = &image;
        depthStencilAttachment->imageView = image->GetVulkanImageView();
    }

    /* --- DESTRUCTOR --- */

    void DynamicRenderer::Destroy()
    {
        delete[](colorAttachments);
        delete[](colorAttachmentImages);
        delete[](colorAttachmentFormats);
        delete(depthStencilAttachment);
    }
}