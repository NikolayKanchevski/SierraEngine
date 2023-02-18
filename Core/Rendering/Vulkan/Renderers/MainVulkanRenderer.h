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

using namespace Sierra::Engine::Classes;
using namespace Sierra::Engine::Components;

namespace Sierra::Core::Rendering::Vulkan::Renderers
{

    class MainVulkanRenderer : public VulkanRenderer
    {
    private:
        struct UniformData
        {
            /* Vertex Uniform Data */
            Matrix4x4 view;
            Matrix4x4 projection;
        };

        struct ObjectData
        {
            Matrix4x4 model;
        };

        struct StorageData
        {
            ObjectData objectDatas[MAX_MESHES];

            DirectionalLight::ShaderDirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
            PointLight::ShaderPointLight pointLights[MAX_POINT_LIGHTS];

            uint directionalLightCount;
            uint pointLightCount;
            float _align1_;
            float _align2_;
        };

        struct SkyboxPushConstant
        {
            glm::mat4x4 model;
        };

        typedef Pipeline<MeshPushConstant, UniformData, StorageData> ScenePipeline;
        typedef Pipeline<SkyboxPushConstant, UniformData, Abstractions::NullStorageBuffer> SkyboxPipeline;

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