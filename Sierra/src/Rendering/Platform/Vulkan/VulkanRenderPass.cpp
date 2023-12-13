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
        // Allocate framebuffer attachments
        std::vector<VkImageView> framebufferAttachments;
        framebufferAttachments.reserve(createInfo.attachments.size());

        // Allocate attachment descriptions
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        attachmentDescriptions.resize(createInfo.attachments.size());

        // Set attachment descriptions
        uint32 clearAttachmentCount = 0;
        for (uint32 i = 0; i < createInfo.attachments.size(); i++)
        {
            const RenderPassAttachment &attachment = createInfo.attachments[i];
            attachmentDescriptions[i].flags = 0;
            attachmentDescriptions[i].format = VulkanImage::ImageFormatToVkFormat(attachment.image->GetFormat());
            attachmentDescriptions[i].samples = VulkanImage::ImageSamplingToVkSampleCountFlags(attachment.image->GetSampling());
            attachmentDescriptions[i].loadOp = AttachmentLoadOperationToVkAttachmentLoadOp(attachment.loadOperation);
            attachmentDescriptions[i].storeOp = AttachmentStoreOperationToVkAttachmentStoreOp(attachment.storeOperation);
            attachmentDescriptions[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescriptions[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescriptions[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachmentDescriptions[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // TODO: Specify in attachment info
            clearAttachmentCount += (attachment.loadOperation == AttachmentLoadOperation::Clear) * 1;

            // Add to framebuffer attachments
            SR_ERROR_IF(attachment.image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Could not use image of attachment [{0}] in render pass [{1}] with a graphics API, which differs from [GraphicsAPI::Vulkan]!", i, GetName());
            framebufferAttachments.push_back(static_cast<VulkanImage&>(*attachment.image).GetVulkanImageView());
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
            const SubpassDescription &subpass = createInfo.subpassDescriptions[i];
            subpassDescriptions[i].flags = 0;
            subpassDescriptions[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

            // Create render target attachment references (they live in the outer scope, in the attachment references vectors, so they are not deallocated after the loop)
            for (const auto renderTargetIndex : subpass.renderTargets)
            {
                const RenderPassAttachment &renderTarget = createInfo.attachments[renderTargetIndex];

                // Check attachment type, then create and assign a VkAttachmentReference
                if (renderTarget.type & AttachmentType::Color)
                {
                    colorAttachmentReferences[i].push_back({ .attachment = renderTargetIndex, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
                }
                else if (renderTarget.type & AttachmentType::Depth)
                {
                    depthAttachmentReference = { .attachment = renderTargetIndex, .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
                }
            }

            // Create input attachment references
            for (const auto inputIndex : subpass.inputs)
            {
                inputAttachmentReferences[i].push_back({ .attachment = inputIndex, .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
            }

            // Reference subpass attachments
            subpassDescriptions[i].inputAttachmentCount = inputAttachmentReferences[i].size();
            subpassDescriptions[i].pInputAttachments = inputAttachmentReferences[i].data();
            subpassDescriptions[i].colorAttachmentCount = colorAttachmentReferences[i].size();
            subpassDescriptions[i].pColorAttachments = colorAttachmentReferences[i].data();
            subpassDescriptions[i].pDepthStencilAttachment = depthAttachmentReference.layout != VK_IMAGE_LAYOUT_UNDEFINED ? &depthAttachmentReference : nullptr;
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
            lastDependency.srcSubpass = createInfo.subpassDescriptions.size() - 1;
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
        renderPassCreateInfo.attachmentCount = attachmentDescriptions.size();
        renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
        renderPassCreateInfo.subpassCount = subpassDescriptions.size();
        renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
        renderPassCreateInfo.dependencyCount = subpassDependencies.size();
        renderPassCreateInfo.pDependencies = subpassDependencies.data();

        // Create render pass
        VkResult result = device.GetFunctionTable().vkCreateRenderPass(device.GetLogicalDevice(), &renderPassCreateInfo, nullptr, &renderPass);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create render pass [{0}]! Error code: {1}.", GetName(), result);

        // Set clear values
        VkClearValue defaultClearValue = { };
        defaultClearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
        defaultClearValue.depthStencil = { 1.0f, 0 };
        clearValues = std::vector<VkClearValue>(clearAttachmentCount, defaultClearValue);

        // Set up framebuffer create info
        VkFramebufferCreateInfo framebufferCreateInfo = { };
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = framebufferAttachments.size();
        framebufferCreateInfo.pAttachments = framebufferAttachments.data();
        framebufferCreateInfo.width = createInfo.attachments[0].image->GetWidth();
        framebufferCreateInfo.height = createInfo.attachments[0].image->GetHeight();
        framebufferCreateInfo.layers = 1;

        // Create framebuffer
        result = device.GetFunctionTable().vkCreateFramebuffer(device.GetLogicalDevice(), &framebufferCreateInfo, nullptr, &framebuffer);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create framebuffer for render pass [{0}]! Error code: {1}.", GetName(), result);
    }

    /* --- DESTRUCTOR --- */

    void VulkanRenderPass::Destroy()
    {
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