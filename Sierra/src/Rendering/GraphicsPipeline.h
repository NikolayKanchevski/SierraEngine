//
// Created by Nikolay Kanchevski on 28.12.23.
//

#pragma once

#include "RenderingResource.h"

#include "Pipeline.h"
#include "Shader.h"
#include "PipelineLayout.h"
#include "RenderPass.h"

namespace Sierra
{

    enum class VertexInput
    {
        Float,
        Float2,
        Float3,
        Float4,

        Position2D      = Float2,
        Position3D      = Float3,
        Normal          = Float3,
        ColorRGB        = Float3,
        ColorRGBA       = Float4,
        UV              = Float2
    };

    enum class ShadeMode
    {
        Fill,
        Wireframe
    };

    enum class CullMode
    {
        None,
        Front,
        Back
    };

    enum class FrontFaceMode
    {
        Clockwise,
        CounterClockwise
    };

    struct GraphicsPipelineCreateInfo
    {
        const std::string &name = "Graphics Pipeline";
        uint32 concurrentFrameCount = 1;

        const std::initializer_list<VertexInput> &vertexInputs = { };
        const std::unique_ptr<Shader> &vertexShader;
        const std::optional<std::reference_wrapper<std::unique_ptr<Shader>>> fragmentShader = std::nullopt;

        const std::unique_ptr<PipelineLayout> &layout;
        const std::unique_ptr<RenderPass> &renderPass;
        uint32 subpassIndex = 0;

        ImageSampling sampling = ImageSampling::x1;
        ShadeMode shadeMode = ShadeMode::Fill;
        CullMode cullMode = CullMode::Back;
        FrontFaceMode frontFaceMode = FrontFaceMode::CounterClockwise;
    };

    class SIERRA_API GraphicsPipeline : public virtual Pipeline, public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void BindVertexBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Buffer> &vertexBuffer, uint64 offset = 0) const = 0;
        virtual void BindIndexBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Buffer> &indexBuffer, uint64 offset = 0) const = 0;

        virtual void Draw(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 vertexCount) const = 0;
        virtual void DrawIndexed(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 indexCount, uint64 indexOffset = 0, uint64 vertexOffset = 0) const = 0;

        /* --- DESTRUCTOR --- */
        ~GraphicsPipeline() override = default;

    protected:
        explicit GraphicsPipeline(const GraphicsPipelineCreateInfo &createInfo);

    };

}
