//
// Created by Nikolay Kanchevski on 14.11.24.
//

#include "VulkanFramebuffer.h"

#include "VulkanImage.h"
#include "VulkanRenderPass.h"
#include "VulkanErrorHandler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanFramebuffer::VulkanFramebuffer(const VulkanDevice& givenDevice, const FramebufferCreateInfo& createInfo)
        : VulkanResource(createInfo.name), Framebuffer(createInfo), device(&givenDevice), width(createInfo.width), height(createInfo.height)
    {
        SR_THROW_IF(createInfo.width > device->GetLimits().maxFramebufferWidth, ValueOutOfRangeError(SR_FORMAT("Cannot create framebuffer [{0}], as specified width is exceeds device [{1}]'s max framebuffer width - use Device::GetLimits() to query limits", createInfo.name, device->GetName()), createInfo.width, 1U, device->GetLimits().maxFramebufferWidth));
        SR_THROW_IF(createInfo.height > device->GetLimits().maxFramebufferHeight, ValueOutOfRangeError(SR_FORMAT("Cannot create framebuffer [{0}], as specified height is exceeds device [{1}]'s max framebuffer height - use Device::GetLimits() to query limits", createInfo.name, device->GetName()), createInfo.height, 1U, device->GetLimits().maxFramebufferHeight));

        SR_THROW_IF(createInfo.templateRenderPass.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot create framebuffer [{0}] using render pass [{1}] as template, as its backend type differs from [RenderingBackendType::Vulkan]", createInfo.name, createInfo.templateRenderPass.GetName())));
        const VulkanRenderPass& vulkanTemplateRenderPass = static_cast<const VulkanRenderPass&>(createInfo.templateRenderPass);

        std::vector<VkImageView> attachments(createInfo.attachments.size());
        clearValues.resize(createInfo.attachments.size());

        for (size i = 0; i < createInfo.attachments.size(); i++)
        {
            const FramebufferAttachment& attachment = createInfo.attachments[i];

            SR_THROW_IF(attachment.image.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot create framebuffer [{0}] using image [{1}] for attachment [{2}], as its backend type differs from [RenderingBackendType::Vulkan]", createInfo.name, attachment.image.GetName(), i)));
            const VulkanImage& vulkanImage = static_cast<const VulkanImage&>(attachment.image);

            attachments[i] = vulkanImage.GetVulkanImageView();
            switch (vulkanImage.GetFormat())
            {
                case ImageFormat::D16_UNorm:
                case ImageFormat::D32_Float:
                {
                    clearValues[i].depthStencil = { attachment.clearDepth, 0 };
                    break;
                }
                default:
                {
                    clearValues[i].color = { attachment.clearColor.r, attachment.clearColor.g, attachment.clearColor.b, attachment.clearColor.a };
                    break;
                }
            }

            if (attachment.resolveImage != nullptr)
            {
                SR_THROW_IF(attachment.resolveImage->GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot create framebuffer [{0}] using image [{1}] for resolve of attachment [{2}], as its backend type differs from [RenderingBackendType::Vulkan]", createInfo.name, attachment.resolveImage->GetName(), i)));
                const VulkanImage& vulkanResolveImage = static_cast<const VulkanImage&>(*attachment.resolveImage);

                attachments.emplace_back(vulkanResolveImage.GetVulkanImageView());
                switch (vulkanResolveImage.GetFormat())
                {
                    case ImageFormat::D16_UNorm:
                    case ImageFormat::D32_Float:
                    {
                        clearValues.emplace_back().depthStencil = { attachment.clearDepth, 0 };
                        break;
                    }
                    default:
                    {
                        clearValues.emplace_back().color = { attachment.clearColor.r, attachment.clearColor.g, attachment.clearColor.b, attachment.clearColor.a };
                        break;
                    }
                }
            }
        }

        const VkFramebufferCreateInfo framebufferCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = vulkanTemplateRenderPass.GetVulkanRenderPass(),
            .attachmentCount = static_cast<uint32>(attachments.size()),
            .pAttachments = attachments.data(),
            .width = createInfo.width,
            .height = createInfo.height,
            .layers = 1
        };

        // Create framebuffer
        const VkResult result = device->GetFunctionTable().vkCreateFramebuffer(device->GetVulkanDevice(), &framebufferCreateInfo, nullptr, &framebuffer);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create framebuffer [{0}]", createInfo.name));
        device->SetResourceName(framebuffer, VK_OBJECT_TYPE_FRAMEBUFFER, createInfo.name);
    }

    /* --- DESTRUCTOR --- */

    VulkanFramebuffer::~VulkanFramebuffer() noexcept
    {
        device->GetFunctionTable().vkDestroyFramebuffer(device->GetVulkanDevice(), framebuffer, nullptr);
    }

}