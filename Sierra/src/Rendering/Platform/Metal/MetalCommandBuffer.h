//
// Created by Nikolay Kanchevski on 12.12.23.
//

#pragma once

#include "../../CommandBuffer.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalCommandBuffer final : public CommandBuffer, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalCommandBuffer(const MetalDevice &device, const CommandBufferCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Begin() override;
        void End() override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline MTL::CommandBuffer* GetMetalCommandBuffer() const { return commandBuffer; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        const MetalDevice &device;
        MTL::CommandBuffer* commandBuffer = nullptr;

    };

}
