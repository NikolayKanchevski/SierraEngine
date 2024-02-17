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
        [[nodiscard]] inline id<MTLRenderPipelineState> GetRenderPipelineState() const { return renderPipelineState; }
        [[nodiscard]] inline id<MTLDepthStencilState> GetDepthStencilState() const { return depthStencilState; }
        [[nodiscard]] inline const MetalPipelineLayout& GetLayout() const { return layout; }

        [[nodiscard]] inline uint32 GetVertexByteStride() const { return vertexByteStride; }
        [[nodiscard]] inline bool HasFragmentShader() const { return hasFragmentShader; }

        [[nodiscard]] inline MTLCullMode GetCullMode() const { return cullMode; }
        [[nodiscard]] inline MTLTriangleFillMode GetTriangleFillMode() const { return triangleFillMode; }
        [[nodiscard]] inline MTLWinding GetWinding() const { return winding; }

        /* --- DESTRUCTOR --- */
        ~MetalGraphicsPipeline() override;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static MTLCullMode CullModeToCullMode(CullMode cullMode);
        [[nodiscard]] static MTLTriangleFillMode ShadeModeToTriangleFillMode(ShadeMode shadeMode);
        [[nodiscard]] static MTLWinding FrontFaceModeToWinding(FrontFaceMode frontFaceMode);

    private:
        const MetalPipelineLayout &layout;

        id<MTLRenderPipelineState> renderPipelineState = nil;
        id<MTLDepthStencilState> depthStencilState = nil;

        uint32 vertexByteStride = 0;
        bool hasFragmentShader = false;

        MTLCullMode cullMode = MTLCullModeNone;
        MTLTriangleFillMode triangleFillMode = MTLTriangleFillModeFill;
        MTLWinding winding = MTLWindingCounterClockwise;

    };

}
