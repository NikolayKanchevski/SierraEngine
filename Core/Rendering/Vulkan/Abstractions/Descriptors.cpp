//
// Created by Nikolay Kanchevski on 30.09.22.
//


#include "Descriptors.h"

#include "Texture.h"
#include "../VK.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    // ********************* Descriptor Set Layout ********************* \\

    /* --- CONSTRUCTORS --- */

    DescriptorSetLayout::DescriptorSetLayout(const DescriptorSetLayoutCreateInfo &createInfo)
        : bindings(std::move(createInfo.bindings))
    {
        // Create a pointer to layout binding array
        VkDescriptorBindingFlags* bindingFlags = new VkDescriptorBindingFlags[bindings.size()];
        VkDescriptorSetLayoutBinding* layoutBindings = new VkDescriptorSetLayoutBinding[createInfo.bindings.size()];

        // Foreach pair in the provided tuple retrieve the created set layout binding
        bool containsBindlessDescriptors = false;

        uint i = 0;
        for (const auto &pair : createInfo.bindings)
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

        VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo{};
        if (VK::GetDevice()->GetDescriptorIndexingSupported() && containsBindlessDescriptors)
        {
            bindingFlagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
            bindingFlagsCreateInfo.bindingCount = bindings.size();
            bindingFlagsCreateInfo.pBindingFlags = bindingFlags;
            layoutCreateInfo.pNext = &bindingFlagsCreateInfo;
        }

        // Create the Vulkan descriptor set layout
        VK_ASSERT(
            vkCreateDescriptorSetLayout(VK::GetLogicalDevice(), &layoutCreateInfo, nullptr, &vkDescriptorSetLayout),
            FORMAT_STRING("Failed to create descriptor layout with [{0}] binging(s)", layoutCreateInfo.bindingCount)
        );

        delete[] layoutBindings;
        delete[] bindingFlags;
    }

    UniquePtr<DescriptorSetLayout> DescriptorSetLayout::Create(DescriptorSetLayoutCreateInfo createInfo)
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

    DescriptorPool::DescriptorPool(const DescriptorPoolCreateInfo &givenCreateInfo)
        : createInfo(givenCreateInfo)
    {
        // Set up the descriptor pool creation info
        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.maxSets = createInfo.multiplier;

        VkDescriptorPoolSize* poolSizes = new VkDescriptorPoolSize[defaultPoolSizes.size()];
        for (uint i = defaultPoolSizes.size(); i--;)
        {
            poolSizes[i].type = static_cast<VkDescriptorType>(defaultPoolSizes[i].first);
            poolSizes[i].descriptorCount = defaultPoolSizes[i].second * createInfo.multiplier;
        }

        descriptorPoolCreateInfo.pPoolSizes = poolSizes;
        descriptorPoolCreateInfo.poolSizeCount = defaultPoolSizes.size();
        descriptorPoolCreateInfo.flags = createInfo.flags;

        // Create the Vulkan descriptor pool
        VK_ASSERT(
            vkCreateDescriptorPool(VK::GetLogicalDevice(), &descriptorPoolCreateInfo, nullptr, &vkDescriptorPool),
            FORMAT_STRING("Failed to create descriptor pool with [{0}] max sets and [{1}] pool sizes", createInfo.multiplier, descriptorPoolCreateInfo.poolSizeCount)
        );

        delete[] poolSizes;
    }

    UniquePtr<DescriptorPool> DescriptorPool::Create(DescriptorPoolCreateInfo givenCreateInfo)
    {
        return std::make_unique<DescriptorPool>(givenCreateInfo);
    }

    /* --- GETTER METHODS --- */

    UniquePtr<DescriptorPool>& DescriptorPool::GetPool()
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
            currentPool = GetPool()->vkDescriptorPool;
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
            currentPool = GetPool()->vkDescriptorPool;
            allocateInfo.descriptorPool = currentPool;

            result = vkAllocateDescriptorSets(VK::GetLogicalDevice(), &allocateInfo, &descriptorSet);
            if (result != VK_SUCCESS)
            {
                ASSERT_ERROR_FORMATTED("Failed to allocate descriptor set! Error code {0}", VK_TO_STRING(result, Result));
            }
        }
        else
        {
            if (result != VK_SUCCESS)
            {
                ASSERT_ERROR_FORMATTED("Failed to allocate descriptor set! Error code {0}", VK_TO_STRING(result, Result));
            }
        }
    }

    void DescriptorPool::AllocateBindlessDescriptorSet(const std::vector<uint> &bindings, const SharedPtr<DescriptorSetLayout> &descriptorSetLayout, VkDescriptorSet &descriptorSet)
    {
//        VkDescriptorSetVariableDescriptorCountAllocateInfo variableDescriptorCountAllocateInfo{};
//        variableDescriptorCountAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
//
//        uint* descriptorCounts = new uint[bindings.size()];
//        for (uint i = bindings.size(); i--;)
//        {
//            descriptorCounts[i] = descriptorSetLayout->bindings[bindings[i]].arraySize;
//        }
//
//        variableDescriptorCountAllocateInfo.descriptorSetCount = bindings.size();
//        variableDescriptorCountAllocateInfo.pDescriptorCounts = descriptorCounts;
//
//        auto vkDescriptorSetLayout = descriptorSetLayout->GetVulkanDescriptorSetLayout();
//
//        // Set up the allocation info
//        VkDescriptorSetAllocateInfo allocateInfo{};
//        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//        allocateInfo.descriptorPool = this->vkDescriptorPool;
//        auto const descriptorLayouts = std::vector<VkDescriptorSetLayout>(bindings.size(), vkDescriptorSetLayout);
//        allocateInfo.pSetLayouts = descriptorLayouts.data();
//        allocateInfo.descriptorSetCount = bindings.size();
//        allocateInfo.pNext = &variableDescriptorCountAllocateInfo;
//
//        // Create the Vulkan descriptor set
//        VK_ASSERT(
//            vkAllocateDescriptorSets(VK::GetLogicalDevice(), &allocateInfo, &descriptorSet),
//            "Failed to allocate descriptor set"
//        );
//
//        delete[] descriptorCounts;
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
        vkDestroyDescriptorPool(VK::GetLogicalDevice(), this->vkDescriptorPool, nullptr);
    }

    // ********************* Descriptor Set ********************* \\

    /* --- CONSTRUCTORS --- */

    DescriptorSet::DescriptorSet(const UniquePtr<DescriptorSetLayout> &descriptorSetLayout)
        : descriptorSetLayout(descriptorSetLayout)
    {
        DescriptorPool::AllocateDescriptorSet(descriptorSetLayout, this->vkDescriptorSet);
    }

    UniquePtr<DescriptorSet> DescriptorSet::Create(const UniquePtr<DescriptorSetLayout> &givenDescriptorSetLayout)
    {
        return std::make_unique<DescriptorSet>(givenDescriptorSetLayout);
    }

    /* --- SETTER METHODS --- */

    DescriptorSet* DescriptorSet::WriteBuffer(const uint binding, const UniquePtr<Buffer> &buffer)
    {
        // Check if the current binding is not available
        ASSERT_ERROR_FORMATTED_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [{0}]", binding);

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.offset = 0;
        bufferInfo.range = buffer->GetMemorySize();
        bufferInfo.buffer = buffer->GetVulkanBuffer();
        descriptorBufferInfos[binding] = bufferInfo;

        // Create write descriptor
        VkWriteDescriptorSet writeDescriptor{};
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.descriptorType = static_cast<VkDescriptorType>(descriptorSetLayout->bindings[binding].descriptorType);
        writeDescriptor.dstBinding = binding;
        writeDescriptor.dstArrayElement = 0;
        writeDescriptor.dstSet = this->vkDescriptorSet;
        writeDescriptor.pBufferInfo = &descriptorBufferInfos[binding];
        writeDescriptor.descriptorCount = 1;

        // Add write descriptor to the list
        writeDescriptorSets[binding] = writeDescriptor;

        return this;
    }

    DescriptorSet* DescriptorSet::WriteImage(const uint binding, const UniquePtr<Image> &image, const UniquePtr<Sampler> &sampler, const ImageLayout imageLayout)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = sampler->GetVulkanSampler();
        imageInfo.imageView = image->GetVulkanImageView();
        imageInfo.imageLayout = (VkImageLayout) imageLayout;

        descriptorImageInfos[binding] = imageInfo;
        WriteImage(binding, &descriptorImageInfos[binding]);

        return this;
    }

    DescriptorSet* DescriptorSet::WriteTexture(const uint binding, const SharedPtr<Texture> &texture)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = texture->GetVulkanSampler();
        imageInfo.imageLayout = (VkImageLayout) texture->GetImage()->GetLayout();
        imageInfo.imageView = texture->GetImage()->GetVulkanImageView();

        descriptorImageInfos[binding] = imageInfo;
        WriteImage(binding, &descriptorImageInfos[binding]);

        return this;
    }

    DescriptorSet* DescriptorSet::WriteCubemap(const uint binding, const UniquePtr<Cubemap> &cubemap)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = cubemap->GetSampler()->GetVulkanSampler();
        imageInfo.imageLayout = (VkImageLayout) cubemap->GetImage()->GetLayout();
        imageInfo.imageView = cubemap->GetImage()->GetVulkanImageView();

        descriptorImageInfos[binding] = imageInfo;
        WriteImage(binding, &descriptorImageInfos[binding]);

        return this;
    }

    void DescriptorSet::Allocate()
    {
        // Collect all write descriptor sets in a vector
        std::vector<VkWriteDescriptorSet> writeSets;
        writeSets.reserve(writeDescriptorSets.size());

        for (const auto &writeSet : writeDescriptorSets)
        {
            writeSets.push_back(writeSet.second);
        }

        // Update descriptor sets
        vkUpdateDescriptorSets(VK::GetLogicalDevice(), writeDescriptorSets.size(), writeSets.data(), 0, nullptr);
    }

    void DescriptorSet::WriteImage(const uint binding, const VkDescriptorImageInfo *imageInfo)
    {
        // Check if the current binding is not available
        ASSERT_ERROR_FORMATTED_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [{0}]", binding);

        // Create write descriptor
        VkWriteDescriptorSet writeDescriptor{};
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.descriptorType = static_cast<VkDescriptorType>(descriptorSetLayout->bindings[binding].descriptorType);
        writeDescriptor.dstBinding = binding;
        writeDescriptor.dstArrayElement = 0;
        writeDescriptor.dstSet = this->vkDescriptorSet;
        writeDescriptor.pImageInfo = imageInfo;
        writeDescriptor.descriptorCount = 1;

        // Add write descriptor to the list
        writeDescriptorSets[binding] = writeDescriptor;
    }

    /* --- DESTRUCTOR --- */

    DescriptorSet::~DescriptorSet()
    {

    }

    // ********************* Bindless Descriptor Set ********************* \\

    /* --- CONSTRUCTORS --- */

    BindlessDescriptorSet::BindlessDescriptorSet(const std::vector<uint> &givenBindings, SharedPtr<DescriptorSetLayout> &descriptorSetLayout)
        : boundBindings(givenBindings), descriptorSetLayout(descriptorSetLayout)
    {
        // Check if the current binding is not available
        for (const auto &binding : givenBindings)
        {
            ASSERT_ERROR_FORMATTED_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [{0}]", binding);
        }

        // Create descriptor set to pool
        DescriptorPool::AllocateBindlessDescriptorSet(givenBindings, descriptorSetLayout, this->vkDescriptorSet);
    }

    SharedPtr<BindlessDescriptorSet> BindlessDescriptorSet::Build(const std::vector<uint> &givenBindings, SharedPtr<DescriptorSetLayout> &givenDescriptorSetLayout)
    {
        return std::make_shared<BindlessDescriptorSet>(givenBindings, givenDescriptorSetLayout);
    }

    /* --- GETTER METHODS --- */

    bool BindlessDescriptorSet::IsBindingConfigured(const uint binding) const
    {
        bool bindingContained = false;
        for (const auto &boundBinding : boundBindings)
        {
            if (boundBinding == binding)
            {
                bindingContained = true;
                break;
            }
        }

        return bindingContained;
    }

    uint BindlessDescriptorSet::GetFirstFreeIndex(const uint binding) const
    {
        ASSERT_ERROR_FORMATTED_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [{0}]", binding);

        for (uint i = 0; i < descriptorSetLayout->bindings[binding].arraySize; i++)
        {
            if (!IsIndexAllocated(binding, i))
            {
                return i;
            }
        }

        ASSERT_ERROR("Descriptor set is full");

        return 0;
    }

    bool BindlessDescriptorSet::IsIndexAllocated(const uint binding, const uint arrayIndex) const
    {
        return descriptorInfos.find(binding) != descriptorInfos.end() && descriptorInfos.at(binding).find(arrayIndex) != descriptorInfos.at(binding).end() && descriptorInfos.at(binding).at(arrayIndex).allocatedOrReserved;
    }

    /* --- SETTER METHODS --- */

    void BindlessDescriptorSet::FreeIndex(const uint binding, const uint arrayIndex, const bool reallocate)
    {
        ASSERT_ERROR_FORMATTED_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [{0}]", binding);

        ASSERT_ERROR_FORMATTED_IF(!IsBindingConfigured(binding), "Descriptor set is not configured to work with the specified binding: [{0}]", binding);

        if (arrayIndex >= descriptorSetLayout->bindings[binding].arraySize)
        {
            ASSERT_WARNING_FORMATTED("Array index [{0}] is outside of the bounds of descriptor set array. Action suspended", arrayIndex);
            return;
        }

        descriptorInfos[binding].erase(arrayIndex);

        if (reallocate)
        {
            Allocate();
        }
    }

    uint BindlessDescriptorSet::ReserveIndex(const uint binding, uint arrayIndex)
    {
        ASSERT_ERROR_FORMATTED_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [{0}]", binding);

        ASSERT_ERROR_FORMATTED_IF(!IsBindingConfigured(binding), "Descriptor set is not configured to work with the specified binding: [{0}]", binding);

        if (arrayIndex == -1)
        {
            arrayIndex = GetFirstFreeIndex(binding);
        }
        else
        {
            if (arrayIndex >= descriptorSetLayout->bindings[binding].arraySize)
            {
                ASSERT_WARNING_FORMATTED("Array index [{0}] is outside of the bounds of descriptor set array. Action suspended", arrayIndex);
                return arrayIndex;
            }
        }

        descriptorInfos[binding][arrayIndex].allocatedOrReserved = true;

        return arrayIndex;
    }

    uint BindlessDescriptorSet::WriteBuffer(const uint binding, const UniquePtr<Buffer> &buffer, const bool overwrite, uint arrayIndex)
    {
        ASSERT_ERROR_FORMATTED_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [{0}]", binding);

        ASSERT_ERROR_FORMATTED_IF(!IsBindingConfigured(binding), "Descriptor set is not configured to work with the specified binding: [{0}]", binding);

        if (arrayIndex == -1)
        {
            arrayIndex = GetFirstFreeIndex(binding);
        }

        if (!overwrite && descriptorInfos[binding][arrayIndex].allocatedOrReserved)
        {
            return arrayIndex;
        }

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.offset = 0;
        bufferInfo.range = buffer->GetMemorySize();
        bufferInfo.buffer = buffer->GetVulkanBuffer();

        descriptorInfos[binding][arrayIndex].bufferInfo = bufferInfo;
        descriptorInfos[binding][arrayIndex].allocatedOrReserved = true;

        // Create write descriptor
        VkWriteDescriptorSet writeDescriptor{};
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.descriptorType = static_cast<VkDescriptorType>(descriptorSetLayout->bindings[binding].descriptorType);
        writeDescriptor.dstBinding = binding;
        writeDescriptor.dstArrayElement = arrayIndex;
        writeDescriptor.dstSet = this->vkDescriptorSet;
        writeDescriptor.pBufferInfo = &descriptorInfos[binding][arrayIndex].bufferInfo;
        writeDescriptor.descriptorCount = 1;

        // Add write descriptor to the list
        writeDescriptorSets.push_back(writeDescriptor);

        return arrayIndex;
    }

    uint BindlessDescriptorSet::WriteImage(const uint binding, const VkDescriptorImageInfo *imageInfo, const bool overwrite, uint arrayIndex)
    {
        ASSERT_ERROR_FORMATTED_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [{0}]", binding);

        ASSERT_ERROR_FORMATTED_IF(!IsBindingConfigured(binding), "Descriptor set is not configured to work with the specified binding: [{0}]", binding);

        if (arrayIndex == -1)
        {
            arrayIndex = GetFirstFreeIndex(binding);
        }

        if (!overwrite && descriptorInfos[binding][arrayIndex].allocatedOrReserved)
        {
            return arrayIndex;
        }

        descriptorInfos[binding][arrayIndex].allocatedOrReserved = true;

        // Create write descriptor
        VkWriteDescriptorSet writeDescriptor{};
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.descriptorType = static_cast<VkDescriptorType>(descriptorSetLayout->bindings[binding].descriptorType);
        writeDescriptor.dstBinding = binding;
        writeDescriptor.dstArrayElement = arrayIndex;
        writeDescriptor.dstSet = this->vkDescriptorSet;
        writeDescriptor.pImageInfo = imageInfo;
        writeDescriptor.descriptorCount = 1;

        // Add write descriptor to the list
        writeDescriptorSets.push_back(writeDescriptor);

        return arrayIndex;
    }

    uint BindlessDescriptorSet::WriteTexture(const uint binding, const SharedPtr<Texture> &texture, const bool overwrite, uint arrayIndex)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = texture->GetVulkanSampler();
        imageInfo.imageLayout = (VkImageLayout) texture->GetImage()->GetLayout();
        imageInfo.imageView = texture->GetImage()->GetVulkanImageView();

        if (arrayIndex == -1)
        {
            arrayIndex = GetFirstFreeIndex(binding);
        }

        if (!overwrite && descriptorInfos[binding][arrayIndex].allocatedOrReserved)
        {
            return arrayIndex;
        }

        descriptorInfos[binding][arrayIndex].imageInfo = imageInfo;

        return WriteImage(binding, &descriptorInfos[binding][arrayIndex].imageInfo, overwrite, arrayIndex);
    }

    void BindlessDescriptorSet::Allocate()
    {
        vkUpdateDescriptorSets(VK::GetLogicalDevice(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
    }

}
