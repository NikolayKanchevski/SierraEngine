//
// Created by Nikolay Kanchevski on 23.12.22.
//

#pragma once

#include "../Abstractions/VulkanRenderer.h"
#include "../Abstractions/OffscreenRenderer.h"
#include "../../../../Engine/Classes/Mesh.h"
#include "../../../../Engine/Components/Lighting/PointLight.h"
#include "../../../../Engine/Components/Lighting/DirectionalLight.h"
#include "../Abstractions/Cubemap.h"

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

            uint directionalLightCount;
            uint pointLightCount;
            float _align1_;
            float _align2_;
        };

        struct SkyboxUniformData
        {
            Matrix4x4 view;
            Matrix4x4 projection;
            Matrix4x4 model;
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
        };


        typedef Pipeline<Abstractions::NullPushConstant, SkyboxUniformData, Abstractions::NullStorageBuffer> SKYBOX_PIPELINE;
        typedef Pipeline<MeshPushConstant, UniformData, StorageData> OFFSCREEN_PIPELINE;

    public:
        /* --- CONSTRUCTORS --- */
        MainVulkanRenderer(const VulkanRendererCreateInfo &createInfo);
        static UniquePtr<MainVulkanRenderer> Create(VulkanRendererCreateInfo createInfo);

        /* --- POLLING METHODS --- */
        void Update() override;
        void Render() override;

        /* --- SETTER METHODS --- */
        void SetShadingType(const ShadingType newShadingType) override;
        void SetSampling(const Sampling newSampling) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline UniquePtr<OFFSCREEN_PIPELINE>& GetScenePipeline() { return scenePipeline; }
        [[nodiscard]] inline VkDescriptorSet GetRenderedTextureDescriptorSet() const { return offscreenImageDescriptorSets[swapchain->GetCurrentFrameIndex()]; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        void InitializeOffscreenRendering();
        void TerminateOffscreenRendering();

        void CreateSkyboxRenderingObjects();
        void CreateSceneRenderingObjects();

        void CreateTimestampQueries();
        void CreateOffscreenDescriptorSets();

        SharedPtr<DescriptorSetLayout> sceneDescriptorSetLayout;
        UniquePtr<OffscreenRenderer> sceneOffscreenRenderer;
        UniquePtr<OFFSCREEN_PIPELINE> scenePipeline;

        SharedPtr<DescriptorSetLayout> skyboxDescriptorSetLayout;
        UniquePtr<Mesh> skyboxMesh;
        UniquePtr<SKYBOX_PIPELINE> skyboxPipeline;
        UniquePtr<Cubemap> skyboxCubemap;

        std::vector<VkDescriptorSet> offscreenImageDescriptorSets;
        std::vector<UniquePtr<TimestampQuery>> offscreenTimestampQueries;

    };

}