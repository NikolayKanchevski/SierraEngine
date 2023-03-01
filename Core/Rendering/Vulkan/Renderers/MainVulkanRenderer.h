//
// Created by Nikolay Kanchevski on 23.12.22.
//

#pragma once

#include "../../RenderingSettings.h"
#include "../Abstractions/Abstractions.h"
#include "VulkanRenderer.h"
#include "../../../../Engine/Classes/Mesh.h"
#include "../Abstractions/OffscreenRenderer.h"
#include "../../../../Engine/Components/Lighting/PointLight.h"
#include "../../../../Engine/Components/Lighting/DirectionalLight.h"
#include "../../RenderingTemplates.h"

using namespace Sierra::Engine::Classes;
using namespace Sierra::Engine::Components;

namespace Sierra::Core::Rendering::Vulkan::Renderers
{

    class MainVulkanRenderer : public VulkanRenderer
    {
    private:
        typedef GraphicsPipeline<MeshPushConstant, UniformData, StorageData> ScenePipeline;
        typedef GraphicsPipeline<SkyboxPushConstant, UniformData, NullType> SkyboxPipeline;

    public:
        /* --- CONSTRUCTORS --- */
        MainVulkanRenderer(const VulkanRendererCreateInfo &createInfo);
        static UniquePtr<MainVulkanRenderer> Create(VulkanRendererCreateInfo createInfo);

        /* --- POLLING METHODS --- */
        void Update() override;
        void Render() override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkDescriptorSet GetRenderedTextureDescriptorSet() const override { return offscreenImageDescriptorSets[swapchain->GetCurrentFrameIndex()]; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        void InitializeOffscreenRendering();
        void TerminateOffscreenRendering();

        void CreateSkyboxRenderingObjects();
        void CreateSceneRenderingObjects();

        void CreateTimestampQueries();
        void CreateOffscreenDescriptorSets();

        void DrawUI() override;

        Sampling sampling = Sampling::MSAAx1;
        ShadingType shadingType = ShadingType::FILL;

        SharedPtr<DescriptorSetLayout> sceneDescriptorSetLayout;
        UniquePtr<OffscreenRenderer> sceneOffscreenRenderer;
        UniquePtr<ScenePipeline> scenePipeline;

        SharedPtr<DescriptorSetLayout> skyboxDescriptorSetLayout;
        UniquePtr<Mesh> skyboxMesh;
        UniquePtr<SkyboxPipeline> skyboxPipeline;

        std::vector<VkDescriptorSet> offscreenImageDescriptorSets;
        std::vector<UniquePtr<TimestampQuery>> offscreenTimestampQueries;
    };

}