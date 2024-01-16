//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "VulkanRenderPass.h"

#include "VulkanImage.h"
#include "VulkanCommandBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanRenderPass::VulkanRenderPass(const VulkanDevice &device, const RenderPassCreateInfo &createInfo)
        : RenderPass(createInfo), VulkanResource(createInfo.name), device(device)
    {
        SR_ERROR_IF(!device.IsExtensionLoaded(VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME), "[Vulkan]: Cannot create render pass [{0}], as the provided device [{1}] does not support the {2} extension!", GetName(), device.GetName(), VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME);

        // Allocate attachment data
        framebufferImageAttachmentFormats.resize(createInfo.attachments.size());
        framebufferImageAttachments.resize(createInfo.attachments.size());
        std::vector<VkAttachmentDescription> attachmentDescriptions(createInfo.attachments.size());

        // Set attachment descriptions
        for (uint32 i = 0; i < createInfo.attachments.size(); i++)
        {
            const RenderPassAttachment &attachment = *(createInfo.attachments.begin() + i);
            SR_ERROR_IF(attachment.templateImage->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not use image of attachment [{0}] in render pass [{1}] with a graphics API, which differs from [GraphicsAPI::Vulkan]!", i, GetName());
            const VulkanImage &vulkanTemplateImage = static_cast<VulkanImage&>(*attachment.templateImage);

            // Set up framebuffer attachment
            framebufferImageAttachmentFormats[i] = VulkanImage::ImageFormatToVkFormat(attachment.templateImage->GetFormat());
            framebufferImageAttachments[i].sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENT_IMAGE_INFO;
            framebufferImageAttachments[i].usage = vulkanTemplateImage.GetVulkanUsageFlags();
            framebufferImageAttachments[i].width = attachment.templateImage->GetWidth();
            framebufferImageAttachments[i].height = attachment.templateImage->GetHeight();
            framebufferImageAttachments[i].layerCount = attachment.templateImage->GetLayerCount();
            framebufferImageAttachments[i].viewFormatCount = 1;
            framebufferImageAttachments[i].pViewFormats = &framebufferImageAttachmentFormats[i];

            // Set up render pass attachment
            attachmentDescriptions[i].format = VulkanImage::ImageFormatToVkFormat(attachment.templateImage->GetFormat());
            attachmentDescriptions[i].samples = VulkanImage::ImageSamplingToVkSampleCountFlags(attachment.templateImage->GetSampling());
            attachmentDescriptions[i].loadOp = AttachmentLoadOperationToVkAttachmentLoadOp(attachment.loadOperation);
            attachmentDescriptions[i].storeOp = AttachmentStoreOperationToVkAttachmentStoreOp(attachment.storeOperation);
            attachmentDescriptions[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescriptions[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescriptions[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachmentDescriptions[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // TODO: Specify in attachment info
            colorAttachmentCount += (attachment.type == AttachmentType::Color) * 1;
        }

        // Allocate subpass descriptions
        std::vector<VkSubpassDescription> subpassDescriptions;
        subpassDescriptions.resize(createInfo.subpassDescriptions.size());

        // Allocate attachment references (a vector for every attachment type, for every subpass)
        std::vector<std::vector<VkAttachmentReference>> colorAttachmentReferences;
        colorAttachmentReferences.resize(createInfo.subpassDescriptions.size());

        VkAttachmentReference depthAttachmentReference = { };
        depthAttachmentReference.attachment = 0;
        depthAttachmentReference.layout = VK_IMAGE_LAYOUT_UNDEFINED;

        std::vector<std::vector<VkAttachmentReference>> inputAttachmentReferences;
        inputAttachmentReferences.resize(createInfo.subpassDescriptions.size());

        // Set subpass descriptions
        for (uint32 i = 0; i < createInfo.subpassDescriptions.size(); i++)
        {
            const SubpassDescription &subpass = *(createInfo.subpassDescriptions.begin() + i);
            subpassDescriptions[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

            // Create render target attachment references (they live in the outer scope, in the attachment references vectors, so they are not deallocated after the loop)
            for (const auto renderTargetIndex : subpass.renderTargets)
            {
                const RenderPassAttachment &renderTarget = *(createInfo.attachments.begin() + renderTargetIndex);

                // Check attachment type, then create and assign a VkAttachmentReference
                if (renderTarget.type & AttachmentType::Color)
                {
                    colorAttachmentReferences[i].push_back({ .attachment = renderTargetIndex, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
                }
                else if (renderTarget.type & AttachmentType::Depth)
                {
                    depthAttachmentReference = { .attachment = renderTargetIndex, .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
                    hasDepthAttachment = true;
                }
            }

            // Create input attachment references
            for (const auto inputIndex : subpass.inputs)
            {
                inputAttachmentReferences[i].push_back({ .attachment = inputIndex, .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
            }

            // Reference subpass attachments
            subpassDescriptions[i].inputAttachmentCount = static_cast<uint32>(inputAttachmentReferences[i].size());
            subpassDescriptions[i].pInputAttachments = inputAttachmentReferences[i].data();
            subpassDescriptions[i].colorAttachmentCount = static_cast<uint32>(colorAttachmentReferences[i].size());
            subpassDescriptions[i].pColorAttachments = colorAttachmentReferences[i].data();
            subpassDescriptions[i].pDepthStencilAttachment = hasDepthAttachment ? &depthAttachmentReference : nullptr;
        }

        // Create subpass dependencies
        std::vector<VkSubpassDependency> subpassDependencies;
        if (createInfo.subpassDescriptions.size() != 1)
        {
            // Resize dependencies (we need one before the first, one after the last and one between each two subpasses)
            subpassDependencies.resize(createInfo.subpassDescriptions.size() + 1);

            // Create entry dependency
            VkSubpassDependency &firstDependency = subpassDependencies.front();
            firstDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            firstDependency.dstSubpass = 0;
            firstDependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            firstDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            firstDependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            firstDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            firstDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            firstDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

            // Create inner dependencies
            for (uint32 i = 1; i < subpassDependencies.size() - 1; i++)
            {
                subpassDependencies[i].srcSubpass = i - 1;
                subpassDependencies[i].dstSubpass = i;
                subpassDependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                subpassDependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                subpassDependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                subpassDependencies[i].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                subpassDependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }

            // Create exit dependency
            VkSubpassDependency &lastDependency = subpassDependencies.back();
            lastDependency.srcSubpass = static_cast<uint32>(createInfo.subpassDescriptions.size()) - 1;
            lastDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
            lastDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            lastDependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            lastDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            lastDependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            lastDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        }
        else
        {
            // Resize dependencies (we need just two - one before and one after the single subpass)
            subpassDependencies.resize(2);

            // Create entry dependency
            subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
            subpassDependencies[0].dstSubpass = 0;
            subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            subpassDependencies[0].srcAccessMask = 0;
            subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

            // Create exit dependency
            subpassDependencies[1].srcSubpass = 0;
            subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
            subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        }

        // Set up render pass create info
        VkRenderPassCreateInfo renderPassCreateInfo = { };
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = static_cast<uint32>(attachmentDescriptions.size());
        renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
        renderPassCreateInfo.subpassCount = static_cast<uint32>(subpassDescriptions.size());
        renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
        renderPassCreateInfo.dependencyCount = static_cast<uint32>(subpassDependencies.size());
        renderPassCreateInfo.pDependencies = subpassDependencies.data();

        // Create render pass
        VkResult result = device.GetFunctionTable().vkCreateRenderPass(device.GetLogicalDevice(), &renderPassCreateInfo, nullptr, &renderPass);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create render pass [{0}]! Error code: {1}.", GetName(), result);

        // Set up framebuffer attachment create info
        VkFramebufferAttachmentsCreateInfo framebufferAttachmentsCreateInfo = { };
        framebufferAttachmentsCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENTS_CREATE_INFO;
        framebufferAttachmentsCreateInfo.attachmentImageInfoCount = static_cast<uint32>(framebufferImageAttachments.size());
        framebufferAttachmentsCreateInfo.pAttachmentImageInfos = framebufferImageAttachments.data();

        // Set up framebuffer create info
        VkFramebufferCreateInfo framebufferCreateInfo = { };
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.pNext = &framebufferAttachmentsCreateInfo;
        framebufferCreateInfo.flags = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = framebufferAttachmentsCreateInfo.attachmentImageInfoCount;
        framebufferCreateInfo.width = framebufferAttachmentsCreateInfo.pAttachmentImageInfos[0].width;
        framebufferCreateInfo.height = framebufferAttachmentsCreateInfo.pAttachmentImageInfos[0].height;
        framebufferCreateInfo.layers = 1;

        // Create framebuffer
        result = device.GetFunctionTable().vkCreateFramebuffer(device.GetLogicalDevice(), &framebufferCreateInfo, nullptr, &framebuffer);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create framebuffer of render pass [{0}]! Error code: {1}.", GetName(), result);
    }

    /* --- POLLING METHODS --- */

    void VulkanRenderPass::Resize(const uint32 width, const uint32 height)
    {
        // Destroy old framebuffer
        device.GetFunctionTable().vkDestroyFramebuffer(device.GetLogicalDevice(), framebuffer, nullptr);

        // Change attachments' size
        for (auto &framebufferImageAttachment : framebufferImageAttachments)
        {
            framebufferImageAttachment.width = width;
            framebufferImageAttachment.height = height;
        }

        // Set up framebuffer attachment create info
        VkFramebufferAttachmentsCreateInfo framebufferAttachmentsCreateInfo = { };
        framebufferAttachmentsCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENTS_CREATE_INFO;
        framebufferAttachmentsCreateInfo.attachmentImageInfoCount = static_cast<uint32>(framebufferImageAttachments.size());
        framebufferAttachmentsCreateInfo.pAttachmentImageInfos = framebufferImageAttachments.data();

        // Set up framebuffer create info
        VkFramebufferCreateInfo framebufferCreateInfo = { };
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.pNext = &framebufferAttachmentsCreateInfo;
        framebufferCreateInfo.flags = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = framebufferAttachmentsCreateInfo.attachmentImageInfoCount;
        framebufferCreateInfo.width = framebufferAttachmentsCreateInfo.pAttachmentImageInfos[0].width;
        framebufferCreateInfo.height = framebufferAttachmentsCreateInfo.pAttachmentImageInfos[0].height;
        framebufferCreateInfo.layers = 1;

        // Recreate framebuffer
        const VkResult result = device.GetFunctionTable().vkCreateFramebuffer(device.GetLogicalDevice(), &framebufferCreateInfo, nullptr, &framebuffer);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not resize framebuffer of render pass [{0}]! Error code: {1}.", GetName(), result);

        // Set object names
        device.SetObjectName(renderPass, VK_OBJECT_TYPE_RENDER_PASS, GetName());
        device.SetObjectName(framebuffer, VK_OBJECT_TYPE_FRAMEBUFFER, "Framebuffer of [" + GetName() + "]");
    }

    void VulkanRenderPass::Begin(std::unique_ptr<CommandBuffer> &commandBuffer, const std::initializer_list<RenderPassBeginAttachment> &attachments) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot begin render pass [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<VulkanCommandBuffer&>(*commandBuffer);

        // Collect attachment views
        std::vector<VkClearValue> clearValues(attachments.size());
        std::vector<VkImageView> attachmentViews(attachments.size());
        for (uint32 i = 0; i < attachments.size(); i++)
        {
            const RenderPassBeginAttachment &attachment = *(attachments.begin() + i);
            SR_ERROR_IF(attachment.image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot begin render pass [{0}] using image [{1}] as attachment [{2}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), attachment.image->GetName(), i);
            const VulkanImage &vulkanImage = static_cast<VulkanImage&>(*attachment.image);

            // Configure attachment info
            attachmentViews[i] = vulkanImage.GetVulkanImageView();
            switch (framebufferImageAttachmentFormats[i])
            {
                case VK_FORMAT_D16_UNORM:
                case VK_FORMAT_D32_SFLOAT:
                {
                    clearValues[i].depthStencil = { 1.0f, 0 };
                    break;
                }
                default:
                {
                    clearValues[i].color = { attachment.clearColor.r, attachment.clearColor.g, attachment.clearColor.b, attachment.clearColor.a };
                    break;
                }
            }
        }

        // Set up dynamic attachments
        VkRenderPassAttachmentBeginInfo attachmentBeginInfo = { };
        attachmentBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO;
        attachmentBeginInfo.attachmentCount = static_cast<uint32>(attachmentViews.size());
        attachmentBeginInfo.pAttachments = attachmentViews.data();

        // Set up begin info
        VkRenderPassBeginInfo beginInfo = { };
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.renderPass = renderPass;
        beginInfo.framebuffer = framebuffer;
        beginInfo.renderArea.extent.width = attachments.begin()->image->GetWidth();
        beginInfo.renderArea.extent.height = attachments.begin()->image->GetHeight();
        beginInfo.renderArea.offset.x = 0;
        beginInfo.renderArea.offset.y = 0;
        beginInfo.clearValueCount = static_cast<uint32>(clearValues.size());
        beginInfo.pClearValues = clearValues.data();
        beginInfo.pNext = &attachmentBeginInfo;

        // Begin render pass
        device.GetFunctionTable().vkCmdBeginRenderPass(vulkanCommandBuffer.GetVulkanCommandBuffer(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Define viewport
        VkViewport viewport = { };
        viewport.x = 0;
        viewport.y = static_cast<float32>(beginInfo.renderArea.extent.height);
        viewport.width = static_cast<float32>(beginInfo.renderArea.extent.width);
        viewport.height = -static_cast<float32>(beginInfo.renderArea.extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        // Set viewport
        device.GetFunctionTable().vkCmdSetViewport(vulkanCommandBuffer.GetVulkanCommandBuffer(), 0, 1, &viewport);

        // Define scissor
        VkRect2D scissor = { };
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = beginInfo.renderArea.extent.width;
        scissor.extent.height = beginInfo.renderArea.extent.height;

        // Set scissor
        device.GetFunctionTable().vkCmdSetScissor(vulkanCommandBuffer.GetVulkanCommandBuffer(), 0, 1, &scissor);
    }

    void VulkanRenderPass::BeginNextSubpass(std::unique_ptr<CommandBuffer> &commandBuffer) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot begin next subpass of render pass [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(*commandBuffer);

        // Begin next subpass
        device.GetFunctionTable().vkCmdNextSubpass(vulkanCommandBuffer.GetVulkanCommandBuffer(), VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanRenderPass::End(std::unique_ptr<CommandBuffer> &commandBuffer) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot end render pass [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<VulkanCommandBuffer&>(*commandBuffer);

        // Begin render pass
        device.GetFunctionTable().vkCmdEndRenderPass(vulkanCommandBuffer.GetVulkanCommandBuffer());
    }

    /* --- DESTRUCTOR --- */

    VulkanRenderPass::~VulkanRenderPass()
    {
        device.GetFunctionTable().vkDestroyFramebuffer(device.GetLogicalDevice(), framebuffer, nullptr);
        device.GetFunctionTable().vkDestroyRenderPass(device.GetLogicalDevice(), renderPass, nullptr);
    }

    /* --- CONVERSIONS --- */

    VkAttachmentLoadOp VulkanRenderPass::AttachmentLoadOperationToVkAttachmentLoadOp(const AttachmentLoadOperation loadOperation)
    {
        switch (loadOperation)
        {
            case AttachmentLoadOperation::Clear:        return VK_ATTACHMENT_LOAD_OP_CLEAR;
            case AttachmentLoadOperation::Load:         return VK_ATTACHMENT_LOAD_OP_LOAD;
            default:                                    break;
        }

        return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }

    VkAttachmentStoreOp VulkanRenderPass::AttachmentStoreOperationToVkAttachmentStoreOp(const AttachmentStoreOperation storeOperation)
    {
        switch (storeOperation)
        {
            case AttachmentStoreOperation::Store:       return VK_ATTACHMENT_STORE_OP_STORE;
            case AttachmentStoreOperation::Discard:     return VK_ATTACHMENT_STORE_OP_DONT_CARE;
            default:                                    break;
        }

        return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }

}
