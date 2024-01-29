//
// Created by Nikolay Kanchevski on 10.01.24.
//

#pragma once

#include "VulkanResource.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanSampler.h"

namespace Sierra
{

    struct VulkanDescriptorPoolCreateInfo
    {
        const std::string &name;
        uint32 initialImageCount = 16;
        uint32 initialTextureCount = 128;
        uint32 initialUniformBufferCount = 8;
        uint32 initialStorageBufferCount = 16;
    };

    class SIERRA_API VulkanDescriptorPool final : public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanDescriptorPool(const VulkanDevice &device, const VulkanDescriptorPoolCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] VkDescriptorPool GetVulkanDescriptorPool() const { return freePools.front(); }

        /* --- OPERATORS --- */
        VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
        VulkanDescriptorPool &operator=(const VulkanDescriptorPool&) = delete;

        /* --- DESTRUCTOR --- */
        ~VulkanDescriptorPool();

    private:
        const VulkanDevice &device;

        std::queue<VkDescriptorPool> freePools;
        std::queue<VkDescriptorPool> fullPools;

        uint32 initialImageCount = 0;
        uint32 initialTextureCount = 0;
        uint32 initialUniformBufferCount = 0;
        uint32 initialStorageBufferCount = 0;
        uint32 poolCount = 0;

        [[nodiscard]] VkDescriptorPool CreateNewDescriptorPool();

        friend class VulkanImGuiRenderTask;
        void Reallocate();

    };

    struct VulkanDescriptorSetCreateInfo
    {

    };

    class SIERRA_API VulkanPushDescriptorSet final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit VulkanPushDescriptorSet(const VulkanDescriptorSetCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void BindBuffer(uint32 binding, const VulkanBuffer &buffer, uint32 arrayIndex = 0, uint64 memoryRange = 0, uint64 offset = 0);
        void BindImage(uint32 binding, const VulkanImage &image, const VulkanSampler* sampler, VkImageLayout imageLayout, uint32 arrayIndex = 0);

        /* --- GETTER METHODS --- */
        [[nodiscard]] const std::vector<VkWriteDescriptorSet>& GetWriteDescriptorSets() const { return writeDescriptorSets; }

        /* --- OPERATORS --- */
        VulkanPushDescriptorSet(const VulkanPushDescriptorSet&) = delete;
        VulkanPushDescriptorSet &operator=(const VulkanPushDescriptorSet&) = delete;

        /* --- DESTRUCTOR --- */
        ~VulkanPushDescriptorSet() = default;

    private:
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;

        std::deque<VkDescriptorBufferInfo> bufferInfos;
        std::deque<VkDescriptorImageInfo> imageInfos;

    };

}
