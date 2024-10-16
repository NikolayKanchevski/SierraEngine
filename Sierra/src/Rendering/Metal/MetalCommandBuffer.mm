//
// Created by Nikolay Kanchevski on 12.12.23.
//

#include "MetalCommandBuffer.h"

#include "MetalBuffer.h"
#include "MetalImage.h"
#include "MetalRenderPass.h"
#include "../DeviceErrors.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */

    MTLRenderStages BufferCommandUsageToRenderStages(const BufferCommandUsage bufferCommandUsage) noexcept
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
            default:                                     break;
        }

        return 0;
    }

    MTLRenderStages ImageCommandUsageToRenderStages(const ImageCommandUsage imageCommandUsage) noexcept
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
            default:                                    break;
        }

        return 0;
    }

    /* --- CONSTRUCTORS --- */

    MetalCommandBuffer::MetalCommandBuffer(const MetalQueue& queue, const CommandBufferCreateInfo& createInfo)
        : CommandBuffer(createInfo), queue(queue), name(createInfo.name)
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
        commandBuffer = [queue.GetMetalCommandQueue() commandBufferWithDescriptor: commandBufferDescriptor];
        queue.GetDevice().SetResourceName(commandBuffer, name);

        // Create completion synchronization
        completionSemaphoreSignalValue = queue.GetDevice().GetNewSemaphoreSignalValue();
        #if SR_ENABLE_LOGGING
            [commandBuffer addCompletedHandler: ^(id<MTLCommandBuffer> executedCommandBuffer)
            {
                const NSError* const error = executedCommandBuffer.error;
//                SR_THROW_IF(error != nil, UnknownDeviceError(SR_FORMAT("Could not execute command buffer [{0}]", std::string_view(executedCommandBuffer.label.UTF8String, executedCommandBuffer.label.length))));

                // NOTE: This has been left like this in hopes of finding an annoying page fault bug on Apple platforms
                if (error != nil) SR_ERROR("CB {0} err {1}", std::string_view(executedCommandBuffer.label.UTF8String, executedCommandBuffer.label.length), std::string_view(error.localizedDescription.UTF8String, error.localizedDescription.length));
            }];
        #endif

        [commandBufferDescriptor release];
    }

    void MetalCommandBuffer::End()
    {
        SR_THROW_IF(currentRenderPass != nullptr, InvalidOperationError(SR_FORMAT("Cannot end command buffer [{0}], as current render pass [{1}] has not been ended", name, currentRenderPass->GetName())));

        if (currentBlitEncoder != nil)
        {
            [currentBlitEncoder endEncoding];
            currentBlitEncoder = nil;
        }

        currentResourceTable = nullptr;

        initialVertexBufferOffset = 0;
        currentVertexBuffer = nullptr;

        initialIndexBufferOffset = 0;
        currentIndexBuffer = nullptr;
    }

    void MetalCommandBuffer::SynchronizeBufferUsage(const Buffer& buffer, const BufferCommandUsage previousUsage, const BufferCommandUsage nextUsage, const size offset, const size memorySize)
    {
        SR_THROW_IF(buffer.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot synchronize usage of buffer [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", buffer.GetName(), name)));
        const MetalBuffer& metalBuffer = static_cast<const MetalBuffer&>(buffer);

        CommandBuffer::SynchronizeBufferUsage(metalBuffer, previousUsage, nextUsage, offset, memorySize);

        const id<MTLResource> bufferResource = metalBuffer.GetMetalBuffer();
        [currentRenderEncoder memoryBarrierWithResources: &bufferResource count: 1 afterStages: BufferCommandUsageToRenderStages(previousUsage) beforeStages: BufferCommandUsageToRenderStages(nextUsage)];
    }

    void MetalCommandBuffer::SynchronizeImageUsage(const Image& image, const ImageCommandUsage previousUsage, const ImageCommandUsage nextUsage, const uint32 baseLevel, const uint32 levelCount, const uint32 baseLayer, const uint32 layerCount)
    {
        SR_THROW_IF(image.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot synchronize usage of image [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", image.GetName(), name)));
        const MetalImage& metalImage = static_cast<const MetalImage&>(image);

        CommandBuffer::SynchronizeImageUsage(metalImage, previousUsage, nextUsage, baseLevel, levelCount, baseLayer, layerCount);

        const id<MTLResource> textureResource = metalImage.GetMetalTexture();
        [currentRenderEncoder memoryBarrierWithResources: &textureResource count: 1 afterStages: ImageCommandUsageToRenderStages(previousUsage) beforeStages: ImageCommandUsageToRenderStages(nextUsage)];
    }

    void MetalCommandBuffer::CopyBufferToBuffer(const Buffer& sourceBuffer, const Buffer& destinationBuffer, const size sourceOffset, const size destinationOffset, const size memorySize)
    {
        SR_THROW_IF(sourceBuffer.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot copy from buffer [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", sourceBuffer.GetName(), name)));
        const MetalBuffer& metalSourceBuffer = static_cast<const MetalBuffer&>(sourceBuffer);

        SR_THROW_IF(destinationBuffer.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot copy to buffer [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", destinationBuffer.GetName(), name)));
        const MetalBuffer& metalDestinationBuffer = static_cast<const MetalBuffer&>(destinationBuffer);

        CommandBuffer::CopyBufferToBuffer(metalSourceBuffer, metalDestinationBuffer, sourceOffset, destinationOffset, memorySize);

        UpdateBlitEncoder();
        [currentBlitEncoder copyFromBuffer: metalSourceBuffer.GetMetalBuffer() sourceOffset: sourceOffset toBuffer: metalDestinationBuffer.GetMetalBuffer() destinationOffset: destinationOffset size: memorySize];
    }

    void MetalCommandBuffer::CopyBufferToImage(const Buffer& sourceBuffer, const Image& destinationImage, const uint32 level, const uint32 layer, const size sourceOffset, const Vector3UInt destinationPixelOffset, const Vector3UInt pixelRange)
    {
        SR_THROW_IF(sourceBuffer.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot copy from buffer [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", sourceBuffer.GetName(), name)));
        const MetalBuffer& metalSourceBuffer = static_cast<const MetalBuffer&>(sourceBuffer);

        SR_THROW_IF(destinationImage.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot copy to image [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", destinationImage.GetName(), name)));
        const MetalImage& metalDestinationImage = static_cast<const MetalImage&>(destinationImage);

        CommandBuffer::CopyBufferToImage(metalSourceBuffer, metalDestinationImage, level, layer, sourceOffset, destinationPixelOffset, pixelRange);

        UpdateBlitEncoder();
        [currentBlitEncoder optimizeContentsForGPUAccess: metalDestinationImage.GetMetalTexture() slice: layer level: level];
        [currentBlitEncoder copyFromBuffer: metalSourceBuffer.GetMetalBuffer() sourceOffset: sourceOffset sourceBytesPerRow: static_cast<uint64>(static_cast<float32>(destinationImage.GetWidth() * ImageFormatToBlockSize(destinationImage.GetFormat())) * Sierra::ImageFormatToPixelMemorySize(destinationImage.GetFormat())) sourceBytesPerImage: 0 sourceSize: MTLSizeMake(pixelRange.x, pixelRange.y, pixelRange.z) toTexture: metalDestinationImage.GetMetalTexture() destinationSlice: layer destinationLevel: level destinationOrigin: MTLOriginMake(destinationPixelOffset.x, destinationPixelOffset.y, destinationPixelOffset.z)];
    }

    void MetalCommandBuffer::GenerateMipMapsForImage(const Image& image)
    {
        SR_THROW_IF(image.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot generate mip maps for image [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", image.GetName(), name)));
        const MetalImage& metalImage = static_cast<const MetalImage&>(image);

        CommandBuffer::GenerateMipMapsForImage(metalImage);

        if (metalImage.GetLevelCount() == 1)
        {
            SR_WARNING("Did not generate any mip maps for image [{0}], as it only contains a single level.", metalImage.GetName());
            return;
        }

        UpdateBlitEncoder();
        [currentBlitEncoder optimizeContentsForGPUAccess: metalImage.GetMetalTexture()];
        [currentBlitEncoder generateMipmapsForTexture: metalImage.GetMetalTexture()];
    }

    void MetalCommandBuffer::BindResourceTable(const ResourceTable& resourceTable)
    {
        SR_THROW_IF(resourceTable.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot bind resource table [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", resourceTable.GetName(), name)));
        const MetalResourceTable& metalResourceTable = static_cast<const MetalResourceTable&>(resourceTable);

        CommandBuffer::BindResourceTable(metalResourceTable);
        SR_THROW_IF(currentRenderPass != nullptr, InvalidOperationError(SR_FORMAT("Cannot bind resource table [{0}], as current render pass [{1}] has not been ended, and resource table binding must be done prior to beginning a render pass", metalResourceTable.GetName(), currentRenderPass->GetName())));
        SR_THROW_IF(currentComputePipeline != nullptr, InvalidOperationError(SR_FORMAT("Cannot bind resource table [{0}], as current compute pipeline [{1}] has not been ended, and resource table binding must be done prior to beginning a compute pipeline", metalResourceTable.GetName(), currentComputePipeline->GetName())));

        currentResourceTable = &metalResourceTable;
    }

    void MetalCommandBuffer::PushConstants(const void* memory, const size sourceOffset, const size memorySize)
    {
        CommandBuffer::PushConstants(memory, sourceOffset, memorySize);
        SR_THROW_IF(currentGraphicsPipeline == nullptr && currentComputePipeline == nullptr, InvalidOperationError(SR_FORMAT("Cannot push constants within command buffer [{0}] if no pipeline has been begun", name)));

        if (currentGraphicsPipeline != nullptr)
        {
            [currentRenderEncoder setVertexBytes: reinterpret_cast<const uint8*>(memory) + sourceOffset length: memorySize atIndex: MetalDevice::PUSH_CONSTANT_INDEX];
            if (currentGraphicsPipeline->HasFragmentShader()) [currentRenderEncoder setFragmentBytes: reinterpret_cast<const uint8*>(memory) + sourceOffset length: memorySize atIndex: MetalDevice::PUSH_CONSTANT_INDEX];
        }

        if (currentComputePipeline != nullptr)
        {
            [currentComputeEncoder setBytes: reinterpret_cast<const uint8*>(memory) + sourceOffset length: memorySize atIndex: MetalDevice::PUSH_CONSTANT_INDEX];
        }
    }

    void MetalCommandBuffer::BeginRenderPass(const RenderPass& renderPass, const std::span<const RenderPassBeginAttachment> attachments)
    {
        SR_THROW_IF(renderPass.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot begin render pass [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", renderPass.GetName(), name)));
        const MetalRenderPass& metalRenderPass = static_cast<const MetalRenderPass&>(renderPass);

        CommandBuffer::BeginRenderPass(metalRenderPass, attachments);
        SR_THROW_IF(currentRenderPass != nullptr, InvalidOperationError(SR_FORMAT("Cannot begin render pass [{0}] within command buffer [{1}], as current render pass [{2}] has not been ended", metalRenderPass.GetName(), name, currentRenderPass->GetName())));

        for (size i = 0; i < attachments.size(); i++)
        {
            const RenderPassBeginAttachment& attachment = attachments[i];
            for (MTLRenderPassAttachmentDescriptor* const renderPassAttachmentDescriptor : metalRenderPass.GetAttachment(i))
            {
                SR_THROW_IF(attachment.outputImage.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot begin render pass [{0}] within command buffer [{1}] using image [{2}] as template output of attachment [{3}], as its backend type differs from [RenderingBackendType::Metal]", renderPass.GetName(), name, attachment.outputImage.GetName(), i)));
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
                    SR_THROW_IF(attachment.resolverImage->GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot begin render pass [{0}] within command buffer [{1}] using image [{2}] as template resolver of attachment [{3}], as its backend type differs from [RenderingBackendType::Metal]", renderPass.GetName(), name, attachment.resolverImage->GetName(), i)));
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

        currentSubpass = 0;
        currentRenderPass = &metalRenderPass;

        BeginNextSubpass(renderPass);

        // Bind bindless argument buffer
        if (currentResourceTable != nullptr)
        {
            [currentRenderEncoder setVertexBuffer: currentResourceTable->GetMetalArgumentBuffer() offset: 0 atIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_INDEX];
            [currentRenderEncoder setFragmentBuffer: currentResourceTable->GetMetalArgumentBuffer() offset: 0 atIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_INDEX];

            for (const auto &[key, resource] : currentResourceTable->GetBoundUniformBuffers()) [currentRenderEncoder useResource: resource usage: MTLResourceUsageRead stages: MTLRenderStageVertex | MTLRenderStageFragment];
            for (const auto &[key, resource] : currentResourceTable->GetBoundStorageBuffers()) [currentRenderEncoder useResource: resource usage: MTLResourceUsageRead | MTLResourceUsageWrite stages: MTLRenderStageVertex | MTLRenderStageFragment];
            for (const auto &[key, resource] : currentResourceTable->GetBoundSampledImages()) [currentRenderEncoder useResource: resource usage: MTLResourceUsageRead stages: MTLRenderStageVertex | MTLRenderStageFragment];
            for (const auto &[key, resource] : currentResourceTable->GetBoundStorageImages()) [currentRenderEncoder useResource: resource usage: MTLResourceUsageRead | MTLResourceUsageWrite stages: MTLRenderStageVertex | MTLRenderStageFragment];
        }
    }

    void MetalCommandBuffer::BeginNextSubpass(const RenderPass& renderPass)
    {
        SR_THROW_IF(renderPass.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot begin next subpass of render pass [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", renderPass.GetName(), name)));
        const MetalRenderPass& metalRenderPass = static_cast<const MetalRenderPass&>(renderPass);

        CommandBuffer::BeginNextSubpass(metalRenderPass);
        SR_THROW_IF(currentRenderPass == nullptr, InvalidOperationError(SR_FORMAT("Cannot begin next subpass [{0}] of render pass [{1}] within command buffer [{2}], as it must have been begun first", currentSubpass + 1, metalRenderPass.GetName(), name)));
        SR_THROW_IF(currentRenderPass != &metalRenderPass, InvalidOperationError(SR_FORMAT("Cannot begin next subpass [{0}] of render pass [{1}] within command buffer [{2}], as current render pass [{2}] has not been ended", currentSubpass + 1, metalRenderPass.GetName(), name, currentRenderPass->GetName())));
        SR_THROW_IF(currentSubpass != 0 && currentSubpass + 1 >= metalRenderPass.GetSubpassCount(), ValueOutOfRangeError(SR_FORMAT("Cannot begin next subpass [{0}] of render pass [{1}] within command buffer [{2}]", currentSubpass + 1, metalRenderPass.GetName(), name), currentSubpass, 0U, metalRenderPass.GetSubpassCount() - 1));

        // Begin encoding next subpass
        currentRenderEncoder = [commandBuffer renderCommandEncoderWithDescriptor: metalRenderPass.GetSubpass(currentSubpass)];
        queue.GetDevice().SetResourceName(currentRenderEncoder, SR_FORMAT("Render encoder for render pass [{0}]", name));

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
    }

    void MetalCommandBuffer::EndRenderPass(const RenderPass& renderPass)
    {
        SR_THROW_IF(renderPass.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot end render pass [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", renderPass.GetName(), name)));
        const MetalRenderPass& metalRenderPass = static_cast<const MetalRenderPass&>(renderPass);

        CommandBuffer::EndRenderPass(metalRenderPass);
        SR_THROW_IF(currentRenderPass == nullptr, InvalidOperationError(SR_FORMAT("Cannot end render pass [{0}] within command buffer [{1}], as it has not been begun yet", metalRenderPass.GetName(), name)));
        SR_THROW_IF(currentRenderPass != &metalRenderPass, InvalidOperationError(SR_FORMAT("Cannot end render pass [{0}] within command buffer [{1}], as current render pass [{2}] has not been ended", metalRenderPass.GetName(), name, currentRenderPass->GetName())));

        [currentRenderEncoder endEncoding];
        currentRenderEncoder = nil;

        currentSubpass = 0;
        currentRenderPass = nullptr;
    }

    void MetalCommandBuffer::BeginGraphicsPipeline(const GraphicsPipeline& graphicsPipeline)
    {
        SR_THROW_IF(graphicsPipeline.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot begin graphics pipeline [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", graphicsPipeline.GetName(), name)));
        const MetalGraphicsPipeline& metalGraphicsPipeline = static_cast<const MetalGraphicsPipeline&>(graphicsPipeline);

        CommandBuffer::BeginGraphicsPipeline(graphicsPipeline);
        SR_THROW_IF(currentRenderPass == nullptr, InvalidOperationError(SR_FORMAT("Cannot begin graphics pipeline [{0}] within command buffer [{1}], as no render pass has been begun", metalGraphicsPipeline.GetName(), name)));
        SR_THROW_IF(currentGraphicsPipeline != nullptr && currentGraphicsPipeline != &metalGraphicsPipeline, InvalidOperationError(SR_FORMAT("Cannot begin graphics pipeline [{0}] within command buffer [{1}], as current graphics pipeline [{2}] has not been ended", metalGraphicsPipeline.GetName(), name, currentGraphicsPipeline->GetName())));

        // Bind pipeline and set appropriate settings
        [currentRenderEncoder setCullMode: metalGraphicsPipeline.GetCullMode()];
        [currentRenderEncoder setTriangleFillMode: metalGraphicsPipeline.GetTriangleFillMode()];
        [currentRenderEncoder setFrontFacingWinding: metalGraphicsPipeline.GetWinding()];
        [currentRenderEncoder setRenderPipelineState: metalGraphicsPipeline.GetRenderPipelineState()];
        if (metalGraphicsPipeline.GetDepthStencilState() != nil) [currentRenderEncoder setDepthStencilState: metalGraphicsPipeline.GetDepthStencilState()];

        currentGraphicsPipeline = &metalGraphicsPipeline;
    }

    void MetalCommandBuffer::EndGraphicsPipeline(const GraphicsPipeline& graphicsPipeline)
    {
        SR_THROW_IF(graphicsPipeline.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot end graphics pipeline [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", graphicsPipeline.GetName(), name)));
        const MetalGraphicsPipeline& metalGraphicsPipeline = static_cast<const MetalGraphicsPipeline&>(graphicsPipeline);

        CommandBuffer::EndGraphicsPipeline(metalGraphicsPipeline);
        SR_THROW_IF(currentGraphicsPipeline == nullptr, InvalidOperationError(SR_FORMAT("Cannot end graphics pipeline [{0}] within command buffer [{1}], as it has not been begun yet", metalGraphicsPipeline.GetName(), name)));
        SR_THROW_IF(currentGraphicsPipeline != &metalGraphicsPipeline, InvalidOperationError(SR_FORMAT("Cannot end graphics pipeline [{0}] within command buffer [{1}], as current graphics pipeline [{2}] has not been ended", metalGraphicsPipeline.GetName(), name, currentGraphicsPipeline->GetName())));

        currentGraphicsPipeline = nullptr;
    }

    void MetalCommandBuffer::BindVertexBuffer(const Buffer& vertexBuffer, const size offset)
    {
        SR_THROW_IF(vertexBuffer.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot bind vertex buffer [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", vertexBuffer.GetName(), name)));
        const MetalBuffer& metalVertexBuffer = static_cast<const MetalBuffer&>(vertexBuffer);

        CommandBuffer::BindVertexBuffer(metalVertexBuffer, offset);

        [currentRenderEncoder setVertexBuffer: metalVertexBuffer.GetMetalBuffer() offset: offset atIndex: MetalDevice::VERTEX_BUFFER_INDEX];
        initialVertexBufferOffset = offset;
    }

    void MetalCommandBuffer::BindIndexBuffer(const Buffer& indexBuffer, const size offset)
    {
        SR_THROW_IF(indexBuffer.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot bind index buffer [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", indexBuffer.GetName(), name)));
        const MetalBuffer& metalIndexBuffer = static_cast<const MetalBuffer&>(indexBuffer);

        CommandBuffer::BindIndexBuffer(metalIndexBuffer, offset);

        currentIndexBuffer = &metalIndexBuffer;
        initialIndexBufferOffset = offset;
    }

    void MetalCommandBuffer::SetScissor(const Vector4UInt scissor)
    {
        CommandBuffer::SetScissor(scissor);
        SR_THROW_IF(currentRenderPass == nullptr, InvalidOperationError(SR_FORMAT("Cannot set scissor within command buffer [{0}], as no render pass has been begun", name)));

        [currentRenderEncoder setScissorRect: { scissor.x, scissor.y, scissor.z, scissor.w }];
    }

    void MetalCommandBuffer::Draw(const uint32 vertexCount, const size vertexOffset)
    {
        CommandBuffer::Draw(vertexCount, vertexOffset);
        SR_THROW_IF(currentGraphicsPipeline == nullptr, InvalidOperationError(SR_FORMAT("Cannot draw within command buffer [{0}], as no graphics pipeline has been begun", name)));

        const size vertexBufferOffset = initialVertexBufferOffset + (static_cast<uint64>(vertexOffset) * currentGraphicsPipeline->GetVertexStride());
        SR_THROW_IF(currentVertexBuffer != nullptr && vertexBufferOffset > currentVertexBuffer->GetMemorySize(), ValueOutOfRangeError(SR_FORMAT("Cannot draw from invalid vertex index in vertex buffer [{0}] within command buffer [{1}]", currentVertexBuffer->GetName(), name), vertexBufferOffset, size(0), currentVertexBuffer->GetMemorySize()));

        if (vertexBufferOffset > 0) [currentRenderEncoder setVertexBufferOffset: vertexBufferOffset atIndex: MetalDevice::VERTEX_BUFFER_INDEX];
        [currentRenderEncoder drawPrimitives: MTLPrimitiveTypeTriangle vertexStart: 0 vertexCount: vertexCount];
    }

    void MetalCommandBuffer::DrawIndexed(const uint32 indexCount, const size indexOffset, const size vertexOffset)
    {
        CommandBuffer::DrawIndexed(indexCount, indexOffset, vertexOffset);
        SR_THROW_IF(currentRenderPass == nullptr, InvalidOperationError(SR_FORMAT("Cannot draw indexed within command buffer [{0}], as no graphics pipeline has been begun", name)));
        SR_THROW_IF(currentIndexBuffer == nullptr, InvalidOperationError(SR_FORMAT("Cannot draw indexed within command buffer [{0}], as no index buffer has been bound", name)));

        const size indexBufferOffset = initialIndexBufferOffset + indexOffset * sizeof(uint32);
        SR_THROW_IF(indexBufferOffset > currentIndexBuffer->GetMemorySize(), ValueOutOfRangeError(SR_FORMAT("Cannot draw from invalid index offset in index buffer [{0}] within command buffer [{1}]", currentIndexBuffer->GetName(), name), indexBufferOffset, size(0), currentVertexBuffer->GetMemorySize()));

        const size vertexBufferOffset = initialVertexBufferOffset + (static_cast<size>(vertexOffset) * currentGraphicsPipeline->GetVertexStride());
        SR_THROW_IF(currentVertexBuffer != nullptr && vertexBufferOffset > currentVertexBuffer->GetMemorySize(), ValueOutOfRangeError(SR_FORMAT("Cannot draw indexed from invalid vertex offset in vertex buffer [{0}] within command buffer [{1}]", currentVertexBuffer->GetName(), name), vertexBufferOffset, size(0), currentVertexBuffer->GetMemorySize()));

        if (vertexBufferOffset > 0) [currentRenderEncoder setVertexBufferOffset: vertexBufferOffset atIndex: MetalDevice::VERTEX_BUFFER_INDEX];
        [currentRenderEncoder drawIndexedPrimitives: MTLPrimitiveTypeTriangle indexCount: indexCount indexType: MTLIndexTypeUInt32 indexBuffer: currentIndexBuffer->GetMetalBuffer() indexBufferOffset: indexBufferOffset instanceCount: 1];
    }

    void MetalCommandBuffer::BeginComputePipeline(const ComputePipeline& computePipeline)
    {
        SR_THROW_IF(computePipeline.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot begin compute pipeline [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", computePipeline.GetName(), name)));
        const MetalComputePipeline& metalComputePipeline = static_cast<const MetalComputePipeline&>(computePipeline);

        CommandBuffer::BeginComputePipeline(metalComputePipeline);
        SR_THROW_IF(currentComputePipeline == nullptr, InvalidOperationError(SR_FORMAT("Cannot begin compute pipeline [{0}] within command buffer [{1}], as no render pass has been begun", metalComputePipeline.GetName(), name)));
        SR_THROW_IF(currentComputePipeline != nullptr && currentComputePipeline != &metalComputePipeline, InvalidOperationError(SR_FORMAT("Cannot begin compute pipeline [{0}] within command buffer [{1}], as current compute pipeline [{2}] has not been ended", metalComputePipeline.GetName(), name, currentComputePipeline->GetName())));

        // End any prior transfer operations
        if (currentBlitEncoder != nil)
        {
            [currentBlitEncoder endEncoding];
            currentBlitEncoder = nil;
        }

        // Begin encoding compute commands
        currentComputeEncoder = [commandBuffer computeCommandEncoderWithDispatchType: MTLDispatchTypeConcurrent];
        queue.GetDevice().SetResourceName(currentComputeEncoder, SR_FORMAT("Compute encoder for pipeline [{0}]", name));

        // Assign provided compute pipeline
        [currentComputeEncoder setComputePipelineState: metalComputePipeline.GetComputePipelineState()];
        currentComputePipeline = &metalComputePipeline;

        // Bind bindless argument buffer
        if (currentResourceTable != nullptr)
        {
            [currentComputeEncoder setBuffer: currentResourceTable->GetMetalArgumentBuffer() offset: 0 atIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_INDEX];

            for (const auto &[key, resource] : currentResourceTable->GetBoundUniformBuffers()) [currentComputeEncoder useResource: resource usage: MTLResourceUsageRead];
            for (const auto &[key, resource] : currentResourceTable->GetBoundStorageBuffers()) [currentComputeEncoder useResource: resource usage: MTLResourceUsageRead | MTLResourceUsageWrite];
            for (const auto &[key, resource] : currentResourceTable->GetBoundSampledImages()) [currentComputeEncoder useResource: resource usage: MTLResourceUsageRead];
            for (const auto &[key, resource] : currentResourceTable->GetBoundStorageImages()) [currentComputeEncoder useResource: resource usage: MTLResourceUsageRead | MTLResourceUsageWrite];
        }
    }

    void MetalCommandBuffer::EndComputePipeline(const ComputePipeline& computePipeline)
    {
        SR_THROW_IF(computePipeline.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot end compute pipeline [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", computePipeline.GetName(), name)));
        const MetalComputePipeline& metalComputePipeline = static_cast<const MetalComputePipeline&>(computePipeline);

        CommandBuffer::EndComputePipeline(computePipeline);
        SR_THROW_IF(currentComputePipeline == nullptr, InvalidOperationError(SR_FORMAT("Cannot end compute pipeline [{0}] within command buffer [{1}], as it has not been begun yet", metalComputePipeline.GetName(), name)));
        SR_THROW_IF(currentComputePipeline != &metalComputePipeline, InvalidOperationError(SR_FORMAT("Cannot end compute pipeline [{0}] within command buffer [{1}], as current compute pipeline [{2}] has not been ended", metalComputePipeline.GetName(), name, currentComputePipeline->GetName())));

        [currentComputeEncoder endEncoding];
        currentComputeEncoder = nil;

        currentComputePipeline = nullptr;
    }

    void MetalCommandBuffer::Dispatch(const Vector3UInt workGroupSize)
    {
        CommandBuffer::Dispatch(workGroupSize);

        SR_THROW_IF(currentComputePipeline == nullptr, InvalidOperationError(SR_FORMAT("Cannot dispatch within command buffer [{0}], as no compute pipeline has been begun", name)));
        SR_THROW_IF(workGroupSize.x > queue.GetDevice().GetLimits().maxWorkGroupSize.x, ValueOutOfRangeError(SR_FORMAT("Cannot dispatch command buffer [{0}], as specified work group's horizontal axis is greater than the max work group horizontal axis of device [{1}] - use Device::GetLimits() to query limits", name, queue.GetDevice().GetName()), workGroupSize.x, 1U, queue.GetDevice().GetLimits().maxWorkGroupSize.x));
        SR_THROW_IF(workGroupSize.y > queue.GetDevice().GetLimits().maxWorkGroupSize.y, ValueOutOfRangeError(SR_FORMAT("Cannot dispatch command buffer [{0}], as specified work group's horizontal axis is greater than the max work group vertical axis of device [{1}] - use Device::GetLimits() to query limits", name, queue.GetDevice().GetName()), workGroupSize.x, 1U, queue.GetDevice().GetLimits().maxWorkGroupSize.y));
        SR_THROW_IF(workGroupSize.z > queue.GetDevice().GetLimits().maxWorkGroupSize.z, ValueOutOfRangeError(SR_FORMAT("Cannot dispatch command buffer [{0}], as specified work group's horizontal axis is greater than the max work group depth axis of device [{1}] - use Device::GetLimits() to query limits", name, queue.GetDevice().GetName()), workGroupSize.x, 1U, queue.GetDevice().GetLimits().maxWorkGroupSize.z));

        SR_THROW_IF(currentComputePipeline == nullptr, InvalidOperationError(SR_FORMAT("Cannot dispatch within command buffer [{0}], as no compute pipeline has been begun", name)));

        // Dispatch work groups
        [currentComputeEncoder dispatchThreadgroups: MTLSizeMake(workGroupSize.x, workGroupSize.y, workGroupSize.z) threadsPerThreadgroup: MTLSizeMake(1, 1, 1)];
    }

    void MetalCommandBuffer::BeginDebugRegion(const std::string_view regionName, const Color32 color)
    {
        CommandBuffer::BeginDebugRegion(regionName, color);
        SR_THROW_IF(debugRegionBegan, InvalidOperationError(SR_FORMAT("Cannot begin debug region [{0}] within command buffer [{1}], as current debug region has not been ended", regionName, name)));

        NSString* const group = [NSString  stringWithCString: regionName.data() length: regionName.size()];
        [commandBuffer pushDebugGroup: group];

        debugRegionBegan = true;
    }

    void MetalCommandBuffer::InsertDebugMarker(const std::string_view markerName, const Color32 color)
    {
        CommandBuffer::InsertDebugMarker(markerName, color);
        SR_THROW_IF(debugRegionBegan, InvalidOperationError(SR_FORMAT("Cannot end debug region within command buffer [{0}], as one must have been begun first", name)));

        NSString* const signpost = [NSString stringWithCString: markerName.data() length: markerName.size()];
        [currentRenderEncoder insertDebugSignpost: signpost];
    }

    void MetalCommandBuffer::EndDebugRegion()
    {
        [commandBuffer popDebugGroup];

        debugRegionBegan = false;
    }

    std::unique_ptr<Buffer>& MetalCommandBuffer::QueueBufferForDestruction(std::unique_ptr<Buffer> &&buffer)
    {
        return queuedBuffersForDestruction.emplace(std::move(buffer));
    }

    std::unique_ptr<Image>& MetalCommandBuffer::QueueImageForDestruction(std::unique_ptr<Image> &&image)
    {
        return queuedImagesForDestruction.emplace(std::move(image));
    }

    /* --- PRIVATE METHODS --- */

    void MetalCommandBuffer::UpdateBlitEncoder()
    {
        if (currentBlitEncoder != nil) return;

        currentBlitEncoder = [commandBuffer blitCommandEncoder];
        queue.GetDevice().SetResourceName(currentBlitEncoder, "Transfer encoder");
    }

}
