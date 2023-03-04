//
// Created by Nikolay Kanchevski on 5.02.23.
//

#pragma once

#include "VulkanRenderer.h"

#include "../../RenderingTemplates.h"
#include "../Abstractions/DynamicRenderer.h"
#include "../../../../Engine/Classes/Mesh.h"
#include "../../../../Engine/Components/Lighting/PointLight.h"
#include "../../../../Engine/Components/Lighting/DirectionalLight.h"

using namespace Sierra::Engine::Classes;
using namespace Sierra::Engine::Components;

namespace Sierra::Core::Rendering::Vulkan::Renderers
{
    class DeferredVulkanRenderer : public VulkanRenderer
    {
    private:
        enum RenderedImageValue
        {
            RenderedImage = 0,
            PositionBuffer = 1,
            DiffuseBuffer = 2,
            SpecularBuffer = 3,
            ShininessBuffer = 5,
            NormalBuffer = 6,
            DepthBuffer = 7
        };

        struct MergingRendererPushConstant
        {
            RenderedImageValue renderedImageValue = RenderedImageValue::RenderedImage;

            float _align1_;
            float _align2_;
            float _align3_;
        };

        typedef GraphicsPipeline<MeshPushConstant, UniformData, StorageData> ScenePipeline;
        typedef GraphicsPipeline<MergingRendererPushConstant, NullType, StorageData> MergingPipeline;
        typedef GraphicsPipeline<SkyboxPushConstant, UniformData, NullType> SkyboxPipeline;
    public:
        /* --- CONSTRUCTORS --- */
        DeferredVulkanRenderer(const VulkanRendererCreateInfo &createInfo);
        static UniquePtr<DeferredVulkanRenderer> Create(VulkanRendererCreateInfo createInfo);

        /* --- POLLING METHODS --- */
        void Update() override;
        void DrawUI() override;
        void Render() override;

        /* --- SETTER METHODS --- */

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkDescriptorSet GetRenderedTextureDescriptorSet() const override { return renderedImageDescriptorSets[swapchain->GetCurrentFrameIndex()]; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;
    private:
        UniquePtr<Image> IDBuffer;
        UniquePtr<Image> positionBuffer;
        UniquePtr<Image> diffuseBuffer;
        UniquePtr<Image> specularAndShininessBuffer;
        UniquePtr<Image> normalBuffer;
        UniquePtr<Image> depthStencilBuffer;
        UniquePtr<Image> renderedImage;

        UniquePtr<ScenePipeline> bufferPipeline;
        UniquePtr<DynamicRenderer> bufferRenderer;
        SharedPtr<DescriptorSetLayout> bufferDescriptorSetLayout;

        UniquePtr<Mesh> skyboxMesh;
        UniquePtr<DynamicRenderer> skyboxRenderer;
        UniquePtr<SkyboxPipeline> skyboxPipeline;
        SharedPtr<DescriptorSetLayout> skyboxDescriptorSetLayout;

        UniquePtr<DynamicRenderer> mergingRenderer;
        UniquePtr<MergingPipeline> mergingPipeline;
        SharedPtr<DescriptorSetLayout> mergingDescriptorSetLayout;

        UniquePtr<Sampler> textureSampler;
        std::vector<VkDescriptorSet> renderedImageDescriptorSets;
        std::vector<UniquePtr<TimestampQuery>> renderTimestampQueries;

        void InitializeRenderer();
        void CreateSceneRenderingObjects();
        void CreateSkyboxRenderingObjects();
    };

}