//
// Created by Nikolay Kanchevski on 1.01.24.
//

#pragma once

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLRenderPipelineState = void;
        using MTLDepthStencilState = void;

        using MTLCullMode = ulong;
        using MTLTriangleFillMode = ulong;
        using MTLWinding = ulong;
    }
#endif

#include "MetalResource.h"
#include "../GraphicsPipeline.h"

#include "MetalDevice.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API MTLCullMode CullModeToCullMode(CullMode cullMode) noexcept;
    [[nodiscard]] SIERRA_API MTLTriangleFillMode ShadeModeToTriangleFillMode(ShadeMode shadeMode) noexcept;
    [[nodiscard]] SIERRA_API MTLWinding FrontFaceModeToWinding(FrontFaceMode frontFaceMode) noexcept;

    class SIERRA_API MetalGraphicsPipeline final : public GraphicsPipeline, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalGraphicsPipeline(const MetalDevice& device, const GraphicsPipelineCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override;

        [[nodiscard]] id<MTLRenderPipelineState> GetRenderPipelineState() const noexcept { return renderPipelineState; }
        [[nodiscard]] id<MTLDepthStencilState> GetDepthStencilState() const noexcept { return depthStencilState; }

        [[nodiscard]] size GetVertexStride() const noexcept { return vertexStride; }
        [[nodiscard]] bool HasFragmentShader() const noexcept { return hasFragmentShader; }

        [[nodiscard]] MTLCullMode GetCullMode() const noexcept { return cullMode; }
        [[nodiscard]] MTLTriangleFillMode GetTriangleFillMode() const noexcept { return triangleFillMode; }
        [[nodiscard]] MTLWinding GetWinding() const noexcept { return winding; }

        /* --- COPY SEMANTICS --- */
        MetalGraphicsPipeline(const MetalGraphicsPipeline&) = delete;
        MetalGraphicsPipeline& operator=(const MetalGraphicsPipeline&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalGraphicsPipeline(MetalGraphicsPipeline&&) = delete;
        MetalGraphicsPipeline& operator=(MetalGraphicsPipeline&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MetalGraphicsPipeline() noexcept override;

    private:
        const MTLCullMode cullMode;
        const MTLTriangleFillMode triangleFillMode;
        const MTLWinding winding;

        id<MTLRenderPipelineState> renderPipelineState = nil;
        id<MTLDepthStencilState> depthStencilState = nil;

        uint32 vertexStride = 0;
        bool hasFragmentShader = false;


    };

}
