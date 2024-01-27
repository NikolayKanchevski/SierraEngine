//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "VulkanRenderPass.h"

#include "VulkanImage.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanRenderPass::VulkanRenderPass(const VulkanDevice &device, const RenderPassCreateInfo &createInfo)
        : RenderPass(createInfo), VulkanResource(createInfo.name), device(device)
    {
        SR_ERROR_IF(!device.IsExtensionLoaded(VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME), "[Vulkan]: Cannot create render pass [{0}], as the provided device [{1}] does not support the {2} extension!", GetName(), device.GetName(), VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME);

        // Allocate attachment data
        framebufferImageAttachments.resize(createInfo.attachments.size());
        framebufferImageAttachments.reserve(createInfo.attachments.size() * 2);

        framebufferAttachmentImageFormats.resize(createInfo.attachments.size());

        std::vector<VkAttachmentDescription> attachmentDescriptions(createInfo.attachments.size());
        attachmentDescriptions.reserve(createInfo.attachments.size() * 2);

        // Set attachment descriptions
        for (uint32 i = 0; i < createInfo.attachments.size(); i++)
        {
            const RenderPassAttachment &attachment = *(createInfo.attachments.begin() + i);

            SR_ERROR_IF(attachment.templateImage->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not use image [{0}] of attachment [10}] in render pass [{2}], as its graphics API differs from [GraphicsAPI::Vulkan]!", attachment.templateImage->GetName(), i, GetName());
            const VulkanImage &vulkanTemplateImage = static_cast<VulkanImage&>(*attachment.templateImage);

            // Set up framebuffer attachment format
            VkFormat &framebufferAttachmentImageFormat = framebufferAttachmentImageFormats[i];
            framebufferAttachmentImageFormat = VulkanImage::ImageFormatToVkFormat(vulkanTemplateImage.GetFormat());

            // Set up framebuffer attachment
            VkFramebufferAttachmentImageInfo &framebufferAttachment = framebufferImageAttachments[i];
            framebufferAttachment.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENT_IMAGE_INFO;
            framebufferAttachment.usage = vulkanTemplateImage.GetVulkanUsageFlags();
            framebufferAttachment.width = vulkanTemplateImage.GetWidth();
            framebufferAttachment.height = vulkanTemplateImage.GetHeight();
            framebufferAttachment.layerCount = vulkanTemplateImage.GetLayerCount();
            framebufferAttachment.viewFormatCount = 1;
            framebufferAttachment.pViewFormats = &framebufferAttachmentImageFormat;

            // Set up render pass attachment
            VkAttachmentDescription &attachmentDescription = attachmentDescriptions[i];
            attachmentDescription.format = VulkanImage::ImageFormatToVkFormat(vulkanTemplateImage.GetFormat());
            attachmentDescription.samples = VulkanImage::ImageSamplingToVkSampleCountFlags(attachment.resolveImage.has_value() ? attachment.resolveImage->get()->GetSampling() : vulkanTemplateImage.GetSampling());
            attachmentDescription.loadOp = AttachmentLoadOperationToVkAttachmentLoadOp(attachment.loadOperation);
            attachmentDescription.storeOp = AttachmentStoreOperationToVkAttachmentStoreOp(attachment.storeOperation);
            attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

            // Handle resolve attachment
            if (attachment.resolveImage.has_value())
            {
                SR_ERROR_IF(attachment.resolveImage->get()->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not use image [{0}] of attachment [{1}] in render pass [{2}] for resolving, as its graphics API differs from [GraphicsAPI::Vulkan]!", attachment.resolveImage->get()->GetName(), i, GetName());
                const VulkanImage &vulkanResolveImage = static_cast<VulkanImage&>(*attachment.resolveImage->get());

                // Add resolve attachment
                VkAttachmentDescription &resolveAttachmentDescription = attachmentDescriptions.emplace_back();
                resolveAttachmentDescription.format = VulkanImage::ImageFormatToVkFormat(vulkanResolveImage.GetFormat());
                resolveAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
                resolveAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                resolveAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                resolveAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                resolveAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                resolveAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                resolveAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Resolving an image requires it

                // Set up framebuffer resolve attachment
                VkFramebufferAttachmentImageInfo &framebufferResolveAttachmentInfo = framebufferImageAttachments.emplace_back();
                framebufferResolveAttachmentInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENT_IMAGE_INFO;
                framebufferResolveAttachmentInfo.usage = vulkanTemplateImage.GetVulkanUsageFlags();
                framebufferResolveAttachmentInfo.width = vulkanResolveImage.GetWidth();
                framebufferResolveAttachmentInfo.height = vulkanResolveImage.GetHeight();
                framebufferResolveAttachmentInfo.layerCount = vulkanResolveImage.GetLayerCount();
                framebufferResolveAttachmentInfo.viewFormatCount = 1;
                framebufferResolveAttachmentInfo.pViewFormats = &framebufferAttachmentImageFormat;

                resolveAttachmentCount++;
            }
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

        std::vector<std::vector<VkAttachmentReference>> resolveAttachmentReferences;
        resolveAttachmentReferences.resize(createInfo.subpassDescriptions.size());

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
                if (renderTarget.type == RenderPassAttachmentType::Color)
                {
                    colorAttachmentReferences[i].push_back({ .attachment = renderTargetIndex, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
                }
                else if (renderTarget.type == RenderPassAttachmentType::Depth)
                {
                    depthAttachmentReference = { .attachment = renderTargetIndex, .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
                    hasDepthAttachment = true;
                }

                if (renderTarget.resolveImage.has_value())
                {
                    resolveAttachmentReferences[i].push_back({ .attachment = static_cast<uint32>(createInfo.attachments.size()) + renderTargetIndex, .layout = VK_IMAGE_LAYOUT_GENERAL });
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
            subpassDescriptions[i].pResolveAttachments = resolveAttachmentReferences[i].data();
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
            subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            subpassDependencies[0].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

            // Create exit dependency
            subpassDependencies[1].srcSubpass = 0;
            subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
            subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            subpassDependencies[1].dstAccessMask = VK_ACCESS_NONE;
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

    /* --- DESTRUCTOR --- */

    VulkanRenderPass::~VulkanRenderPass()
    {
        device.GetFunctionTable().vkDestroyFramebuffer(device.GetLogicalDevice(), framebuffer, nullptr);
        device.GetFunctionTable().vkDestroyRenderPass(device.GetLogicalDevice(), renderPass, nullptr);
    }

    /* --- CONVERSIONS --- */

    VkAttachmentLoadOp VulkanRenderPass::AttachmentLoadOperationToVkAttachmentLoadOp(const RenderPassAttachmentLoadOperation loadOperation)
    {
        switch (loadOperation)
        {
            case RenderPassAttachmentLoadOperation::Clear:        return VK_ATTACHMENT_LOAD_OP_CLEAR;
            case RenderPassAttachmentLoadOperation::Load:         return VK_ATTACHMENT_LOAD_OP_LOAD;
        }

        return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }

    VkAttachmentStoreOp VulkanRenderPass::AttachmentStoreOperationToVkAttachmentStoreOp(const RenderPassAttachmentStoreOperation storeOperation)
    {
        switch (storeOperation)
        {
            case RenderPassAttachmentStoreOperation::Store:       return VK_ATTACHMENT_STORE_OP_STORE;
            case RenderPassAttachmentStoreOperation::Discard:     return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }

        return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }

}
