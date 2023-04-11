//
// Created by Nikolay Kanchevski on 25.02.23.
//

#pragma once

#include "../RenderingTemplates.h"
#include "Abstractions/Pipelines.h"
#include "Abstractions/DynamicRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{
    class ShadowMapRenderer
    {
    public:
        /* --- CONSTRUCTORS --- */
        template<typename PC>
        inline ShadowMapRenderer(const UniquePtr<GraphicsPipeline<PC, UniformData, StorageData>> &pipelineToCopyFrom)
        {
            /* --- General Shadow Mapping Objects --- */

            sampler = Sampler::Create({
                .applyBilinearFiltering = true,
                .enableAnisotropy = false,
                .addressMode = SamplerAddressMode::CLAMP_TO_BORDER,
                .borderColor = SamplerBorderColor::FLOAT_OPAQUE_WHITE,
                .compareOp = SamplerCompareOp::LESS
            });

            shadowMap = Image::Create({
                .dimensions = { SHADOW_RESOLUTION, SHADOW_RESOLUTION },
                .format = VK::GetDevice()->GetBestDepthImageFormat(),
                .usage = ImageUsage::DEPTH_STENCIL_ATTACHMENT | ImageUsage::SAMPLED
            });

            shadowRenderer = DynamicRenderer::Create({
                .attachments = {
                    {
                        .image = shadowMap,
                        .loadOp = LoadOp::CLEAR,
                        .storeOp = StoreOp::STORE
                    }
                }
            });

            auto vertexShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Shadows/ShadowMapping.vert", .shaderType = ShaderType::VERTEX });

            shadowPipeline = ShadowPipeline::CreateFromAnotherPipeline({
                .shaders = { vertexShader },
                .dynamicRenderingInfo = GraphicsPipelineDynamicRenderingInfo {
                    .attachments = { { shadowMap } }
                },
                .cullMode = CullMode::BACK
            }, pipelineToCopyFrom, PipelineCopyOp::STORAGE_BUFFERS);

            /* --- Variance Shadow Mapping Objects --- */

            varianceMap = Image::Create({
                .dimensions = { SHADOW_RESOLUTION, SHADOW_RESOLUTION },
                .format = ImageFormat::R16G16_UNORM,
                .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
            });

            varianceRenderer = DynamicRenderer::Create({
                .attachments = {
                    {
                        .image = varianceMap,
                        .loadOp = LoadOp::CLEAR,
                        .storeOp = StoreOp::STORE
                    },
                    {
                        .image = shadowMap,
                        .loadOp = LoadOp::CLEAR,
                        .storeOp = StoreOp::DONT_CARE
                    }
                }
            });

            auto varianceFragmentShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Shadows/VarianceShadowMapping.frag", .shaderType = ShaderType::FRAGMENT });

            variancePipeline = ShadowPipeline::CreateFromAnotherPipeline({
                .shaders = { vertexShader, varianceFragmentShader },
                .dynamicRenderingInfo = GraphicsPipelineDynamicRenderingInfo {
                    .attachments = { { varianceMap }, { shadowMap } }
                }
            }, pipelineToCopyFrom, PipelineCopyOp::STORAGE_BUFFERS);

            /* --- Moment Shadow Mapping Objects --- */

            momentMap = Image::Create({
                .dimensions = { SHADOW_RESOLUTION, SHADOW_RESOLUTION },
                .format = ImageFormat::R16G16B16A16_SFLOAT,
                .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
            });

            momentRenderer = DynamicRenderer::Create({
                .attachments = {
                    {
                        .image = momentMap,
                        .loadOp = LoadOp::CLEAR,
                        .storeOp = StoreOp::STORE
                    },
                    {
                        .image = shadowMap,
                        .loadOp = LoadOp::CLEAR,
                        .storeOp = StoreOp::DONT_CARE
                    }
                }
            });

            auto momentFragmentShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Shadows/MomentShadowMapping.frag", .shaderType = ShaderType::FRAGMENT });

            momentPipeline = ShadowPipeline::CreateFromAnotherPipeline({
                .shaders = { vertexShader, momentFragmentShader },
                .dynamicRenderingInfo = GraphicsPipelineDynamicRenderingInfo {
                    .attachments = { { momentMap }, { shadowMap } }
                }
            }, pipelineToCopyFrom, PipelineCopyOp::STORAGE_BUFFERS);

            /* --- Shadow Blurring Objects --- */

            horizontalBlurImage = Image::Create({
                .dimensions = { SHADOW_RESOLUTION, SHADOW_RESOLUTION },
                .format = ImageFormat::R16G16_UNORM,
                .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
            });

            finalBlurImage = Image::Create({
                .dimensions = { SHADOW_RESOLUTION, SHADOW_RESOLUTION },
                .format = ImageFormat::R16G16_UNORM,
                .usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED
            });

            blurRenderer = DynamicRenderer::Create({
                .attachments = {
                    {
                        .image = horizontalBlurImage,
                        .loadOp = LoadOp::CLEAR,
                        .storeOp = StoreOp::STORE
                    }
                }
            });

            auto blurVertexShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Post-Processing/Post-Processing.vert", .shaderType = ShaderType::VERTEX });
            auto blurFragmentShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Post-Processing/GaussianBlur.frag", .shaderType = ShaderType::FRAGMENT });

            blurPipeline = BlurPipeline::Create({
                .shaders = { blurVertexShader, blurFragmentShader },
                .dynamicRenderingInfo = GraphicsPipelineDynamicRenderingInfo {
                    .attachments = {
                        {horizontalBlurImage }
                    }
                },
                .manuallyHandleDescriptorSets = true
            });

            horizontalBlurDescriptorSet = DescriptorSet::Create(blurPipeline->GetDescriptorSetLayout());
            horizontalBlurDescriptorSet->WriteImage(2, momentMap, sampler)->Allocate();

            finalBlurDescriptorSet = DescriptorSet::Create(blurPipeline->GetDescriptorSetLayout());
            finalBlurDescriptorSet->WriteImage(2, horizontalBlurImage, sampler)->Allocate();
        }

        /* --- POLLING METHODS --- */
        void Update();
        void Render(const UniquePtr<CommandBuffer> &commandBuffer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline UniquePtr<Image>& GetShadowMap() { return shadowMap; }
        [[nodiscard]] inline UniquePtr<Sampler>& GetShadowMapSampler() { return sampler; }

        /* --- DESTRUCTORS --- */
        void Destroy();
        DELETE_COPY(ShadowMapRenderer);

    private:
        struct LightPushConstant
        {
            uint meshID;
            uint directionalLightID;
        };

        enum BlurDirection
        {
            HORIZONTAL = 0,
            VERTICAL = 1
        };

        struct BlurPushConstant
        {
            BlurDirection blurDirection;
        };

        typedef GraphicsPipeline<LightPushConstant, NullType, StorageData> ShadowPipeline;
        typedef GraphicsPipeline<BlurPushConstant, NullType, NullType> BlurPipeline;

        const uint SHADOW_RESOLUTION = 1024;

        UniquePtr<Image> shadowMap;
        UniquePtr<DynamicRenderer> shadowRenderer;
        UniquePtr<ShadowPipeline> shadowPipeline;

        UniquePtr<Image> varianceMap;
        UniquePtr<DynamicRenderer> varianceRenderer;
        UniquePtr<ShadowPipeline> variancePipeline;

        UniquePtr<Image> momentMap;
        UniquePtr<DynamicRenderer> momentRenderer;
        UniquePtr<ShadowPipeline> momentPipeline;

        UniquePtr<Image> horizontalBlurImage;
        UniquePtr<Image> finalBlurImage;

        UniquePtr<DescriptorSet> horizontalBlurDescriptorSet;
        UniquePtr<DescriptorSet> finalBlurDescriptorSet;

        UniquePtr<DynamicRenderer> blurRenderer;
        UniquePtr<BlurPipeline> blurPipeline;

        UniquePtr<Sampler> sampler;
    };

    class GridRenderer
    {
    public:

        /* --- CONSTRUCTORS --- */
        template<typename PC, typename SB>
        inline GridRenderer(UniquePtr<Image>& givenOutputImage, const UniquePtr<GraphicsPipeline<PC, UniformData, SB>> &pipelineToCopyFrom, const bool clearImage)
            : outputImage(givenOutputImage)
        {
            // Create dynamic renderer
            renderer = DynamicRenderer::Create({
                .attachments = {
                    { .image = outputImage, .loadOp = clearImage ? LoadOp::CLEAR : LoadOp::LOAD, .storeOp = StoreOp::STORE }
                }
            });

            // Load shaders
            auto vertexShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Grid/Grid.vert", .shaderType = ShaderType::VERTEX });
            auto fragmentShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Grid/Grid.frag", .shaderType = ShaderType::FRAGMENT });

            // Create pipeline
            pipeline = PipelineType::CreateFromAnotherPipeline({
                .maxConcurrentFrames = pipelineToCopyFrom->GetMaxConcurrentFrames(),
                .shaders = { vertexShader, fragmentShader },
                .dynamicRenderingInfo = GraphicsPipelineDynamicRenderingInfo {
                    .attachments = { { outputImage } }
                }
            }, pipelineToCopyFrom, PipelineCopyOp::UNIFORM_BUFFERS);
        }

        /* --- POLLING METHODS --- */
        void RenderGrid(const UniquePtr<CommandBuffer> &commandBuffer);

        /* --- DESTRUCTOR --- */
        void Destroy();

    private:
        UniquePtr<Image>& outputImage;
        UniquePtr<DynamicRenderer> renderer;

        typedef GraphicsPipeline<NullType, UniformData, NullType> PipelineType;
        UniquePtr<PipelineType> pipeline;

    };

    class Raycaster
    {
    public:

        /* --- CONSTRUCTORS --- */
        template<typename PC, typename SB>
        inline Raycaster(const UniquePtr<Image> &IDImage, const UniquePtr<Image> &depthImage, const UniquePtr<GraphicsPipeline<PC, UniformData, SB>> &pipelineToCopyFrom)
        {
            // Create buffer to hold output data
            dataBuffer = Buffer::Create({
                .memorySize = sizeof(WriteBuffer),
                .bufferUsage = BufferUsage::STORAGE
            });

            // Write arbitrary memory to buffer, so that we do not read garbage memory later on
            dataBuffer->CopyFromPointer(&data);

            // Load raycast shader
            SharedPtr<Shader> raycastShader = Shader::Create({
                .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Compute/Raycast.comp",
                .shaderType = ShaderType::COMPUTE
            });

            // Create pipeline
            computePipeline = ComputePipeline<PushConstant, UniformData, NullType>::CreateFromAnotherPipeline({
                .shader = raycastShader,
            }, pipelineToCopyFrom, PipelineCopyOp::UNIFORM_BUFFERS);

            // Write data to pipeline
            computePipeline->GetDescriptorSet()
                ->WriteImage(1, IDImage)
                ->WriteImage(2, depthImage)
                ->WriteBuffer(3, dataBuffer)
            ->Allocate();
        }

        /* --- POLLING METHODS --- */
        void UpdateData(const UniquePtr<CommandBuffer> &commandBuffer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline entt::entity GetHoveredEntityID() const { return data.entityID == 0 ? entt::null : static_cast<entt::entity>(data.entityID); }
        [[nodiscard]] inline Vector3 GetHoveredWorldPosition() const { return data.worldPosition; }

        /* --- DESTRUCTOR --- */
        void Destroy();

    private:
        struct PushConstant
        {
            Vector2 mousePosition;
        };

        struct WriteBuffer
        {
            Vector3 worldPosition;
            uint entityID;
        };

        WriteBuffer data{};
        UniquePtr<Buffer> dataBuffer;
        UniquePtr<ComputePipeline<PushConstant, UniformData, NullType>> computePipeline;
    };

    namespace RenderingUtilities
    {
        template<typename PC>
        inline static UniquePtr<ShadowMapRenderer> CreateShadowMapRenderer(const UniquePtr<GraphicsPipeline<PC, UniformData, StorageData>> &pipelineToCopyFrom)
        {
            return std::make_unique<ShadowMapRenderer>(pipelineToCopyFrom);
        }

        template<typename PC, typename SB>
        inline static UniquePtr<GridRenderer> CreateGridRenderer(UniquePtr<Image>& outputImage, const UniquePtr<GraphicsPipeline<PC, UniformData, SB>> &pipelineToCopyFrom, const bool clearImage = false)
        {
            return std::make_unique<GridRenderer>(outputImage, pipelineToCopyFrom, clearImage);
        }

        template<typename PC, typename SB>
        inline static UniquePtr<Raycaster> CreateRaycaster(const UniquePtr<Image> &IDImage, const UniquePtr<Image> &depthImage, const UniquePtr<GraphicsPipeline<PC, UniformData, SB>> &pipelineToCopyFrom)
        {
            return std::make_unique<Raycaster>(IDImage, depthImage, pipelineToCopyFrom);
        }
    }

}