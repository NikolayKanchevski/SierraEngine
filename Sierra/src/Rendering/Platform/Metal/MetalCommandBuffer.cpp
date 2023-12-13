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
        // Since in Metal you cannot reuse a command buffer after submitting it, one is created upon Begin()
    }

    /* --- POLLING METHODS --- */

    void MetalCommandBuffer::Begin()
    {
        // Create command buffer
        commandBuffer = device.GetCommandQueue()->commandBuffer();
    }

    void MetalCommandBuffer::End()
    {

    }

    /* --- DESTRUCTOR --- */

    void MetalCommandBuffer::Destroy()
    {

    }

}