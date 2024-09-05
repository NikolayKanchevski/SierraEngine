//
// Created by Nikolay Kanchevski on 1.01.24.
//

#pragma once

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLRenderPipelineState = void;
        using MTLDepthStencilState = void;

        using MTLCullMode = std::uintptr_t;
        using MTLTriangleFillMode = std::uintptr_t;
        using MTLWinding = std::uintptr_t;
    }
#endif

#include "MetalResource.h"
#include "../../GraphicsPipeline.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalGraphicsPipeline final : public GraphicsPipeline, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalGraphicsPipeline(const MetalDevice& device, const GraphicsPipelineCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const override;

        [[nodiscard]] id<MTLRenderPipelineState> GetRenderPipelineState() const { return renderPipelineState; }
        [[nodiscard]] id<MTLDepthStencilState> GetDepthStencilState() const { return depthStencilState; }

        [[nodiscard]] uint32 GetVertexByteStride() const { return vertexByteStride; }
        [[nodiscard]] bool HasFragmentShader() const { return hasFragmentShader; }

        [[nodiscard]] MTLCullMode GetCullMode() const { return cullMode; }
        [[nodiscard]] MTLTriangleFillMode GetTriangleFillMode() const { return triangleFillMode; }
        [[nodiscard]] MTLWinding GetWinding() const { return winding; }

        /* --- DESTRUCTOR --- */
        ~MetalGraphicsPipeline() override;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static MTLCullMode CullModeToCullMode(CullMode cullMode);
        [[nodiscard]] static MTLTriangleFillMode ShadeModeToTriangleFillMode(ShadeMode shadeMode);
        [[nodiscard]] static MTLWinding FrontFaceModeToWinding(FrontFaceMode frontFaceMode);

    private:
        id<MTLRenderPipelineState> renderPipelineState = nil;
        id<MTLDepthStencilState> depthStencilState = nil;

        uint32 vertexByteStride = 0;
        bool hasFragmentShader = false;

        const MTLCullMode cullMode = MTLCullModeNone;
        const MTLTriangleFillMode triangleFillMode = MTLTriangleFillModeFill;
        const MTLWinding winding = MTLWindingCounterClockwise;

    };

}
