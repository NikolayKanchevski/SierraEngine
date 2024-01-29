//
// Created by Nikolay Kanchevski on 10.01.24.
//

#include "VulkanDescriptors.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanDescriptorPool::VulkanDescriptorPool(const VulkanDevice &device, const VulkanDescriptorPoolCreateInfo &createInfo)
        : VulkanResource(createInfo.name), device(device), initialImageCount(createInfo.initialImageCount), initialTextureCount(createInfo.initialTextureCount), initialUniformBufferCount(createInfo.initialUniformBufferCount), initialStorageBufferCount(createInfo.initialStorageBufferCount)
    {
        Reallocate();
    }

    /* --- POLLING METHODS --- */

    void VulkanDescriptorPool::Reallocate()
    {
        if (!freePools.empty())
        {
            fullPools.emplace(freePools.front());
            freePools.pop();
        }

        // This is a very simple formula to calculate a somewhat-plausible max set number, but we do not really care, as we will reallocate when it has been exceeded
        uint32 maxSets =
            initialImageCount +
            initialTextureCount +
            (initialUniformBufferCount + initialStorageBufferCount) / 2
        ;

        // Set up pool sizes
        std::vector<VkDescriptorPoolSize> poolSizes;
        poolSizes.reserve(4);

        if (initialImageCount > 0) poolSizes.push_back({ .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .descriptorCount = initialImageCount });
        if (initialTextureCount > 0) poolSizes.push_back({ .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = initialTextureCount });
        if (initialUniformBufferCount > 0) poolSizes.push_back({ .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = initialUniformBufferCount });
        if (initialStorageBufferCount > 0) poolSizes.push_back({ .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = initialStorageBufferCount });

        // Set up descriptor pool create info
        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = { };
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        descriptorPoolCreateInfo.maxSets = maxSets;
        descriptorPoolCreateInfo.poolSizeCount = poolSizes.size();
        descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();

        // Create and allocate pool
        freePools.push(VK_NULL_HANDLE);
        const VkResult result = device.GetFunctionTable().vkCreateDescriptorPool(device.GetLogicalDevice(), &descriptorPoolCreateInfo, nullptr, &freePools.front());
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create descriptor pool [{0}]! Error code: {1}.", GetName(), result);

        device.SetObjectName(freePools.front(), VK_OBJECT_TYPE_DESCRIPTOR_POOL, "Descriptor pool [" + std::to_string(poolCount) + "] of [" + GetName() + "]");
        poolCount++;
    }

    /* --- DESTRUCTORS --- */

    VulkanDescriptorPool::~VulkanDescriptorPool()
    {
        while (!fullPools.empty())
        {
            device.GetFunctionTable().vkDestroyDescriptorPool(device.GetLogicalDevice(), fullPools.front(), nullptr);
            fullPools.pop();
        }

        while (!freePools.empty())
        {
            device.GetFunctionTable().vkDestroyDescriptorPool(device.GetLogicalDevice(), freePools.front(), nullptr);
            freePools.pop();
        }
    }

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