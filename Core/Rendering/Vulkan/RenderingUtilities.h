//
// Created by Nikolay Kanchevski on 25.02.23.
//

#pragma once

#include "../RenderingTemplates.h"
#include "Abstractions/Pipelines.h"
#include "Abstractions/DynamicRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

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