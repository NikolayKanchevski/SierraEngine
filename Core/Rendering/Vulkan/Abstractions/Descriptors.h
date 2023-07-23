//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "Buffer.h"
#include "Cubemap.h"
#include "../VulkanTypes.h"

namespace Sierra::Rendering
{
    struct DescriptorSetLayoutBinding
    {
        DescriptorType descriptorType;
        uint arraySize = 1;
        ShaderType shaderStages;
    };

    struct DescriptorSetLayoutCreateInfo
    {
        std::unordered_map<uint, DescriptorSetLayoutBinding> bindings;
        DescriptorSetLayoutFlag flags = DescriptorSetLayoutFlag::NONE;
    };

    class DescriptorSetLayout
    {
    public:
        /* --- CONSTRUCTORS --- */
        DescriptorSetLayout(const DescriptorSetLayoutCreateInfo &createInfo);
        static UniquePtr<DescriptorSetLayout> Create(const DescriptorSetLayoutCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool IsBindingPresent(const uint binding) const { return bindings.count(binding) != 0; }
        [[nodiscard]] inline DescriptorType GetDescriptorTypeForBinding(const uint binding) const { return bindings.find(binding)->second.descriptorType; }
        [[nodiscard]] inline VkDescriptorSetLayout GetVulkanDescriptorSetLayout() const { return vkDescriptorSetLayout; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        friend class DescriptorSet;
        DELETE_COPY(DescriptorSetLayout);

    private:
        VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
        std::unordered_map<uint, DescriptorSetLayoutBinding> bindings;

    };

    struct DescriptorPoolCreateInfo
    {
        uint multiplier = 512;
    };

    class DescriptorPool
    {
    public:
        /* --- CONSTRUCTOR --- */
        DescriptorPool(const DescriptorPoolCreateInfo &createInfo);
        static UniquePtr<DescriptorPool> Create(const DescriptorPoolCreateInfo &createInfo);

        /* --- SETTER METHODS --- */
        static void ResetPools();
        static void DisposePools();

        /* --- GETTER METHODS --- */
        static UniquePtr<DescriptorPool>& GetFreeDescriptorPool();
        [[nodiscard]] inline VkDescriptorPool GetVulkanDescriptorPool() const { return vkDescriptorPool; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        friend class DescriptorSet;
        DELETE_COPY(DescriptorPool);

    private:
        VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;

        static void AllocateDescriptorSet(const UniquePtr<DescriptorSetLayout> &descriptorSetLayout, VkDescriptorSet &descriptorSet);
        inline const static std::vector<std::pair<DescriptorType, float>> DEFAULT_POOL_SIZES =
        {
            { DescriptorType::SAMPLER,                  0.5f },
            { DescriptorType::COMBINED_IMAGE_SAMPLER,   4.0f },
            { DescriptorType::SAMPLED_IMAGE,            4.0f },
            { DescriptorType::STORAGE_IMAGE,            1.0f },
            { DescriptorType::UNIFORM_TEXEL_BUFFER,     1.0f },
            { DescriptorType::STORAGE_TEXEL_BUFFER,     1.0f },
            { DescriptorType::UNIFORM_BUFFER,           2.0f },
            { DescriptorType::STORAGE_BUFFER,           2.0f },
            { DescriptorType::UNIFORM_BUFFER_DYNAMIC,   1.0f },
            { DescriptorType::STORAGE_BUFFER_DYNAMIC,   1.0f },
            { DescriptorType::INPUT_ATTACHMENT,         0.5f }
        };

        static inline VkDescriptorPool currentPool = VK_NULL_HANDLE;
        inline static std::vector<UniquePtr<DescriptorPool>> usedPools;
        inline static std::vector<UniquePtr<DescriptorPool>> freePools;

    };

    struct DescriptorSetCreateInfo
    {
        const UniquePtr<DescriptorSetLayout> &descriptorSetLayout;
    };

    class DescriptorSet
    {
    public:
        /* --- CONSTRUCTORS --- */
        DescriptorSet(const DescriptorSetCreateInfo &createInfo);
        [[nodiscard]] static UniquePtr<DescriptorSet> Create(const DescriptorSetCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool IsAllocated() const { return allocated; }

        /* --- SETTER METHODS --- */
        DescriptorSet* SetBuffer(uint binding, const UniquePtr<Buffer> &buffer, uint64 offset = 0, uint64 range = 0, uint arrayIndex = 0);
        DescriptorSet* SetImage(uint binding, const UniquePtr<Image> &image, const UniquePtr<Sampler> &sampler = Sampler::Default, ImageLayout imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL, uint arrayIndex = 0);
        DescriptorSet* SetTexture(uint binding, const SharedPtr<Texture> &texture, uint arrayIndex = 0);
        DescriptorSet* SetCubemap(uint binding, const UniquePtr<Cubemap> &cubemap, uint arrayIndex = 0);
        void Allocate();

        /* --- GETTER METHODS --- */
        [[nodiscard]] VkDescriptorSet GetVulkanDescriptorSet() const { return vkDescriptorSet; }

        /* --- DESTRUCTOR --- */
        DELETE_COPY(DescriptorSet);

    private:
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
        const UniquePtr<DescriptorSetLayout> &descriptorSetLayout;

        // Binding | Data
        std::unordered_map<uint, VkWriteDescriptorSet> writeDescriptorSets;
        std::unordered_map<uint, VkDescriptorImageInfo> descriptorImageInfos;
        std::unordered_map<uint, VkDescriptorBufferInfo> descriptorBufferInfos;

        bool allocated = true;
        void SetVulkanImage(uint binding, const VkDescriptorImageInfo *imageInfo, uint arrayIndex);
    };

    struct PushDescriptorSetCreateInfo
    {
        uint descriptorSetIndex = 0;
    };

    class PushDescriptorSet
    {
    public:
        /* --- CONSTRUCTORS --- */
        PushDescriptorSet([[maybe_unused]] const PushDescriptorSetCreateInfo &createInfo);
        static UniquePtr<PushDescriptorSet> Create([[maybe_unused]] const PushDescriptorSetCreateInfo &createInfo);

        /* --- SETTER METHODS --- */
        PushDescriptorSet* SetBuffer(uint binding, const UniquePtr<Buffer> &buffer, uint arrayIndex = 0, uint64 size = 0, uint64 offset = 0);
        PushDescriptorSet* SetImage(uint binding, const UniquePtr<Image> &image, const UniquePtr<Sampler> &sampler = Sampler::Default, uint arrayIndex = 0, DescriptorType descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER);
        PushDescriptorSet* SetTexture(uint binding, const SharedPtr<Texture> &texture, uint arrayIndex = 0);
        PushDescriptorSet* SetCubemap(uint binding, const UniquePtr<Cubemap> &cubemap, uint arrayIndex = 0);

        /* --- GETTER METHODS --- */
        bool IsEmpty() const { return writeDescriptorSets.empty(); }
        [[nodiscard]] inline uint GetDescriptorSetIndex() const { return descriptorSetIndex; }

        /* --- DESTRUCTOR --- */
        friend class Pipeline;
        DELETE_COPY(PushDescriptorSet);

    private:
        uint descriptorSetIndex = 0;
        // Binding | Data
        std::unordered_map<uint, VkWriteDescriptorSet> writeDescriptorSets;
        std::unordered_map<uint, VkDescriptorImageInfo> descriptorImageInfos;
        std::unordered_map<uint, VkDescriptorBufferInfo> descriptorBufferInfos;
        void SetVulkanImage(uint binding, const VkDescriptorImageInfo *imageInfo, uint arrayIndex, DescriptorType descriptorType);

    };
}
