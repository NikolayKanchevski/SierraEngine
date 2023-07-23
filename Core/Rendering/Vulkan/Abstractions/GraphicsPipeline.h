//
// Created by Nikolay Kanchevski on 18.05.23.
//

#pragma once

#include "Pipeline.h"
#include "RenderPass.h"
#include "DynamicRenderer.h"

#include "../../../Engine/Classes/Mesh.h"

namespace Sierra::Rendering
{

    struct GraphicsPipelineDynamicRenderingInfo
    {
        const UniquePtr<DynamicRenderer> &dynamicRenderer;
    };

    struct GraphicsPipelineRenderPassInfo
    {
        uint subpass = 0;
        UniquePtr<RenderPass> &renderPass;
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
        CullMode cullMode = CullMode::BACK;
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
        void DrawMesh(const UniquePtr<CommandBuffer> &commandBuffer, const SharedPtr<Engine::Mesh> &mesh);

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