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
        // Create a render pass for every subpass (Metal does not have subpasses)
        renderPasses.resize(createInfo.subpassDescriptions.size());
        for (uint32 i = 0; i < createInfo.subpassDescriptions.size(); i++)
        {
            const SubpassDescription &subpass = createInfo.subpassDescriptions[i];

            // Allocate render pass descriptor
            renderPasses[i] = MTL::RenderPassDescriptor::alloc()->init();

            // Reference render targets
            uint32 colorAttachmentIndex = 0;
            for (const auto renderTargetIndex : subpass.renderTargets)
            {
                const RenderPassAttachment &renderTarget = createInfo.attachments[renderTargetIndex];
                SR_ERROR_IF(renderTarget.image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not use image of attachment [{0}] in subpass [{1}] within render pass [{2}] with a graphics API, which differs from [GraphicsAPI::Metal]!", renderTargetIndex, i, GetName());

                const MetalImage &image = static_cast<MetalImage&>(*renderTarget.image);
                if (renderTarget.type & AttachmentType::Color)
                {
                    // Add color attachment
                    MTL::RenderPassColorAttachmentDescriptor* colorAttachment = renderPasses[i]->colorAttachments()->object(colorAttachmentIndex);
                    colorAttachment->setTexture(image.GetMetalTexture());
                    colorAttachment->setLoadAction(AttachmentLoadOperationToLoadAction(renderTarget.loadOperation));
                    colorAttachment->setStoreAction(AttachmentStoreOperationToStoreAction(renderTarget.storeOperation));
                    colorAttachment->setClearColor(MTL::ClearColor(0.0f, 0.0f, 0.0f, 1.0f));
                    colorAttachmentIndex++;
                }
                else if (renderTarget.type & AttachmentType::Depth)
                {
                    // Set depth attachment
                    MTL::RenderPassDepthAttachmentDescriptor* depthAttachment = renderPasses[i]->depthAttachment();
                    depthAttachment->setTexture(image.GetMetalTexture());
                    depthAttachment->setLoadAction(AttachmentLoadOperationToLoadAction(renderTarget.loadOperation));
                    depthAttachment->setStoreAction(AttachmentStoreOperationToStoreAction(renderTarget.storeOperation));
                }
            }

            for (const auto inputIndex : subpass.inputs)
            {
                // TODO: Handle input attachments
            }
        }
    }

    /* --- DESTRUCTOR --- */

    void MetalRenderPass::Destroy()
    {
        for (auto renderPass : renderPasses)
        {
            renderPass->release();
        }
        renderPasses.clear();
    }

    /* --- CONVERSIONS --- */

    MTL::LoadAction MetalRenderPass::AttachmentLoadOperationToLoadAction(const AttachmentLoadOperation loadOperation)
    {
        switch (loadOperation)
        {
            case AttachmentLoadOperation::Clear:        return MTL::LoadActionClear;
            case AttachmentLoadOperation::Load:         return MTL::LoadActionLoad;
            default:                                    break;
        }

        return MTL::LoadActionDontCare;
    }

    MTL::StoreAction MetalRenderPass::AttachmentStoreOperationToStoreAction(const AttachmentStoreOperation storeOperation)
    {
        switch (storeOperation)
        {
            case AttachmentStoreOperation::Store:       return MTL::StoreActionStore;
            case AttachmentStoreOperation::Discard:     return MTL::StoreActionDontCare;
            default:                                    break;
        }

        return MTL::StoreActionDontCare;
    }

}