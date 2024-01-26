//
// Created by Nikolay Kanchevski on 12.12.23.
//

#include "MetalCommandBuffer.h"

#include "MetalBuffer.h"
#include "MetalImage.h"

#include "MetalRenderPass.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalCommandBuffer::MetalCommandBuffer(const MetalDevice &device, const CommandBufferCreateInfo &createInfo)
        : CommandBuffer(createInfo), MetalResource(createInfo.name), device(device)
    {
        // Since in Metal you cannot reuse a command buffer after submitting it, one is created upon every Begin() call
    }

    /* --- POLLING METHODS --- */

    void MetalCommandBuffer::Begin()
    {
        #if SR_PLATFORM_macOS
            if (commandBuffer != nullptr) commandBuffer->release(); // NOTE: For some reason not calling this causes a memory leak on macOS (despite spec stating manual release is unnecessary, and thus, and a crash on iOS)
        #endif

        // Set up command buffer descriptor
        MTL::CommandBufferDescriptor* commandBufferDescriptor = MTL::CommandBufferDescriptor::alloc()->init();
        #if SR_ENABLE_LOGGING
            commandBufferDescriptor->setErrorOptions(MTL::CommandBufferErrorOptionEncoderExecutionStatus);
        #endif

        // Create command buffer
        commandBuffer = device.GetCommandQueue()->commandBuffer(commandBufferDescriptor);
        device.SetResourceName(commandBuffer, GetName());

        // Create completion semaphore
        finishedExecution = false;
        commandBuffer->addCompletedHandler(^(MTL::CommandBuffer*) { finishedExecution = true; });

        // Release command buffer descriptor memory
        commandBufferDescriptor->release();
    }

    void MetalCommandBuffer::End()
    {
        currentIndexBuffer = nullptr;
        currentIndexBufferOffset = 0;
    }

    void MetalCommandBuffer::BeginRenderPass(const std::unique_ptr<RenderPass> &renderPass, const std::initializer_list<RenderPassBeginAttachment> &attachments)
    {
        SR_ERROR_IF(renderPass->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin render pass [{0}], whose graphics API differs from [GraphicsAPI::Metal], from command buffer [{1}]!", renderPass->GetName(), GetName());
        const MetalRenderPass &metalRenderPass = static_cast<MetalRenderPass&>(*renderPass);

        SR_ERROR_IF(attachments.size() != metalRenderPass.GetAttachmentCount(), "[Metal]: Cannot begin render pass [{0}] within command buffer [{1}] with [{2}] attachments, as it was created to hold [{3}]!", renderPass->GetName(), GetName(), attachments.size(), metalRenderPass.GetAttachmentCount());
        for (uint32 i = 0; i < metalRenderPass.GetSubpassCount(); i++)
        {
            const MTL::RenderPassDescriptor* subpass = metalRenderPass.GetSubpass(i);

            uint32 subpassColorAttachmentCount = 0;
            for (const auto renderTargetIndex : metalRenderPass.GetSubpassRenderTargets(i))
            {
                const RenderPassBeginAttachment &attachment = *(attachments.begin() + renderTargetIndex);

                SR_ERROR_IF(attachment.image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin render pass [{0}] within command buffer [{1}] using image [{1}] as attachment [{2}], as its graphics API differs from [GraphicsAPI::Metal]!", renderPass->GetName(), GetName(), attachment.image->GetName(), renderTargetIndex);
                const MetalImage &metalImage = static_cast<MetalImage&>(*attachment.image);

                if (metalRenderPass.GetDepthAttachmentIndex() == renderTargetIndex)
                {
                    subpass->depthAttachment()->setTexture(metalImage.GetMetalTexture());
                }
                else
                {
                    if (std::find(metalRenderPass.GetResolvedColorAttachmentIndices().begin(), metalRenderPass.GetResolvedColorAttachmentIndices().end(), i) != metalRenderPass.GetResolvedColorAttachmentIndices().end())
                    {
                        SR_ERROR_IF(renderTargetIndex + 1 >= attachments.size(), "[Metal]: Cannot begin render pass [{0}] within command buffer [{1}], as resolve attachment at index [{2}] is missing its consecutive color attachment!", renderPass->GetName(), GetName(), renderTargetIndex);
                        const RenderPassBeginAttachment &colorAttachment = *(attachments.begin() + renderTargetIndex + 1);

                        SR_ERROR_IF(colorAttachment.image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin render pass [{0}] within command buffer [{1}] using image [{2}] as attachment [{3}], as its graphics API differs from [GraphicsAPI::Metal]!", renderPass->GetName(), GetName(), colorAttachment.image->GetName(), renderTargetIndex + 1);
                        const MetalImage &metalColorImage = static_cast<MetalImage&>(*colorAttachment.image);

                        subpass->colorAttachments()->object(subpassColorAttachmentCount)->setResolveTexture(metalImage.GetMetalTexture());
                        subpass->colorAttachments()->object(subpassColorAttachmentCount)->setTexture(metalColorImage.GetMetalTexture());
                        subpassColorAttachmentCount++;
                    }

                    subpass->colorAttachments()->object(subpassColorAttachmentCount)->setTexture(metalImage.GetMetalTexture());
                    subpassColorAttachmentCount++;
                }
            }
        }

        BeginNextSubpass(renderPass);
    }

    void MetalCommandBuffer::BeginNextSubpass(const std::unique_ptr<RenderPass> &renderPass)
    {
        SR_ERROR_IF(renderPass->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin next subpass of render pass [{0}], whose graphics API differs from [GraphicsAPI::Metal], from command buffer [{1}]!", renderPass->GetName(), GetName());
        const MetalRenderPass &metalRenderPass = static_cast<MetalRenderPass&>(*renderPass);

        // Begin encoding next subpass
        currentRenderEncoder = commandBuffer->renderCommandEncoder(metalRenderPass.GetSubpass(currentSubpass));

        // Define viewport
        MTL::Viewport viewport = { };
        viewport.originX = 0.0f;
        viewport.originY = 0.0f;
        viewport.width = static_cast<float64>(metalRenderPass.GetSubpass(currentSubpass)->renderTargetWidth());
        viewport.height = static_cast<float64>(metalRenderPass.GetSubpass(currentSubpass)->renderTargetHeight());
        viewport.znear = 0.0f;
        viewport.zfar = 1.0f;

        // Set viewport
        currentRenderEncoder->setViewport(viewport);

        // Define scissor
        MTL::ScissorRect scissor = { };
        scissor.x = 0;
        scissor.y = 0;
        scissor.width = static_cast<uint64>(metalRenderPass.GetSubpass(currentSubpass)->renderTargetWidth());
        scissor.height = static_cast<uint64>(metalRenderPass.GetSubpass(currentSubpass)->renderTargetHeight());

        // Set scissor
        currentRenderEncoder->setScissorRect(scissor);

        // Increment current subpass for future usage
        currentSubpass++;
    }

    void MetalCommandBuffer::EndRenderPass(const std::unique_ptr<RenderPass> &renderPass)
    {
        SR_ERROR_IF(renderPass->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot end render pass [{0}], whose graphics API differs from [GraphicsAPI::Metal], from command buffer [{1}]!", renderPass->GetName(), GetName());

        SR_ERROR_IF(currentRenderEncoder == nullptr, "[Metal]: Cannot end render pass [{0}], as it has not been began within command buffer [{1}]!", renderPass->GetName(), GetName());
        currentRenderEncoder->endEncoding();

        currentRenderEncoder = nullptr;
        currentSubpass = 0;
    }

    void MetalCommandBuffer::BeginGraphicsPipeline(const std::unique_ptr<GraphicsPipeline> &graphicsPipeline)
    {
        SR_ERROR_IF(graphicsPipeline->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin graphics pipeline [{0}], whose graphics API differs from [GraphicsAPI::Metal], from command buffer [{1}]!", graphicsPipeline->GetName(), GetName());
        const MetalGraphicsPipeline &metalGraphicsPipeline = static_cast<MetalGraphicsPipeline&>(*graphicsPipeline);

        SR_ERROR_IF(currentRenderEncoder == nullptr, "[Metal]: Cannot begin graphics pipeline [{0}] if no render pass is active within command buffer [{1}]!", graphicsPipeline->GetName(), GetName());

        // Bind pipeline and set appropriate settings
        currentRenderEncoder->setCullMode(metalGraphicsPipeline.GetCullMode());
        currentRenderEncoder->setTriangleFillMode(metalGraphicsPipeline.GetTriangleFillMode());
        currentRenderEncoder->setFrontFacingWinding(metalGraphicsPipeline.GetWinding());
        currentRenderEncoder->setRenderPipelineState(metalGraphicsPipeline.GetRenderPipelineState());

        // Save pipeline
        currentGraphicsPipeline = &metalGraphicsPipeline;
    }

    void MetalCommandBuffer::EndGraphicsPipeline(const std::unique_ptr<GraphicsPipeline> &graphicsPipeline)
    {
        SR_ERROR_IF(graphicsPipeline->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot end graphics pipeline [{0}], from command buffer [{1}]!", graphicsPipeline->GetName(), GetName());

        SR_ERROR_IF(currentRenderEncoder == nullptr, "[Metal]: Cannot end graphics pipeline [{0}] if no render pass is active within command buffer [{1}]!", graphicsPipeline->GetName(), GetName());
        currentGraphicsPipeline = nullptr;
    }

    void MetalCommandBuffer::BindVertexBuffer(const std::unique_ptr<Buffer> &vertexBuffer, const uint64 offset)
    {
        SR_ERROR_IF(vertexBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind vertex buffer [{0}], whose graphics API differs from [GraphicsAPI::Metal], within command buffer [{1}]!", vertexBuffer->GetName(), GetName());
        const MetalBuffer &metalVertexBuffer = static_cast<MetalBuffer&>(*vertexBuffer);

        SR_ERROR_IF(currentRenderEncoder == nullptr, "[Metal]: Cannot bind vertex buffer [{0}] if no render pass is active within command buffer [{1}]!", vertexBuffer->GetName(), GetName());

        SR_ERROR_IF(offset > vertexBuffer->GetMemorySize(), "[Metal]: Cannot bind vertex buffer [{0}] within command buffer [{1}] using specified offset of [{2}] bytes, which is outside of the [{3}] bytes size of the size of the buffer!", vertexBuffer->GetName(), GetName(), offset, vertexBuffer->GetMemorySize());
        currentRenderEncoder->setVertexBuffer(metalVertexBuffer.GetMetalBuffer(), offset, MetalPipelineLayout::VERTEX_BUFFER_SHADER_INDEX);
    }

    void MetalCommandBuffer::BindIndexBuffer(const std::unique_ptr<Buffer> &indexBuffer, const uint64 offset)
    {
        SR_ERROR_IF(indexBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind index buffer [{0}], whose graphics API differs from [GraphicsAPI::Metal], within command buffer [{1}]!", indexBuffer->GetName(), GetName());
        const MetalBuffer &metalIndexBuffer = static_cast<MetalBuffer&>(*indexBuffer);

        SR_ERROR_IF(offset > indexBuffer->GetMemorySize(), "[Metal]: Cannot bind index buffer [{0}] within command buffer [{1}] using specified offset of [{2}] bytes, which is outside of the [{3}] bytes size of the size of the buffer!", indexBuffer->GetName(), GetName(), offset, indexBuffer->GetMemorySize());
        SR_ERROR_IF(currentRenderEncoder == nullptr, "[Metal]: Cannot bind index buffer [{0}] if no render pass is active within command buffer [{1}]!", indexBuffer->GetName(), GetName());

        currentIndexBuffer = metalIndexBuffer.GetMetalBuffer();
        currentIndexBufferOffset = offset;
    }

    void MetalCommandBuffer::Draw(const uint32 vertexCount)
    {
        SR_ERROR_IF(currentRenderEncoder == nullptr, "[Metal]: Cannot draw if no render pass is active within command buffer [{0}]!", GetName());
        currentRenderEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), vertexCount);
    }

    void MetalCommandBuffer::DrawIndexed(const uint32 indexCount, const uint64 indexOffset, const uint64 vertexOffset)
    {
        SR_ERROR_IF(currentRenderEncoder == nullptr, "[Metal]: Cannot draw indexed if no render pass is active within command buffer [{0}]!", GetName());
        currentRenderEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, indexCount, MTL::IndexTypeUInt32, currentIndexBuffer, currentIndexBufferOffset + indexOffset, 1, static_cast<int32>(vertexOffset), 0);
    }

    void MetalCommandBuffer::PushConstants(const void* data, const uint16 memoryRange, const uint16 offset)
    {
        const MetalPipelineLayout &currentPipelineLayout = currentGraphicsPipeline->GetLayout();
        SR_ERROR_IF(memoryRange > currentPipelineLayout.GetPushConstantSize(), "[Metal]: Cannot push [{0}] bytes of push constant data within command buffer [{1}], as specified memory range is bigger than specified in the current pipeline's layout, which is [{2}] bytes!", memoryRange, GetName(), currentPipelineLayout.GetPushConstantSize());
        SR_ERROR_IF(currentRenderEncoder == nullptr, "[Metal]: Cannot push constants if no render pass is active within command buffer [{0}]!", GetName());

        currentRenderEncoder->setVertexBytes(data, memoryRange, currentPipelineLayout.GetPushConstantIndex());
        if (currentGraphicsPipeline->HasFragmentShader()) currentRenderEncoder->setFragmentBytes(data, memoryRange, currentPipelineLayout.GetPushConstantIndex());
    }

    void MetalCommandBuffer::BindBuffer(const uint32 binding, const std::unique_ptr<Buffer> &buffer, const uint32 arrayIndex, const uint64 memoryRange, const uint64 offset)
    {
        SR_ERROR_IF(buffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind buffer [{0}], whose graphics API differs from [GraphicsAPI::Metal], to binding [{1}] within command buffer [{2}]!", buffer->GetName(), binding, GetName());
        const MetalBuffer &metalBuffer = static_cast<MetalBuffer&>(*buffer);

        SR_ERROR_IF(offset + memoryRange > buffer->GetMemorySize(), "[Metal]: Cannot bind [{0}] bytes (offset by another [{1}] bytes) from buffer [{2}] within command buffer [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryRange, offset, buffer->GetName(), GetName(), offset + memoryRange, buffer->GetMemorySize());
        const MetalPipelineLayout &currentPipelineLayout = currentGraphicsPipeline->GetLayout();

        SR_ERROR_IF(currentRenderEncoder == nullptr, "[Metal]: Cannot bind buffer [{0}] if no render pass is active within command buffer [{1}]!", buffer->GetName(), GetName());
        currentRenderEncoder->setVertexBuffer(metalBuffer.GetMetalBuffer(), offset, currentPipelineLayout.GetBindingIndex(binding, arrayIndex));
        if (currentGraphicsPipeline->HasFragmentShader()) currentRenderEncoder->setFragmentBuffer(metalBuffer.GetMetalBuffer(), offset, currentPipelineLayout.GetBindingIndex(binding, arrayIndex));
    }

    void MetalCommandBuffer::BindImage(const uint32 binding, const std::unique_ptr<Image> &image, const uint32 arrayIndex)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind image [{0}], whose graphics API differs from [GraphicsAPI::Metal], to binding [{1}] within command buffer [{2}]!", image->GetName(), binding, GetName());
        const MetalImage &metalImage = static_cast<MetalImage&>(*image);

        const MetalPipelineLayout &currentPipelineLayout = currentGraphicsPipeline->GetLayout();
        SR_ERROR_IF(currentRenderEncoder == nullptr, "[Metal]: Cannot bind image [{0}] if no render pass is active within command buffer [{1}]!", image->GetName(), GetName());

        currentRenderEncoder->setVertexTexture(metalImage.GetMetalTexture(), currentPipelineLayout.GetBindingIndex(binding, arrayIndex));
        if (currentGraphicsPipeline->HasFragmentShader()) currentRenderEncoder->setFragmentTexture(metalImage.GetMetalTexture(), currentPipelineLayout.GetBindingIndex(binding, arrayIndex));
    }

    void MetalCommandBuffer::BeginDebugRegion(const std::string &regionName, const Color &color)
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif
        commandBuffer->pushDebugGroup(NS::String::string(regionName.c_str(), NS::ASCIIStringEncoding));
    }

    void MetalCommandBuffer::InsertDebugMarker(const std::string &markerName, const Color &color)
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif
        currentRenderEncoder->insertDebugSignpost(NS::String::string(markerName.c_str(), NS::ASCIIStringEncoding));
    }

    void MetalCommandBuffer::EndDebugRegion()
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif
        commandBuffer->popDebugGroup();
    }

    void MetalCommandBuffer::SynchronizeBufferUsage(const std::unique_ptr<Buffer> &buffer, const BufferCommandUsage previousUsage, const BufferCommandUsage nextUsage, const uint64 memorySize, const uint64 offset)
    {
        SR_ERROR_IF(buffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not synchronize usage of buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", buffer->GetName(), GetName());
        const MetalBuffer &metalBuffer = static_cast<MetalBuffer&>(*buffer);

        const MTL::Resource* bufferResource = metalBuffer.GetMetalBuffer();
        currentRenderEncoder->memoryBarrier(&bufferResource, 1, BufferCommandUsageToRenderStages(previousUsage), BufferCommandUsageToRenderStages(nextUsage));
    }

    void MetalCommandBuffer::SynchronizeImageUsage(const std::unique_ptr<Image> &image, const ImageCommandUsage previousUsage, const ImageCommandUsage nextUsage, const uint32 baseMipLevel, const uint32 mipLevelCount, const uint32 baseLayer, const uint32 layerCount)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not synchronize usage of image [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", image->GetName(), GetName());
        const MetalImage &metalImage = static_cast<MetalImage&>(*image);

        SR_ERROR_IF(baseMipLevel >= image->GetMipLevelCount(), "[Metal]: Cannot synchronize mip level [{0}] of image [{1}] within command buffer [{2}], as it does not have it!", baseMipLevel, image->GetName(), GetName());
        SR_ERROR_IF(baseMipLevel + mipLevelCount >= image->GetMipLevelCount(), "[Metal]: Cannot synchronize mip levels [{0}-{1}] of image [{2}] within command buffer [{3}], as they exceed image's mip level count - [{4}]!", baseMipLevel, baseMipLevel + mipLevelCount, image->GetName(), GetName(), image->GetMipLevelCount());
        SR_ERROR_IF(baseLayer >= image->GetLayerCount(), "[Metal]: Cannot synchronize layer [{0}] of image [{1}] within command buffer [{2}], as it does not have it!", baseLayer, image->GetName(), GetName());
        SR_ERROR_IF(baseLayer + layerCount >= image->GetLayerCount(), "[Metal]: Cannot synchronize layers [{0}-{1}] of image [{2}] within command buffer [{3}], as they exceed image's layer count - [{4}]!", baseLayer, baseLayer + layerCount, image->GetName(), GetName(), image->GetLayerCount());

        const MTL::Resource* textureResource = metalImage.GetMetalTexture();
        currentRenderEncoder->memoryBarrier(&textureResource, 1, ImageCommandUsageToRenderStages(previousUsage), ImageCommandUsageToRenderStages(nextUsage));
    }

    void MetalCommandBuffer::CopyBufferToBuffer(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Buffer> &destinationBuffer, uint64 memoryRange, const uint64 sourceOffset, const uint64 destinationOffset)
    {
        SR_ERROR_IF(sourceBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not copy from buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", sourceBuffer->GetName(), GetName());
        const MetalBuffer &metalSourceBuffer = static_cast<MetalBuffer&>(*sourceBuffer);

        SR_ERROR_IF(destinationBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not copy to buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", destinationBuffer->GetName(), GetName());
        const MetalBuffer &metalDestinationBuffer = static_cast<MetalBuffer&>(*destinationBuffer);

        memoryRange = memoryRange != 0 ? memoryRange : sourceBuffer->GetMemorySize();
        SR_ERROR_IF(sourceOffset + memoryRange > sourceBuffer->GetMemorySize(), "[Metal]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, from buffer [{2}] within command buffer [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryRange, sourceOffset, sourceBuffer->GetName(), GetName(), sourceOffset + memoryRange, sourceBuffer->GetMemorySize());
        SR_ERROR_IF(destinationOffset + memoryRange > destinationBuffer->GetMemorySize(), "[Metal]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, to buffer [{2}] within command buffer [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryRange, destinationOffset, destinationBuffer->GetName(), GetName(), destinationOffset + memoryRange, destinationBuffer->GetMemorySize());
        // Record copy
        MTL::BlitCommandEncoder* blitEncoder = commandBuffer->blitCommandEncoder();
        blitEncoder->copyFromBuffer(metalSourceBuffer.GetMetalBuffer(), sourceOffset, metalDestinationBuffer.GetMetalBuffer(), destinationOffset, memoryRange);
        blitEncoder->endEncoding();
    }

    void MetalCommandBuffer::CopyBufferToImage(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Image> &destinationImage, const Vector2UInt &pixelRange, const uint32 sourceOffset, const Vector2UInt &destinationOffset, const uint32 mipLevel, const uint32 baseLayer, const uint32 layerCount)
    {
        SR_ERROR_IF(sourceBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not copy from buffer [{0}], whose graphics API differs from [GraphicsAPI::Metal], to image [{1}] within command buffer [{2}]!", sourceBuffer->GetName(), destinationImage->GetName(), GetName());
        const MetalBuffer &metalSourceBuffer = static_cast<MetalBuffer&>(*sourceBuffer);

        SR_ERROR_IF(destinationImage->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not from buffer [{0}] to image [{1}], graphics API differs from [GraphicsAPI::Metal], within command buffer [{2}]!", sourceBuffer->GetName(), destinationImage->GetName(), GetName());
        const MetalImage &metalDestinationImage = static_cast<MetalImage&>(*destinationImage);

        SR_ERROR_IF(mipLevel >= destinationImage->GetMipLevelCount(), "[Metal]: Cannot copy from buffer [{0}] to mip level [{1}] of image [{2}] within command buffer [{3}], as it does not have it!", sourceBuffer->GetName(), mipLevel, destinationImage->GetName(), GetName());
        SR_ERROR_IF(baseLayer >= destinationImage->GetLayerCount(), "[Metal]: Cannot copy from buffer [{0}] to layer [{1}] of image [{2}] within command buffer [{3}], as it does not have it!", sourceBuffer->GetName(), baseLayer, destinationImage->GetName(), GetName());
        SR_ERROR_IF(baseLayer + layerCount >= destinationImage->GetLayerCount(), "[Metal]: Cannot copy from buffer [{0}] to layers [{1}-{2}] of image [{3}] within command buffer [{4}], as they exceed image's layer count - [{5}]!", sourceBuffer->GetName(), baseLayer, baseLayer + layerCount, destinationImage->GetName(), GetName(), destinationImage->GetLayerCount());
        SR_ERROR_IF((destinationOffset + pixelRange).x >= destinationImage->GetWidth() || (destinationOffset + pixelRange).y >= destinationImage->GetHeight(), "[Metal]: Cannot copy from buffer [{0}] pixel range [{1}x{2}], which is offset by another [{3}x{4}] pixels to image [{5}] within command buffer [{6}], as resulting pixel range of a total of [{7}x{8}] pixels is outside of image's dimensions - [{9}x{10}] !", sourceBuffer->GetName(), pixelRange.x, pixelRange.y, destinationOffset.x, destinationOffset.y, destinationImage->GetName(), GetName(), (destinationOffset + pixelRange).x, (destinationOffset + pixelRange).y, destinationImage->GetWidth(), destinationImage->GetHeight());

        const uint64 memoryRange = static_cast<uint64>(pixelRange.x != 0 ? pixelRange.x : destinationImage->GetWidth()) * (pixelRange.y != 0 ? pixelRange.y : destinationImage->GetHeight()) * destinationImage->GetPixelSize();
        SR_ERROR_IF(sourceOffset + (static_cast<uint64>(pixelRange.x) * pixelRange.y * destinationImage->GetPixelSize()) > sourceBuffer->GetMemorySize(), "[Metal]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, from buffer [{2}] within command buffer [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", pixelRange.x * pixelRange.y * destinationImage->GetPixelSize(), sourceOffset, sourceBuffer->GetName(), GetName(), sourceOffset + (static_cast<uint64>(pixelRange.x) * pixelRange.y * destinationImage->GetPixelSize()), sourceBuffer->GetMemorySize());
        SR_ERROR_IF((static_cast<uint64>(destinationOffset.x) + destinationOffset.y + pixelRange.x + pixelRange.y) * destinationImage->GetPixelSize() > destinationImage->GetMemorySize(), "[Metal]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, to image [{2}] within command buffer [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the image - [{5}]!", pixelRange.x * pixelRange.y * destinationImage->GetPixelSize(), (destinationOffset.x + destinationOffset.y) * destinationImage->GetPixelSize(), destinationImage->GetName(), GetName(), (static_cast<uint64>(destinationOffset.x) + destinationOffset.y + pixelRange.x + pixelRange.y) * destinationImage->GetPixelSize(), destinationImage->GetMemorySize());

        // Record copy
        MTL::BlitCommandEncoder* blitEncoder = commandBuffer->blitCommandEncoder();
        blitEncoder->copyFromBuffer(metalSourceBuffer.GetMetalBuffer(), sourceOffset, static_cast<uint64>(destinationImage->GetWidth()) * destinationImage->GetPixelSize(), 0, MTL::Size(destinationImage->GetWidth(), destinationImage->GetHeight(), 1), metalDestinationImage.GetMetalTexture(), memoryRange, mipLevel, MTL::Origin(destinationOffset.x, destinationOffset.y, 0));
        blitEncoder->endEncoding();
    }

    /* --- CONVERSIONS --- */

    MTL::RenderStages MetalCommandBuffer::BufferCommandUsageToRenderStages(const BufferCommandUsage bufferCommandUsage)
    {
        switch (bufferCommandUsage)
        {
            case BufferCommandUsage::MemoryRead:
            case BufferCommandUsage::MemoryWrite:
            case BufferCommandUsage::ComputeRead:
            case BufferCommandUsage::ComputeWrite:       return 0;
            case BufferCommandUsage::VertexRead:
            case BufferCommandUsage::IndexRead:          return MTL::RenderStageVertex;
            case BufferCommandUsage::GraphicsWrite:
            case BufferCommandUsage::GraphicsRead:       return MTL::RenderStageFragment;
        }
    }

    MTL::RenderStages MetalCommandBuffer::ImageCommandUsageToRenderStages(const ImageCommandUsage imageCommandUsage)
    {
        switch (imageCommandUsage)
        {
            case ImageCommandUsage::MemoryRead:
            case ImageCommandUsage::MemoryWrite:
            case ImageCommandUsage::ComputeRead:
            case ImageCommandUsage::ComputeWrite:       return 0;
            case ImageCommandUsage::AttachmentRead:
            case ImageCommandUsage::AttachmentWrite:
            case ImageCommandUsage::DepthRead:
            case ImageCommandUsage::DepthWrite:
            case ImageCommandUsage::GraphicsWrite:
            case ImageCommandUsage::GraphicsRead:
            case ImageCommandUsage::Present:            return MTL::RenderStageFragment;
        }
    }

}
