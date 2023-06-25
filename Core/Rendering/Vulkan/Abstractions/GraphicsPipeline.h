//
// Created by Nikolay Kanchevski on 18.05.23.
//

#pragma once

#include "Pipeline.h"
#include "RenderPass.h"
#include "DynamicRenderer.h"

#include "../../../Engine/Classes/Mesh.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    using Engine::Classes::Mesh;

    struct GraphicsPipelineDynamicRenderingInfo
    {
        const UniquePtr<DynamicRenderer> &dynamicRenderer;
    };

    struct GraphicsPipelineRenderPassInfo
    {
        UniquePtr<RenderPass> &renderPass;
        uint subpass = 0;
    };

    struct GraphicsPipelineCreateInfo
    {
        std::vector<SharedPtr<Shader>> shaders;
        Optional<CompiledPipelineShaderInfo> shaderInfo = std::nullopt;

        Optional<GraphicsPipelineDynamicRenderingInfo> dynamicRenderingInfo = std::nullopt;
        // OR
        Optional<GraphicsPipelineRenderPassInfo> renderPassInfo = std::nullopt;

        bool enableDepthBias = false;

        Sampling sampling = Sampling::MSAAx1;
        FrontFace frontFace = FrontFace::COUNTER_CLOCKWISE;
        CullMode cullMode = CullMode::FRONT;
        ShadingType shadingType = ShadingType::FILL;
    };

    class GraphicsPipeline : public Pipeline
    {
    public:
        /* --- CONSTRUCTORS --- */
        GraphicsPipeline(const GraphicsPipelineCreateInfo &createInfo);
        static UniquePtr<GraphicsPipeline> Create(const GraphicsPipelineCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Draw(const UniquePtr<CommandBuffer> &commandBuffer, uint vertexCount);
        void DrawMesh(const UniquePtr<CommandBuffer> &commandBuffer, const SharedPtr<Mesh> &mesh);

        /* --- SETTER METHODS --- */
        void SetFrontFace(FrontFace givenFrontFace);
        void SetCullMode(CullMode givenCullMode);
        void SetShadingType(ShadingType givenShadingType);

        /* --- DESTRUCTOR --- */
        void Destroy() override;
        DELETE_COPY(GraphicsPipeline);

    private:
        Optional<GraphicsPipelineRenderPassInfo> renderPassInfo;
        Optional<GraphicsPipelineDynamicRenderingInfo> dynamicRenderingInfo;

        Sampling sampling;
        FrontFace frontFace;
        CullMode cullMode;
        ShadingType shadingType;
        bool depthBiasEnabled;

        void CreatePipeline() override;

    };

}