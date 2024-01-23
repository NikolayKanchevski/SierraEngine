//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "MetalRenderPass.h"

#include "MetalImage.h"
#include "MetalCommandBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalRenderPass::MetalRenderPass(const MetalDevice &device, const RenderPassCreateInfo &createInfo)
        : RenderPass(createInfo), MetalResource(createInfo.name)
    {
        // Create a render pass for every subpass (Metal does not have subpasses)
        renderPassDescriptors.resize(createInfo.subpassDescriptions.size());
        for (uint32 i = 0; i < createInfo.subpassDescriptions.size(); i++)
        {
            const SubpassDescription &subpass = *(createInfo.subpassDescriptions.begin() + i);

            // Allocate render pass descriptor
            renderPassDescriptors[i] = MTL::RenderPassDescriptor::alloc()->init()->retain();
            renderPassDescriptors[i]->setRenderTargetWidth(createInfo.attachments.begin()->templateImage->GetWidth());
            renderPassDescriptors[i]->setRenderTargetHeight(createInfo.attachments.begin()->templateImage->GetHeight());

            // Reference render targets
            uint32 subpassColorAttachmentCount = 0;
            for (const auto renderTargetIndex : subpass.renderTargets)
            {
                const RenderPassAttachment &renderTarget = *(createInfo.attachments.begin() + renderTargetIndex);
                SR_ERROR_IF(renderTarget.templateImage->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not use image of attachment [{0}] in subpass [{1}] within render pass [{2}] with a graphics API, which differs from [GraphicsAPI::Metal]!", renderTargetIndex, i, GetName());

                const MetalImage &metalImage = static_cast<MetalImage&>(*renderTarget.templateImage);
                if (renderTarget.type & AttachmentType::Color)
                {
                    // Add color attachment
                    MTL::RenderPassColorAttachmentDescriptor* colorAttachment = renderPassDescriptors[i]->colorAttachments()->object(subpassColorAttachmentCount);
                    colorAttachment->setTexture(metalImage.GetMetalTexture()); // NOTE: We assign texture here, even though it will be overwritten at Begin(), so that pipeline can query its pixel format
                    colorAttachment->setClearColor(MTL::ClearColor(1.0f, 0.0f, 0.0f, 1.0f));
                    colorAttachment->setLoadAction(AttachmentLoadOperationToLoadAction(renderTarget.loadOperation));
                    colorAttachment->setStoreAction(AttachmentStoreOperationToStoreAction(renderTarget.storeOperation));
                    subpassColorAttachmentCount++;

                }
                else if (renderTarget.type & AttachmentType::Depth)
                {
                    // Set depth attachment
                    MTL::RenderPassDepthAttachmentDescriptor* depthAttachment = renderPassDescriptors[i]->depthAttachment();
                    depthAttachment->setTexture(metalImage.GetMetalTexture()); // NOTE: We assign texture here, even though it will be overwritten at Begin(), so that pipeline can query its pixel format
                    depthAttachment->setLoadAction(AttachmentLoadOperationToLoadAction(renderTarget.loadOperation));
                    depthAttachment->setStoreAction(AttachmentStoreOperationToStoreAction(renderTarget.storeOperation));
                    depthAttachmentIndex = i;
                }
            }
            colorAttachmentCount += subpassColorAttachmentCount;

            // NOTE: Input attachments are not applicable in Metal, so instead a simple regular resource binding is done during command rendering
        }
    }

    /* --- POLLING METHODS --- */

    void MetalRenderPass::Resize(const uint32 width, const uint32 height)
    {
        for (const auto &renderPass : renderPassDescriptors)
        {
            renderPass->setRenderTargetWidth(width);
            renderPass->setRenderTargetHeight(height);
        }
    }

    /* --- DESTRUCTOR --- */

    MetalRenderPass::~MetalRenderPass()
    {
        for (auto* renderPass : renderPassDescriptors)
        {
            renderPass->release();
        }
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
