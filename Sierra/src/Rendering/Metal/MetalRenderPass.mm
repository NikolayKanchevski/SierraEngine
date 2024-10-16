//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "MetalRenderPass.h"

#include "MetalImage.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */

    MTLLoadAction AttachmentLoadOperationToLoadAction(const RenderPassAttachmentLoadOperation loadOperation) noexcept
    {
        switch (loadOperation)
        {
            case RenderPassAttachmentLoadOperation::Clear:        return MTLLoadActionClear;
            case RenderPassAttachmentLoadOperation::Load:         return MTLLoadActionLoad;
        }

        return MTLLoadActionDontCare;
    }

    MTLStoreAction AttachmentStoreOperationToStoreAction(const RenderPassAttachmentStoreOperation storeOperation) noexcept
    {
        switch (storeOperation)
        {
            case RenderPassAttachmentStoreOperation::Store:       return MTLStoreActionStore;
            case RenderPassAttachmentStoreOperation::Discard:     return MTLStoreActionDontCare;
        }

        return MTLStoreActionDontCare;
    }

    /* --- CONSTRUCTORS --- */

    MetalRenderPass::MetalRenderPass(const MetalDevice& device, const RenderPassCreateInfo& createInfo)
        : RenderPass(createInfo), name(createInfo.name)
    {
        const uint32 expectedAttachmentWidth = createInfo.attachments.begin()->templateOutputImage.GetWidth();
        const uint32 expectedAttachmentHeight = createInfo.attachments.begin()->templateOutputImage.GetHeight();
        SR_THROW_IF(expectedAttachmentWidth > device.GetLimits().maxRenderPassWidth, ValueOutOfRangeError(SR_FORMAT("Cannot create render pass [{0}], as specified some attachments' width is greater than device [{1}]'s max render pass width - use Device::GetLimits() to query limits", name, device.GetName()), expectedAttachmentWidth, 1U, device.GetLimits().maxRenderPassWidth));
        SR_THROW_IF(expectedAttachmentWidth > device.GetLimits().maxRenderPassHeight, ValueOutOfRangeError(SR_FORMAT("Cannot create render pass [{0}], as specified some attachments' height is greater than device [{1}]'s max render pass height - use Device::GetLimits() to query limits", name, device.GetName()), expectedAttachmentWidth, 1U, device.GetLimits().maxRenderPassHeight));

        attachmentMap.resize(createInfo.attachments.size());
        subpasses.resize(createInfo.subpassDescriptions.size());

        for (size i = 0; i < subpasses.size(); i++)
        {
            const SubpassDescription& subpassDescription = createInfo.subpassDescriptions[i];

            // Configure a dedicated render pass for every subpass, because Metal lacks support for them
            MTLRenderPassDescriptor* const subpassDescriptor = (subpasses[i] = [[MTLRenderPassDescriptor alloc] init]);
            [subpassDescriptor setRenderTargetWidth: createInfo.attachments.begin()->templateOutputImage.GetWidth()];
            [subpassDescriptor setRenderTargetHeight: createInfo.attachments.begin()->templateOutputImage.GetHeight()];

            for (const uint32 renderTarget : subpassDescription.renderTargets)
            {
                const RenderPassAttachment& attachment = createInfo.attachments[renderTarget];

                MTLRenderPassAttachmentDescriptor* attachmentDescriptor = nullptr;
                switch (attachment.type)
                {
                    case RenderPassAttachmentType::Color:
                    {
                        attachmentDescriptor = [[subpassDescriptor colorAttachments] objectAtIndexedSubscript: renderTarget];
                        colorAttachmentCount++;
                        break;
                    }
                    case RenderPassAttachmentType::Depth:
                    {
                        attachmentDescriptor = [subpassDescriptor depthAttachment];
                        hasDepthAttachment = true;
                        break;
                    }
                }

                SR_THROW_IF(attachment.templateOutputImage.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot create render pass [{0}] using image [{1}] as template output of attachment [{2}], as its backend type differs from [RenderingBackendType::Metal]", name, attachment.templateOutputImage.GetName(), i)));
                const MetalImage& metalTemplateOutputImage = static_cast<const MetalImage&>(attachment.templateOutputImage);

                SR_THROW_IF(metalTemplateOutputImage.GetWidth() == 0, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}]'s template output image's width must be greater than [0]", createInfo.name, i)));
                SR_THROW_IF(metalTemplateOutputImage.GetWidth() != expectedAttachmentWidth, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}] does not have the same width as the rest", createInfo.name, i)));
                SR_THROW_IF(metalTemplateOutputImage.GetHeight() == 0, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}]'s template output image's height must be greater than [0]", createInfo.name, i)));
                SR_THROW_IF(metalTemplateOutputImage.GetHeight() != expectedAttachmentHeight, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}] does not have the same height as the rest", createInfo.name, i)));

                // Configure attachment
                attachmentMap[renderTarget].emplace_back(attachmentDescriptor);
                if (attachment.templateResolverImage == nullptr)
                {
                    [attachmentDescriptor setTexture: metalTemplateOutputImage.GetMetalTexture()];
                    [attachmentDescriptor setLoadAction: AttachmentLoadOperationToLoadAction(attachment.loadOperation)];
                    [attachmentDescriptor setStoreAction: AttachmentStoreOperationToStoreAction(attachment.storeOperation)];
                }
                else
                {
                    SR_THROW_IF(attachment.templateResolverImage->GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot create render pass [{0}] using image [{1}] as template resolver of attachment [{2}], as its backend type differs from [RenderingBackendType::Metal]", name, attachment.templateResolverImage->GetName(), i)));
                    const MetalImage& metalTemplateResolverImage = static_cast<const MetalImage&>(*attachment.templateResolverImage);

                    SR_THROW_IF(metalTemplateResolverImage.GetWidth() == 0, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}]'s template resolver image's  width must be greater than [0]", createInfo.name, i)));
                    SR_THROW_IF(metalTemplateResolverImage.GetWidth() != expectedAttachmentWidth, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}] does not have the same width as the rest", createInfo.name, i)));
                    SR_THROW_IF(metalTemplateResolverImage.GetHeight() == 0, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}]'s template resolver image's  height must be greater than [0]", createInfo.name, i)));
                    SR_THROW_IF(metalTemplateResolverImage.GetHeight() != expectedAttachmentHeight, InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachment [{1}] does not have the same height as the rest", createInfo.name, i)));

                    [attachmentDescriptor setTexture: metalTemplateResolverImage.GetMetalTexture()];
                    [attachmentDescriptor setResolveTexture: metalTemplateOutputImage.GetMetalTexture()];
                    [attachmentDescriptor setLoadAction: AttachmentLoadOperationToLoadAction(attachment.loadOperation)];
                    [attachmentDescriptor setStoreAction: MTLStoreActionMultisampleResolve];
                }
            }
        }
    }

    /* --- POLLING METHODS --- */

    void MetalRenderPass::Resize(const uint32 width, const uint32 height)
    {
        RenderPass::Resize(width, height);

        for (const MTLRenderPassDescriptor* const renderPass : subpasses)
        {
            [renderPass setRenderTargetWidth: width];
            [renderPass setRenderTargetHeight: height];
        }
    }

    /* --- DESTRUCTOR --- */

    MetalRenderPass::~MetalRenderPass() noexcept
    {
        for (MTLRenderPassDescriptor* const subpass : subpasses)
        {
            [subpass release];
        }
    }

}
