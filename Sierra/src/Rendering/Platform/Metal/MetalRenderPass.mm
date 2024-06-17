//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "MetalRenderPass.h"

#include "MetalImage.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalRenderPass::MetalRenderPass(const MetalDevice &device, const RenderPassCreateInfo &createInfo)
        : RenderPass(createInfo), MetalResource(createInfo.name)
    {
        attachmentMap.resize(createInfo.attachments.size());
        subpasses.resize(createInfo.subpassDescriptions.size());
        for (size i = 0; i < subpasses.size(); i++)
        {
            const SubpassDescription &subpassDescription = createInfo.subpassDescriptions[i];

            // Configure a dedicated render pass for every subpass, because Metal lacks support for them
            MTLRenderPassDescriptor* const subpassDescriptor = (subpasses[i] = [[MTLRenderPassDescriptor alloc] init]);
            [subpassDescriptor setRenderTargetWidth: createInfo.attachments.begin()->templateOutputImage.GetWidth()];
            [subpassDescriptor setRenderTargetHeight: createInfo.attachments.begin()->templateOutputImage.GetHeight()];

            for (const uint32 renderTarget : subpassDescription.renderTargets)
            {
                const RenderPassAttachment &attachment = createInfo.attachments[renderTarget];

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

                SR_ERROR_IF(attachment.templateOutputImage.GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not use image [{0}] of attachment [{1}]'s output image within render pass [{2}], as its graphics API differs from [GraphicsAPI::Metal]!", attachment.templateOutputImage.GetName(), i, GetName());
                const MetalImage &metalOutputImage = static_cast<const MetalImage&>(attachment.templateOutputImage);

                // Configure attachment
                attachmentMap[renderTarget].emplace_back(attachmentDescriptor);
                if (attachment.templateResolverImage == nullptr)
                {
                    [attachmentDescriptor setTexture: metalOutputImage.GetMetalTexture()];
                    [attachmentDescriptor setLoadAction: AttachmentLoadOperationToLoadAction(attachment.loadOperation)];
                    [attachmentDescriptor setStoreAction: AttachmentStoreOperationToStoreAction(attachment.storeOperation)];
                }
                else
                {
                    SR_ERROR_IF(attachment.templateResolverImage->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot not use image [{0}] of attachment [{1}]'s resolver image within render pass [{2}], as its graphics API differs from [GraphicsAPI::Metal]!", attachment.templateResolverImage->GetName(), i, GetName());
                    const MetalImage &metalResolveImage = static_cast<const MetalImage&>(*attachment.templateResolverImage);

                    [attachmentDescriptor setTexture: metalResolveImage.GetMetalTexture()];
                    [attachmentDescriptor setResolveTexture: metalOutputImage.GetMetalTexture()];
                    [attachmentDescriptor setLoadAction: AttachmentLoadOperationToLoadAction(attachment.loadOperation)];
                    [attachmentDescriptor setStoreAction: MTLStoreActionMultisampleResolve];
                }
            }
        }
    }

    /* --- POLLING METHODS --- */

    void MetalRenderPass::Resize(const uint32 width, const uint32 height)
    {
        for (MTLRenderPassDescriptor* const renderPass : subpasses)
        {
            [renderPass setRenderTargetWidth: width];
            [renderPass setRenderTargetHeight: height];
        }
    }

    /* --- DESTRUCTOR --- */

    MetalRenderPass::~MetalRenderPass()
    {
        for (MTLRenderPassDescriptor* const subpass : subpasses)
        {
            [subpass release];
        }
    }

    /* --- CONVERSIONS --- */

    MTLLoadAction MetalRenderPass::AttachmentLoadOperationToLoadAction(const RenderPassAttachmentLoadOperation loadOperation)
    {
        switch (loadOperation)
        {
            case RenderPassAttachmentLoadOperation::Clear:        return MTLLoadActionClear;
            case RenderPassAttachmentLoadOperation::Load:         return MTLLoadActionLoad;
        }

        return MTLLoadActionDontCare;
    }

    MTLStoreAction MetalRenderPass::AttachmentStoreOperationToStoreAction(const RenderPassAttachmentStoreOperation storeOperation)
    {
        switch (storeOperation)
        {
            case RenderPassAttachmentStoreOperation::Store:       return MTLStoreActionStore;
            case RenderPassAttachmentStoreOperation::Discard:     return MTLStoreActionDontCare;
        }

        return MTLStoreActionDontCare;
    }

}
