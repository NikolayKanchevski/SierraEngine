//
// Created by Nikolay Kanchevski on 10.01.24.
//

#include "VulkanDescriptorSets.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanPushDescriptorSet::VulkanPushDescriptorSet(const VulkanDescriptorSetCreateInfo &createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    void VulkanPushDescriptorSet::BindBuffer(const uint32 binding, const VulkanBuffer &buffer, const uint32 arrayIndex, const uint64 memoryRange, const uint64 offset)
    {
        // Check if resource has already been bound
        auto descriptorSetIterator = std::find_if(writeDescriptorSets.begin(), writeDescriptorSets.end(), [binding, arrayIndex](const VkWriteDescriptorSet &item) { return item.dstBinding == binding && item.dstArrayElement == arrayIndex; });
        if (descriptorSetIterator == writeDescriptorSets.end())
        {
            writeDescriptorSets.emplace_back();
            descriptorSetIterator = writeDescriptorSets.end() - 1;
        }

        // Overwrite resource data
        descriptorSetIterator->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorSetIterator->dstSet = VK_NULL_HANDLE;
        descriptorSetIterator->dstBinding = binding;
        descriptorSetIterator->dstArrayElement = arrayIndex;
        descriptorSetIterator->descriptorCount = 1;
        descriptorSetIterator->descriptorType = buffer.GetVulkanUsageFlags() & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

        // Set up buffer info
        VkDescriptorBufferInfo* bufferInfo = descriptorSetIterator->pBufferInfo != nullptr ? const_cast<VkDescriptorBufferInfo*>(descriptorSetIterator->pBufferInfo) : &bufferInfos.emplace_back();
        bufferInfo->buffer = buffer.GetVulkanBuffer();
        bufferInfo->offset = offset;
        bufferInfo->range = memoryRange;
        descriptorSetIterator->pBufferInfo = bufferInfo;
    }

    void VulkanPushDescriptorSet::BindImage(const uint32 binding, const VulkanImage &image, const VulkanSampler* sampler, const VkImageLayout imageLayout, const uint32 arrayIndex)
    {
        // Check if resource has already been bound
        auto descriptorSetIterator = std::find_if(writeDescriptorSets.begin(), writeDescriptorSets.end(), [binding, arrayIndex](const VkWriteDescriptorSet &item) { return item.dstBinding == binding && item.dstArrayElement == arrayIndex; });
        if (descriptorSetIterator == writeDescriptorSets.end())
        {
            writeDescriptorSets.emplace_back();
            descriptorSetIterator = writeDescriptorSets.end() - 1;
        }

        // Overwrite resource data
        descriptorSetIterator->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorSetIterator->dstSet = VK_NULL_HANDLE;
        descriptorSetIterator->dstBinding = binding;
        descriptorSetIterator->dstArrayElement = arrayIndex;
        descriptorSetIterator->descriptorCount = 1;
        descriptorSetIterator->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

        // Set up buffer info
        VkDescriptorImageInfo* imageInfo = descriptorSetIterator->pImageInfo != nullptr ? const_cast<VkDescriptorImageInfo*>(descriptorSetIterator->pImageInfo) : &imageInfos.emplace_back();
        imageInfo->sampler = sampler != nullptr ? sampler->GetVulkanSampler() : VK_NULL_HANDLE;
        imageInfo->imageView = image.GetVulkanImageView();
        imageInfo->imageLayout = imageLayout;
        descriptorSetIterator->pImageInfo = imageInfo;
    }

}