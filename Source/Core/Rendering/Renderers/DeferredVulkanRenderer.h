//
// Created by Nikolay Kanchevski on 5.02.23.
//

#pragma once

#include "VulkanRenderer.h"

#include "../RenderingSettings.h"
#include "../Modules/Raycaster.h"
#include "../Abstractions/Queries.h"
#include "../Abstractions/GraphicsPipeline.h"

namespace Sierra::Rendering
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
            Vector2 _align1_;
        };

        struct ObjectData
        {
            Matrix4x4 modelMatrix;
            Matrix4x4 normalMatrix; // A 4x4 matrix instead of a 3x3 for manual memory alignment
        };

        struct StorageData
        {
            ObjectData objectData[MAX_MESHES];

            struct {
                Matrix4x4 viewSpaceMatrix;

                Vector3 direction;
                float intensity;

                Vector3 color;
                float _align1_;
            } directionalLights[MAX_DIRECTIONAL_LIGHTS];

            struct {
                Matrix4x4 viewSpaceMatrix;

                Vector3 color;
                float intensity;

                Vector3 position;
                float linear;

                float quadratic;
                Vector3 _align1_;
            } pointLights[MAX_POINT_LIGHTS];

            uint32 directionalLightCount;
            uint32 pointLightCount;

            float _align1_;
            float _align2_;
        };

        struct BufferPushConstant
        {
            struct {
                Vector3 diffuse;
                float specular;

                float shininess;
                float vertexExaggeration;
                Vector2 _align1_;
            } material;

            uint32 meshID = 0;
            uint32 entityID = 0;
            uint32 meshTexturesPresence = 0;
            float _align1_;
        };

        enum RendererOutput
        {
            RenderedImage = 0,
            PositionBuffer = 1,
            DiffuseBuffer = 2,
            SpecularBuffer = 3,
            ShininessBuffer = 4,
            NormalBuffer = 5,
            DepthBuffer = 6
        };

        struct CompositionPushConstant
        {
            Matrix4x4 skyboxModel = Matrix4x4(1.0f);
        };

    public:
        /* --- CONSTRUCTORS --- */
        explicit DeferredVulkanRenderer(const VulkanRendererCreateInfo &createInfo);
        static UniquePtr<DeferredVulkanRenderer> Create(const VulkanRendererCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Update() override;
        void DrawUI() override;
        void Render() override;

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        // Global resources
        std::vector<UniquePtr<CommandBuffer>> commandBuffers;
        std::vector<UniquePtr<Buffer>> uniformBuffers;
        std::vector<UniquePtr<Buffer>> storageBuffers;
        UniquePtr<RenderPass> deferredRenderPass;
        UniquePtr<Framebuffer> deferredFramebuffer;

        // Early depth pass resources
        UniquePtr<GraphicsPipeline> earlyDepthPassPipeline;
        UniquePtr<DescriptorSetLayout> earlyDepthPassDescriptorSetLayout;

        // G-Buffer pipeline resources
        UniquePtr<Image> IDBuffer;
        UniquePtr<Image> diffuseBuffer;
        UniquePtr<Image> specularAndShininessBuffer;
        UniquePtr<Image> normalBuffer;
        UniquePtr<Image> depthBuffer;
        UniquePtr<Image> compositionImage;
        UniquePtr<Sampler> bufferSampler;
        UniquePtr<GraphicsPipeline> bufferPipeline;
        UniquePtr<DescriptorSetLayout> bufferDescriptorSetLayout;

        // Composition pipeline resources
        CompositionPushConstant compositionPushConstantData{};
        UniquePtr<GraphicsPipeline> compositionPipeline;
        UniquePtr<DescriptorSetLayout> compositionDescriptorSetLayout;

        // Modules
        UniquePtr<Modules::Raycaster> raycaster;

        // UI-Related resources
        std::vector<VkDescriptorSet> renderedImageDescriptorSets;
        std::vector<UniquePtr<TimestampQuery>> renderTimestampQueries;

        void InitializeRenderer();
    };

}