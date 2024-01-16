//
// Created by Nikolay Kanchevski on 12.12.23.
//

#include "MetalCommandBuffer.h"

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
        // Create command buffer
        #if SR_PLATFORM_macOS
            if (commandBuffer != nullptr) commandBuffer->release(); // NOTE: For some reason not calling this causes a memory leak on macOS (despite spec stating manual release is unnecessary, and thus, and a crash on iOS)
        #endif
        commandBuffer = device.GetCommandQueue()->commandBuffer();
        MTL_SET_OBJECT_NAME(commandBuffer, GetName().c_str());
    }

    void MetalCommandBuffer::End()
    {
        currentIndexBuffer = nullptr;
        currentIndexBufferOffset = 0;

        currentRenderEncoder = nullptr;
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
        currentCommandEncoder->insertDebugSignpost(NS::String::string(markerName.c_str(), NS::ASCIIStringEncoding));
    }

    void MetalCommandBuffer::EndDebugRegion() const
    {
        #if !SR_ENABLE_LOGGING
            return;
        #endif
        commandBuffer->popDebugGroup();
    }

    /* --- PRIVATE METHODS --- */

    void MetalCommandBuffer::PushRenderEncoder(MTL::RenderCommandEncoder* renderCommandEncoder)
    {
        currentRenderEncoder = renderCommandEncoder;
        currentCommandEncoder = renderCommandEncoder;
    }

    void MetalCommandBuffer::PopRenderEncoder()
    {
        currentRenderEncoder->endEncoding();
        #if SR_PLATFORM_macOS
            currentRenderEncoder->release(); // NOTE: For some reason not calling this causes a memory leak on macOS (despite spec stating manual release is unnecessary, and thus, and a crash on iOS)
        #endif
        currentRenderEncoder = nullptr;
        currentCommandEncoder = nullptr;
    }

    void MetalCommandBuffer::BindIndexBuffer(MTL::Buffer* indexBuffer, const uint64 offset)
    {
        currentIndexBuffer = indexBuffer;
        currentIndexBufferOffset = offset;
    }

}
