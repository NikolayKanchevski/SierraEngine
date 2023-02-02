//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "Buffer.h"
#include "Cubemap.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    class Texture;

    class DescriptorSetLayout
    {
    private:
        struct DescriptorSetLayoutBinding
        {
            VkDescriptorSetLayoutBinding bindingInfo;
            VkDescriptorBindingFlags bindingFlags = 0;
            uint arraySize = 0;
        };

    public:
        /* --- CONSTRUCTORS --- */
        DescriptorSetLayout(const std::unordered_map<uint, DescriptorSetLayoutBinding>& givenBindings);

        class Builder
        {
        public:
            Builder &AddBinding(uint binding, VkDescriptorType descriptorType, VkDescriptorBindingFlags bindingFlags = 0, uint arraySize = 1, VkSampler const *immutableSamplers = nullptr);
            Builder &SetShaderStages(VkShaderStageFlags givenShaderStages);
            [[nodiscard]] SharedPtr<DescriptorSetLayout> Build() const;

        private:
            VkShaderStageFlags shaderStages = -1;
            std::unordered_map<uint, DescriptorSetLayoutBinding> bindings;

        };

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkDescriptorSetLayout GetVulkanDescriptorSetLayout() const { return this->vkDescriptorSetLayout; }
        [[nodiscard]] inline bool IsBindingPresent(const uint binding) const { return bindings.count(binding) != 0; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DescriptorSetLayout(const DescriptorSetLayout &) = delete;
        DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

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
        /* --- CONSTRUCTOR --- */
        DescriptorPool(uint givenMaxSets, VkDescriptorPoolCreateFlags givenPoolCreateFlags, std::vector<VkDescriptorPoolSize> givenPoolSizes);

        class Builder
        {
        public:
            Builder& AddPoolSize(VkDescriptorType descriptorType);
            Builder& SetPoolFlags(VkDescriptorPoolCreateFlags givenPoolCreateFlags);
            Builder& SetMaxSets(uint givenMaxSets);
            [[nodiscard]] UniquePtr<DescriptorPool> Build();

        private:
            uint maxSets = 1024;
            VkDescriptorPoolCreateFlags poolCreateFlags = 0;
            std::vector<VkDescriptorPoolSize> poolSizes;

        };

        /* --- SETTER METHODS --- */
        void AllocateDescriptorSet(const SharedPtr<DescriptorSetLayout> &descriptorSetLayout, VkDescriptorSet &descriptorSet);
        void AllocateBindlessDescriptorSet(const std::vector<uint> &bindings, const SharedPtr<DescriptorSetLayout> &descriptorSetLayout, VkDescriptorSet &descriptorSet);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkDescriptorPool GetVulkanDescriptorPool() const { return this->vkDescriptorPool; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DescriptorPool(const DescriptorPool &) = delete;
        DescriptorPool &operator=(const DescriptorPool &) = delete;
        friend class DescriptorSet;

    private:
        VkDescriptorPool vkDescriptorPool;

    };

    class DescriptorSet
    {
    public:
        /* --- CONSTRUCTORS --- */
        DescriptorSet(SharedPtr<DescriptorSetLayout> &descriptorSetLayout);
        [[nodiscard]] static UniquePtr<DescriptorSet> Build(SharedPtr<DescriptorSetLayout> &givenDescriptorSetLayout);

        /* --- SETTER METHODS --- */
        void WriteBuffer(uint binding, const UniquePtr<Buffer> &buffer);
        void WriteImage(uint binding, const VkDescriptorImageInfo *imageInfo);
        void WriteTexture(uint binding, const SharedPtr<Texture> &texture);
        void WriteCubemap(uint binding, const UniquePtr<Cubemap> &cubemap);
        void Allocate();

        /* --- GETTER METHODS --- */
        [[nodiscard]] VkDescriptorSet GetVulkanDescriptorSet() const { return this->vkDescriptorSet; }

        /* --- DESTRUCTOR --- */
        ~DescriptorSet();
        DescriptorSet(const DescriptorSet &) = delete;
        DescriptorSet &operator=(const DescriptorSet &) = delete;

    private:
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;

        SharedPtr<DescriptorSetLayout> &descriptorSetLayout;

        std::unordered_map<uint, VkWriteDescriptorSet> writeDescriptorSets;
        std::unordered_map<uint, VkDescriptorImageInfo> descriptorImageInfos;
        std::unordered_map<uint, VkDescriptorBufferInfo> descriptorBufferInfos;

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
        uint ReserveIndex(uint binding, int arrayIndex = -1);

        uint WriteBuffer(uint binding, const UniquePtr<Buffer> &buffer, bool overwrite = false, int arrayIndex = -1);
        uint WriteImage(uint binding, const VkDescriptorImageInfo *imageInfo, bool overwrite = false, int arrayIndex = -1);
        uint WriteTexture(uint binding, const SharedPtr<Texture> &texture, bool overwrite = false, int arrayIndex = -1);
        void Allocate();

        /* --- GETTER METHODS --- */
        [[nodiscard]] VkDescriptorSet GetVulkanDescriptorSet() const { return this->vkDescriptorSet; }

        /* --- DESTRUCTOR --- */
        ~BindlessDescriptorSet();
        BindlessDescriptorSet(const BindlessDescriptorSet &) = delete;
        BindlessDescriptorSet &operator=(const BindlessDescriptorSet &) = delete;

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
