//
// Created by Nikolay Kanchevski on 23.12.22.
//

#pragma once

#include "../Abstractions/VulkanRenderer.h"
#include "../Abstractions/OffscreenRenderer.h"
#include "../../../../Engine/Classes/Mesh.h"
#include "../../../../Engine/Components/Lighting/PointLight.h"
#include "../../../../Engine/Components/Lighting/DirectionalLight.h"

#define OFFSCREEN_PIPELINE Pipeline<MeshPushConstant, UniformData, StorageData>

using Sierra::Engine::Classes::MeshPushConstant;
using namespace Sierra::Engine::Components;

namespace Sierra::Core::Rendering::Vulkan::Renderers
{

    class MainVulkanRenderer : public VulkanRenderer
    {
    private:
        struct UniformData
        {
            /* Vertex Uniform Data */
            glm::mat4x4 view;
            glm::mat4x4 projection;

            uint32_t directionalLightCount;
            uint32_t pointLightCount;
            float _align1_;
            float _align2_;
        };

        struct ObjectData
        {
            glm::mat4x4 model;
        };

        struct StorageData
        {
            ObjectData objectDatas[MAX_MESHES];
            DirectionalLight::ShaderDirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
            PointLight::ShaderPointLight pointLights[MAX_POINT_LIGHTS];
        };

    public:
        /* --- CONSTRUCTORS --- */
        MainVulkanRenderer(const VulkanRendererCreateInfo &createInfo);
        static std::unique_ptr<MainVulkanRenderer> Create(VulkanRendererCreateInfo createInfo);

        /* --- POLLING METHODS --- */
        void Update() override;
        void Render() override;

        /* --- SETTER METHODS --- */
        void SetShadingType(const ShadingType newShadingType) override;
        void SetSampling(const Sampling newSampling) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline std::unique_ptr<OFFSCREEN_PIPELINE>& GetPipeline() { return offscreenGraphicsPipeline; }
        [[nodiscard]] inline VkDescriptorSet GetRenderedTextureDescriptorSet() const { return offscreenImageDescriptorSets[swapchain->GetCurrentFrameIndex()]; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        void InitializeOffscreenRendering();
        void CreateOffscreenDescriptorSets();
        void TerminateOffscreenRendering();

        std::shared_ptr<DescriptorSetLayout> descriptorSetLayout;
        std::unique_ptr<OffscreenRenderer> offscreenRenderer;
        std::vector<VkDescriptorSet> offscreenImageDescriptorSets;
        std::unique_ptr<OFFSCREEN_PIPELINE> offscreenGraphicsPipeline;
        std::vector<std::unique_ptr<TimestampQuery>> offscreenTimestampQueries;

    };

}