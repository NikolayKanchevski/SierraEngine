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
    }

    void MetalCommandBuffer::End()
    {

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

}
