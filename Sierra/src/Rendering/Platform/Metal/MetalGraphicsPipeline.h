//
// Created by Nikolay Kanchevski on 1.01.24.
//

#pragma once

#include "../../GraphicsPipeline.h"
#include "MetalResource.h"

#include "MetalDevice.h"
#include "MetalPipelineLayout.h"
#include "MetalBuffer.h"

namespace Sierra
{

    class SIERRA_API MetalGraphicsPipeline final : public GraphicsPipeline, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalGraphicsPipeline(const MetalDevice &device, const GraphicsPipelineCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline MTL::RenderPipelineState* GetRenderPipelineState() const { return renderPipelineState; }
        [[nodiscard]] inline const MetalPipelineLayout& GetLayout() const { return layout; }
        [[nodiscard]] inline bool HasFragmentShader() const { return hasFragmentShader; }

        [[nodiscard]] inline MTL::CullMode GetCullMode() const { return cullMode; }
        [[nodiscard]] inline MTL::TriangleFillMode GetTriangleFillMode() const { return triangleFillMode; }
        [[nodiscard]] inline MTL::Winding GetWinding() const { return winding; }

        /* --- DESTRUCTOR --- */
        ~MetalGraphicsPipeline() override;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static MTL::CullMode CullModeToCullMode(CullMode cullMode);
        [[nodiscard]] static MTL::TriangleFillMode ShadeModeToTriangleFillMode(ShadeMode shadeMode);
        [[nodiscard]] static MTL::Winding FrontFaceModeToWinding(FrontFaceMode frontFaceMode);

    private:
        const MetalPipelineLayout &layout;

        MTL::RenderPipelineState* renderPipelineState = nullptr;
        bool hasFragmentShader = false;

        MTL::CullMode cullMode;
        MTL::TriangleFillMode triangleFillMode;
        MTL::Winding winding;

    };

}
