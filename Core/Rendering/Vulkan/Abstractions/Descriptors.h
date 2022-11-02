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
            [[nodiscard]] std::unique_ptr<DescriptorSetLayout> Build() const;

        private:
            VkShaderStageFlags shaderStages = -1;
            std::unordered_map<uint32_t, DescriptorSetLayoutBinding> bindings;

        };

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkDescriptorSetLayout GetVulkanDescriptorSetLayout() const { return this->vkDescriptorSetLayout; }

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
        DescriptorPool(uint32_t givenMaxSets, VkDescriptorPoolCreateFlags givenPoolCreateFlags, std::vector<VkDescriptorPoolSize> givenPoolSizes, std::unique_ptr<DescriptorSetLayout> &givenSetLayout);

        class Builder
        {
        public:
            Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& SetPoolFlags(VkDescriptorPoolCreateFlags givenPoolCreateFlags);
            Builder& SetMaxSets(uint32_t givenMaxSets);
            [[nodiscard]] std::shared_ptr<DescriptorPool> Build(std::unique_ptr<DescriptorSetLayout> &givenSetLayout);

        private:
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolCreateFlags = 0;
            std::vector<VkDescriptorPoolSize> poolSizes;

        };

        /* --- SETTER METHODS --- */
        void AllocateDescriptorSet(VkDescriptorSet &descriptorSet);
        void AllocateBindlessDescriptorSet(uint32_t binding, VkDescriptorSet &descriptorSet);

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
        DescriptorSet(std::shared_ptr<DescriptorPool> &givenDescriptorPool);
        [[nodiscard]] static std::unique_ptr<DescriptorSet> Build(std::shared_ptr<DescriptorPool> &givenDescriptorPool);

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
        VkDescriptorSet vkDescriptorSet;

        std::shared_ptr<DescriptorPool> &descriptorPool;
        std::unique_ptr<DescriptorSetLayout> &descriptorSetLayout;

        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        std::unordered_map<uint32_t, VkDescriptorImageInfo> descriptorImageInfos;
        std::unordered_map<uint32_t, VkDescriptorBufferInfo> descriptorBufferInfos;

    };

    class BindlessDescriptorSet
    {

        #define DESCRIPTOR_TYPE_NONE 0
        #define DESCRIPTOR_TYPE_BUFFER_TRANSFER 1
        #define DESCRIPTOR_TYPE_IMAGE_TRANSFER 2
        #define DESCRIPTOR_TYPE_TEXTURE_TRANSFER 2

    private:
        inline static std::string const DescriptorTypeToString(const uint32_t descriptorType)
        {
            switch (descriptorType)
            {
                case DESCRIPTOR_TYPE_NONE:
                    return "DESCRIPTOR_TYPE_NONE";
                case DESCRIPTOR_TYPE_BUFFER_TRANSFER:
                    return "DESCRIPTOR_TYPE_BUFFER_TRANSFER";
                case DESCRIPTOR_TYPE_IMAGE_TRANSFER:
                    return "DESCRIPTOR_TYPE_IMAGE_TRANSFER";
                default:
                    return "";
            }
        }

    public:
        /* --- CONSTRUCTORS --- */
        BindlessDescriptorSet(uint32_t givenBinding, uint32_t givenDescriptorType, std::shared_ptr<DescriptorPool> &givenDescriptorPool);
        [[nodiscard]] static std::unique_ptr<BindlessDescriptorSet> Build(uint32_t givenBinding, uint32_t givenDescriptorType, std::shared_ptr<DescriptorPool> &givenDescriptorPool);

        /* --- SETTER METHODS --- */
        void WriteBuffer(const std::unique_ptr<Buffer> &buffer, uint32_t arrayIndex = 0);
        void WriteImage(const VkDescriptorImageInfo *imageInfo, uint32_t arrayIndex = 0);
        void WriteTexture(const std::shared_ptr<Texture> &texture, uint32_t arrayIndex = 0);
        void Allocate();

        /* --- GETTER METHODS --- */
        [[nodiscard]] VkDescriptorSet GetVulkanDescriptorSet() const { return this->vkDescriptorSet; }

        /* --- DESTRUCTOR --- */
        ~BindlessDescriptorSet();
        BindlessDescriptorSet(const BindlessDescriptorSet &) = delete;
        BindlessDescriptorSet &operator=(const BindlessDescriptorSet &) = delete;

    private:
        VkDescriptorSet vkDescriptorSet;

        uint32_t boundBinding;
        uint32_t descriptorType = DESCRIPTOR_TYPE_NONE;

        std::shared_ptr<DescriptorPool> &descriptorPool;
        std::unique_ptr<DescriptorSetLayout> &descriptorSetLayout;

        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        std::unordered_map<uint32_t, VkDescriptorImageInfo> descriptorImageInfos;
        std::unordered_map<uint32_t, VkDescriptorBufferInfo> descriptorBufferInfos;
    };
}
