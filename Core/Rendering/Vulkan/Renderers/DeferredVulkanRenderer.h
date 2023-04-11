//
// Created by Nikolay Kanchevski on 5.02.23.
//

#pragma once

#include "VulkanRenderer.h"

#include "../RenderingUtilities.h"
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
            ShininessBuffer = 4,
            NormalBuffer = 5,
            ShadowBuffer = 6,
            DepthBuffer = 7
        };

        struct MergingRendererPushConstant
        {
            Matrix4x4 skyboxModel;
            Matrix4x4 lightSpaceMatrix;

            RenderedImageValue renderedImageValue = RenderedImageValue::RenderedImage;
            BoolGLSL enableShadows = true;
        };

        typedef GraphicsPipeline<MeshPushConstant, UniformData, StorageData> ScenePipeline;
        typedef GraphicsPipeline<MergingRendererPushConstant, UniformData, StorageData> CompositionPipeline;

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
        UniquePtr<Image> IDBuffer;
        UniquePtr<Image> diffuseBuffer;
        UniquePtr<Image> specularAndShininessBuffer;
        UniquePtr<Image> normalBuffer;
        UniquePtr<Image> depthStencilBuffer;
        UniquePtr<Image> renderedImage;

        UniquePtr<DynamicRenderer> bufferPass;
        UniquePtr<ScenePipeline> bufferPipeline;

        UniquePtr<Cubemap> skyboxCubemap;
        UniquePtr<DynamicRenderer> compositionPass;
        UniquePtr<CompositionPipeline> compositionPipeline;

        UniquePtr<ShadowMapRenderer> shadowRenderer;
        UniquePtr<Raycaster> raycaster;

        UniquePtr<Sampler> bufferSampler;
        std::vector<VkDescriptorSet> renderedImageDescriptorSets;
        std::vector<UniquePtr<TimestampQuery>> renderTimestampQueries;

        void InitializeRenderer();
    };

}