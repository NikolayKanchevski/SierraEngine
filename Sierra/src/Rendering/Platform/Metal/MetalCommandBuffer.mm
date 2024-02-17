//
// Created by Nikolay Kanchevski on 12.12.23.
//

#include "MetalCommandBuffer.h"

#include "MetalBuffer.h"
#include "MetalImage.h"
#include "MetalSampler.h"

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
        // Set up command buffer descriptor
        MTLCommandBufferDescriptor* const commandBufferDescriptor = [[MTLCommandBufferDescriptor alloc] init];
        #if SR_ENABLE_LOGGING
            [commandBufferDescriptor setErrorOptions: MTLCommandBufferErrorOptionEncoderExecutionStatus];
        #endif

        // Create command buffer
        commandBuffer = [device.GetCommandQueue() commandBufferWithDescriptor: commandBufferDescriptor];
        device.SetResourceName(commandBuffer, GetName());

        // Create completion synchronization
        completionSignalValue = device.GetNewSignalValue();
        [commandBuffer addCompletedHandler: ^(id<MTLCommandBuffer> executedCommandBuffer) {
            queuedBuffers = std::queue<std::unique_ptr<Buffer>>();
            queuedImages = std::queue<std::unique_ptr<Image>>();

            #if SR_ENABLE_LOGGING
                NSError* const error = executedCommandBuffer.error;
                SR_ERROR_IF(error != nil, "[Metal]: Submission of command buffer [{0}] failed! Error code: {1}.", GetName(), error.description.UTF8String);
            #endif
            #if SR_PLATFORM_macOS
                [executedCommandBuffer release];
            #endif
        }];

        [commandBufferDescriptor release];
    }

    void MetalCommandBuffer::End()
    {
        if (currentBlitEncoder != nil)
        {
            [currentBlitEncoder endEncoding];
            #if SR_PLATFORM_macOS
                [currentBlitEncoder release];
            #endif
            currentBlitEncoder = nil;
        }

        currentIndexBuffer = nil;
        currentIndexBufferByteOffset = 0;
        currentVertexBufferByteOffset = 0;
    }

    void MetalCommandBuffer::SynchronizeBufferUsage(const std::unique_ptr<Buffer> &buffer, const BufferCommandUsage previousUsage, const BufferCommandUsage nextUsage, const uint64 memorySize, const uint64 byteOffset)
    {
        SR_ERROR_IF(buffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not synchronize usage of buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", buffer->GetName(), GetName());
        const MetalBuffer &metalBuffer = static_cast<MetalBuffer&>(*buffer);

        const id<MTLResource> bufferResource = metalBuffer.GetMetalBuffer();
        [currentRenderEncoder memoryBarrierWithResources: &bufferResource count: 1 afterStages: BufferCommandUsageToRenderStages(previousUsage) beforeStages: BufferCommandUsageToRenderStages(nextUsage)];
    }

    void MetalCommandBuffer::SynchronizeImageUsage(const std::unique_ptr<Image> &image, const ImageCommandUsage previousUsage, const ImageCommandUsage nextUsage, const uint32 baseMipLevel, uint32 mipLevelCount, const uint32 baseLayer, uint32 layerCount)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not synchronize usage of image [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", image->GetName(), GetName());
        const MetalImage &metalImage = static_cast<MetalImage&>(*image);

        SR_ERROR_IF(baseMipLevel >= image->GetMipLevelCount(), "[Metal]: Cannot synchronize mip level [{0}] of image [{1}] within command buffer [{2}], as it does not have it!", baseMipLevel, image->GetName(), GetName());
        SR_ERROR_IF(baseLayer >= image->GetLayerCount(), "[Metal]: Cannot synchronize layer [{0}] of image [{1}] within command buffer [{2}], as it does not have it!", baseLayer, image->GetName(), GetName());

        mipLevelCount = mipLevelCount != 0 ? mipLevelCount : image->GetMipLevelCount() - baseMipLevel;
        layerCount = layerCount != 0 ? layerCount : image->GetLayerCount() - baseLayer;

        SR_ERROR_IF(baseMipLevel + mipLevelCount > image->GetMipLevelCount(), "[Metal]: Cannot synchronize mip levels [{0}-{1}] of image [{2}] within command buffer [{3}], as they exceed image's mip level count - [{4}]!", baseMipLevel, baseMipLevel + mipLevelCount - 1, image->GetName(), GetName(), image->GetMipLevelCount());
        SR_ERROR_IF(baseLayer + layerCount > image->GetLayerCount(), "[Metal]: Cannot synchronize layers [{0}-{1}] of image [{2}] within command buffer [{3}], as they exceed image's layer count - [{4}]!", baseLayer, baseLayer + layerCount - 1, image->GetName(), GetName(), image->GetLayerCount());

        const id<MTLResource> textureResource = metalImage.GetMetalTexture();
        [currentRenderEncoder memoryBarrierWithResources: &textureResource count: 1 afterStages: ImageCommandUsageToRenderStages(previousUsage) beforeStages: ImageCommandUsageToRenderStages(nextUsage)];
    }

    void MetalCommandBuffer::CopyBufferToBuffer(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Buffer> &destinationBuffer, uint64 memoryRange, const uint64 sourceByteOffset, const uint64 destinationByteOffset)
    {
        SR_ERROR_IF(sourceBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not copy from buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", sourceBuffer->GetName(), GetName());
        const MetalBuffer &metalSourceBuffer = static_cast<MetalBuffer&>(*sourceBuffer);

        SR_ERROR_IF(destinationBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not copy to buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", destinationBuffer->GetName(), GetName());
        const MetalBuffer &metalDestinationBuffer = static_cast<MetalBuffer&>(*destinationBuffer);

        memoryRange = memoryRange != 0 ? memoryRange : sourceBuffer->GetMemorySize();
        SR_ERROR_IF(sourceByteOffset + memoryRange > sourceBuffer->GetMemorySize(), "[Metal]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, from buffer [{2}] within command buffer [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryRange, sourceByteOffset, sourceBuffer->GetName(), GetName(), sourceByteOffset + memoryRange, sourceBuffer->GetMemorySize());
        SR_ERROR_IF(destinationByteOffset + memoryRange > destinationBuffer->GetMemorySize(), "[Metal]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, to buffer [{2}] within command buffer [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryRange, destinationByteOffset, destinationBuffer->GetName(), GetName(), destinationByteOffset + memoryRange, destinationBuffer->GetMemorySize());

        if (currentBlitEncoder == nil)
        {
            currentBlitEncoder = [commandBuffer blitCommandEncoder];
            device.SetResourceName(currentBlitEncoder , "Transfer Encoder");
        }

        [currentBlitEncoder copyFromBuffer: metalSourceBuffer.GetMetalBuffer() sourceOffset: sourceByteOffset toBuffer: metalDestinationBuffer.GetMetalBuffer() destinationOffset: destinationByteOffset size: memoryRange];
    }

    void MetalCommandBuffer::CopyBufferToImage(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Image> &destinationImage, const uint32 mipLevel, const Vector2UInt &pixelRange, const uint32 layer, const  uint64 sourceByteOffset, const Vector2UInt &destinationPixelOffset)
    {
        SR_ERROR_IF(sourceBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not copy from buffer [{0}], whose graphics API differs from [GraphicsAPI::Metal], to image [{1}] within command buffer [{2}]!", sourceBuffer->GetName(), destinationImage->GetName(), GetName());
        const MetalBuffer &metalSourceBuffer = static_cast<MetalBuffer&>(*sourceBuffer);

        SR_ERROR_IF(destinationImage->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not from buffer [{0}] to image [{1}], graphics API differs from [GraphicsAPI::Metal], within command buffer [{2}]!", sourceBuffer->GetName(), destinationImage->GetName(), GetName());
        const MetalImage &metalDestinationImage = static_cast<MetalImage&>(*destinationImage);

        SR_ERROR_IF(mipLevel >= destinationImage->GetMipLevelCount(), "[Metal]: Cannot copy from buffer [{0}] to mip level [{1}] of image [{2}] within command buffer [{3}], as image does not contain it!", sourceBuffer->GetName(), mipLevel, destinationImage->GetName(), GetName());
        SR_ERROR_IF(layer >= destinationImage->GetLayerCount(), "[Metal]: Cannot copy from buffer [{0}] to layer [{1}] of image [{2}] within command buffer [{3}], as image does not contain it!", sourceBuffer->GetName(), layer, destinationImage->GetName(), GetName());

        const MTLSize sourceSize = MTLSizeMake(pixelRange.x != 0 ? pixelRange.x : destinationImage->GetWidth() >> mipLevel, pixelRange.y != 0 ? pixelRange.y : destinationImage->GetHeight() >> mipLevel, 1);
        SR_ERROR_IF(destinationPixelOffset.x + sourceSize.width > destinationImage->GetWidth() || destinationPixelOffset.y + sourceSize.height > destinationImage->GetHeight(), "[Metal]: Cannot copy from buffer [{0}] pixel range [{1}x{2}], which is offset by another [{3}x{4}] pixels to image [{5}] within command buffer [{6}], as resulting pixel range of a total of [{7}x{8}] pixels exceeds the image's dimensions - [{9}x{10}]!", sourceBuffer->GetName(), sourceSize.width, sourceSize.height, destinationPixelOffset.x, destinationPixelOffset.y, destinationImage->GetName(), GetName(), destinationPixelOffset.x + sourceSize.width, destinationPixelOffset.y + sourceSize.height, destinationImage->GetWidth(), destinationImage->GetHeight());
        SR_ERROR_IF(sourceByteOffset + static_cast<uint64>(sourceSize.width) * sourceSize.height * destinationImage->GetPixelMemorySize() > sourceBuffer->GetMemorySize(), "[Metal]: Cannot copy from buffer [{0}] pixel range [{1}x{2}], which is offset by another [{3}] bytes, to image [{4}], as total memory range [{5}] overflows that of the buffer - [{6}]!", sourceBuffer->GetName(), sourceSize.width, sourceSize.height, sourceByteOffset, destinationImage->GetName(), sourceByteOffset + sourceSize.width * sourceSize.height * destinationImage->GetPixelMemorySize(), sourceBuffer->GetMemorySize());

        if (currentBlitEncoder == nil)
        {
            currentBlitEncoder = [commandBuffer blitCommandEncoder];
            device.SetResourceName(currentBlitEncoder , "Transfer Encoder");
        }

        [currentBlitEncoder copyFromBuffer: metalSourceBuffer.GetMetalBuffer() sourceOffset: sourceByteOffset sourceBytesPerRow: sourceSize.width * destinationImage->GetPixelMemorySize() sourceBytesPerImage: 0 sourceSize: sourceSize toTexture: metalDestinationImage.GetMetalTexture() destinationSlice: layer destinationLevel: mipLevel destinationOrigin: MTLOriginMake(destinationPixelOffset.x, destinationPixelOffset.y, 0)];
    }

    void MetalCommandBuffer::GenerateMipMapsForImage(const std::unique_ptr<Image> &image)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot generate mip maps for image [{0}], whose graphics API differs from [GraphicsAPI::Metal], within command buffer [{1}]!", image->GetName(), GetName());
        SR_ERROR_IF(image->GetMipLevelCount() <= 1, "[Metal]: Cannot generate mip maps for image [{0}], as it has a single mip level only!", image->GetName());

        if (currentBlitEncoder == nil)
        {
            const id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];
            device.SetResourceName(blitEncoder, "Transfer Encoder");
        }

        const MetalImage &metalImage = static_cast<MetalImage&>(*image);
        [currentBlitEncoder generateMipmapsForTexture: metalImage.GetMetalTexture()];
    }

    void MetalCommandBuffer::BeginRenderPass(const std::unique_ptr<RenderPass> &renderPass, const std::initializer_list<RenderPassBeginAttachment> &attachments)
    {
        SR_ERROR_IF(renderPass->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin render pass [{0}], whose graphics API differs from [GraphicsAPI::Metal], from command buffer [{1}]!", renderPass->GetName(), GetName());
        const MetalRenderPass &metalRenderPass = static_cast<MetalRenderPass&>(*renderPass);

        SR_ERROR_IF(attachments.size() != metalRenderPass.GetAttachmentCount(), "[Metal]: Cannot begin render pass [{0}] within command buffer [{1}] with [{2}] attachments, as it was created to hold [{3}]!", renderPass->GetName(), GetName(), attachments.size(), metalRenderPass.GetAttachmentCount());
        for (uint32 i = 0; i < metalRenderPass.GetSubpassCount(); i++)
        {
            MTLRenderPassDescriptor* const subpass = metalRenderPass.GetSubpass(i);

            uint32 subpassColorAttachmentCount = 0;
            for (const auto renderTargetIndex : metalRenderPass.GetSubpassRenderTargets(i))
            {
                const RenderPassBeginAttachment &attachment = *(attachments.begin() + renderTargetIndex);

                SR_ERROR_IF(attachment.image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin render pass [{0}] within command buffer [{1}] using image [{1}] as attachment [{2}], as its graphics API differs from [GraphicsAPI::Metal]!", renderPass->GetName(), GetName(), attachment.image->GetName(), renderTargetIndex);
                const MetalImage &metalImage = static_cast<MetalImage&>(*attachment.image);

                if (metalRenderPass.GetDepthAttachmentIndex() == renderTargetIndex)
                {
                    [subpass.depthAttachment setTexture: metalImage.GetMetalTexture()];
                }
                else
                {
                    if (std::find(metalRenderPass.GetResolvedColorAttachmentIndices().begin(), metalRenderPass.GetResolvedColorAttachmentIndices().end(), i) != metalRenderPass.GetResolvedColorAttachmentIndices().end())
                    {
                        SR_ERROR_IF(renderTargetIndex + 1 >= attachments.size(), "[Metal]: Cannot begin render pass [{0}] within command buffer [{1}], as resolve attachment at index [{2}] is missing its consecutive color attachment!", renderPass->GetName(), GetName(), renderTargetIndex);
                        const RenderPassBeginAttachment &colorAttachment = *(attachments.begin() + renderTargetIndex + 1);

                        SR_ERROR_IF(colorAttachment.image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin render pass [{0}] within command buffer [{1}] using image [{2}] as attachment [{3}], as its graphics API differs from [GraphicsAPI::Metal]!", renderPass->GetName(), GetName(), colorAttachment.image->GetName(), renderTargetIndex + 1);
                        const MetalImage &metalColorImage = static_cast<MetalImage&>(*colorAttachment.image);

                        [subpass.colorAttachments[subpassColorAttachmentCount] setResolveTexture: metalImage.GetMetalTexture()];
                        [subpass.colorAttachments[subpassColorAttachmentCount] setTexture: metalColorImage.GetMetalTexture()];
                        subpassColorAttachmentCount++;
                    }

                    [subpass.colorAttachments[subpassColorAttachmentCount] setTexture: metalImage.GetMetalTexture()];
                    [subpass.colorAttachments[subpassColorAttachmentCount] setClearColor: MTLClearColorMake(attachment.clearColor.r, attachment.clearColor.g, attachment.clearColor.b, attachment.clearColor.a)];
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
        currentRenderEncoder = [commandBuffer renderCommandEncoderWithDescriptor: metalRenderPass.GetSubpass(currentSubpass)];
        device.SetResourceName(currentComputeEncoder, "Render encoder for render pass [" + renderPass->GetName() + "]");

        // Define viewport
        MTLViewport viewport = { };
        viewport.originX = 0.0f;
        viewport.originY = 0.0f;
        viewport.width = static_cast<float64>(metalRenderPass.GetSubpass(currentSubpass).renderTargetWidth);
        viewport.height = static_cast<float64>(metalRenderPass.GetSubpass(currentSubpass).renderTargetHeight);
        viewport.znear = 0.0f;
        viewport.zfar = 1.0f;

        // Set viewport
        [currentRenderEncoder setViewport: viewport];

        // Define scissor
        MTLScissorRect scissor = { };
        scissor.x = 0;
        scissor.y = 0;
        scissor.width = static_cast<uint64>(metalRenderPass.GetSubpass(currentSubpass).renderTargetWidth);
        scissor.height = static_cast<uint64>(metalRenderPass.GetSubpass(currentSubpass).renderTargetHeight);

        // Set scissor
        [currentRenderEncoder setScissorRect: scissor];

        // Increment current subpass for future usage
        currentSubpass++;
    }

    void MetalCommandBuffer::EndRenderPass(const std::unique_ptr<RenderPass> &renderPass)
    {
        SR_ERROR_IF(renderPass->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot end render pass [{0}], whose graphics API differs from [GraphicsAPI::Metal], from command buffer [{1}]!", renderPass->GetName(), GetName());

        SR_ERROR_IF(currentRenderEncoder == nil, "[Metal]: Cannot end render pass [{0}], as it has not been began within command buffer [{1}]!", renderPass->GetName(), GetName());
        [currentRenderEncoder endEncoding];
        #if SR_PLATFORM_macOS
                [currentRenderEncoder release];
        #endif

        currentRenderEncoder = nil;
        currentSubpass = 0;
    }

    void MetalCommandBuffer::BeginGraphicsPipeline(const std::unique_ptr<GraphicsPipeline> &graphicsPipeline)
    {
        SR_ERROR_IF(graphicsPipeline->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin graphics pipeline [{0}], whose graphics API differs from [GraphicsAPI::Metal], from command buffer [{1}]!", graphicsPipeline->GetName(), GetName());
        const MetalGraphicsPipeline &metalGraphicsPipeline = static_cast<MetalGraphicsPipeline&>(*graphicsPipeline);

        SR_ERROR_IF(currentRenderEncoder == nil, "[Metal]: Cannot begin graphics pipeline [{0}] if no render encoder is active within command buffer [{1}]!", graphicsPipeline->GetName(), GetName());

        // Bind pipeline and set appropriate settings
        [currentRenderEncoder setCullMode: metalGraphicsPipeline.GetCullMode()];
        [currentRenderEncoder setTriangleFillMode: metalGraphicsPipeline.GetTriangleFillMode()];
        [currentRenderEncoder setFrontFacingWinding: metalGraphicsPipeline.GetWinding()];
        [currentRenderEncoder setRenderPipelineState: metalGraphicsPipeline.GetRenderPipelineState()];
        if (metalGraphicsPipeline.GetDepthStencilState() != nil) [currentRenderEncoder setDepthStencilState: metalGraphicsPipeline.GetDepthStencilState()];

        // Save pipeline
        currentGraphicsPipeline = &metalGraphicsPipeline;
    }

    void MetalCommandBuffer::EndGraphicsPipeline(const std::unique_ptr<GraphicsPipeline> &graphicsPipeline)
    {
        SR_ERROR_IF(graphicsPipeline->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot end graphics pipeline [{0}], from command buffer [{1}]!", graphicsPipeline->GetName(), GetName());

        SR_ERROR_IF(currentRenderEncoder == nil, "[Metal]: Cannot end graphics pipeline [{0}] if no render encoder is active within command buffer [{1}]!", graphicsPipeline->GetName(), GetName());
        currentGraphicsPipeline = nil;
    }

    void MetalCommandBuffer::BindVertexBuffer(const std::unique_ptr<Buffer> &vertexBuffer, const uint64 byteOffset)
    {
        SR_ERROR_IF(vertexBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind vertex buffer [{0}], whose graphics API differs from [GraphicsAPI::Metal], within command buffer [{1}]!", vertexBuffer->GetName(), GetName());
        const MetalBuffer &metalVertexBuffer = static_cast<MetalBuffer&>(*vertexBuffer);

        SR_ERROR_IF(currentRenderEncoder == nil, "[Metal]: Cannot bind vertex buffer [{0}] if no render encoder is active within command buffer [{1}]!", vertexBuffer->GetName(), GetName());

        SR_ERROR_IF(byteOffset > vertexBuffer->GetMemorySize(), "[Metal]: Cannot bind vertex buffer [{0}] within command buffer [{1}] using specified offset of [{2}] bytes, which is outside of the [{3}] bytes size of the size of the buffer!", vertexBuffer->GetName(), GetName(), byteOffset, vertexBuffer->GetMemorySize());
        [currentRenderEncoder setVertexBuffer: metalVertexBuffer.GetMetalBuffer() offset: byteOffset atIndex: MetalPipelineLayout::VERTEX_BUFFER_SHADER_INDEX];
        currentVertexBufferByteOffset = byteOffset;
    }

    void MetalCommandBuffer::BindIndexBuffer(const std::unique_ptr<Buffer> &indexBuffer, const uint64 byteOffset)
    {
        SR_ERROR_IF(indexBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind index buffer [{0}], whose graphics API differs from [GraphicsAPI::Metal], within command buffer [{1}]!", indexBuffer->GetName(), GetName());
        const MetalBuffer &metalIndexBuffer = static_cast<MetalBuffer&>(*indexBuffer);

        SR_ERROR_IF(byteOffset > indexBuffer->GetMemorySize(), "[Metal]: Cannot bind index buffer [{0}] within command buffer [{1}] using specified offset of [{2}] bytes, which is outside of the [{3}] bytes size of the size of the buffer!", indexBuffer->GetName(), GetName(), byteOffset, indexBuffer->GetMemorySize());
        SR_ERROR_IF(currentRenderEncoder == nil, "[Metal]: Cannot bind index buffer [{0}] if no render encoder is active within command buffer [{1}]!", indexBuffer->GetName(), GetName());

        currentIndexBuffer = metalIndexBuffer.GetMetalBuffer();
        currentIndexBufferByteOffset = byteOffset;
    }

    void MetalCommandBuffer::SetScissor(const Vector4UInt &scissor)
    {
        SR_ERROR_IF(currentRenderEncoder == nil, "[Metal]: Cannot set scissor if no render encoder is active within command buffer [{0}]!", GetName());
        [currentRenderEncoder setScissorRect: { scissor.x, scissor.y, scissor.z, scissor.w }];
    }

    void MetalCommandBuffer::Draw(const uint32 vertexCount, const uint32 vertexOffset)
    {
        SR_ERROR_IF(currentRenderEncoder == nil, "[Metal]: Cannot draw if no render encoder is active within command buffer [{0}]!", GetName());
        const uint64 newVertexBufferByteOffset = currentVertexBufferByteOffset + (static_cast<uint64>(vertexOffset) * currentGraphicsPipeline->GetVertexByteStride());
        if (newVertexBufferByteOffset > 0) [currentRenderEncoder setVertexBufferOffset: newVertexBufferByteOffset atIndex: MetalPipelineLayout::VERTEX_BUFFER_SHADER_INDEX];
        [currentRenderEncoder drawPrimitives: MTLPrimitiveTypeTriangle vertexStart: 0 vertexCount: vertexCount];
    }

    void MetalCommandBuffer::DrawIndexed(const uint32 indexCount, const uint32 indexOffset, const uint32 vertexOffset)
    {
        SR_ERROR_IF(currentRenderEncoder == nil, "[Metal]: Cannot draw indexed if no render encoder is active within command buffer [{0}]!", GetName());
        const uint64 newVertexBufferByteOffset = currentVertexBufferByteOffset + (static_cast<uint64>(vertexOffset) * currentGraphicsPipeline->GetVertexByteStride());
        if (newVertexBufferByteOffset > 0) [currentRenderEncoder setVertexBufferOffset: newVertexBufferByteOffset atIndex: MetalPipelineLayout::VERTEX_BUFFER_SHADER_INDEX];
        [currentRenderEncoder drawIndexedPrimitives: MTLPrimitiveTypeTriangle indexCount: indexCount indexType: MTLIndexTypeUInt32 indexBuffer: currentIndexBuffer indexBufferOffset: currentIndexBufferByteOffset + indexOffset * sizeof(uint32) instanceCount: 1];
    }

    void MetalCommandBuffer::BeginComputePipeline(const std::unique_ptr<ComputePipeline> &computePipeline)
    {
        SR_ERROR_IF(computePipeline->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin compute graphicsPipeline [{0}], whose graphics API differs from [GraphicsAPI::Metal], from command buffer [{1}]!", computePipeline->GetName(), GetName());
        const MetalComputePipeline &metalComputePipeline = static_cast<MetalComputePipeline&>(*computePipeline);

        // Begin encoding compute commands
        currentComputeEncoder = [commandBuffer computeCommandEncoderWithDispatchType: MTLDispatchTypeConcurrent];
        device.SetResourceName(currentComputeEncoder, "Compute encoder for pipeline [" + computePipeline->GetName() + "]");

        // Assign provided compute pipeline
        [currentComputeEncoder setComputePipelineState: metalComputePipeline.GetComputePipelineState()];
        currentComputePipeline = &metalComputePipeline;
    }

    void MetalCommandBuffer::EndComputePipeline(const std::unique_ptr<ComputePipeline> &computePipeline)
    {
        SR_ERROR_IF(computePipeline->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot end compute pipeline [{0}], from command buffer [{1}]!", computePipeline->GetName(), GetName());
        [currentComputeEncoder endEncoding];

        #if SR_PLATFORM_macOS
            [currentComputeEncoder release];
        #endif
        currentComputeEncoder = nil;
    }

    void MetalCommandBuffer::Dispatch(const uint32 xWorkGroupCount, const uint32 yWorkGroupCount, const uint32 zWorkGroupCount)
    {
        SR_ERROR_IF(currentComputeEncoder == nil, "[Metal]: Cannot dispatch if no compute pipeline is active within command buffer [{0}]!", GetName());

        // Dispatch work groups
        [currentComputeEncoder dispatchThreadgroups: MTLSizeMake(xWorkGroupCount, yWorkGroupCount, zWorkGroupCount) threadsPerThreadgroup: MTLSizeMake(1, 1, 1)];
    }

    void MetalCommandBuffer::PushConstants(const void* data, const uint16 memoryRange, const uint16 byteOffset)
    {
        SR_ERROR_IF(currentRenderEncoder == nil && currentComputeEncoder == nil, "[Metal]: Cannot push constants if no encoder is active within command buffer [{0}]!", GetName());
        if (currentComputeEncoder != nil)
        {
            const MetalPipelineBinding bindingData = currentComputePipeline->GetLayout().GetPushConstantBinding();
            [currentComputeEncoder setBytes: data length: memoryRange atIndex: bindingData.index];
        }
        else
        {
            const MetalPipelineBinding bindingData = currentGraphicsPipeline->GetLayout().GetPushConstantBinding();
            [currentRenderEncoder setVertexBytes: data length: memoryRange atIndex: bindingData.index];
            if (currentGraphicsPipeline->HasFragmentShader()) [currentRenderEncoder setFragmentBytes: data length: memoryRange atIndex: bindingData.index];
        }
    }

    void MetalCommandBuffer::BindBuffer(const uint32 binding, const std::unique_ptr<Buffer> &buffer, const uint32 arrayIndex, const uint64 memoryRange, const uint64 byteOffset)
    {
        SR_ERROR_IF(buffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind buffer [{0}], whose graphics API differs from [GraphicsAPI::Metal], to binding [{1}] within command buffer [{2}]!", buffer->GetName(), binding, GetName());
        const MetalBuffer &metalBuffer = static_cast<MetalBuffer&>(*buffer);

        SR_ERROR_IF(byteOffset + memoryRange > buffer->GetMemorySize(), "[Metal]: Cannot bind [{0}] bytes (offset by another [{1}] bytes) from buffer [{2}] within command buffer [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryRange, byteOffset, buffer->GetName(), GetName(), byteOffset + memoryRange, buffer->GetMemorySize());

        SR_ERROR_IF(currentRenderEncoder == nil && currentComputeEncoder == nil, "[Metal]: Cannot bind buffer [{0}] if no encoder is active within command buffer [{1}]!", buffer->GetName(), GetName());
        if (currentComputeEncoder != nil)
        {
            const MetalPipelineBinding bindingData = currentComputePipeline->GetLayout().GetBindingData(binding);
            [currentComputeEncoder setBuffer: metalBuffer.GetMetalBuffer() offset: byteOffset atIndex: bindingData.index];
        }
        else
        {
            const MetalPipelineBinding bindingData = currentGraphicsPipeline->GetLayout().GetBindingData(binding);
            [currentRenderEncoder setVertexBuffer: metalBuffer.GetMetalBuffer() offset: byteOffset atIndex: bindingData.index];
            if (currentGraphicsPipeline->HasFragmentShader()) [currentRenderEncoder setFragmentBuffer: metalBuffer.GetMetalBuffer() offset: byteOffset atIndex: bindingData.index];
        }
    }

    void MetalCommandBuffer::BindImage(const uint32 binding, const std::unique_ptr<Image> &image, const uint32 arrayIndex)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind image [{0}], whose graphics API differs from [GraphicsAPI::Metal], to binding [{1}] within command buffer [{2}]!", image->GetName(), binding, GetName());
        const MetalImage &metalImage = static_cast<MetalImage&>(*image);

        SR_ERROR_IF(currentRenderEncoder == nil && currentComputeEncoder == nil, "[Metal]: Cannot bind image [{0}] if no encoder is active within command buffer [{1}]!", image->GetName(), GetName());
        if (currentComputeEncoder != nil)
        {
            const MetalPipelineBinding bindingData = currentComputePipeline->GetLayout().GetBindingData(binding);
            [currentComputeEncoder setTexture: metalImage.GetMetalTexture() atIndex: bindingData.index];
        }
        else
        {
            const MetalPipelineBinding bindingData = currentGraphicsPipeline->GetLayout().GetBindingData(binding);
            [currentRenderEncoder setVertexTexture: metalImage.GetMetalTexture() atIndex: bindingData.index];
            if (currentGraphicsPipeline->HasFragmentShader()) [currentRenderEncoder setFragmentTexture: metalImage.GetMetalTexture() atIndex: bindingData.index];
        }
    }

    void MetalCommandBuffer::BindImage(const uint32 binding, const std::unique_ptr<Image> &image, const std::unique_ptr<Sampler> &sampler, const uint32 arrayIndex)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind image [{0}], whose graphics API differs from [GraphicsAPI::Metal], to binding [{1}] within command buffer [{2}]!", image->GetName(), binding, GetName());
        const MetalImage &metalImage = static_cast<MetalImage&>(*image);

        SR_ERROR_IF(sampler->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind image [{0}] using sampler [{1}], whose graphics API differs from [GraphicsAPI::Metal], to binding [{2}] within command buffer [{3}]!", image->GetName(), sampler->GetName(), binding, GetName());
        const MetalSampler &metalSampler = static_cast<MetalSampler&>(*sampler);

        SR_ERROR_IF(currentRenderEncoder == nil && currentComputeEncoder == nil, "[Metal]: Cannot bind image [{0}] if no encoder is active within command buffer [{1}]!", image->GetName(), GetName());

        if (currentComputeEncoder != nil)
        {
            const MetalPipelineBinding bindingData = currentComputePipeline->GetLayout().GetBindingData(binding);
            [currentComputeEncoder setTexture: metalImage.GetMetalTexture() atIndex: bindingData.index];
            [currentComputeEncoder setSamplerState: metalSampler.GetSamplerState() atIndex: bindingData.index];
        }
        else
        {
            const MetalPipelineBinding bindingData = currentGraphicsPipeline->GetLayout().GetBindingData(binding);
            [currentRenderEncoder setVertexTexture: metalImage.GetMetalTexture() atIndex: bindingData.index];
            [currentRenderEncoder setVertexSamplerState: metalSampler.GetSamplerState() atIndex: bindingData.data.textureData.samplerIndex];
            if (currentGraphicsPipeline->HasFragmentShader())
            {
                [currentRenderEncoder setFragmentTexture: metalImage.GetMetalTexture() atIndex: bindingData.index];
                [currentRenderEncoder setFragmentSamplerState: metalSampler.GetSamplerState() atIndex: bindingData.data.textureData.samplerIndex];
            }
        }
    }

    void MetalCommandBuffer::BeginDebugRegion(const std::string &regionName, const Color &color)
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif
        [commandBuffer pushDebugGroup: [NSString stringWithCString: regionName.c_str() encoding: NSASCIIStringEncoding]];
    }

    void MetalCommandBuffer::InsertDebugMarker(const std::string &markerName, const Color &color)
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif
        [currentRenderEncoder insertDebugSignpost: [NSString stringWithCString: markerName.c_str() encoding: NSASCIIStringEncoding]];
    }

    void MetalCommandBuffer::EndDebugRegion()
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif
        [commandBuffer popDebugGroup];
    }

    /* --- CONVERSIONS --- */

    MTLRenderStages MetalCommandBuffer::BufferCommandUsageToRenderStages(const BufferCommandUsage bufferCommandUsage)
    {
        switch (bufferCommandUsage)
        {
            case BufferCommandUsage::None:
            case BufferCommandUsage::MemoryRead:
            case BufferCommandUsage::MemoryWrite:
            case BufferCommandUsage::ComputeRead:
            case BufferCommandUsage::ComputeWrite:       return 0;
            case BufferCommandUsage::VertexRead:
            case BufferCommandUsage::IndexRead:          return MTLRenderStageVertex;
            case BufferCommandUsage::GraphicsWrite:
            case BufferCommandUsage::GraphicsRead:       return MTLRenderStageFragment;
        }
    }

    MTLRenderStages MetalCommandBuffer::ImageCommandUsageToRenderStages(const ImageCommandUsage imageCommandUsage)
    {
        switch (imageCommandUsage)
        {
            case ImageCommandUsage::None:
            case ImageCommandUsage::MemoryRead:
            case ImageCommandUsage::MemoryWrite:
            case ImageCommandUsage::ComputeRead:
            case ImageCommandUsage::ComputeWrite:       return 0;
            case ImageCommandUsage::ColorRead:
            case ImageCommandUsage::ColorWrite:
            case ImageCommandUsage::DepthRead:
            case ImageCommandUsage::DepthWrite:
            case ImageCommandUsage::GraphicsWrite:
            case ImageCommandUsage::GraphicsRead:
            case ImageCommandUsage::Present:            return MTLRenderStageFragment;
        }
    }

}
