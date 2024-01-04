//
// Created by Nikolay Kanchevski on 1.01.24.
//

#pragma once

#include "../../GraphicsPipeline.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalGraphicsPipeline : public GraphicsPipeline, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalGraphicsPipeline(const MetalDevice &device, const GraphicsPipelineCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Begin(std::unique_ptr<CommandBuffer> &commandBuffer) const override;
        void End(std::unique_ptr<CommandBuffer> &commandBuffer) const override;

        void Draw(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 vertexCount) const override;

        /* --- DESTRUCTOR --- */
        ~MetalGraphicsPipeline();

        /* --- CONVERSIONS --- */
        [[nodiscard]] static MTL::CullMode CullModeToCullMode(CullMode cullMode);
        [[nodiscard]] static MTL::TriangleFillMode ShadeModeToTriangleFillMode(ShadeMode shadeMode);
        [[nodiscard]] static MTL::Winding FrontFaceModeToWinding(FrontFaceMode frontFaceMode);

    private:
        MTL::RenderPipelineState* renderPipelineState = nullptr;

        const MTL::CullMode cullMode;
        const MTL::TriangleFillMode triangleFillMode;
        const MTL::Winding frontFacingWinding;

    };

}
