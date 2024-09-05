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

    MetalCommandBuffer::MetalCommandBuffer(const MetalDevice& device, const CommandBufferCreateInfo& createInfo)
        : CommandBuffer(createInfo), device(device), name(createInfo.name)
    {
        // Since in Metal you cannot reuse a command buffer after submitting it, one is created upon every Begin() call
    }

    /* --- POLLING METHODS --- */

    void MetalCommandBuffer::Begin()
    {
        // Free queued resources
        queuedBuffersForDestruction = { };
        queuedImagesForDestruction = { };

        // Set up command buffer descriptor
        MTLCommandBufferDescriptor* const commandBufferDescriptor = [[MTLCommandBufferDescriptor alloc] init];
        #if SR_ENABLE_LOGGING
            [commandBufferDescriptor setErrorOptions: MTLCommandBufferErrorOptionEncoderExecutionStatus];
        #endif

        // Create command buffer
        commandBuffer = [device.GetCommandQueue() commandBufferWithDescriptor: commandBufferDescriptor];
        device.SetResourceName(commandBuffer, name);

        // Create completion synchronization
        completionSignalValue = device.GetNewSignalValue();
        #if SR_ENABLE_LOGGING
            [commandBuffer addCompletedHandler: ^(id<MTLCommandBuffer> executedCommandBuffer)
            {
                NSError* const error = executedCommandBuffer.error;
                SR_ERROR_IF(error != nil, "[Metal]: Submission of command buffer [{0}] failed! Error code: {1}.", name, error.description.UTF8String);
            }];
        #endif

        [commandBufferDescriptor release];
    }

    void MetalCommandBuffer::End()
    {
        if (currentBlitEncoder != nil)
        {
            [currentBlitEncoder endEncoding];
            currentBlitEncoder = nil;
        }

        currentIndexBuffer = nil;
        currentIndexBufferByteOffset = 0;
        currentVertexBufferByteOffset = 0;

        currentResourceTable = nullptr;
    }

    void MetalCommandBuffer::SynchronizeBufferUsage(const Buffer& buffer, const BufferCommandUsage previousUsage, const BufferCommandUsage nextUsage, const size memorySize, const uint64 byteOffset)
    {
        SR_ERROR_IF(buffer.GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not synchronize usage of buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", buffer.GetName(), name);
        const MetalBuffer& metalBuffer = static_cast<const MetalBuffer&>(buffer);

        const id<MTLResource> bufferResource = metalBuffer.GetMetalBuffer();
        [currentRenderEncoder memoryBarrierWithResources: &bufferResource count: 1 afterStages: BufferCommandUsageToRenderStages(previousUsage) beforeStages: BufferCommandUsageToRenderStages(nextUsage)];
    }

    void MetalCommandBuffer::SynchronizeImageUsage(const Image& image, const ImageCommandUsage previousUsage, const ImageCommandUsage nextUsage, const uint32 baseLevel, uint32 levelCount, const uint32 baseLayer, uint32 layerCount)
    {
        SR_ERROR_IF(image.GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not synchronize usage of image [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", image.GetName(), name);
        const MetalImage& metalImage = static_cast<const MetalImage&>(image);

        SR_ERROR_IF(baseLevel >= image.GetLevelCount(), "[Metal]: Cannot synchronize level [{0}] of image [{1}] within command buffer [{2}], as it does not have it!", baseLevel, image.GetName(), name);
        SR_ERROR_IF(baseLayer >= image.GetLayerCount(), "[Metal]: Cannot synchronize layer [{0}] of image [{1}] within command buffer [{2}], as it does not have it!", baseLayer, image.GetName(), name);

        levelCount = levelCount != 0 ? levelCount : image.GetLevelCount() - baseLevel;
        layerCount = layerCount != 0 ? layerCount : image.GetLayerCount() - baseLayer;

        SR_ERROR_IF(baseLevel + levelCount > image.GetLevelCount(), "[Metal]: Cannot synchronize levels [{0}-{1}] of image [{2}] within command buffer [{3}], as they exceed image's level count - [{4}]!", baseLevel, baseLevel + levelCount - 1, image.GetName(), name, image.GetLevelCount());
        SR_ERROR_IF(baseLayer + layerCount > image.GetLayerCount(), "[Metal]: Cannot synchronize layers [{0}-{1}] of image [{2}] within command buffer [{3}], as they exceed image's layer count - [{4}]!", baseLayer, baseLayer + layerCount - 1, image.GetName(), name, image.GetLayerCount());

        const id<MTLResource> textureResource = metalImage.GetMetalTexture();
        [currentRenderEncoder memoryBarrierWithResources: &textureResource count: 1 afterStages: ImageCommandUsageToRenderStages(previousUsage) beforeStages: ImageCommandUsageToRenderStages(nextUsage)];
    }

    void MetalCommandBuffer::CopyBufferToBuffer(const Buffer& sourceBuffer, const Buffer& destinationBuffer, uint64 memoryByteSize, const uint64 sourceByteOffset, const uint64 destinationByteOffset)
    {
        SR_ERROR_IF(sourceBuffer.GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not copy from buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", sourceBuffer.GetName(), name);
        const MetalBuffer& metalSourceBuffer = static_cast<const MetalBuffer&>(sourceBuffer);

        SR_ERROR_IF(destinationBuffer.GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not copy to buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", destinationBuffer.GetName(), name);
        const MetalBuffer& metalDestinationBuffer = static_cast<const MetalBuffer&>(destinationBuffer);

        memoryByteSize = memoryByteSize != 0 ? memoryByteSize : sourceBuffer.GetMemorySize();
        SR_ERROR_IF(sourceByteOffset + memoryByteSize > sourceBuffer.GetMemorySize(), "[Metal]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, from buffer [{2}] within command buffer [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryByteSize, sourceByteOffset, sourceBuffer.GetName(), name, sourceByteOffset + memoryByteSize, sourceBuffer.GetMemorySize());
        SR_ERROR_IF(destinationByteOffset + memoryByteSize > destinationBuffer.GetMemorySize(), "[Metal]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, to buffer [{2}] within command buffer [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryByteSize, destinationByteOffset, destinationBuffer.GetName(), name, destinationByteOffset + memoryByteSize, destinationBuffer.GetMemorySize());

        if (currentBlitEncoder == nil)
        {
            currentBlitEncoder = [commandBuffer blitCommandEncoder];
            device.SetResourceName(currentBlitEncoder, "Transfer Encoder");
        }

        [currentBlitEncoder copyFromBuffer: metalSourceBuffer.GetMetalBuffer() sourceOffset: sourceByteOffset toBuffer: metalDestinationBuffer.GetMetalBuffer() destinationOffset: destinationByteOffset size: memoryByteSize];
    }

    void MetalCommandBuffer::CopyBufferToImage(const Buffer& sourceBuffer, const Image& destinationImage, const uint32 level, const uint32 layer, const Vector3UInt& pixelRange, const uint64 sourceByteOffset, const Vector3UInt& destinationPixelOffset)
    {
        SR_ERROR_IF(sourceBuffer.GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not copy from buffer [{0}], whose graphics API differs from [GraphicsAPI::Metal], to image [{1}] within command buffer [{2}]!", sourceBuffer.GetName(), destinationImage.GetName(), name);
        const MetalBuffer& metalSourceBuffer = static_cast<const MetalBuffer&>(sourceBuffer);

        SR_ERROR_IF(destinationImage.GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not from buffer [{0}] to image [{1}], graphics API differs from [GraphicsAPI::Metal], within command buffer [{2}]!", sourceBuffer.GetName(), destinationImage.GetName(), name);
        const MetalImage& metalDestinationImage = static_cast<const MetalImage&>(destinationImage);

        SR_ERROR_IF(level >= destinationImage.GetLevelCount(), "[Metal]: Cannot copy from buffer [{0}] to level [{1}] of image [{2}] within command buffer [{3}], as image does not contain it!", sourceBuffer.GetName(), level, destinationImage.GetName(), name);
        SR_ERROR_IF(layer >= destinationImage.GetLayerCount(), "[Metal]: Cannot copy from buffer [{0}] to layer [{1}] of image [{2}] within command buffer [{3}], as image does not contain it!", sourceBuffer.GetName(), layer, destinationImage.GetName(), name);

        const MTLSize sourceSize = MTLSizeMake(pixelRange.x != 0 ? pixelRange.x : destinationImage.GetWidth() >> level, pixelRange.y != 0 ? pixelRange.y : destinationImage.GetHeight() >> level, pixelRange.z != 0 ? pixelRange.z : destinationImage.GetDepth() >> level);
        SR_ERROR_IF(destinationPixelOffset.x + sourceSize.width > destinationImage.GetWidth() || destinationPixelOffset.y + sourceSize.height > destinationImage.GetHeight() || destinationPixelOffset.z + sourceSize.depth > destinationImage.GetDepth(), "[Metal]: Cannot copy from buffer [{0}] pixel range [{1}x{2}x{3}], which is offset by another [{4}x{5}x{6}] pixels to image [{7}] within command buffer [{8}], as resulting pixel range of a total of [{9}x{10}x{11}] pixels exceeds the image's dimensions - [{12}x{13}x{14}]!", sourceBuffer.GetName(), sourceSize.width, sourceSize.height, sourceSize.depth, destinationPixelOffset.x, destinationPixelOffset.y, destinationPixelOffset.z, destinationImage.GetName(), name, sourceSize.width + destinationPixelOffset.x, sourceSize.height + destinationPixelOffset.y, sourceSize.depth + destinationPixelOffset.z, destinationImage.GetWidth(), destinationImage.GetHeight(), destinationImage.GetDepth());

        if (currentBlitEncoder == nil)
        {
            currentBlitEncoder = [commandBuffer blitCommandEncoder];
            device.SetResourceName(currentBlitEncoder, "Transfer Encoder");
        }

        [currentBlitEncoder optimizeContentsForGPUAccess: metalDestinationImage.GetMetalTexture() slice: layer level: level];
        [currentBlitEncoder copyFromBuffer: metalSourceBuffer.GetMetalBuffer() sourceOffset: sourceByteOffset sourceBytesPerRow: static_cast<uint64>(static_cast<float32>(destinationImage.GetWidth() * ImageFormatToBlockSize(destinationImage.GetFormat())) * Sierra::ImageFormatToPixelMemorySize(destinationImage.GetFormat())) sourceBytesPerImage: 0 sourceSize: sourceSize toTexture: metalDestinationImage.GetMetalTexture() destinationSlice: layer destinationLevel: level destinationOrigin: MTLOriginMake(destinationPixelOffset.x, destinationPixelOffset.y, destinationPixelOffset.z)];
    }

    void MetalCommandBuffer::GenerateMipMapsForImage(const Image& image)
    {
        SR_ERROR_IF(image.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot generate mip maps for image [{0}], whose graphics API differs from [GraphicsAPI::Metal], within command buffer [{1}]!", image.GetName(), name);
        SR_ERROR_IF(image.GetLevelCount() <= 1, "[Metal]: Cannot generate mip maps for image [{0}], as it has a single level only!", image.GetName());

        if (currentBlitEncoder == nil)
        {
            const id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];
            device.SetResourceName(blitEncoder, "Transfer Encoder");
        }

        const MetalImage& metalImage = static_cast<const MetalImage&>(image);
        [currentBlitEncoder optimizeContentsForGPUAccess: metalImage.GetMetalTexture()];
        [currentBlitEncoder generateMipmapsForTexture: metalImage.GetMetalTexture()];
    }

    void MetalCommandBuffer::BindResourceTable(const ResourceTable& resourceTable)
    {
        SR_ERROR_IF(resourceTable.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind resource table [{0}], whose graphics API differs from [GraphicsAPI::Metal], within command buffer [{1}]!", resourceTable.GetName(), name);
        const MetalResourceTable& metalResourceTable = static_cast<const MetalResourceTable&>(resourceTable);

        currentResourceTable = &metalResourceTable;
    }

    void MetalCommandBuffer::PushConstants(const void* memory, const uint16 memoryByteSize, const uint16 sourceByteOffset)
    {
        SR_ERROR_IF(currentRenderEncoder == nil && currentComputeEncoder == nil, "[Metal]: Cannot push constants if no encoder is active within command buffer [{0}]!", name);
        if (currentComputeEncoder != nil)
        {
            [currentComputeEncoder setBytes:reinterpret_cast<const uint8*>(memory) + sourceByteOffset length: memoryByteSize atIndex: MetalDevice::PUSH_CONSTANT_INDEX];
        }
        else
        {
            [currentRenderEncoder setVertexBytes:reinterpret_cast<const uint8*>(memory) + sourceByteOffset length: memoryByteSize atIndex: MetalDevice::PUSH_CONSTANT_INDEX];
            if (currentGraphicsPipeline->HasFragmentShader()) [currentRenderEncoder setFragmentBytes:reinterpret_cast<const uint8*>(memory) + sourceByteOffset length: memoryByteSize atIndex: MetalDevice::PUSH_CONSTANT_INDEX];
        }
    }

    void MetalCommandBuffer::BeginRenderPass(const RenderPass& renderPass, const std::span<const RenderPassBeginAttachment> attachments)
    {
        SR_ERROR_IF(renderPass.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin render pass [{0}], whose graphics API differs from [GraphicsAPI::Metal], from command buffer [{1}]!", renderPass.GetName(), name);
        const MetalRenderPass& metalRenderPass = static_cast<const MetalRenderPass&>(renderPass);

        SR_ERROR_IF(attachments.size() != metalRenderPass.GetAttachmentCount(), "[Metal]: Cannot begin render pass [{0}] within command buffer [{1}] with [{2}] attachments, as it was created to hold [{3}]!", renderPass.GetName(), name, attachments.size(), metalRenderPass.GetAttachmentCount());
        for (size i = 0; i < attachments.size(); i++)
        {
            const RenderPassBeginAttachment& attachment = attachments[i];
            for (MTLRenderPassAttachmentDescriptor* const renderPassAttachmentDescriptor : metalRenderPass.GetAttachment(i))
            {
                SR_ERROR_IF(attachment.outputImage.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin render pass [{0}] using image [{1}] as attachment [{2}]'s output image, because its graphics API differs from [GraphicsAPI::Metal]!", name, attachment.outputImage.GetName(), i);
                const MetalImage& metalOutputImage = static_cast<const MetalImage&>(attachment.outputImage);

                [renderPassAttachmentDescriptor setTexture: metalOutputImage.GetMetalTexture()];
                switch (metalOutputImage.GetFormat())
                {
                    case ImageFormat::D16_UNorm:
                    case ImageFormat::D32_Float:
                    {
                        [reinterpret_cast<MTLRenderPassDepthAttachmentDescriptor*>(renderPassAttachmentDescriptor) setClearDepth: attachment.clearValue.r];
                        break;
                    }
                    default:
                    {
                        [reinterpret_cast<MTLRenderPassColorAttachmentDescriptor*>(renderPassAttachmentDescriptor) setClearColor: MTLClearColorMake(attachment.clearValue.r, attachment.clearValue.g, attachment.clearValue.b, attachment.clearValue.a)];
                        break;
                    }
                }

                if (attachment.resolverImage != nullptr)
                {
                    SR_ERROR_IF(attachment.resolverImage->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin render pass [{0}] using image [{1}] as attachment [{2}]'s resolver image, as its graphics API differs from [GraphicsAPI::Metal]!", name, attachment.resolverImage->GetName(), i);
                    const MetalImage& metalResolverImage = static_cast<const MetalImage&>(*attachment.resolverImage);

                    [renderPassAttachmentDescriptor setTexture: metalResolverImage.GetMetalTexture()];
                    [renderPassAttachmentDescriptor setResolveTexture: metalOutputImage.GetMetalTexture()];
                }
            }
        }

        // End any prior transfer operations
        if (currentBlitEncoder != nil)
        {
            [currentBlitEncoder endEncoding];
            currentBlitEncoder = nil;
        }

        BeginNextSubpass(renderPass);
    }

    void MetalCommandBuffer::BeginNextSubpass(const RenderPass& renderPass)
    {
        SR_ERROR_IF(renderPass.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin next subpass of render pass [{0}], whose graphics API differs from [GraphicsAPI::Metal], from command buffer [{1}]!", renderPass.GetName(), name);
        const MetalRenderPass& metalRenderPass = static_cast<const MetalRenderPass&>(renderPass);

        // Begin encoding next subpass
        currentRenderEncoder = [commandBuffer renderCommandEncoderWithDescriptor: metalRenderPass.GetSubpass(currentSubpass)];
        device.SetResourceName(currentComputeEncoder, fmt::format("Render encoder for render pass [{0}]", name));

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
            for (const auto& [key, resource] : currentResourceTable->GetBoundResources()) [currentRenderEncoder useResource: resource usage: key.GetUsage() stages: MTLRenderStageVertex | MTLRenderStageFragment];
        }
    }

    void MetalCommandBuffer::EndRenderPass(const RenderPass& renderPass)
    {
        SR_ERROR_IF(renderPass.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot end render pass [{0}], whose graphics API differs from [GraphicsAPI::Metal], from command buffer [{1}]!", renderPass.GetName(), name);

        SR_ERROR_IF(currentRenderEncoder == nil, "[Metal]: Cannot end render pass [{0}], as it has not been began within command buffer [{1}]!", renderPass.GetName(), name);
        [currentRenderEncoder endEncoding];

        currentRenderEncoder = nil;
        currentSubpass = 0;
    }

    void MetalCommandBuffer::BeginGraphicsPipeline(const GraphicsPipeline& graphicsPipeline)
    {
        SR_ERROR_IF(graphicsPipeline.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin graphics pipeline [{0}], whose graphics API differs from [GraphicsAPI::Metal], from command buffer [{1}]!", graphicsPipeline.GetName(), name);
        const MetalGraphicsPipeline& metalGraphicsPipeline = static_cast<const MetalGraphicsPipeline&>(graphicsPipeline);

        SR_ERROR_IF(currentRenderEncoder == nil, "[Metal]: Cannot begin graphics pipeline [{0}] if no render encoder is active within command buffer [{1}]!", graphicsPipeline.GetName(), name);

        // Bind pipeline and set appropriate settings
        [currentRenderEncoder setCullMode: metalGraphicsPipeline.GetCullMode()];
        [currentRenderEncoder setTriangleFillMode: metalGraphicsPipeline.GetTriangleFillMode()];
        [currentRenderEncoder setFrontFacingWinding: metalGraphicsPipeline.GetWinding()];
        [currentRenderEncoder setRenderPipelineState: metalGraphicsPipeline.GetRenderPipelineState()];
        if (metalGraphicsPipeline.GetDepthStencilState() != nil) [currentRenderEncoder setDepthStencilState: metalGraphicsPipeline.GetDepthStencilState()];

        // Save pipeline
        currentGraphicsPipeline = &metalGraphicsPipeline;
    }

    void MetalCommandBuffer::EndGraphicsPipeline(const GraphicsPipeline& graphicsPipeline)
    {
        SR_ERROR_IF(graphicsPipeline.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot end graphics pipeline [{0}], from command buffer [{1}]!", graphicsPipeline.GetName(), name);

        SR_ERROR_IF(currentRenderEncoder == nil, "[Metal]: Cannot end graphics pipeline [{0}] if no render encoder is active within command buffer [{1}]!", graphicsPipeline.GetName(), name);
        currentGraphicsPipeline = nil;
    }

    void MetalCommandBuffer::BindVertexBuffer(const Buffer& vertexBuffer, const uint64 byteOffset)
    {
        SR_ERROR_IF(vertexBuffer.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind vertex buffer [{0}], whose graphics API differs from [GraphicsAPI::Metal], within command buffer [{1}]!", vertexBuffer.GetName(), name);
        const MetalBuffer& metalVertexBuffer = static_cast<const MetalBuffer&>(vertexBuffer);

        SR_ERROR_IF(byteOffset > vertexBuffer.GetMemorySize(), "[Metal]: Cannot bind vertex buffer [{0}] within command buffer [{1}] using specified offset of [{2}] bytes, which is not within a valid range of the [{3}] bytes the buffer holds!", vertexBuffer.GetName(), name, byteOffset, vertexBuffer.GetMemorySize());
        [currentRenderEncoder setVertexBuffer: metalVertexBuffer.GetMetalBuffer() offset: byteOffset atIndex: MetalDevice::VERTEX_BUFFER_INDEX];
        currentVertexBufferByteOffset = byteOffset;
    }

    void MetalCommandBuffer::BindIndexBuffer(const Buffer& indexBuffer, const uint64 byteOffset)
    {
        SR_ERROR_IF(indexBuffer.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind index buffer [{0}], whose graphics API differs from [GraphicsAPI::Metal], within command buffer [{1}]!", indexBuffer.GetName(), name);
        const MetalBuffer& metalIndexBuffer = static_cast<const MetalBuffer&>(indexBuffer);

        SR_ERROR_IF(byteOffset > indexBuffer.GetMemorySize(), "[Metal]: Cannot bind index buffer [{0}] within command buffer [{1}] using specified offset of [{2}] bytes, which is not within a valid range of the [{3}] bytes the buffer holds!", indexBuffer.GetName(), name, byteOffset, indexBuffer.GetMemorySize());
        currentIndexBuffer = metalIndexBuffer.GetMetalBuffer();
        currentIndexBufferByteOffset = byteOffset;
    }

    void MetalCommandBuffer::SetScissor(const Vector4UInt& scissor)
    {
        SR_ERROR_IF(currentRenderEncoder == nil, "[Metal]: Cannot set scissor if no render encoder is active within command buffer [{0}]!", name);
        [currentRenderEncoder setScissorRect: { scissor.x, scissor.y, scissor.z, scissor.w }];
    }

    void MetalCommandBuffer::Draw(const uint32 vertexCount, const uint32 vertexOffset)
    {
        SR_ERROR_IF(currentRenderEncoder == nil, "[Metal]: Cannot draw if no render encoder is active within command buffer [{0}]!", name);
        const uint64 newVertexBufferByteOffset = currentVertexBufferByteOffset + (static_cast<uint64>(vertexOffset) * currentGraphicsPipeline->GetVertexByteStride());
        if (newVertexBufferByteOffset > 0) [currentRenderEncoder setVertexBufferOffset: newVertexBufferByteOffset atIndex: MetalDevice::VERTEX_BUFFER_INDEX];
        [currentRenderEncoder drawPrimitives: MTLPrimitiveTypeTriangle vertexStart: 0 vertexCount: vertexCount];
    }

    void MetalCommandBuffer::DrawIndexed(const uint32 indexCount, const uint32 indexOffset, const uint32 vertexOffset)
    {
        SR_ERROR_IF(currentRenderEncoder == nil, "[Metal]: Cannot draw indexed if no render encoder is active within command buffer [{0}]!", name);
        const uint64 newVertexBufferByteOffset = currentVertexBufferByteOffset + (static_cast<uint64>(vertexOffset) * currentGraphicsPipeline->GetVertexByteStride());
        if (newVertexBufferByteOffset > 0) [currentRenderEncoder setVertexBufferOffset: newVertexBufferByteOffset atIndex: MetalDevice::VERTEX_BUFFER_INDEX];
        [currentRenderEncoder drawIndexedPrimitives: MTLPrimitiveTypeTriangle indexCount: indexCount indexType: MTLIndexTypeUInt32 indexBuffer: currentIndexBuffer indexBufferOffset: currentIndexBufferByteOffset + indexOffset * sizeof(uint32) instanceCount: 1];
    }

    void MetalCommandBuffer::BeginComputePipeline(const ComputePipeline& computePipeline)
    {
        SR_ERROR_IF(computePipeline.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin compute graphicsPipeline [{0}], whose graphics API differs from [GraphicsAPI::Metal], from command buffer [{1}]!", computePipeline.GetName(), name);
        const MetalComputePipeline& metalComputePipeline = static_cast<const MetalComputePipeline&>(computePipeline);

        // End any prior transfer operations
        if (currentBlitEncoder != nil)
        {
            [currentBlitEncoder endEncoding];
            currentBlitEncoder = nil;
        }

        // Begin encoding compute commands
        currentComputeEncoder = [commandBuffer computeCommandEncoderWithDispatchType: MTLDispatchTypeConcurrent];
        device.SetResourceName(currentComputeEncoder, fmt::format("Compute encoder for pipeline [{0}]", name));

        // Assign provided compute pipeline
        [currentComputeEncoder setComputePipelineState: metalComputePipeline.GetComputePipelineState()];
        currentComputePipeline = &metalComputePipeline;

        // Bind bindless argument buffer
        if (currentResourceTable != nullptr)
        {
            [currentComputeEncoder setBuffer: currentResourceTable->GetMetalArgumentBuffer() offset: 0 atIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_INDEX];
            for (const auto& [key, resource] : currentResourceTable->GetBoundResources()) [currentComputeEncoder useResource: resource usage: key.GetUsage()];
        }
    }

    void MetalCommandBuffer::EndComputePipeline(const ComputePipeline& computePipeline)
    {
        SR_ERROR_IF(computePipeline.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot end compute pipeline [{0}], from command buffer [{1}]!", computePipeline.GetName(), name);
        [currentComputeEncoder endEncoding];
        currentComputeEncoder = nil;
    }

    void MetalCommandBuffer::Dispatch(const uint32 xWorkGroupCount, const uint32 yWorkGroupCount, const uint32 zWorkGroupCount)
    {
        SR_ERROR_IF(currentComputeEncoder == nil, "[Metal]: Cannot dispatch if no compute pipeline is active within command buffer [{0}]!", name);

        // Dispatch work groups
        [currentComputeEncoder dispatchThreadgroups: MTLSizeMake(xWorkGroupCount, yWorkGroupCount, zWorkGroupCount) threadsPerThreadgroup: MTLSizeMake(1, 1, 1)];
    }

    void MetalCommandBuffer::BeginDebugRegion(const std::string_view regionName, const Color color)
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif

        NSString* const group = [[NSString alloc] initWithBytes: regionName.data() length: regionName.size() encoding: NSASCIIStringEncoding];
        [commandBuffer pushDebugGroup: group];
        [group release];
    }

    void MetalCommandBuffer::InsertDebugMarker(const std::string_view markerName, const Color color)
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif

        NSString* const signpost = [[NSString alloc] initWithBytes: markerName.data() length: markerName.size() encoding: NSASCIIStringEncoding];
        [currentRenderEncoder insertDebugSignpost: signpost];
        [signpost release];
    }

    void MetalCommandBuffer::EndDebugRegion()
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif
        [commandBuffer popDebugGroup];
    }

    std::unique_ptr<Buffer>& MetalCommandBuffer::QueueBufferForDestruction(std::unique_ptr<Buffer> &&buffer)
    {
        return queuedBuffersForDestruction.emplace(std::move(buffer));
    }

    std::unique_ptr<Image>& MetalCommandBuffer::QueueImageForDestruction(std::unique_ptr<Image> &&image)
    {
        return queuedImagesForDestruction.emplace(std::move(image));
    }

    /* --- GETTER METHODS --- */

    std::string_view MetalCommandBuffer::GetName() const
    {
        return { [commandBuffer.label UTF8String], [commandBuffer.label length] };
    }

    /* --- DESTRUCTOR --- */

    MetalCommandBuffer::~MetalCommandBuffer()
    {
        queuedBuffersForDestruction = { };
        queuedImagesForDestruction = { };
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
