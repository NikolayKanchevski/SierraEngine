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

    VulkanRenderPass::VulkanRenderPass(const VulkanDevice& device, const RenderPassCreateInfo& createInfo)
        : RenderPass(createInfo), device(device), name(createInfo.name), subpassCount(createInfo.subpassDescriptions.size())
    {
        const uint32 expectedAttachmentWidth = createInfo.attachments.begin()->templateOutputImage.GetWidth();
        const uint32 expectedAttachmentHeight = createInfo.attachments.begin()->templateOutputImage.GetHeight();
        SR_THROW_IF(expectedAttachmentWidth > device.GetLimits().maxRenderPassWidth, ValueOutOfRangeError(SR_FORMAT("Cannot create render pass [{0}], as specified some attachments' width is greater than device [{1}]'s max render pass width - use Device::GetLimits() to query limits", name, device.GetName()), expectedAttachmentWidth, 1U, device.GetLimits().maxRenderPassWidth));
        SR_THROW_IF(expectedAttachmentWidth > device.GetLimits().maxRenderPassHeight, ValueOutOfRangeError(SR_FORMAT("Cannot create render pass [{0}], as specified some attachments' height is greater than device [{1}]'s max render pass height - use Device::GetLimits() to query limits", name, device.GetName()), expectedAttachmentWidth, 1U, device.GetLimits().maxRenderPassHeight));

        SR_THROW_IF(!device.IsExtensionLoaded(VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME), UnsupportedFeatureError(SR_FORMAT("Device [{0}] cannot create render pass [{1}]", device.GetName(), name)));

        // Allocate attachment data
        framebufferImageAttachments.resize(createInfo.attachments.size());
        framebufferAttachmentImageFormats.resize(createInfo.attachments.size());
        framebufferAttachmentImageFormats.reserve(createInfo.attachments.size() * 2); // NOTE: We are reserving twice the space, so we can potentially put resolver images at back without reallocating and invalidating pointer connections

        // Set attachment descriptions
        std::vector<VkAttachmentDescription> attachmentDescriptions(createInfo.attachments.size());
        attachmentDescriptions.reserve(createInfo.attachments.size() * 2); // NOTE: We are reserving twice the space, so we can potentially put resolve attachments at back without reallocating and invalidating pointer connections
        for (size i = 0; i < createInfo.attachments.size(); i++)
        {
            const RenderPassAttachment& attachment = createInfo.attachments[i];

            SR_THROW_IF(attachment.templateOutputImage.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot create render pass [{0}] using image [{1}] as template output of attachment [{2}], as its backend type differs from [RenderingBackendType::Vulkan]", name, attachment.templateOutputImage.GetName(), i)));
            const VulkanImage& vulkanTemplateOutputImage = static_cast<const VulkanImage&>(attachment.templateOutputImage);

            SR_THROW_IF(vulkanTemplateOutputImage.GetWidth() == 0, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}]'s template output image's width must be greater than [0]", createInfo.name, i)));
            SR_THROW_IF(vulkanTemplateOutputImage.GetWidth() != expectedAttachmentWidth, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}] does not have the same width as the rest", createInfo.name, i)));
            SR_THROW_IF(vulkanTemplateOutputImage.GetHeight() == 0, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}]'s template output image's height must be greater than [0]", createInfo.name, i)));
            SR_THROW_IF(vulkanTemplateOutputImage.GetHeight() != expectedAttachmentHeight, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}] does not have the same height as the rest", createInfo.name, i)));

            // Set up framebuffer attachment format of output image
            VkFormat& framebufferAttachmentImageFormat = framebufferAttachmentImageFormats[i];
            framebufferAttachmentImageFormat = ImageFormatToVkFormat(vulkanTemplateOutputImage.GetFormat());

            // Set up framebuffer attachment of output image
            VkFramebufferAttachmentImageInfo& framebufferOutputImageAttachment = framebufferImageAttachments[i];
            framebufferOutputImageAttachment.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENT_IMAGE_INFO;
            framebufferOutputImageAttachment.usage = vulkanTemplateOutputImage.GetVulkanUsageFlags();
            framebufferOutputImageAttachment.width = vulkanTemplateOutputImage.GetWidth();
            framebufferOutputImageAttachment.height = vulkanTemplateOutputImage.GetHeight();
            framebufferOutputImageAttachment.layerCount = vulkanTemplateOutputImage.GetLayerCount();
            framebufferOutputImageAttachment.viewFormatCount = 1;
            framebufferOutputImageAttachment.pViewFormats = &framebufferAttachmentImageFormat;

            // Set up render pass attachment of output image
            VkAttachmentDescription& outputImageAttachment = attachmentDescriptions[i];
            outputImageAttachment.format = ImageFormatToVkFormat(vulkanTemplateOutputImage.GetFormat());
            outputImageAttachment.samples = ImageSamplingToVkSampleCountFlags(vulkanTemplateOutputImage.GetSampling());
            outputImageAttachment.loadOp = AttachmentLoadOperationToVkAttachmentLoadOp(attachment.loadOperation);
            outputImageAttachment.storeOp = AttachmentStoreOperationToVkAttachmentStoreOp(attachment.storeOperation);
            outputImageAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            outputImageAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            outputImageAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            outputImageAttachment.finalLayout = attachment.type == RenderPassAttachmentType::Color ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            if (attachment.templateResolverImage != nullptr)
            {
                SR_THROW_IF(attachment.templateResolverImage->GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot create render pass [{0}] using image [{1}] as template resolver of attachment [{2}], as its backend type differs from [RenderingBackendType::Vulkan]", name, attachment.templateResolverImage->GetName(), i)));
                const VulkanImage& vulkanTemplateResolverImage = static_cast<const VulkanImage&>(*attachment.templateResolverImage);

                SR_THROW_IF(vulkanTemplateResolverImage.GetWidth() == 0, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}]'s template resolver image's width must be greater than [0]", createInfo.name, i)));
                SR_THROW_IF(vulkanTemplateResolverImage.GetWidth() != expectedAttachmentWidth, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}] does not have the same width as the rest", createInfo.name, i)));
                SR_THROW_IF(vulkanTemplateResolverImage.GetHeight() == 0, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}]'s template resolver image's height must be greater than [0]", createInfo.name, i)));
                SR_THROW_IF(vulkanTemplateResolverImage.GetHeight() != expectedAttachmentHeight, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}] does not have the same height as the rest", createInfo.name, i)));

                // Set up framebuffer attachment format of resolver image
                VkFormat& resolveAttachmentImageFormat = framebufferAttachmentImageFormats.emplace_back();
                resolveAttachmentImageFormat = ImageFormatToVkFormat(vulkanTemplateResolverImage.GetFormat());

                // Set up framebuffer attachment of resolver image
                VkFramebufferAttachmentImageInfo& framebufferResolverImageAttachment = framebufferImageAttachments.emplace_back();
                framebufferResolverImageAttachment.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENT_IMAGE_INFO;
                framebufferResolverImageAttachment.usage = vulkanTemplateResolverImage.GetVulkanUsageFlags();
                framebufferResolverImageAttachment.width = vulkanTemplateResolverImage.GetWidth();
                framebufferResolverImageAttachment.height = vulkanTemplateResolverImage.GetHeight();
                framebufferResolverImageAttachment.layerCount = vulkanTemplateResolverImage.GetLayerCount();
                framebufferResolverImageAttachment.viewFormatCount = 1;
                framebufferResolverImageAttachment.pViewFormats = &resolveAttachmentImageFormat;

                // Set up render pass attachment of resolver image
                VkAttachmentDescription& resolverImageAttachment = attachmentDescriptions.emplace_back();
                resolverImageAttachment.format = ImageFormatToVkFormat(vulkanTemplateResolverImage.GetFormat());
                resolverImageAttachment.samples = ImageSamplingToVkSampleCountFlags(vulkanTemplateResolverImage.GetSampling());
                resolverImageAttachment.loadOp = outputImageAttachment.loadOp;
                resolverImageAttachment.storeOp = outputImageAttachment.storeOp;
                resolverImageAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                resolverImageAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                resolverImageAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                resolverImageAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                // In order to resolve an image, it must be in VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL layout
                outputImageAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                resolveAttachmentCount++;
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
                    colorAttachmentReferences[i].push_back({ .attachment = (renderTarget.templateResolverImage != nullptr) * static_cast<uint32_t>(createInfo.attachments.size() + renderTargetIndex), .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
                }
                else if (renderTarget.type == RenderPassAttachmentType::Depth)
                {
                        depthAttachmentReference = { .attachment = renderTargetIndex, .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
                    hasDepthAttachment = true;
                }

                if (renderTarget.templateResolverImage != nullptr)
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
            subpassDescriptions[i].pDepthStencilAttachment = hasDepthAttachment ?& depthAttachmentReference : nullptr;
            subpassDescriptions[i].pResolveAttachments = resolveAttachmentReferences[i].data();
        }

        // Create subpass dependencies
        std::vector<VkSubpassDependency> subpassDependencies;
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
        VkResult result = device.GetFunctionTable().vkCreateRenderPass(device.GetVulkanDevice(), &renderPassCreateInfo, nullptr, &renderPass);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create render pass [{0}]", name));

        // Set up framebuffer attachment create info
        const VkFramebufferAttachmentsCreateInfo framebufferAttachmentsCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENTS_CREATE_INFO,
            .attachmentImageInfoCount = static_cast<uint32>(framebufferImageAttachments.size()),
            .pAttachmentImageInfos = framebufferImageAttachments.data()
        };

        // Set up framebuffer create info
        const VkFramebufferCreateInfo framebufferCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = &framebufferAttachmentsCreateInfo,
            .flags = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT,
            .renderPass = renderPass,
            .attachmentCount = framebufferAttachmentsCreateInfo.attachmentImageInfoCount,
            .width = framebufferAttachmentsCreateInfo.pAttachmentImageInfos[0].width,
            .height = framebufferAttachmentsCreateInfo.pAttachmentImageInfos[0].height,
            .layers = framebufferAttachmentsCreateInfo.pAttachmentImageInfos[0].layerCount,
        };

        // Create framebuffer
        result = device.GetFunctionTable().vkCreateFramebuffer(device.GetVulkanDevice(), &framebufferCreateInfo, nullptr, &framebuffer);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create render pass [{0}], as creation of framebuffer failed", name));
    }

    /* --- POLLING METHODS --- */

    void VulkanRenderPass::Resize(const uint32 width, const uint32 height)
    {
        // Destroy old framebuffer
        device.GetFunctionTable().vkDestroyFramebuffer(device.GetVulkanDevice(), framebuffer, nullptr);

        // Change attachments' size
        for (VkFramebufferAttachmentImageInfo& framebufferImageAttachment : framebufferImageAttachments)
        {
            framebufferImageAttachment.width = width;
            framebufferImageAttachment.height = height;
        }

        // Set up framebuffer attachment create info
        const VkFramebufferAttachmentsCreateInfo framebufferAttachmentsCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENTS_CREATE_INFO,
            .attachmentImageInfoCount = static_cast<uint32>(framebufferImageAttachments.size()),
            .pAttachmentImageInfos = framebufferImageAttachments.data()
        };

        // Set up framebuffer create info
        const VkFramebufferCreateInfo framebufferCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = &framebufferAttachmentsCreateInfo,
            .flags = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT,
            .renderPass = renderPass,
            .attachmentCount = framebufferAttachmentsCreateInfo.attachmentImageInfoCount,
            .width = framebufferAttachmentsCreateInfo.pAttachmentImageInfos[0].width,
            .height = framebufferAttachmentsCreateInfo.pAttachmentImageInfos[0].height,
            .layers = 1
        };

        // Recreate framebuffer
        const VkResult result = device.GetFunctionTable().vkCreateFramebuffer(device.GetVulkanDevice(), &framebufferCreateInfo, nullptr, &framebuffer);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not resize render pass [{0}]", name));

        // Set object names
        device.SetResourceName(renderPass, VK_OBJECT_TYPE_RENDER_PASS, name);
        device.SetResourceName(framebuffer, VK_OBJECT_TYPE_FRAMEBUFFER, SR_FORMAT("Framebuffer of render pass [{0}]", name));
    }

    /* --- DESTRUCTOR --- */

    VulkanRenderPass::~VulkanRenderPass() noexcept
    {
        device.GetFunctionTable().vkDestroyFramebuffer(device.GetVulkanDevice(), framebuffer, nullptr);
        device.GetFunctionTable().vkDestroyRenderPass(device.GetVulkanDevice(), renderPass, nullptr);
    }

}
