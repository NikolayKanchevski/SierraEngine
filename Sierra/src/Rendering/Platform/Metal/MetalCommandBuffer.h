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
        [[nodiscard]] inline MTL::RenderCommandEncoder* GetCurrentRenderEncoder() const { return currentRenderEncoder; }

        /* --- DESTRUCTOR --- */
        ~MetalCommandBuffer() = default;

    private:
        const MetalDevice &device;
        MTL::CommandBuffer* commandBuffer = nullptr;
        MTL::RenderCommandEncoder* currentRenderEncoder = nullptr;

        friend class MetalRenderPass;
        void PushRenderEncoder(MTL::RenderCommandEncoder* renderCommandEncoder);
        void PopRenderEncoder();

    };

}
