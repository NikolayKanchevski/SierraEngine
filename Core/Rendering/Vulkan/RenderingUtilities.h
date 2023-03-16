//
// Created by Nikolay Kanchevski on 25.02.23.
//

#pragma once

#include "../RenderingTemplates.h"
#include "Abstractions/Pipelines.h"

namespace Sierra::Core::Rendering::Vulkan
{

    class Raycaster
    {
    public:
        /* --- CONSTRUCTORS --- */

        template<typename PC, typename SB>
        Raycaster(const UniquePtr<Image> &IDImage, const UniquePtr<Image> &depthImage, const UniquePtr<GraphicsPipeline<PC, UniformData, SB>> &pipelineToCopyFrom)
        {
            dataBuffer = Buffer::Create({
                .memorySize = sizeof(WriteBuffer),
                .bufferUsage = BufferUsage::STORAGE
            });

            // Write arbitrary memory to buffer, so that we do not read garbage memory later on
            dataBuffer->CopyFromPointer(&data);

            SharedPtr<Shader> raycastShader = Shader::Create({
                .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Compute/Raycast.comp",
                .shaderType = ShaderType::COMPUTE
            });

            computePipeline = ComputePipeline<PushConstant, UniformData, NullType>::CreateFromAnotherPipeline({
                .shader = raycastShader,
            }, pipelineToCopyFrom, PipelineCopyOp::UNIFORM_BUFFERS);

            computePipeline->GetDescriptorSet()
                ->WriteImage(1, IDImage, Sampler::Default)
                ->WriteImage(2, depthImage, Sampler::Default)
                ->WriteBuffer(3, dataBuffer)
            ->Allocate();
        }

        /* --- POLLING METHODS --- */
        void UpdateData(const UniquePtr<CommandBuffer> &commandBuffer);

        /* --- GETTER METHODS --- */
        inline entt::entity GetHoveredEntityID() { return data.entityID == 0 ? entt::null : static_cast<entt::entity>(data.entityID); }
        inline Vector3 GetHoveredWorldPosition() { return data.worldPosition; }

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
        inline static UniquePtr<Raycaster> CreateRaycaster(const UniquePtr<Image> &IDImage, const UniquePtr<Image> &depthImage, const UniquePtr<GraphicsPipeline<PC, UniformData, SB>> &pipelineToCopyFrom)
        {
            return std::make_unique<Raycaster>(IDImage, depthImage, pipelineToCopyFrom);
        }
    }

}