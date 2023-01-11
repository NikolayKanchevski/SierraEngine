//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <vulkan/vulkan.h>

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
            uint32_t arraySize = 0;
        };

    public:
        /* --- CONSTRUCTORS --- */
        DescriptorSetLayout(const std::unordered_map<uint32_t, DescriptorSetLayoutBinding>& givenBindings);

        class Builder
        {
        public:
            Builder &AddBinding(uint32_t binding, VkDescriptorType descriptorType, VkDescriptorBindingFlags bindingFlags = 0, uint32_t arraySize = 1, VkSampler const *immutableSamplers = nullptr);
            Builder &SetShaderStages(VkShaderStageFlags givenShaderStages);
            [[nodiscard]] std::shared_ptr<DescriptorSetLayout> Build() const;

        private:
            VkShaderStageFlags shaderStages = -1;
            std::unordered_map<uint32_t, DescriptorSetLayoutBinding> bindings;

        };

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkDescriptorSetLayout GetVulkanDescriptorSetLayout() const { return this->vkDescriptorSetLayout; }
        [[nodiscard]] inline bool IsBindingPresent(const uint32_t binding) const { return bindings.count(binding) != 0; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DescriptorSetLayout(const DescriptorSetLayout &) = delete;
        DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

    private:
        friend class DescriptorPool;
        friend class DescriptorSet;
        friend class BindlessDescriptorSet;

        VkDescriptorSetLayout vkDescriptorSetLayout;
        std::unordered_map<uint32_t, DescriptorSetLayoutBinding> bindings;

    };

    class DescriptorPool
    {
    public:
        /* --- CONSTRUCTOR --- */
        DescriptorPool(uint32_t givenMaxSets, VkDescriptorPoolCreateFlags givenPoolCreateFlags, std::vector<VkDescriptorPoolSize> givenPoolSizes);

        class Builder
        {
        public:
            Builder& AddPoolSize(VkDescriptorType descriptorType);
            Builder& SetPoolFlags(VkDescriptorPoolCreateFlags givenPoolCreateFlags);
            Builder& SetMaxSets(uint32_t givenMaxSets);
            [[nodiscard]] std::unique_ptr<DescriptorPool> Build();

        private:
            uint32_t maxSets = 1024;
            VkDescriptorPoolCreateFlags poolCreateFlags = 0;
            std::vector<VkDescriptorPoolSize> poolSizes;

        };

        /* --- SETTER METHODS --- */
        void AllocateDescriptorSet(const std::shared_ptr<DescriptorSetLayout> &descriptorSetLayout, VkDescriptorSet &descriptorSet);
        void AllocateBindlessDescriptorSet(const std::vector<uint32_t> &bindings, const std::shared_ptr<DescriptorSetLayout> &descriptorSetLayout, VkDescriptorSet &descriptorSet);

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
        DescriptorSet(std::shared_ptr<DescriptorSetLayout> &descriptorSetLayout);
        [[nodiscard]] static std::unique_ptr<DescriptorSet> Build(std::shared_ptr<DescriptorSetLayout> &givenDescriptorSetLayout);

        /* --- SETTER METHODS --- */
        void WriteBuffer(uint32_t binding, const std::unique_ptr<Buffer> &buffer);
        void WriteImage(uint32_t binding, const VkDescriptorImageInfo *imageInfo);
        void WriteTexture(uint32_t binding, const std::shared_ptr<Texture> &texture);
        void WriteCubemap(uint32_t binding, const std::unique_ptr<Cubemap> &cubemap);
        void Allocate();

        /* --- GETTER METHODS --- */
        [[nodiscard]] VkDescriptorSet GetVulkanDescriptorSet() const { return this->vkDescriptorSet; }

        /* --- DESTRUCTOR --- */
        ~DescriptorSet();
        DescriptorSet(const DescriptorSet &) = delete;
        DescriptorSet &operator=(const DescriptorSet &) = delete;

    private:
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;

        std::shared_ptr<DescriptorSetLayout> &descriptorSetLayout;

        std::unordered_map<uint32_t, VkWriteDescriptorSet> writeDescriptorSets;
        std::unordered_map<uint32_t, VkDescriptorImageInfo> descriptorImageInfos;
        std::unordered_map<uint32_t, VkDescriptorBufferInfo> descriptorBufferInfos;

    };

    class BindlessDescriptorSet
    {
    public:
        /* --- CONSTRUCTORS --- */
        BindlessDescriptorSet(const std::vector<uint32_t> &givenBindings, std::shared_ptr<DescriptorSetLayout> &descriptorSetLayout);
        [[nodiscard]] static std::shared_ptr<BindlessDescriptorSet> Build(const std::vector<uint32_t> &givenBindings, std::shared_ptr<DescriptorSetLayout> &givenDescriptorSetLayout);

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsBindingConfigured(uint32_t binding) const;
        [[nodiscard]] uint32_t GetFirstFreeIndex(uint32_t binding) const;
        [[nodiscard]] bool IsIndexAllocated(uint32_t binding, uint32_t arrayIndex) const;

        /* --- SETTER METHODS --- */
        void FreeIndex(uint32_t binding, uint32_t arrayIndex, bool reallocate = true);
        uint32_t ReserveIndex(uint32_t binding, int arrayIndex = -1);

        uint32_t WriteBuffer(uint32_t binding, const std::unique_ptr<Buffer> &buffer, bool overwrite = false, int arrayIndex = -1);
        uint32_t WriteImage(uint32_t binding, const VkDescriptorImageInfo *imageInfo, bool overwrite = false, int arrayIndex = -1);
        uint32_t WriteTexture(uint32_t binding, const std::shared_ptr<Texture> &texture, bool overwrite = false, int arrayIndex = -1);
        void Allocate();

        /* --- GETTER METHODS --- */
        [[nodiscard]] VkDescriptorSet GetVulkanDescriptorSet() const { return this->vkDescriptorSet; }

        /* --- DESTRUCTOR --- */
        ~BindlessDescriptorSet();
        BindlessDescriptorSet(const BindlessDescriptorSet &) = delete;
        BindlessDescriptorSet &operator=(const BindlessDescriptorSet &) = delete;

    private:
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
        std::vector<uint32_t> boundBindings;

        std::shared_ptr<DescriptorSetLayout> &descriptorSetLayout;

        std::vector<VkWriteDescriptorSet> writeDescriptorSets;

        // Template is as so: [binding][arrayIndex]
        struct DescriptorInfo
        {
            VkDescriptorImageInfo imageInfo;
            VkDescriptorBufferInfo bufferInfo;

            bool allocatedOrReserved = false;
        };

        std::unordered_map<uint32_t, std::unordered_map<uint32_t, DescriptorInfo>> descriptorInfos;
    };
}
