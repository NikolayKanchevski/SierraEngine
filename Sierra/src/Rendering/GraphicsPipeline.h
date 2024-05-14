//
// Created by Nikolay Kanchevski on 28.12.23.
//

#pragma once

#include "RenderingResource.h"

#include "Shader.h"
#include "RenderPass.h"

namespace Sierra
{

    enum class VertexInput : uint8
    {
        Int8,
        UInt8,
        Norm8,
        UNorm8,
        Int16,
        UInt16,
        Norm16,
        UNorm16,
        Float16,
        Int32,
        UInt32,
        Float32,

        Int8_2D,
        UInt8_2D,
        Norm8_2D,
        UNorm8_2D,
        Int16_2D,
        UInt16_2D,
        Norm16_2D,
        UNorm16_2D,
        Float16_2D,
        Int32_2D,
        UInt32_2D,
        Float32_2D,

        Int8_3D,
        UInt8_3D,
        Norm8_3D,
        UNorm8_3D,
        Int16_3D,
        UInt16_3D,
        Norm16_3D,
        UNorm16_3D,
        Float16_3D,
        Int32_3D,
        UInt32_3D,
        Float32_3D,

        Int8_4D,
        UInt8_4D,
        Norm8_4D,
        UNorm8_4D,
        Int16_4D,
        UInt16_4D,
        Norm16_4D,
        UNorm16_4D,
        Float16_4D,
        Int32_4D,
        UInt32_4D,
        Float32_4D,

        Position_2D     = Float32_2D,
        Position_3D     = Float32_3D,
        Normal_2D       = Float32_2D,
        Normal_3D       = Float32_3D,
        UV              = Float32_2D,
        ColorRGB8       = UNorm8_3D,
        ColorRGBA8      = UNorm8_4D,
        ColorRGB16      = UNorm16_3D,
        ColorRGBA16     = UNorm16_4D,
        Color           = ColorRGBA8
    };

    enum class ShadeMode : bool
    {
        Fill,
        Wireframe
    };

    enum class DepthMode : bool
    {
        None,
        WriteDepth
    };

    enum class BlendMode : bool
    {
        None,
        Alpha
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
        std::string_view name = "Graphics Pipeline";

        std::span<const VertexInput> vertexInputs = { };
        const std::unique_ptr<Shader> &vertexShader;
        const std::unique_ptr<Shader> &fragmentShader = nullptr;

        uint16 pushConstantSize = 0;
        ImageSampling sampling = ImageSampling::x1;

        const std::unique_ptr<RenderPass> &templateRenderPass;
        uint32 subpassIndex = 0;

        ShadeMode shadeMode = ShadeMode::Fill;
        DepthMode depthMode = DepthMode::None;
        BlendMode blendMode = BlendMode::None;
        CullMode cullMode = CullMode::None;
        FrontFaceMode frontFaceMode = FrontFaceMode::CounterClockwise;
    };

    class SIERRA_API GraphicsPipeline : public virtual RenderingResource
    {
    public:
        /* --- OPERATORS --- */
        GraphicsPipeline(const GraphicsPipeline&) = delete;
        GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~GraphicsPipeline() = default;

    protected:
        explicit GraphicsPipeline(const GraphicsPipelineCreateInfo &createInfo);

    };

}
