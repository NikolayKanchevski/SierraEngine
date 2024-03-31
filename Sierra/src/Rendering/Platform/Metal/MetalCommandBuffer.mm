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
        [commandBuffer addCompletedHandler: ^(id<MTLCommandBuffer> executedCommandBuffer)
        {
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

        currentResourceTable = nullptr;
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

        if (currentBlitEncoder == nil)
        {
            currentBlitEncoder = [commandBuffer blitCommandEncoder];
            device.SetResourceName(currentBlitEncoder , "Transfer Encoder");
        }

        [currentBlitEncoder optimizeContentsForGPUAccess: metalDestinationImage.GetMetalTexture() slice: layer level: mipLevel];
        [currentBlitEncoder copyFromBuffer: metalSourceBuffer.GetMetalBuffer() sourceOffset: sourceByteOffset sourceBytesPerRow: destinationImage->GetWidth() * ImageFormatToBlockSize(destinationImage->GetFormat()) * destinationImage->GetPixelMemorySize() sourceBytesPerImage: 0 sourceSize: sourceSize toTexture: metalDestinationImage.GetMetalTexture() destinationSlice: layer destinationLevel: mipLevel destinationOrigin: MTLOriginMake(destinationPixelOffset.x, destinationPixelOffset.y, 0)];
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
        [currentBlitEncoder optimizeContentsForGPUAccess: metalImage.GetMetalTexture()];
        [currentBlitEncoder generateMipmapsForTexture: metalImage.GetMetalTexture()];
    }

    void MetalCommandBuffer::BindResourceTable(const std::unique_ptr<ResourceTable> &resourceTable)
    {
        SR_ERROR_IF(resourceTable->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind resource table [{0}], whose graphics API differs from [GraphicsAPI::Metal], within command buffer [{1}]!", resourceTable->GetName(), GetName());
        const MetalResourceTable &metalResourceTable = static_cast<MetalResourceTable&>(*resourceTable);

        currentResourceTable = &metalResourceTable;
    }

    void MetalCommandBuffer::PushConstants(const void* data, const uint16 memoryRange, const uint16 byteOffset)
    {
        SR_ERROR_IF(currentRenderEncoder == nil && currentComputeEncoder == nil, "[Metal]: Cannot push constants if no encoder is active within command buffer [{0}]!", GetName());
        if (currentComputeEncoder != nil)
        {
            [currentComputeEncoder setBytes: reinterpret_cast<const char*>(data) + byteOffset length: memoryRange atIndex: MetalDevice::PUSH_CONSTANT_INDEX];
        }
        else
        {
            [currentRenderEncoder setVertexBytes: reinterpret_cast<const char*>(data) + byteOffset length: memoryRange atIndex: MetalDevice::PUSH_CONSTANT_INDEX];
            if (currentGraphicsPipeline->HasFragmentShader()) [currentRenderEncoder setFragmentBytes: reinterpret_cast<const char*>(data) + byteOffset length: memoryRange atIndex: MetalDevice::PUSH_CONSTANT_INDEX];
        }
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

                        [[subpass.colorAttachments objectAtIndexedSubscript: subpassColorAttachmentCount] setResolveTexture: metalImage.GetMetalTexture()];
                        [[subpass.colorAttachments objectAtIndexedSubscript: subpassColorAttachmentCount] setTexture: metalColorImage.GetMetalTexture()];
                        subpassColorAttachmentCount++;
                    }

                    [[subpass.colorAttachments objectAtIndexedSubscript: subpassColorAttachmentCount] setTexture: metalImage.GetMetalTexture()];
                    [[subpass.colorAttachments objectAtIndexedSubscript: subpassColorAttachmentCount] setClearColor: MTLClearColorMake(attachment.clearColor.r, attachment.clearColor.g, attachment.clearColor.b, attachment.clearColor.a)];
                    subpassColorAttachmentCount++;
                }
            }
        }

        // End any prior transfer operations
        if (currentBlitEncoder != nil)
        {
            [currentBlitEncoder endEncoding];
            #if SR_PLATFORM_macOS
                [currentBlitEncoder release];
            #endif
            currentBlitEncoder = nil;
        }

        BeginNextSubpass(renderPass);
    }

    void MetalCommandBuffer::BeginNextSubpass(const std::unique_ptr<RenderPass> &renderPass)
    {
        SR_ERROR_IF(renderPass->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin next subpass of render pass [{0}], whose graphics API differs from [GraphicsAPI::Metal], from command buffer [{1}]!", renderPass->GetName(), GetName());
        const MetalRenderPass &metalRenderPass = static_cast<MetalRenderPass&>(*renderPass);

        // Begin encoding next subpass
        #if SR_PLATFORM_macOS
            if (currentRenderEncoder != nil) [currentRenderEncoder release];
        #endif
        currentRenderEncoder = [commandBuffer renderCommandEncoderWithDescriptor: metalRenderPass.GetSubpass(currentSubpass)];
        device.SetResourceName(currentComputeEncoder, "Render encoder for render pass [" + std::string(renderPass->GetName()) + "]");

        // Define viewport
        const MTLViewport viewport
        {
            .originX = 0.0f,
            .originY = 0.0f,
            .width = static_cast<float64>(metalRenderPass.GetSubpass(currentSubpass).renderTargetWidth),
            .height = static_cast<float64>(metalRenderPass.GetSubpass(currentSubpass).renderTargetHeight),
            .znear = 0.0f,
            .zfar = 1.0f
        };

        // Set viewport
        [currentRenderEncoder setViewport: viewport];

        // Define scissor
        const MTLScissorRect scissor
        {
            .x = 0,
            .y = 0,
            .width = static_cast<uint64>(metalRenderPass.GetSubpass(currentSubpass).renderTargetWidth),
            .height = static_cast<uint64>(metalRenderPass.GetSubpass(currentSubpass).renderTargetHeight)
        };

        // Set scissor
        [currentRenderEncoder setScissorRect: scissor];

        // Increment current subpass for future usage
        currentSubpass++;

        // Bind bindless argument buffer
        if (currentResourceTable != nullptr)
         {
            [currentRenderEncoder setVertexBuffer: currentResourceTable->GetMetalArgumentBuffer() offset: 0 atIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_INDEX];
            [currentRenderEncoder setFragmentBuffer: currentResourceTable->GetMetalArgumentBuffer() offset: 0 atIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_INDEX];
            for (const auto &[key, resource] : currentResourceTable->GetBoundResources()) [currentRenderEncoder useResource: resource usage: key.GetUsage() stages: MTLRenderStageVertex | MTLRenderStageFragment];
        }
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

        SR_ERROR_IF(byteOffset > vertexBuffer->GetMemorySize(), "[Metal]: Cannot bind vertex buffer [{0}] within command buffer [{1}] using specified offset of [{2}] bytes, which is not within a valid range of the [{3}] bytes the buffer holds!", vertexBuffer->GetName(), GetName(), byteOffset, vertexBuffer->GetMemorySize());
        [currentRenderEncoder setVertexBuffer: metalVertexBuffer.GetMetalBuffer() offset: byteOffset atIndex: MetalDevice::VERTEX_BUFFER_INDEX];
        currentVertexBufferByteOffset = byteOffset;
    }

    void MetalCommandBuffer::BindIndexBuffer(const std::unique_ptr<Buffer> &indexBuffer, const uint64 byteOffset)
    {
        SR_ERROR_IF(indexBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind index buffer [{0}], whose graphics API differs from [GraphicsAPI::Metal], within command buffer [{1}]!", indexBuffer->GetName(), GetName());
        const MetalBuffer &metalIndexBuffer = static_cast<MetalBuffer&>(*indexBuffer);

        SR_ERROR_IF(byteOffset > indexBuffer->GetMemorySize(), "[Metal]: Cannot bind index buffer [{0}] within command buffer [{1}] using specified offset of [{2}] bytes, which is not within a valid range of the [{3}] bytes the buffer holds!", indexBuffer->GetName(), GetName(), byteOffset, indexBuffer->GetMemorySize());
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
        if (newVertexBufferByteOffset > 0) [currentRenderEncoder setVertexBufferOffset: newVertexBufferByteOffset atIndex: MetalDevice::VERTEX_BUFFER_INDEX];
        [currentRenderEncoder drawPrimitives: MTLPrimitiveTypeTriangle vertexStart: 0 vertexCount: vertexCount];
    }

    void MetalCommandBuffer::DrawIndexed(const uint32 indexCount, const uint32 indexOffset, const uint32 vertexOffset)
    {
        SR_ERROR_IF(currentRenderEncoder == nil, "[Metal]: Cannot draw indexed if no render encoder is active within command buffer [{0}]!", GetName());
        const uint64 newVertexBufferByteOffset = currentVertexBufferByteOffset + (static_cast<uint64>(vertexOffset) * currentGraphicsPipeline->GetVertexByteStride());
        if (newVertexBufferByteOffset > 0) [currentRenderEncoder setVertexBufferOffset: newVertexBufferByteOffset atIndex: MetalDevice::VERTEX_BUFFER_INDEX];
        [currentRenderEncoder drawIndexedPrimitives: MTLPrimitiveTypeTriangle indexCount: indexCount indexType: MTLIndexTypeUInt32 indexBuffer: currentIndexBuffer indexBufferOffset: currentIndexBufferByteOffset + indexOffset * sizeof(uint32) instanceCount: 1];
    }

    void MetalCommandBuffer::BeginComputePipeline(const std::unique_ptr<ComputePipeline> &computePipeline)
    {
        SR_ERROR_IF(computePipeline->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin compute graphicsPipeline [{0}], whose graphics API differs from [GraphicsAPI::Metal], from command buffer [{1}]!", computePipeline->GetName(), GetName());
        const MetalComputePipeline &metalComputePipeline = static_cast<MetalComputePipeline&>(*computePipeline);

        // End any prior transfer operations
        if (currentBlitEncoder != nil)
        {
            [currentBlitEncoder endEncoding];
            #if SR_PLATFORM_macOS
                [currentBlitEncoder release];
            #endif
            currentBlitEncoder = nil;
        }

        // Begin encoding compute commands
        currentComputeEncoder = [commandBuffer computeCommandEncoderWithDispatchType: MTLDispatchTypeConcurrent];
        device.SetResourceName(currentComputeEncoder, "Compute encoder for pipeline [" + std::string(computePipeline->GetName()) + "]");

        // Assign provided compute pipeline
        [currentComputeEncoder setComputePipelineState: metalComputePipeline.GetComputePipelineState()];
        currentComputePipeline = &metalComputePipeline;

        // Bind bindless argument buffer
        if (currentResourceTable != nullptr)
        {
            [currentComputeEncoder setBuffer: currentResourceTable->GetMetalArgumentBuffer() offset: 0 atIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_INDEX];
            for (const auto &[key, resource] : currentResourceTable->GetBoundResources()) [currentComputeEncoder useResource: resource usage: key.GetUsage()];
        }
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

    void MetalCommandBuffer::BeginDebugRegion(const std::string_view regionName, const ColorRGBA32 &color)
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif

        NSString* const name = [[NSString alloc] initWithBytes: regionName.data() length: regionName.size() encoding: NSASCIIStringEncoding];
        [commandBuffer pushDebugGroup: name];
        [name release];
    }

    void MetalCommandBuffer::InsertDebugMarker(const std::string_view markerName, const ColorRGBA32 &color)
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif

        NSString* const name = [[NSString alloc] initWithBytes: markerName.data() length: markerName.size() encoding: NSASCIIStringEncoding];
        [currentRenderEncoder insertDebugSignpost: name];
        [name release];
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
