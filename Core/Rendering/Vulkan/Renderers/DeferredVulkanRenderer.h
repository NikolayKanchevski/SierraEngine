//
// Created by Nikolay Kanchevski on 5.02.23.
//

#pragma once

#include "VulkanRenderer.h"

#include "../../Modules/Raycaster.h"
#include "../Abstractions/Queries.h"
#include "../Abstractions/DynamicRenderer.h"
#include "../Abstractions/GraphicsPipeline.h"

#include "../../../../Engine/Classes/Mesh.h"
#include "../../../../Engine/Components/Lighting/PointLight.h"
#include "../../../../Engine/Components/Lighting/DirectionalLight.h"

using namespace Sierra::Engine::Classes;
using namespace Sierra::Engine::Components;
using namespace Sierra::Core::Rendering::Modules;

namespace Sierra::Core::Rendering::Vulkan::Renderers
{
    class DeferredVulkanRenderer : public VulkanRenderer
    {
    private:
        struct UniformData
        {
            Matrix4x4 view;
            Matrix4x4 projection;
            Matrix4x4 inverseView;
            Matrix4x4 inverseProjection;

            float nearPlane;
            float farPlane;
            Vector2 _align_1;
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

        enum RendererOutput
        {
            RenderedImage = 0,
            PositionBuffer = 1,
            DiffuseBuffer = 2,
            SpecularBuffer = 3,
            ShininessBuffer = 4,
            NormalBuffer = 5,
            ShadowBuffer = 6,
            DepthBuffer = 7
        };

        struct CompositionPushConstant
        {
            Matrix4x4 skyboxModel = Matrix4x4(1.0f);
        };

    public:
        /* --- CONSTRUCTORS --- */
        DeferredVulkanRenderer(const VulkanRendererCreateInfo &createInfo);
        static UniquePtr<DeferredVulkanRenderer> Create(VulkanRendererCreateInfo createInfo);

        /* --- POLLING METHODS --- */
        void Update() override;
        void DrawUI() override;
        void Render() override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkDescriptorSet GetRenderedTextureDescriptorSet() const override { return renderedImageDescriptorSets[swapchain->GetCurrentFrameIndex()]; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        // Global resources
        std::vector<UniquePtr<Buffer>> uniformBuffers;
        std::vector<UniquePtr<Buffer>> storageBuffers;

        // G-Buffer pipeline resources
        UniquePtr<Image> IDBuffer;
        UniquePtr<Image> diffuseBuffer;
        UniquePtr<Image> specularAndShininessBuffer;
        UniquePtr<Image> normalBuffer;
        UniquePtr<Image> depthStencilBuffer;
        UniquePtr<Image> renderedImage;
        UniquePtr<Sampler> bufferSampler;

        UniquePtr<DynamicRenderer> bufferPass;
        UniquePtr<GraphicsPipeline> bufferPipeline;
        UniquePtr<DescriptorSetLayout> bufferDescriptorSetLayout;

        // Composition pipeline resources
        UniquePtr<Cubemap> skyboxCubemap;

        CompositionPushConstant compositionPushConstantData{};
        UniquePtr<DynamicRenderer> compositionPass;
        UniquePtr<GraphicsPipeline> compositionPipeline;
        UniquePtr<DescriptorSetLayout> compositionDescriptorSetLayout;

        // Modules
        UniquePtr<Raycaster> raycaster;

        // Rendered image set & timestamp queries
        std::vector<VkDescriptorSet> renderedImageDescriptorSets;
        std::vector<UniquePtr<TimestampQuery>> renderTimestampQueries;

        void InitializeRenderer();
    };

}