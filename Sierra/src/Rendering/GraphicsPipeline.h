//
// Created by Nikolay Kanchevski on 28.12.23.
//

#pragma once

#include "RenderingResource.h"

#include "Shader.h"
#include "RenderPass.h"
#include "Image.h"
#include "CommandBuffer.h"

namespace Sierra
{

    enum class VertexInput
    {
        Float,
        Float2,
        Float3,
        Float4,

        Position        = Float3,
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
        const std::unique_ptr<Shader> &vertexShader;
        const std::optional<std::reference_wrapper<std::unique_ptr<Shader>>> fragmentShader = std::nullopt;
        const std::unique_ptr<RenderPass> &renderPass;
        uint32 subpassIndex = 0;

        const std::initializer_list<VertexInput> vertexInputs = { };
        uint8 pushConstantSize = 0;

        ImageSampling sampling = ImageSampling::x1;
        ShadeMode shadeMode = ShadeMode::Fill;
        CullMode cullMode = CullMode::Back;
        FrontFaceMode frontFaceMode = FrontFaceMode::CounterClockwise;
    };

    class SIERRA_API GraphicsPipeline : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Begin(std::unique_ptr<CommandBuffer> &commandBuffer) const = 0;
        virtual void End(std::unique_ptr<CommandBuffer> &commandBuffer) const = 0;

        virtual void Draw(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 vertexCount) const = 0;

        /* --- OPERATORS --- */
        GraphicsPipeline(const GraphicsPipeline &) = delete;
        GraphicsPipeline &operator=(const GraphicsPipeline &) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~GraphicsPipeline() = default;

    protected:
        explicit GraphicsPipeline(const GraphicsPipelineCreateInfo &createInfo);

    };

}
