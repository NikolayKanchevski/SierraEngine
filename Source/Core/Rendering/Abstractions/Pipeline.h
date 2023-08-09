//
// Created by Nikolay Kanchevski on 18.05.23.
//

#pragma once

#include "Shader.h"
#include "Descriptors.h"
#include "CommandBuffer.h"

namespace Sierra::Rendering
{

    struct PushConstantData
    {
        uint32 size;
        ShaderType shaderStages;
    };

    struct CompiledPipelineShaderInfo
    {
        uint32 descriptorSetCount = 1;
        Optional<PushConstantData> pushConstantData = std::nullopt;
        UniquePtr<DescriptorSetLayout>* descriptorSetLayout = nullptr;
    };

    struct PipelineCreateInfo
    {
        uint32 maxConcurrentFrames;
        std::vector<SharedPtr<Shader>> shaders;
        Optional<CompiledPipelineShaderInfo> shaderInfo;
    };

    class Pipeline
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Pipeline(const PipelineCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Begin(const UniquePtr<CommandBuffer> &commandBuffer);
        void BindDescriptorSet(const UniquePtr<CommandBuffer> &commandBuffer, const UniquePtr<DescriptorSet> &givenDescriptorSet, uint32 offset = 0) const;
        void BindDescriptorSets(const UniquePtr<CommandBuffer> &commandBuffer, const std::vector<ReferenceWrapper<UniquePtr<DescriptorSet>>> &givenDescriptorSets, uint32 offset = 0) const;
        void BindDescriptorSet(const UniquePtr<CommandBuffer> &commandBuffer, const UniquePtr<PushDescriptorSet> &givenDescriptorSet) const;
        void BindDescriptorSets(const UniquePtr<CommandBuffer> &commandBuffer, const std::vector<ReferenceWrapper<UniquePtr<PushDescriptorSet>>> &givenDescriptorSets) const;
        void SetPushConstants(const UniquePtr<CommandBuffer> &commandBuffer, const void* data) const;
        template<typename T, ENABLE_IF(!IsPointer<T>::value && !IsSmartPointer<T>::value)>
        inline void SetPushConstants(const UniquePtr<CommandBuffer> &commandBuffer, const T &data) const { SetPushConstants(commandBuffer, &data); }
        void End([[maybe_unused]] const UniquePtr<CommandBuffer> &commandBuffer);

        /* --- SETTER METHODS --- */
        void SetShaderDefinition(ShaderType shaderType, ShaderDefinition definition);

        void SetSpecializationConstant(uint32 constantID, const void* data, uint32 range);
        template<typename T, ENABLE_IF(!IsPointer<T>::value && !IsSmartPointer<T>::value)>
        inline void SetSpecializationConstant(const uint32 constantID, T &value) { SetSpecializationConstant(constantID, &value, sizeof(T)); };

        void SetShaderBinding(uint32 binding, const UniquePtr<Buffer> &buffer, uint32 arrayIndex = 0, uint64 size = 0, uint64 offset = 0);
        void SetShaderBinding(uint32 binding, const UniquePtr<Image> &image, const UniquePtr<Sampler> &sampler, uint32 arrayIndex = 0);
        void SetShaderBinding(uint32 binding, const SharedPtr<Texture> &texture, uint32 arrayIndex = 0);
        void SetShaderBinding(uint32 binding, const SharedPtr<Cubemap> &cubemap, uint32 arrayIndex = 0);

        void SetShaderMember(const String &memberName, const UniquePtr<Buffer> &buffer, uint32 arrayIndex = 0);
        void SetShaderMember(const String &memberName, const UniquePtr<Image> &image, const UniquePtr<Sampler> &sampler, uint32 arrayIndex = 0, ImageLayout imageLayoutAtDrawTime = ImageLayout::SHADER_READ_ONLY_OPTIMAL);
        void SetShaderMember(const String &memberName, const SharedPtr<Texture> &texture, uint32 arrayIndex = 0);
        void SetShaderMember(const String &memberName, const SharedPtr<Cubemap> &cubemap, uint32 arrayIndex = 0);
        void SetShaderMember(const String &memberName, void* data, uint64 size = 0, uint64 offset = 0);
        template<typename T, ENABLE_IF(!IsPointer<T>::value && !IsSmartPointer<T>::value)>
        inline void SetShaderMember(const String &memberName, T &value, const uint32 offset = 0) { SetShaderMember(memberName, &value, sizeof(T), offset); }

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint64 GetShaderMemberMemorySize(const String &memberName);
        [[nodiscard]] std::unordered_set<uint32> GetShaderMemberBindings(const String &memberName);

        [[nodiscard]] inline VkPipeline GetVulkanPipeline() const { return vkPipeline; }
        [[nodiscard]] [[maybe_unused]] inline VkPipelineLayout GetVulkanPipelineLayout() const { return vkPipelineLayout; }

        /* --- DESTRUCTOR --- */
        virtual void Destroy();
        DELETE_COPY(Pipeline);

    protected:
        VkPipeline vkPipeline = VK_NULL_HANDLE;
        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        VkPipelineCache pipelineCache = VK_NULL_HANDLE;

        VkPipelineBindPoint bindPoint;
        void* pushConstantMemory = nullptr;
        VkPushConstantRange* pushConstantRange = nullptr;

        UniquePtr<DescriptorSetLayout>* descriptorSetLayout = nullptr;
        std::vector<UniquePtr<PushDescriptorSet>> descriptorSets;
        std::unordered_map<ShaderType, SharedPtr<Shader>> shaders;

        bool bound = false;

        uint32 currentFrame = 0;
        const uint32 MAX_CONCURRENT_FRAMES;

        bool usesPrecompiledShaders;
        void BindResources(const UniquePtr<CommandBuffer> &commandBuffer);
        String GetPipelineCacheFilePath() const;

    private:
        bool updatedDescriptorSetForFrame = false;
        uint32 descriptorSetCount;
        Shader::ShaderMembers shaderMembers;

        void CreatePipelineLayout();
        inline virtual void CreatePipeline() { };

        Hash GetPipelineCacheHash() const;
        ShaderMember& GetShaderMember(const String &memberName);
    };

}