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
        attachmentTable.resize(createInfo.attachments.size());
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

                    // Save entry in table
                    attachmentTable[renderTargetIndex] = { .attachmentDescriptor = colorAttachment, .attachmentType = renderTarget.type };
                }
                else if (renderTarget.type & AttachmentType::Depth)
                {
                    // Set depth attachment
                    MTL::RenderPassDepthAttachmentDescriptor* depthAttachment = renderPassDescriptors[i]->depthAttachment();
                    depthAttachment->setTexture(metalImage.GetMetalTexture()); // NOTE: We assign texture here, even though it will be overwritten at Begin(), so that pipeline can query its pixel format
                    depthAttachment->setLoadAction(AttachmentLoadOperationToLoadAction(renderTarget.loadOperation));
                    depthAttachment->setStoreAction(AttachmentStoreOperationToStoreAction(renderTarget.storeOperation));
                    hasDepthAttachment = true;

                    // Save entry in table
                    attachmentTable[renderTargetIndex] = { .attachmentDescriptor = depthAttachment, .attachmentType = renderTarget.type };
                }
            }

            for (const auto inputIndex : subpass.inputs)
            {
                // TODO: Handle input attachments
            }
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

    void MetalRenderPass::Begin(std::unique_ptr<CommandBuffer> &commandBuffer, const std::initializer_list<RenderPassBeginAttachment> &attachments) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin render pass [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName());
        MetalCommandBuffer &metalCommandBuffer = static_cast<MetalCommandBuffer&>(*commandBuffer);

        for (uint32 i = 0; i < attachments.size(); i++)
        {
            const RenderPassBeginAttachment &attachment = *(attachments.begin() + i);
            SR_ERROR_IF(attachment.image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin render pass [{0}] using image [{1}] as attachment [{2}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), attachment.image->GetName(), i);
            const MetalImage &metalImage = static_cast<MetalImage&>(*attachment.image);

            // Update texture & clear color
            attachmentTable[i].attachmentDescriptor->setTexture(metalImage.GetMetalTexture());
            if (attachmentTable[i].attachmentType & AttachmentType::Color) reinterpret_cast<MTL::RenderPassColorAttachmentDescriptor*>(attachmentTable[i].attachmentDescriptor)->setClearColor(MTL::ClearColor(attachment.clearColor.r, attachment.clearColor.g, attachment.clearColor.b, attachment.clearColor.a));
        }

        // Reset current subpass
        currentSubpass = 0;

        // Begin recording rendering commands
        MTL::RenderCommandEncoder* renderCommandEncoder = metalCommandBuffer.GetMetalCommandBuffer()->renderCommandEncoder(renderPassDescriptors[0]);

        // Define viewport
        MTL::Viewport viewport = { };
        viewport.originX = 0.0f;
        viewport.originY = 0.0f;
        viewport.width = static_cast<float64>(attachmentTable[0].attachmentDescriptor->texture()->width());
        viewport.height = static_cast<float64>(attachmentTable[0].attachmentDescriptor->texture()->height());
        viewport.znear = 0.0f;
        viewport.zfar = 1.0f;

        // Set viewport
        renderCommandEncoder->setViewport(viewport);

        // Define scissor
        MTL::ScissorRect scissor = { };
        scissor.x = 0;
        scissor.y = 0;
        scissor.width = attachmentTable[0].attachmentDescriptor->texture()->width();
        scissor.height = attachmentTable[0].attachmentDescriptor->texture()->height();

        // Set scissor
        renderCommandEncoder->setScissorRect(scissor);

        // Save render encoder to command buffer
        metalCommandBuffer.PushRenderEncoder(renderCommandEncoder);
    }

    void MetalRenderPass::BeginNextSubpass(std::unique_ptr<CommandBuffer> &commandBuffer) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin next subpass of render pass [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName());
        MetalCommandBuffer &metalCommandBuffer = static_cast<MetalCommandBuffer&>(*commandBuffer);

        // End previous subpass
        metalCommandBuffer.PopRenderEncoder();
        currentSubpass++;

        // Begin recording rendering commands for next subpass
        MTL::RenderCommandEncoder* renderCommandEncoder = metalCommandBuffer.GetMetalCommandBuffer()->renderCommandEncoder(renderPassDescriptors[currentSubpass]);

        // Define viewport
        MTL::Viewport viewport = { };
        viewport.originX = 0.0f;
        viewport.originY = 0.0f;
        viewport.width = static_cast<float64>(attachmentTable[0].attachmentDescriptor->texture()->width());
        viewport.height = static_cast<float64>(attachmentTable[0].attachmentDescriptor->texture()->height());
        viewport.znear = 0.0f;
        viewport.zfar = 1.0f;

        // Set viewport
        renderCommandEncoder->setViewport(viewport);

        // Define scissor
        MTL::ScissorRect scissor = { };
        scissor.x = 0;
        scissor.y = 0;
        scissor.width = attachmentTable[0].attachmentDescriptor->texture()->width();
        scissor.height = attachmentTable[0].attachmentDescriptor->texture()->width();

        // Set scissor
        renderCommandEncoder->setScissorRect(scissor);

        // Push render encoder
        metalCommandBuffer.PushRenderEncoder(renderCommandEncoder);
    }

    void MetalRenderPass::End(std::unique_ptr<CommandBuffer> &commandBuffer) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot end render pass [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName());
        MetalCommandBuffer &metalCommandBuffer = static_cast<MetalCommandBuffer&>(*commandBuffer);

        // End current subpass
        metalCommandBuffer.PopRenderEncoder();
    }

    /* --- DESTRUCTOR --- */

    MetalRenderPass::~MetalRenderPass()
    {
        for (auto renderPass : renderPassDescriptors)
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
