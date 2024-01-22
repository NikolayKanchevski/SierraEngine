//
// Created by Nikolay Kanchevski on 12.12.23.
//

#include "MetalCommandBuffer.h"

#include "MetalBuffer.h"
#include "MetalImage.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalCommandBuffer::MetalCommandBuffer(const MetalDevice &device, const CommandBufferCreateInfo &createInfo)
        : CommandBuffer(createInfo), MetalResource(createInfo.name), device(device)
    {
        // Since in Metal you cannot reuse a command buffer after submitting it, one is created upon every Begin() call

        // Create and signal completion semaphore
        completionSemaphore = dispatch_semaphore_create(1);
        dispatch_semaphore_signal(completionSemaphore);
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
        MTL_SET_OBJECT_NAME(commandBuffer, GetName());

        // Create completion semaphore
        completionSemaphore = dispatch_semaphore_create(1);
        commandBuffer->addCompletedHandler(^(MTL::CommandBuffer*) { dispatch_semaphore_signal(completionSemaphore); });
    }

    void MetalCommandBuffer::End()
    {
        currentIndexBuffer = nullptr;
        currentIndexBufferOffset = 0;
    }

    void MetalCommandBuffer::BeginDebugRegion(const std::string &regionName, const Color &color) const
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif
        commandBuffer->pushDebugGroup(NS::String::string(regionName.c_str(), NS::ASCIIStringEncoding));
    }

    void MetalCommandBuffer::InsertDebugMarker(const std::string &markerName, const Color &color) const
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif

        if (currentRenderEncoder != nullptr) currentRenderEncoder->insertDebugSignpost(NS::String::string(markerName.c_str(), NS::ASCIIStringEncoding));
        if (currentComputeEncoder != nullptr) currentComputeEncoder->insertDebugSignpost(NS::String::string(markerName.c_str(), NS::ASCIIStringEncoding));
    }

    void MetalCommandBuffer::EndDebugRegion() const
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif
        commandBuffer->popDebugGroup();
    }

    void MetalCommandBuffer::SynchronizeBufferUsage(const std::unique_ptr<Buffer> &buffer, const BufferCommandUsage previousUsage, const BufferCommandUsage nextUsage, const uint64 memorySize, const uint64 offset)
    {
        SR_ERROR_IF(buffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not synchronize usage of buffer [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), buffer->GetName());
        const MetalBuffer &metalBuffer = static_cast<MetalBuffer&>(*buffer);

        const MTL::Resource* bufferResource = metalBuffer.GetMetalBuffer();
        if (currentRenderEncoder != nullptr) currentRenderEncoder->memoryBarrier(&bufferResource, 1, BufferCommandUsageToRenderStages(previousUsage), BufferCommandUsageToRenderStages(nextUsage));
        if (currentComputeEncoder != nullptr) currentComputeEncoder->memoryBarrier(&bufferResource, 1);
    }

    void MetalCommandBuffer::SynchronizeImageUsage(const std::unique_ptr<Image> &image, const ImageCommandUsage previousUsage, const ImageCommandUsage nextUsage, const uint32 baseMipLevel, const uint32 mipLevelCount, const uint32 baseLayer, const uint32 layerCount)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Could not synchronize usage of image [{0}] within command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), image->GetName());
        const MetalImage &metalImage = static_cast<MetalImage&>(*image);

        const MTL::Resource* textureResource = metalImage.GetMetalTexture();
        if (currentRenderEncoder != nullptr) currentRenderEncoder->memoryBarrier(&textureResource, 1, ImageCommandUsageToRenderStages(previousUsage), ImageCommandUsageToRenderStages(nextUsage));
        if (currentComputeEncoder != nullptr) currentComputeEncoder->memoryBarrier(&textureResource, 1);
    }

    /* --- PRIVATE METHODS --- */

    void MetalCommandBuffer::PushRenderEncoder(MTL::RenderCommandEncoder* renderCommandEncoder)
    {
        currentRenderEncoder = renderCommandEncoder;
    }

    void MetalCommandBuffer::PopRenderEncoder()
    {
        currentRenderEncoder->endEncoding();
        #if SR_PLATFORM_macOS
            currentRenderEncoder->release(); // NOTE: For some reason not calling this causes a memory leak on macOS (despite spec stating manual release is unnecessary, and thus, and a crash on iOS)
        #endif
        currentRenderEncoder = nullptr;
    }

    void MetalCommandBuffer::BindIndexBuffer(MTL::Buffer* indexBuffer, const uint64 offset)
    {
        currentIndexBuffer = indexBuffer;
        currentIndexBufferOffset = offset;
    }

    void MetalCommandBuffer::PushComputeEncoder(MTL::ComputeCommandEncoder* computeCommandEncoder)
    {
        currentComputeEncoder = computeCommandEncoder;
    }

    void MetalCommandBuffer::PopComputeEncoder()
    {
        currentComputeEncoder->endEncoding();
        #if SR_PLATFORM_macOS
            currentComputeEncoder->release(); // NOTE: For some reason not calling this causes a memory leak on macOS (despite spec stating manual release is unnecessary, and thus, and a crash on iOS)
        #endif
        currentComputeEncoder = nullptr;
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
