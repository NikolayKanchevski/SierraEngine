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
        attachmentReferences.resize(createInfo.attachments.size());
        subpasses.resize(createInfo.subpassDescriptions.size());

        for (size i = 0; i < subpasses.size(); i++)
        {
            const SubpassDescription& subpassDescription = createInfo.subpassDescriptions[i];
            MetalSubpass& subpass = subpasses[i];

            // Configure a dedicated render pass for every subpass, because Metal lacks support for them
            MTLRenderPassDescriptor* const subpassDescriptor = subpass.renderPass = [[MTLRenderPassDescriptor alloc] init];
            for (const uint32 renderTarget : subpassDescription.renderTargets)
            {
                const RenderPassAttachment& attachment = createInfo.attachments[renderTarget];

                MTLRenderPassAttachmentDescriptor* attachmentDescriptor = nullptr;
                switch (attachment.type)
                {
                    case RenderPassAttachmentType::Color:
                    {
                        attachmentDescriptor = [[subpassDescriptor colorAttachments] objectAtIndexedSubscript: renderTarget];
                        subpass.colorFormats.emplace_back(attachment.format);
                        colorAttachmentCount++;
                        break;
                    }
                    case RenderPassAttachmentType::Depth:
                    {
                        attachmentDescriptor = [subpassDescriptor depthAttachment];
                        subpass.depthFormat = attachment.format;
                        hasDepthAttachment = true;
                        break;
                    }
                }

                // Configure attachment
                attachmentReferences[renderTarget].emplace_back(attachmentDescriptor);
                if (attachment.sampling == ImageSampling::x1)
                {
                    [attachmentDescriptor setLoadAction: AttachmentLoadOperationToLoadAction(attachment.loadOperation)];
                    [attachmentDescriptor setStoreAction: AttachmentStoreOperationToStoreAction(attachment.storeOperation)];
                }
                else
                {
                    [attachmentDescriptor setLoadAction: AttachmentLoadOperationToLoadAction(attachment.loadOperation)];
                    [attachmentDescriptor setStoreAction: MTLStoreActionMultisampleResolve];
                }
            }
        }
    }

    /* --- GETTER METHODS --- */

    MTLRenderPassDescriptor* MetalRenderPass::GetSubpassRenderPass(const size subpassIndex) const
    {
        SR_THROW_IF(subpassIndex >= subpasses.size(), ValueOutOfRangeError(SR_FORMAT("Cannot get invalid subpass [{0}] of Metal render pass [{1}]", subpassIndex, name), subpassIndex, size(0), subpasses.size() - 1));
        return subpasses[subpassIndex].renderPass;
    }

    ImageFormat MetalRenderPass::GetSubpassColorAttachmentFormat(const size subpassIndex, const size attachmentIndex) const
    {
        SR_THROW_IF(subpassIndex >= subpasses.size(), ValueOutOfRangeError(SR_FORMAT("Cannot get depth attachment format of invalid subpass [{0}] of Metal render pass [{1}]", subpassIndex, name), subpassIndex, size(0), subpasses.size() - 1));

        const MetalSubpass& subpass = subpasses[subpassIndex];
        SR_THROW_IF(attachmentIndex >= subpass.colorFormats.size(), ValueOutOfRangeError(SR_FORMAT("Cannot get format of invalid color attachment [{0}] of Metal render pass [{1}]", attachmentIndex, name), attachmentIndex, size(0), subpass.colorFormats.size() - 1));

        return subpass.colorFormats[attachmentIndex];
    }

    ImageFormat MetalRenderPass::GetSubpassDepthAttachmentFormat(const size subpassIndex) const
    {
        SR_THROW_IF(subpassIndex >= subpasses.size(), ValueOutOfRangeError(SR_FORMAT("Cannot get depth attachment's format of invalid subpass [{0}] of Metal render pass [{1}]", subpassIndex, name), subpassIndex, size(0), subpasses.size() - 1));
        return subpasses[subpassIndex].depthFormat;
    }

    std::span<MTLRenderPassAttachmentDescriptor* const> MetalRenderPass::GetAttachmentReferences(const size attachmentIndex) const
    {
        SR_THROW_IF(attachmentIndex >= attachmentReferences.size(), ValueOutOfRangeError(SR_FORMAT("Cannot get invalid attachment [{0}]'s references of Metal render pass [{1}]", attachmentIndex, name), attachmentIndex, size(0), attachmentReferences.size() - 1));
        return attachmentReferences[attachmentIndex];
    }

    /* --- DESTRUCTOR --- */

    MetalRenderPass::~MetalRenderPass() noexcept
    {
        for (const MetalSubpass& subpass : subpasses)
        {
            [subpass.renderPass release];
        }
    }

}
