//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <memory>
#include <vector>
#include "Buffer.h"
#include "Sampler.h"
#include "Texture.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    class Texture;

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
        friend class DescriptorSet;

    private:
        VkDescriptorSetLayout vkDescriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

    };

    class DescriptorPool
    {
    public:
        /* --- CONSTRUCTOR --- */
        DescriptorPool(uint32_t givenMaxSets, VkDescriptorPoolCreateFlags givenPoolCreateFlags, std::vector<VkDescriptorPoolSize> givenPoolSizes, std::unique_ptr<DescriptorSetLayout> &givenSetLayout);

        class Builder
        {
        public:
            Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& SetPoolFlags(VkDescriptorPoolCreateFlags givenPoolCreateFlags);
            Builder& SetMaxSets(uint32_t givenMaxSets);
            [[nodiscard]] std::unique_ptr<DescriptorPool> Build(std::unique_ptr<DescriptorSetLayout> &givenSetLayout);

        private:
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolCreateFlags = 0;
            std::vector<VkDescriptorPoolSize> poolSizes;

        };

        /* --- SETTER METHODS --- */
        void AllocateDescriptorSet(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptorSet);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkDescriptorPool GetVulkanDescriptorPool() const { return this->vkDescriptorPool; }
        [[nodiscard]] inline std::unique_ptr<DescriptorSetLayout>& GetDescriptorSetLayout() const { return this->descriptorSetLayout;}

        /* --- DESTRUCTOR --- */
        void Destroy();
        DescriptorPool(const DescriptorPool &) = delete;
        DescriptorPool &operator=(const DescriptorPool &) = delete;
        friend class DescriptorSet;

    private:
        VkDescriptorPool vkDescriptorPool;
        std::unique_ptr<DescriptorSetLayout> &descriptorSetLayout;

    };

    class DescriptorSet
    {
    public:
        /* --- CONSTRUCTORS --- */
        DescriptorSet(std::unique_ptr<DescriptorPool> &givenDescriptorPool);
        [[nodiscard]] static std::unique_ptr<DescriptorSet> Build(std::unique_ptr<DescriptorPool> &givenDescriptorPool);

        /* --- SETTER METHODS --- */
        void WriteBuffer(uint32_t binding, const std::unique_ptr<Buffer> &buffer);
        void WriteImage(uint32_t binding, const VkDescriptorImageInfo *imageInfo);
        void WriteTexture(uint32_t binding, const std::shared_ptr<Texture> &texture);
        void Allocate();

        /* --- GETTER METHODS --- */
        [[nodiscard]] VkDescriptorSet GetVulkanDescriptorSet() const { return this->vkDescriptorSet; }

        /* --- DESTRUCTOR --- */
        ~DescriptorSet();
        DescriptorSet(const DescriptorSet &) = delete;
        DescriptorSet &operator=(const DescriptorSet &) = delete;

    private:
        uint64_t offset = 0;
        VkDescriptorSet vkDescriptorSet;

        std::unique_ptr<DescriptorPool> &descriptorPool;
        std::unique_ptr<DescriptorSetLayout> &descriptorSetLayout;

        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        std::unordered_map<uint32_t, VkDescriptorImageInfo> descriptorImageInfos;
        std::unordered_map<uint32_t, VkDescriptorBufferInfo> descriptorBufferInfos;

    };
}
