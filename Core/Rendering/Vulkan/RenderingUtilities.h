//
// Created by Nikolay Kanchevski on 25.02.23.
//

#pragma once

#include "../RenderingTemplates.h"
#include "Abstractions/Pipelines.h"

namespace Sierra::Core::Rendering::Vulkan
{

    class RenderingUtilities
    {
    private:
        struct IDPushConstant
        {
            Vector2 mousePosition;
        };

        struct IDWriteBuffer
        {
            Vector3 worldPosition;
            uint entityID;
        };

    public:
        /* --- CONSTRUCTORS --- */
        template<typename PC, typename SB>
        inline static void Initialize(const UniquePtr<Image> &IDImage, const UniquePtr<Image> &depthImage, const UniquePtr<GraphicsPipeline<PC, UniformData, SB>> &pipelineToCopyFrom)
        {
            instance.IDBuffer = Buffer::Create({
                .memorySize = sizeof(IDWriteBuffer),
                .memoryFlags = MemoryFlags::HOST_VISIBLE | MemoryFlags::HOST_COHERENT,
                .bufferUsage = BufferUsage::STORAGE
            });

            // Write arbitrary memory to buffer, so that we do not read garbage memory later on
            instance.IDBuffer->CopyFromPointer(&instance.IDData);

            instance.IDBufferDescriptorSetLayout = DescriptorSetLayout::Builder()
                .SetShaderStages(ShaderType::COMPUTE)
                .AddBinding(UNIFORM_BUFFER_BINDING, DescriptorType::UNIFORM_BUFFER)
                .AddBinding(1, DescriptorType::COMBINED_IMAGE_SAMPLER)
                .AddBinding(2, DescriptorType::COMBINED_IMAGE_SAMPLER)
                .AddBinding(3, DescriptorType::STORAGE_BUFFER)
            .Build();

            SharedPtr<Shader> raycastShader = Shader::Create({
                .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Compute/Raycast.comp",
                .shaderType = ShaderType::COMPUTE
            });

            instance.IDBufferDescriptorSet = DescriptorSet::Build(instance.IDBufferDescriptorSetLayout);

            instance.IDBufferComputePipeline = ComputePipeline<IDPushConstant, UniformData, NullType>::CreateFromAnotherPipeline({
               .shader = raycastShader,
               .descriptorSetLayout = instance.IDBufferDescriptorSetLayout
            }, pipelineToCopyFrom, PipelineCopyOp::UNIFORM_BUFFER);

            instance.IDBufferDescriptorSet->WriteImage(1, IDImage, Sampler::Default);
            instance.IDBufferDescriptorSet->WriteImage(2, depthImage, Sampler::Default, ImageLayout::SHADER_READ_ONLY_OPTIMAL);
            instance.IDBufferDescriptorSet->WriteBuffer(3, instance.IDBuffer);
            instance.IDBufferDescriptorSet->Allocate();
        }

        /* --- POLLING METHODS --- */
        static void UpdateIDBuffer(const UniquePtr<CommandBuffer> &commandBuffer);

        /* --- GETTER METHODS --- */
        inline static entt::entity GetHoveredEntityID() { return instance.IDData.entityID == 0 ? entt::null : static_cast<entt::entity>(instance.IDData.entityID); }
        inline static Vector3 GetHoveredWorldPosition() { return instance.IDData.worldPosition; }

        /* --- DESTRUCTOR --- */
        static void Destroy();

    private:
        RenderingUtilities() = default;
        static RenderingUtilities instance;

        IDWriteBuffer IDData{};
        UniquePtr<Buffer> IDBuffer;
        SharedPtr<DescriptorSetLayout> IDBufferDescriptorSetLayout;
        UniquePtr<DescriptorSet> IDBufferDescriptorSet;
        UniquePtr<ComputePipeline<IDPushConstant, UniformData, NullType>> IDBufferComputePipeline;
    };

}
