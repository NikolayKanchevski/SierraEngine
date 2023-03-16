//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "Buffer.h"
#include "Cubemap.h"
#include "../VulkanTypes.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
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
    };

    class DescriptorSetLayout
    {
    public:
        /* --- CONSTRUCTORS --- */
        DescriptorSetLayout(const DescriptorSetLayoutCreateInfo &createInfo);
        static UniquePtr<DescriptorSetLayout> Create(DescriptorSetLayoutCreateInfo createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool IsBindingPresent(const uint binding) const { return bindings.count(binding) != 0; }
        [[nodiscard]] inline VkDescriptorSetLayout GetVulkanDescriptorSetLayout() const { return this->vkDescriptorSetLayout; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(DescriptorSetLayout);

    private:
        friend class DescriptorPool;
        friend class DescriptorSet;
        friend class BindlessDescriptorSet;

        VkDescriptorSetLayout vkDescriptorSetLayout;
        std::unordered_map<uint, DescriptorSetLayoutBinding> bindings;

    };

    class DescriptorPool
    {
    public:
        struct DescriptorPoolCreateInfo
        {
            uint multiplier = 512;
            VkDescriptorPoolCreateFlags flags = 0;
        };

        /* --- CONSTRUCTOR --- */
        DescriptorPool(const DescriptorPoolCreateInfo &givenCreateInfo);
        static UniquePtr<DescriptorPool> Create(DescriptorPoolCreateInfo givenCreateInfo);

        /* --- GETTER METHODS -- */
        static UniquePtr<DescriptorPool>& GetPool();

        /* --- SETTER METHODS --- */
        static void ResetPools();
        static void DisposePools();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkDescriptorPool GetVulkanDescriptorPool() const { return this->vkDescriptorPool; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(DescriptorPool);

    private:
        VkDescriptorPool vkDescriptorPool;
        DescriptorPoolCreateInfo createInfo;

        static void AllocateDescriptorSet(const UniquePtr<DescriptorSetLayout> &descriptorSetLayout, VkDescriptorSet &descriptorSet);
        static void AllocateBindlessDescriptorSet(const std::vector<uint> &bindings, const SharedPtr<DescriptorSetLayout> &descriptorSetLayout, VkDescriptorSet &descriptorSet);

        inline static std::vector<std::pair<DescriptorType, float>> defaultPoolSizes =
        {
            { DescriptorType::SAMPLER, 0.5f },
            { DescriptorType::COMBINED_IMAGE_SAMPLER, 4.0f },
            { DescriptorType::SAMPLED_IMAGE, 4.0f },
            { DescriptorType::STORAGE_IMAGE, 1.0f },
            { DescriptorType::UNIFORM_TEXEL_BUFFER, 1.0f },
            { DescriptorType::STORAGE_TEXEL_BUFFER, 1.0f },
            { DescriptorType::UNIFORM_BUFFER, 2.0f },
            { DescriptorType::STORAGE_BUFFER, 2.0f },
            { DescriptorType::UNIFORM_BUFFER_DYNAMIC, 1.0f },
            { DescriptorType::STORAGE_BUFFER_DYNAMIC, 1.0f },
            { DescriptorType::INPUT_ATTACHMENT, 0.5f }
        };

        static inline VkDescriptorPool currentPool = VK_NULL_HANDLE;
        inline static std::vector<UniquePtr<DescriptorPool>> usedPools;
        inline static std::vector<UniquePtr<DescriptorPool>> freePools;

        friend class DescriptorSet;
        friend class BindlessDescriptorSet;
    };

    class DescriptorSet
    {
    public:
        /* --- CONSTRUCTORS --- */
        DescriptorSet(const UniquePtr<DescriptorSetLayout> &descriptorSetLayout);
        [[nodiscard]] static UniquePtr<DescriptorSet> Create(const UniquePtr<DescriptorSetLayout> &givenDescriptorSetLayout);

        /* --- SETTER METHODS --- */
        DescriptorSet* WriteBuffer(uint binding, const UniquePtr<Buffer> &buffer);
        DescriptorSet* WriteImage(uint binding, const UniquePtr<Image> &image, const UniquePtr<Sampler> &sampler, ImageLayout imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL);
        DescriptorSet* WriteTexture(uint binding, const SharedPtr<Texture> &texture);
        DescriptorSet* WriteCubemap(uint binding, const UniquePtr<Cubemap> &cubemap);
        void Allocate();

        /* --- GETTER METHODS --- */
        [[nodiscard]] VkDescriptorSet GetVulkanDescriptorSet() const { return this->vkDescriptorSet; }

        /* --- DESTRUCTOR --- */
        ~DescriptorSet();
        DELETE_COPY(DescriptorSet);

    private:
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
        const UniquePtr<DescriptorSetLayout> &descriptorSetLayout;

        std::unordered_map<uint, VkWriteDescriptorSet> writeDescriptorSets;
        std::unordered_map<uint, VkDescriptorImageInfo> descriptorImageInfos;
        std::unordered_map<uint, VkDescriptorBufferInfo> descriptorBufferInfos;

        void WriteImage(uint binding, const VkDescriptorImageInfo *imageInfo);
    };

    class BindlessDescriptorSet
    {
    public:
        /* --- CONSTRUCTORS --- */
        BindlessDescriptorSet(const std::vector<uint> &givenBindings, SharedPtr<DescriptorSetLayout> &descriptorSetLayout);
        [[nodiscard]] static SharedPtr<BindlessDescriptorSet> Build(const std::vector<uint> &givenBindings, SharedPtr<DescriptorSetLayout> &givenDescriptorSetLayout);

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsBindingConfigured(uint binding) const;
        [[nodiscard]] uint GetFirstFreeIndex(uint binding) const;
        [[nodiscard]] bool IsIndexAllocated(uint binding, uint arrayIndex) const;

        /* --- SETTER METHODS --- */
        void FreeIndex(uint binding, uint arrayIndex, bool reallocate = true);
        uint ReserveIndex(uint binding, uint arrayIndex = -1);

        // TODO: Rewrite this
        uint WriteBuffer(uint binding, const UniquePtr<Buffer> &buffer, bool overwrite = false, uint arrayIndex = -1);
        uint WriteImage(uint binding, const VkDescriptorImageInfo *imageInfo, bool overwrite = false, uint arrayIndex = -1);
        uint WriteTexture(uint binding, const SharedPtr<Texture> &texture, bool overwrite = false, uint arrayIndex = -1);
        void Allocate();

        /* --- GETTER METHODS --- */
        [[nodiscard]] VkDescriptorSet GetVulkanDescriptorSet() const { return this->vkDescriptorSet; }

        /* --- DESTRUCTOR --- */
        DELETE_COPY(BindlessDescriptorSet);

    private:
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
        std::vector<uint> boundBindings;

        SharedPtr<DescriptorSetLayout> &descriptorSetLayout;

        std::vector<VkWriteDescriptorSet> writeDescriptorSets;

        // Template is as so: [binding][arrayIndex]
        struct DescriptorInfo
        {
            VkDescriptorImageInfo imageInfo;
            VkDescriptorBufferInfo bufferInfo;

            bool allocatedOrReserved = false;
        };

        std::unordered_map<uint, std::unordered_map<uint, DescriptorInfo>> descriptorInfos;
    };
}
