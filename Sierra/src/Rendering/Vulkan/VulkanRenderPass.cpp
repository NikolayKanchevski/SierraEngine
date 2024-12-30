//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "VulkanRenderPass.h"

#include "VulkanImage.h"
#include "VulkanErrorHandler.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */

    VkAttachmentLoadOp AttachmentLoadOperationToVkAttachmentLoadOp(const RenderPassAttachmentLoadOperation loadOperation) noexcept
    {
        switch (loadOperation)
        {
            case RenderPassAttachmentLoadOperation::Clear:        return VK_ATTACHMENT_LOAD_OP_CLEAR;
            case RenderPassAttachmentLoadOperation::Load:         return VK_ATTACHMENT_LOAD_OP_LOAD;
        }

        return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }

    VkAttachmentStoreOp AttachmentStoreOperationToVkAttachmentStoreOp(const RenderPassAttachmentStoreOperation storeOperation) noexcept
    {
        switch (storeOperation)
        {
            case RenderPassAttachmentStoreOperation::Store:       return VK_ATTACHMENT_STORE_OP_STORE;
            case RenderPassAttachmentStoreOperation::Discard:     return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }

        return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }

    /* --- CONSTRUCTORS --- */

    VulkanRenderPass::VulkanRenderPass(const VulkanDevice& givenDevice, const RenderPassCreateInfo& createInfo)
        : RenderPass(createInfo), device(&givenDevice), name(createInfo.name), subpassCount(createInfo.subpassDescriptions.size())
    {
        // Set attachment descriptions
        std::vector<VkAttachmentDescription> attachmentDescriptions(createInfo.attachments.size());
        attachmentDescriptions.reserve(createInfo.attachments.size() * 2); // NOTE: We are reserving twice the space, so we can potentially put resolve attachments at back without reallocating and invalidating pointer connections
        for (size i = 0; i < createInfo.attachments.size(); i++)
        {
            const RenderPassAttachment& attachment = createInfo.attachments[i];

            // Set up render pass attachment of output image
            VkAttachmentDescription& attachmentDescription = attachmentDescriptions[i];
            attachmentDescription.format = ImageFormatToVkFormat(attachment.format);
            attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescription.loadOp = AttachmentLoadOperationToVkAttachmentLoadOp(attachment.loadOperation);
            attachmentDescription.storeOp = AttachmentStoreOperationToVkAttachmentStoreOp(attachment.storeOperation);
            attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            switch (attachment.type)
            {
                case RenderPassAttachmentType::Color:
                {
                    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    colorAttachmentCount++;
                    break;
                }
                case RenderPassAttachmentType::Depth:
                {
                    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                    hasDepthAttachment = true;
                    break;
                }
            }

            if (attachment.sampling != ImageSampling::x1)
            {
                VkAttachmentDescription& resolveAttachmentDescription = attachmentDescriptions.emplace_back();
                resolveAttachmentDescription.format = attachmentDescription.format;
                resolveAttachmentDescription.samples = ImageSamplingToVkSampleCountFlags(attachment.sampling);
                resolveAttachmentDescription.loadOp = attachmentDescription.loadOp;
                resolveAttachmentDescription.storeOp = attachmentDescription.storeOp;
                resolveAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                resolveAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                resolveAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                resolveAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                // In order to resolve an image, it must be in VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL layout
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }
        }

        // Allocate subpass descriptions
        std::vector<VkSubpassDescription> subpassDescriptions(subpassCount);

        // Allocate attachment references (a vector for every attachment type, for every subpass)
        std::vector<std::vector<VkAttachmentReference>> colorAttachmentReferences(subpassCount);
        std::vector<std::vector<VkAttachmentReference>> resolveAttachmentReferences(subpassCount);
        std::vector<std::vector<VkAttachmentReference>> inputAttachmentReferences(subpassCount);

        VkAttachmentReference depthAttachmentReference
        {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        // Set subpass descriptions
        for (size i = 0; i < subpassCount; i++)
        {
            const SubpassDescription& subpass = createInfo.subpassDescriptions[i];
            subpassDescriptions[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

            // Create render target attachment references (they live in the outer scope, in the attachment references vectors, so they are not deallocated after the loop)
            for (const uint32 renderTargetIndex : subpass.renderTargets)
            {
                const RenderPassAttachment& renderTarget = createInfo.attachments[renderTargetIndex];

                // Check attachment type, then create and assign a VkAttachmentReference
                if (renderTarget.type == RenderPassAttachmentType::Color)
                {
                    colorAttachmentReferences[i].push_back({ .attachment = (renderTarget.sampling != ImageSampling::x1) * static_cast<uint32_t>(createInfo.attachments.size() + renderTargetIndex), .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
                }
                else if (renderTarget.type == RenderPassAttachmentType::Depth)
                {
                    depthAttachmentReference = { .attachment = renderTargetIndex, .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
                }

                if (renderTarget.sampling != ImageSampling::x1)
                {
                    resolveAttachmentReferences[i].push_back({ .attachment = renderTargetIndex, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
                }
            }

            // Create input attachment references
            for (const uint32 inputIndex : subpass.inputs)
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
        std::vector<VkSubpassDependency> subpassDependencies = { };
        if (subpassCount != 1)
        {
            // Resize dependencies (we need one before the first, one after the last and one between each two subpasses)
            subpassDependencies.resize(subpassCount + 1);

            // Create entry dependency
            VkSubpassDependency& firstDependency = subpassDependencies.front();
            firstDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            firstDependency.dstSubpass = 0;
            firstDependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            firstDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            firstDependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            firstDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            firstDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            firstDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

            // Create inner dependencies
            for (size i = 1; i < subpassDependencies.size() - 1; i++)
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
            VkSubpassDependency& lastDependency = subpassDependencies.back();
            lastDependency.srcSubpass = static_cast<uint32>(subpassCount) - 1;
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
        const VkRenderPassCreateInfo renderPassCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = static_cast<uint32>(attachmentDescriptions.size()),
            .pAttachments = attachmentDescriptions.data(),
            .subpassCount = static_cast<uint32>(subpassDescriptions.size()),
            .pSubpasses = subpassDescriptions.data(),
            .dependencyCount = static_cast<uint32>(subpassDependencies.size()),
            .pDependencies = subpassDependencies.data()
        };

        // Create render pass
        VkResult result = device->GetFunctionTable().vkCreateRenderPass(device->GetVulkanDevice(), &renderPassCreateInfo, nullptr, &renderPass);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create render pass [{0}]", name));
        device->SetResourceName(renderPass, VK_OBJECT_TYPE_RENDER_PASS, name);
    }

    /* --- DESTRUCTOR --- */

    VulkanRenderPass::~VulkanRenderPass() noexcept
    {
        device->GetFunctionTable().vkDestroyRenderPass(device->GetVulkanDevice(), renderPass, nullptr);
    }

}