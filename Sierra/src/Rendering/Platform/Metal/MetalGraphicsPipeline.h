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

        /* --- POLLING METHODS --- */
        void Begin(std::unique_ptr<CommandBuffer> &commandBuffer) const override;
        void End(std::unique_ptr<CommandBuffer> &commandBuffer) const override;

        void PushConstants(std::unique_ptr<CommandBuffer> &commandBuffer, const void* data, uint16 memoryRange, uint16 offset = 0) const override;
        void BindBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 binding, const std::unique_ptr<Buffer> &buffer, uint32 arrayIndex = 0, uint64 memoryRange = 0, uint64 offset = 0) const override;
        void BindImage(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 binding, const std::unique_ptr<Image> &image, uint32 arrayIndex = 0) const override;

        void BindVertexBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Buffer> &vertexBuffer, uint64 offset = 0) const override;
        void BindIndexBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Buffer> &indexBuffer, uint64 offset = 0) const override;
        void Draw(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 vertexCount) const override;
        void DrawIndexed(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 indexCount, uint64 indexOffset = 0, uint64 vertexOffset = 0) const override;

        /* --- DESTRUCTOR --- */
        ~MetalGraphicsPipeline() override;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static MTL::CullMode CullModeToCullMode(CullMode cullMode);
        [[nodiscard]] static MTL::TriangleFillMode ShadeModeToTriangleFillMode(ShadeMode shadeMode);
        [[nodiscard]] static MTL::Winding FrontFaceModeToWinding(FrontFaceMode frontFaceMode);

    private:
        const MetalPipelineLayout &pipelineLayout;

        MTL::RenderPipelineState* renderPipelineState = nullptr;
        std::optional<MetalBuffer> pushConstantBuffer;
        bool hasFragmentShader = false;

        const MTL::CullMode cullMode;
        const MTL::TriangleFillMode triangleFillMode;
        const MTL::Winding frontFacingWinding;
        const NS::UInteger VERTEX_BUFFER_INDEX = 30;

    };

}
