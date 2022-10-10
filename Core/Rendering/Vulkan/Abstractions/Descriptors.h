//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <memory>
#include <vector>

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    class DescriptorSetLayout
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit DescriptorSetLayout(const std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>& givenBindings);

        class Builder
        {
        public:
            Builder &AddBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags shaderStages, uint32_t descriptorCount = 1, VkSampler const *immutableSamplers = nullptr);
            [[nodiscard]] std::unique_ptr<DescriptorSetLayout> Build() const;

        private:
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        };

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkDescriptorSetLayout GetVulkanDescriptorSetLayout() const { return this->vkDescriptorSetLayout; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DescriptorSetLayout(const DescriptorSetLayout &) = delete;
        DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

    private:
        friend class DescriptorWriter;

        VkDescriptorSetLayout vkDescriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

    };

    class DescriptorPool
    {
    public:
        /* --- CONSTRUCTOR --- */
        DescriptorPool(uint32_t givenMaxSets, VkDescriptorPoolCreateFlags givenPoolCreateFlags, std::vector<VkDescriptorPoolSize> givenPoolSizes);

        class Builder
        {
        public:
            Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& SetPoolFlags(VkDescriptorPoolCreateFlags givenPoolCreateFlags);
            Builder& SetMaxSets(uint32_t givenMaxSets);
            [[nodiscard]] std::unique_ptr<DescriptorPool> Build() const;

        private:
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolCreateFlags = 0;
            std::vector<VkDescriptorPoolSize> poolSizes;

        };

        /* --- SETTER METHODS --- */
        void AllocateDescriptorSet(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptorSet);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkDescriptorPool GetVulkanDescriptorPool() const { return this->vkDescriptorPool; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DescriptorPool(const DescriptorPool &) = delete;
        DescriptorPool &operator=(const DescriptorPool &) = delete;

    private:
        VkDescriptorPool vkDescriptorPool;

    };

    class DescriptorWriter
    {
    public:
        /* --- CONSTRUCTORS --- */
        DescriptorWriter(std::unique_ptr<DescriptorSetLayout> &givenDescriptorSetLayout, std::unique_ptr<DescriptorPool> &givenDescriptorPool);

        /* --- SETTER METHODS --- */
        DescriptorWriter& WriteBuffer(uint32_t binding, const VkDescriptorBufferInfo *bufferInfo);
        DescriptorWriter& WriteImage(uint32_t binding, const VkDescriptorImageInfo *imageInfo);
        void Build(VkDescriptorSet &descriptorSet);

    private:
        std::unique_ptr<DescriptorPool> &descriptorPool;
        std::unique_ptr<DescriptorSetLayout> &descriptorSetLayout;
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;

        void Overwrite(VkDescriptorSet &descriptorSet);
    };
}
