//
// Created by Nikolay Kanchevski on 28.12.23.
//

#pragma once

#include "RenderingResource.h"

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

    enum class ShadeMode : bool
    {
        Fill,
        Wireframe
    };

    enum class CullMode : uint8
    {
        None,
        Front,
        Back
    };

    enum class FrontFaceMode : bool
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

    class SIERRA_API GraphicsPipeline : public virtual RenderingResource
    {
    public:
        /* --- OPERATORS --- */
        GraphicsPipeline(const GraphicsPipeline&) = delete;
        GraphicsPipeline &operator=(const GraphicsPipeline&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~GraphicsPipeline() = default;

    protected:
        explicit GraphicsPipeline(const GraphicsPipelineCreateInfo &createInfo);

    };

}
