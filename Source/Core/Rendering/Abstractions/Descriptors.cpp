//
// Created by Nikolay Kanchevski on 30.09.22.
//


#include "Descriptors.h"

#include "Texture.h"
#include "../Bases/VK.h"

namespace Sierra::Rendering
{

    // ********************* Descriptor Set Layout ********************* \\

    /* --- CONSTRUCTORS --- */

    DescriptorSetLayout::DescriptorSetLayout(const DescriptorSetLayoutCreateInfo &createInfo)
        : bindings(std::move(createInfo.bindings))
    {
        // Create a pointer to layout binding array
        VkDescriptorBindingFlags* bindingFlags = new VkDescriptorBindingFlags[bindings.size()];
        VkDescriptorSetLayoutBinding* layoutBindings = new VkDescriptorSetLayoutBinding[bindings.size()];

        // Foreach pair in the provided tuple retrieve the created set layout binding
        uint32 i = 0;
        for (const auto &pair : bindings)
        {
            layoutBindings[i].binding = pair.first;
            layoutBindings[i].descriptorType = static_cast<VkDescriptorType>(pair.second.descriptorType);
            layoutBindings[i].descriptorCount = static_cast<VkDescriptorType>(pair.second.arraySize);
            layoutBindings[i].stageFlags = static_cast<VkShaderStageFlags>(pair.second.shaderStages);
            layoutBindings[i].pImmutableSamplers = nullptr;

            bindingFlags[i] = pair.second.arraySize > 1 ? VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT : 0;
            i++;
        }

        // Set up the layout creation info
        VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount = createInfo.bindings.size();
        layoutCreateInfo.pBindings = layoutBindings;
        layoutCreateInfo.flags = static_cast<VkDescriptorSetLayoutCreateFlagBits>(createInfo.flags);

        // Create the Vulkan descriptor set layout
        VK_ASSERT(
            vkCreateDescriptorSetLayout(VK::GetLogicalDevice(), &layoutCreateInfo, nullptr, &vkDescriptorSetLayout),
            FORMAT_STRING("Failed to create descriptor layout with [{0}] binging(s)", layoutCreateInfo.bindingCount)
        );

        delete[](layoutBindings);
        delete[](bindingFlags);
    }

    UniquePtr<DescriptorSetLayout> DescriptorSetLayout::Create(const DescriptorSetLayoutCreateInfo &createInfo)
    {
        return std::make_unique<DescriptorSetLayout>(createInfo);
    }

    /* --- DESTRUCTOR --- */

    void DescriptorSetLayout::Destroy()
    {
        // Destroy the Vulkan descriptor set
        vkDestroyDescriptorSetLayout(VK::GetLogicalDevice(), vkDescriptorSetLayout, nullptr);
    }

    // ********************* Descriptor Pool ********************* \\

    /* --- CONSTRUCTORS --- */

    DescriptorPool::DescriptorPool(const DescriptorPoolCreateInfo &createInfo)
    {
        // Set up the descriptor pool creation info
        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.maxSets = createInfo.multiplier;

        VkDescriptorPoolSize* poolSizes = new VkDescriptorPoolSize[DEFAULT_POOL_SIZES.size()];
        for (uint32 i = DEFAULT_POOL_SIZES.size(); i--;)
        {
            poolSizes[i].type = static_cast<VkDescriptorType>(DEFAULT_POOL_SIZES[i].first);
            poolSizes[i].descriptorCount = static_cast<uint32>(DEFAULT_POOL_SIZES[i].second * createInfo.multiplier);
        }

        descriptorPoolCreateInfo.pPoolSizes = poolSizes;
        descriptorPoolCreateInfo.poolSizeCount = DEFAULT_POOL_SIZES.size();
        descriptorPoolCreateInfo.flags = 0;

        // Create the Vulkan descriptor pool
        VK_ASSERT(
            vkCreateDescriptorPool(VK::GetLogicalDevice(), &descriptorPoolCreateInfo, nullptr, &vkDescriptorPool),
            FORMAT_STRING("Failed to create descriptor pool with [{0}] max sets and [{1}] pool sizes", createInfo.multiplier, descriptorPoolCreateInfo.poolSizeCount)
        );

        delete[](poolSizes);
    }

    UniquePtr<DescriptorPool> DescriptorPool::Create(const DescriptorPoolCreateInfo &createInfo)
    {
        return std::make_unique<DescriptorPool>(createInfo);
    }

    /* --- GETTER METHODS --- */

    UniquePtr<DescriptorPool>& DescriptorPool::GetFreeDescriptorPool()
    {
        if (freePools.empty())
        {
            usedPools.push_back(Create({ }));
        }
        else
        {
            usedPools.push_back(std::move(freePools.back()));
        }

        return usedPools.back();
    }

    void DescriptorPool::AllocateDescriptorSet(const UniquePtr<DescriptorSetLayout> &descriptorSetLayout, VkDescriptorSet &descriptorSet)
    {
        if (currentPool == VK_NULL_HANDLE)
        {
            currentPool = GetFreeDescriptorPool()->vkDescriptorPool;
        }
        VkDescriptorSetLayout vkDescriptorSetLayout = descriptorSetLayout->GetVulkanDescriptorSetLayout();

        // Set up the allocation info
        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = currentPool;
        allocateInfo.pSetLayouts = &vkDescriptorSetLayout;
        allocateInfo.descriptorSetCount = 1;

        VkResult result = vkAllocateDescriptorSets(VK::GetLogicalDevice(), &allocateInfo, &descriptorSet);
        if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
        {
            currentPool = GetFreeDescriptorPool()->vkDescriptorPool;
            allocateInfo.descriptorPool = currentPool;

            result = vkAllocateDescriptorSets(VK::GetLogicalDevice(), &allocateInfo, &descriptorSet);
            if (result != VK_SUCCESS)
            {
                ASSERT_ERROR("Failed to allocate descriptor set! Error code {0}", VK_TO_STRING(result, Result));
            }
        }
        else
        {
            if (result != VK_SUCCESS)
            {
                ASSERT_ERROR("Failed to allocate descriptor set! Error code {0}", VK_TO_STRING(result, Result));
            }
        }
    }

    /* --- SETTER METHODS --- */

    void DescriptorPool::ResetPools()
    {
        for (auto &pool : usedPools)
        {
            vkResetDescriptorPool(VK::GetLogicalDevice(), pool->vkDescriptorPool, 0);
            freePools.push_back(std::move(pool));
        }
        usedPools.clear();

        currentPool = VK_NULL_HANDLE;
    }

    void DescriptorPool::DisposePools()
    {
        for (const auto &pool : usedPools) pool->Destroy();
        for (const auto &pool : freePools) pool->Destroy();
    }

    /* --- DESTRUCTOR --- */

    void DescriptorPool::Destroy()
    {
        vkDestroyDescriptorPool(VK::GetLogicalDevice(), vkDescriptorPool, nullptr);
    }

    // ********************* Descriptor Set ********************* \\

    /* --- CONSTRUCTORS --- */

    DescriptorSet::DescriptorSet(const DescriptorSetCreateInfo &createInfo)
        : descriptorSetLayout(createInfo.descriptorSetLayout)
    {
        DescriptorPool::AllocateDescriptorSet(descriptorSetLayout, vkDescriptorSet);
    }

    UniquePtr<DescriptorSet> DescriptorSet::Create(const DescriptorSetCreateInfo &createInfo)
    {
        return std::make_unique<DescriptorSet>(createInfo);
    }

    /* --- SETTER METHODS --- */

    DescriptorSet* DescriptorSet::SetBuffer(const uint32 binding, const UniquePtr<Buffer> &buffer, const uint64 offset, const uint64 range, const uint32 arrayIndex)
    {
        // Check if the current binding is not available
        ASSERT_ERROR_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [{0}]", binding);

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.offset = offset;
        bufferInfo.range = range != 0 ? range : buffer->GetMemorySize();
        bufferInfo.buffer = buffer->GetVulkanBuffer();
        descriptorBufferInfos[binding] = bufferInfo;

        // Create write descriptor
        VkWriteDescriptorSet writeDescriptor{};
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.descriptorType = static_cast<VkDescriptorType>(descriptorSetLayout->bindings[binding].descriptorType);
        writeDescriptor.dstBinding = binding;
        writeDescriptor.dstArrayElement = arrayIndex;
        writeDescriptor.dstSet = vkDescriptorSet;
        writeDescriptor.pBufferInfo = &descriptorBufferInfos[binding];
        writeDescriptor.descriptorCount = 1;

        // Add write descriptor to the list
        writeDescriptorSets[binding] = writeDescriptor;

        allocated = false;
        return this;
    }

    DescriptorSet* DescriptorSet::SetImage(const uint32 binding, const UniquePtr<Image> &image, const UniquePtr<Sampler> &sampler, const ImageLayout imageLayout, const uint32 arrayIndex)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = sampler->GetVulkanSampler();
        imageInfo.imageView = image->GetVulkanImageView();
        imageInfo.imageLayout = (VkImageLayout) imageLayout;

        descriptorImageInfos[binding] = imageInfo;
        SetVulkanImage(binding, &descriptorImageInfos[binding], arrayIndex);

        return this;
    }

    DescriptorSet* DescriptorSet::SetTexture(const uint32 binding, const SharedPtr<Texture> &texture, const uint32 arrayIndex)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = texture->GetSampler()->GetVulkanSampler();
        imageInfo.imageLayout = static_cast<VkImageLayout>(texture->GetImage()->GetLayout());
        imageInfo.imageView = texture->GetImage()->GetVulkanImageView();

        descriptorImageInfos[binding] = imageInfo;
        SetVulkanImage(binding, &descriptorImageInfos[binding], arrayIndex);

        return this;
    }

    DescriptorSet* DescriptorSet::SetCubemap(const uint32 binding, const SharedPtr<Cubemap> &cubemap, const uint32 arrayIndex)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = cubemap->GetSampler()->GetVulkanSampler();
        imageInfo.imageLayout = (VkImageLayout) cubemap->GetImage()->GetLayout();
        imageInfo.imageView = cubemap->GetImage()->GetVulkanImageView();

        descriptorImageInfos[binding] = imageInfo;
        SetVulkanImage(binding, &descriptorImageInfos[binding], arrayIndex);

        return this;
    }

    void DescriptorSet::Allocate()
    {
        // Collect all write descriptor sets in a vector
        std::vector<VkWriteDescriptorSet> writeDescriptorSetsPtr;
        writeDescriptorSetsPtr.reserve(writeDescriptorSetsPtr.size());
        for (const auto &writeDescriptorSet : writeDescriptorSets)
        {
            writeDescriptorSetsPtr.push_back(writeDescriptorSet.second);
        }

        // Update descriptor sets
        vkUpdateDescriptorSets(VK::GetLogicalDevice(), writeDescriptorSetsPtr.size(), writeDescriptorSetsPtr.data(), 0, nullptr);
        allocated = true;
    }

    void DescriptorSet::SetVulkanImage(uint32 binding, const VkDescriptorImageInfo *imageInfo, const uint32 arrayIndex)
    {
        // Check if the current binding is not available
        ASSERT_ERROR_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [{0}]", binding);

        // Create write descriptor
        VkWriteDescriptorSet writeDescriptor{};
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.descriptorType = static_cast<VkDescriptorType>(descriptorSetLayout->bindings[binding].descriptorType);
        writeDescriptor.dstBinding = binding;
        writeDescriptor.dstArrayElement = arrayIndex;
        writeDescriptor.dstSet = vkDescriptorSet;
        writeDescriptor.pImageInfo = imageInfo;
        writeDescriptor.descriptorCount = 1;

        // Add write descriptor to the list
        writeDescriptorSets[binding] = writeDescriptor;
        allocated = false;
    }

    // ********************* Push Descriptor Set ********************* \\

    /* --- CONSTRUCTORS --- */

    PushDescriptorSet::PushDescriptorSet(const PushDescriptorSetCreateInfo &createInfo)
        : descriptorSetIndex(createInfo.descriptorSetIndex)
    {

    }

    UniquePtr<PushDescriptorSet> PushDescriptorSet::Create(const PushDescriptorSetCreateInfo &createInfo)
    {
        return std::make_unique<PushDescriptorSet>(createInfo);
    }

    /* --- SETTER METHODS --- */

    PushDescriptorSet* PushDescriptorSet::SetBuffer(const uint32 binding, const UniquePtr<Buffer> &buffer, const uint32 arrayIndex, const uint64 size, const uint64 offset)
    {
        // Set buffer copy region
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.offset = offset;
        bufferInfo.range = size != 0 ? size : buffer->GetMemorySize();
        bufferInfo.buffer = buffer->GetVulkanBuffer();
        descriptorBufferInfos[binding] = bufferInfo;

        // Create write descriptor
        VkWriteDescriptorSet writeDescriptor{};
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.descriptorType = static_cast<VkDescriptorType>((bool)(buffer->GetBufferUsage() & BufferUsage::UNIFORM) ? DescriptorType::UNIFORM_BUFFER : DescriptorType::STORAGE_BUFFER);
        writeDescriptor.dstBinding = binding;
        writeDescriptor.dstArrayElement = arrayIndex;
        writeDescriptor.dstSet = VK_NULL_HANDLE;
        writeDescriptor.pBufferInfo = &descriptorBufferInfos[binding];
        writeDescriptor.descriptorCount = 1;

        // Add write descriptor to the list
        writeDescriptorSets[binding] = writeDescriptor;

        return this;
    }

    PushDescriptorSet* PushDescriptorSet::SetImage(const uint32 binding, const UniquePtr<Image> &image, const UniquePtr<Sampler> &sampler, const uint32 arrayIndex, const DescriptorType descriptorType)
    {
        // Set image info
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = sampler->GetVulkanSampler();
        imageInfo.imageView = image->GetVulkanImageView();
        imageInfo.imageLayout = static_cast<VkImageLayout>(ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Write image
        descriptorImageInfos[binding] = imageInfo;
        SetVulkanImage(binding, &descriptorImageInfos[binding], arrayIndex, descriptorType);

        return this;
    }

    PushDescriptorSet* PushDescriptorSet::SetTexture(const uint32 binding, const SharedPtr<Texture> &texture, const uint32 arrayIndex)
    {
        // Set image info (using the image of the texture)
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = texture->GetSampler()->GetVulkanSampler();
        imageInfo.imageView = texture->GetImage()->GetVulkanImageView();
        imageInfo.imageLayout = static_cast<VkImageLayout>(ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Write texture image
        descriptorImageInfos[binding] = imageInfo;
        SetVulkanImage(binding, &descriptorImageInfos[binding], arrayIndex, DescriptorType::COMBINED_IMAGE_SAMPLER);

        return this;
    }

    PushDescriptorSet* PushDescriptorSet::SetCubemap(const uint32 binding, const SharedPtr<Cubemap> &cubemap, const uint32 arrayIndex)
    {
        // Set image info (using the image of the cubemap)
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = cubemap->GetSampler()->GetVulkanSampler();
        imageInfo.imageView = cubemap->GetImage()->GetVulkanImageView();
        imageInfo.imageLayout = static_cast<VkImageLayout>(ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Write cubemap image
        descriptorImageInfos[binding] = imageInfo;
        SetVulkanImage(binding, &descriptorImageInfos[binding], arrayIndex, DescriptorType::COMBINED_IMAGE_SAMPLER);

        return this;
    }

    void PushDescriptorSet::SetVulkanImage(const uint32 binding, const VkDescriptorImageInfo *imageInfo, const uint32 arrayIndex, const DescriptorType descriptorType)
    {
        // Create write descriptor
        VkWriteDescriptorSet writeDescriptor{};
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.descriptorType = static_cast<VkDescriptorType>(descriptorType);
        writeDescriptor.dstBinding = binding;
        writeDescriptor.dstArrayElement = arrayIndex;
        writeDescriptor.dstSet = VK_NULL_HANDLE;
        writeDescriptor.pImageInfo = imageInfo;
        writeDescriptor.descriptorCount = 1;

        // Add write descriptor to the list
        writeDescriptorSets[binding] = writeDescriptor;
    }

}

