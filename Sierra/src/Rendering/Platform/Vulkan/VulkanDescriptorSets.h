//
// Created by Nikolay Kanchevski on 10.01.24.
//

#pragma once

#include "VulkanResource.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"

namespace Sierra
{

    struct VulkanDescriptorSetCreateInfo
    {

    };

    class SIERRA_API VulkanPushDescriptorSet
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit VulkanPushDescriptorSet(const VulkanDescriptorSetCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void BindBuffer(uint32 binding, const VulkanBuffer &buffer, uint32 arrayIndex = 0, uint64 memoryRange = 0, uint64 offset = 0);
        void BindImage(uint32 binding, const VulkanImage &image, uint32 arrayIndex = 0);

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
