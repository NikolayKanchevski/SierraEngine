//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "Buffer.h"
#include "Cubemap.h"
#include "../Types.h"

namespace Sierra::Rendering
{
    struct DescriptorSetLayoutBinding
    {
        DescriptorType descriptorType;
        uint32 arraySize = 1;
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
        explicit DescriptorSetLayout(const DescriptorSetLayoutCreateInfo &createInfo);
        static UniquePtr<DescriptorSetLayout> Create(const DescriptorSetLayoutCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool IsBindingPresent(const uint32 binding) const { return bindings.count(binding) != 0; }
        [[nodiscard]] inline DescriptorType GetDescriptorTypeForBinding(const uint32 binding) const { return bindings.find(binding)->second.descriptorType; }
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
        uint32 multiplier = 512;
    };

    class DescriptorPool
    {
    public:
        /* --- CONSTRUCTOR --- */
        explicit DescriptorPool(const DescriptorPoolCreateInfo &createInfo);
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
        explicit DescriptorSet(const DescriptorSetCreateInfo &createInfo);
        [[nodiscard]] static UniquePtr<DescriptorSet> Create(const DescriptorSetCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool IsAllocated() const { return allocated; }

        /* --- SETTER METHODS --- */
        DescriptorSet* SetBuffer(uint32 binding, const UniquePtr<Buffer> &buffer, uint64 offset = 0, uint64 range = 0, uint32 arrayIndex = 0);
        DescriptorSet* SetImage(uint32 binding, const UniquePtr<Image> &image, const UniquePtr<Sampler> &sampler, ImageLayout imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL, uint32 arrayIndex = 0);
        DescriptorSet* SetTexture(uint32 binding, const SharedPtr<Texture> &texture, uint32 arrayIndex = 0);
        DescriptorSet* SetCubemap(uint32 binding, const SharedPtr<Cubemap> &cubemap, uint32 arrayIndex = 0);
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
        void SetVulkanImage(uint32 binding, const VkDescriptorImageInfo *imageInfo, uint32 arrayIndex);
    };

    struct PushDescriptorSetCreateInfo
    {
        uint32 descriptorSetIndex = 0;
    };

    class PushDescriptorSet
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit PushDescriptorSet([[maybe_unused]] const PushDescriptorSetCreateInfo &createInfo);
        static UniquePtr<PushDescriptorSet> Create([[maybe_unused]] const PushDescriptorSetCreateInfo &createInfo);

        /* --- SETTER METHODS --- */
        PushDescriptorSet* SetBuffer(uint32 binding, const UniquePtr<Buffer> &buffer, uint32 arrayIndex = 0, uint64 size = 0, uint64 offset = 0);
        PushDescriptorSet* SetImage(uint32 binding, const UniquePtr<Image> &image, const UniquePtr<Sampler> &sampler, uint32 arrayIndex = 0, DescriptorType descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER);
        PushDescriptorSet* SetTexture(uint32 binding, const SharedPtr<Texture> &texture, uint32 arrayIndex = 0);
        PushDescriptorSet* SetCubemap(uint32 binding, const SharedPtr<Cubemap> &cubemap, uint32 arrayIndex = 0);

        /* --- GETTER METHODS --- */
        bool IsEmpty() const { return writeDescriptorSets.empty(); }
        [[nodiscard]] inline uint32 GetDescriptorSetIndex() const { return descriptorSetIndex; }

        /* --- DESTRUCTOR --- */
        friend class Pipeline;
        DELETE_COPY(PushDescriptorSet);

    private:
        uint32 descriptorSetIndex = 0;
        // Binding | Data
        std::unordered_map<uint, VkWriteDescriptorSet> writeDescriptorSets;
        std::unordered_map<uint, VkDescriptorImageInfo> descriptorImageInfos;
        std::unordered_map<uint, VkDescriptorBufferInfo> descriptorBufferInfos;
        void SetVulkanImage(uint32 binding, const VkDescriptorImageInfo *imageInfo, uint32 arrayIndex, DescriptorType descriptorType);

    };
}
