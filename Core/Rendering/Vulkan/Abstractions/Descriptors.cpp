//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include <vector>

#include "Descriptors.h"
#include "../VulkanCore.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    // ********************* Descriptor Set Layout ********************* \\

    /* --- SETTER METHODS --- */

    DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::AddBinding(const uint32_t binding, const VkDescriptorType descriptorType, const VkDescriptorBindingFlags bindingFlags, const uint32_t arraySize, const VkSampler *immutableSamplers)
    {
        ASSERT_ERROR_IF(this->shaderStages == -1, "No shader stages specified for descriptor set layout");

        ASSERT_ERROR_IF(bindings.count(binding) != 0, "Binding [" + std::to_string(binding) + "] already in use by a [" + std::to_string(bindings[binding].bindingInfo.descriptorType) + "] descriptor");

        // Set up the layout binding info
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = arraySize;
        layoutBinding.stageFlags = this->shaderStages;
        layoutBinding.pImmutableSamplers = immutableSamplers;

        // Add the binding info to the tuple list
        this->bindings[binding] = { layoutBinding, bindingFlags, arraySize };

        return *this;
    }

    DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::SetShaderStages(const VkShaderStageFlags givenShaderStages)
    {
        this->shaderStages = givenShaderStages;
        return *this;
    }

    /* --- CONSTRUCTORS --- */

    std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const
    {
        // Create the descriptor set layout
        return std::make_unique<DescriptorSetLayout>(this->bindings);
    }

    DescriptorSetLayout::DescriptorSetLayout(const std::unordered_map<uint32_t, DescriptorSetLayoutBinding>& givenBindings)
        : bindings(givenBindings)
    {
        // Create a pointer to layout binding array
        VkDescriptorSetLayoutBinding layoutBindings[givenBindings.size()];

        auto* bindingFlags = new VkDescriptorBindingFlags[bindings.size()];

        // Foreach pair in the provided tuple retrieve the created set layout binding
        uint32_t i = 0;
        for (const auto pair : givenBindings)
        {
            layoutBindings[i] = pair.second.bindingInfo;
            bindingFlags[i] = pair.second.bindingFlags;

            i++;
        }

        // Set up the layout creation info
        VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount = static_cast<uint32_t>(givenBindings.size());
        layoutCreateInfo.pBindings = layoutBindings;

        VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo{};
        if (VulkanCore::GetDescriptorIndexingSupported())
        {
            bindingFlagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
            bindingFlagsCreateInfo.bindingCount = bindings.size();
            bindingFlagsCreateInfo.pBindingFlags = bindingFlags;
            layoutCreateInfo.pNext = &bindingFlagsCreateInfo;
        }

        // Create the Vulkan descriptor set layout
        VK_ASSERT(
            vkCreateDescriptorSetLayout(VulkanCore::GetLogicalDevice(), &layoutCreateInfo, nullptr, &vkDescriptorSetLayout),
            "Failed to create descriptor layout with [" + std::to_string(layoutCreateInfo.bindingCount) + "] binging(s)"
        );
    }

    /* --- DESTRUCTOR --- */

    void DescriptorSetLayout::Destroy()
    {
        // Destroy the Vulkan descriptor set
        vkDestroyDescriptorSetLayout(VulkanCore::GetLogicalDevice(), vkDescriptorSetLayout, nullptr);
    }

    // ********************* Descriptor Pool ********************* \\

    /* --- SETTER METHODS --- */

    DescriptorPool::Builder &DescriptorPool::Builder::AddPoolSize(const VkDescriptorType descriptorType)
    {
        // Add the pool size to the list of pool sizes
        this->poolSizes.push_back({ descriptorType, maxSets });
        return *this;
    }

    DescriptorPool::Builder &DescriptorPool::Builder::SetPoolFlags(const VkDescriptorPoolCreateFlags givenPoolCreateFlags)
    {
        // Set the pool creation flags
        this->poolCreateFlags = givenPoolCreateFlags;
        return *this;
    }

    DescriptorPool::Builder &DescriptorPool::Builder::SetMaxSets(const uint32_t givenMaxSets)
    {
        // Set the max set value
        this->maxSets = givenMaxSets;
        return *this;
    }

    void DescriptorPool::AllocateDescriptorSet(VkDescriptorSet &descriptorSet)
    {
        auto vkDescriptorSetLayout = descriptorSetLayout->GetVulkanDescriptorSetLayout();

        // Set up the allocation info
        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = this->vkDescriptorPool;
        allocateInfo.pSetLayouts = &vkDescriptorSetLayout;
        allocateInfo.descriptorSetCount = 1;

        // Create the Vulkan descriptor set
        VK_ASSERT(
            vkAllocateDescriptorSets(VulkanCore::GetLogicalDevice(), &allocateInfo, &descriptorSet),
            "Failed to allocate descriptor set"
        );
    }

    void DescriptorPool::AllocateBindlessDescriptorSet(const std::vector<uint32_t> &bindings, VkDescriptorSet &descriptorSet)
    {
        VkDescriptorSetVariableDescriptorCountAllocateInfo variableDescriptorCountAllocateInfo{};
        variableDescriptorCountAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;

        uint32_t* descriptorCounts = new uint32_t[bindings.size()];
        for (uint32_t i = bindings.size(); i--;)
        {
            descriptorCounts[i] = descriptorSetLayout->bindings[bindings[i]].arraySize;
        }

        variableDescriptorCountAllocateInfo.descriptorSetCount = bindings.size();
        variableDescriptorCountAllocateInfo.pDescriptorCounts = descriptorCounts;

        auto vkDescriptorSetLayout = descriptorSetLayout->GetVulkanDescriptorSetLayout();

        // Set up the allocation info
        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = this->vkDescriptorPool;
        auto const descriptorLayouts = std::vector<VkDescriptorSetLayout>(bindings.size(), vkDescriptorSetLayout);
        allocateInfo.pSetLayouts = descriptorLayouts.data();
        allocateInfo.descriptorSetCount = bindings.size();
        allocateInfo.pNext = &variableDescriptorCountAllocateInfo;

        // Create the Vulkan descriptor set
        VK_ASSERT(
            vkAllocateDescriptorSets(VulkanCore::GetLogicalDevice(), &allocateInfo, &descriptorSet),
            "Failed to allocate descriptor set"
        );
    }

    /* --- CONSTRUCTORS --- */

    std::shared_ptr<DescriptorPool> DescriptorPool::Builder::Build(std::unique_ptr<DescriptorSetLayout> &givenSetLayout)
    {
        // Create the descriptor pool
        return std::make_shared<DescriptorPool>(this->maxSets, this->poolCreateFlags, this->poolSizes, givenSetLayout);
    }

    DescriptorPool::DescriptorPool(uint32_t givenMaxSets, VkDescriptorPoolCreateFlags givenPoolCreateFlags, std::vector<VkDescriptorPoolSize> givenPoolSizes, std::unique_ptr<DescriptorSetLayout> &givenSetLayout)
        : descriptorSetLayout(givenSetLayout)
    {
        // Set up the descriptor pool creation info
        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(givenPoolSizes.size());
        descriptorPoolCreateInfo.maxSets = givenMaxSets;
        descriptorPoolCreateInfo.flags = givenPoolCreateFlags;
        descriptorPoolCreateInfo.pPoolSizes = givenPoolSizes.data();

        // Create the Vulkan descriptor pool
        VK_ASSERT(
            vkCreateDescriptorPool(VulkanCore::GetLogicalDevice(), &descriptorPoolCreateInfo, nullptr, &vkDescriptorPool),
            "Failed to create descriptor pool with [" + std::to_string(givenMaxSets) + "] max sets and [" + std::to_string(descriptorPoolCreateInfo.poolSizeCount) + "] pool sizes"
        );
    }

    /* --- DESTRUCTOR --- */

    void DescriptorPool::Destroy()
    {
        // Destroy the Vulkan descriptor pool
        vkDestroyDescriptorPool(VulkanCore::GetLogicalDevice(), this->vkDescriptorPool, nullptr);
    }

    // ********************* Descriptor Set ********************* \\

    /* --- CONSTRUCTORS --- */

    DescriptorSet::DescriptorSet(std::shared_ptr<DescriptorPool> &givenDescriptorPool)
        : descriptorPool(givenDescriptorPool), descriptorSetLayout(givenDescriptorPool->descriptorSetLayout)
    {
        // Create descriptor set to pool
        descriptorPool->AllocateDescriptorSet(this->vkDescriptorSet);
    }

    std::unique_ptr<DescriptorSet> DescriptorSet::Build(std::shared_ptr<DescriptorPool> &givenDescriptorPool)
    {
        return std::make_unique<DescriptorSet>(givenDescriptorPool);
    }

    std::shared_ptr<DescriptorSet> DescriptorSet::BuildShared(std::shared_ptr<DescriptorPool> &givenDescriptorPool)
    {
        return std::make_shared<DescriptorSet>(givenDescriptorPool);
    }

    /* --- SETTER METHODS --- */

    void DescriptorSet::WriteBuffer(const uint32_t binding, const std::unique_ptr<Buffer> &buffer)
    {
        // Check if the current binding is not available
        ASSERT_ERROR_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [" + std::to_string(binding) + "]");

        // Get the binding description and check if it expects more than 1 descriptors
        VkDescriptorSetLayoutBinding bindingDescription = descriptorSetLayout->bindings[binding].bindingInfo;

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.range = buffer->GetMemorySize();
        bufferInfo.offset = buffer->GetOffset();
        bufferInfo.buffer = buffer->GetVulkanBuffer();
        descriptorBufferInfos[binding] = bufferInfo;

        // Create write descriptor
        VkWriteDescriptorSet writeDescriptor{};
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.descriptorType = bindingDescription.descriptorType;
        writeDescriptor.dstBinding = binding;
        writeDescriptor.dstArrayElement = 0;
        writeDescriptor.dstSet = this->vkDescriptorSet;
        writeDescriptor.pBufferInfo = &descriptorBufferInfos[binding];
        writeDescriptor.descriptorCount = 1;

        // Add write descriptor to the list
        writeDescriptorSets[binding] = writeDescriptor;
    }

    void DescriptorSet::WriteImage(const uint32_t binding, const VkDescriptorImageInfo *imageInfo)
    {
        // Check if the current binding is not available
        ASSERT_ERROR_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [" + std::to_string(binding) + "]");

        // Get the binding description and check if it expects more than 1 descriptors
        VkDescriptorSetLayoutBinding bindingDescription = descriptorSetLayout->bindings[binding].bindingInfo;

        // Create write descriptor
        VkWriteDescriptorSet writeDescriptor{};
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.descriptorType = bindingDescription.descriptorType;
        writeDescriptor.dstBinding = binding;
        writeDescriptor.dstArrayElement = 0;
        writeDescriptor.dstSet = this->vkDescriptorSet;
        writeDescriptor.pImageInfo = imageInfo;
        writeDescriptor.descriptorCount = 1;

        // Add write descriptor to the list
        writeDescriptorSets[binding] = writeDescriptor;
    }

    void DescriptorSet::WriteTexture(const uint32_t binding, const std::shared_ptr<Texture> &texture)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = texture->GetVulkanSampler();
        imageInfo.imageLayout = texture->GetImage()->GetLayout();
        imageInfo.imageView = texture->GetImage()->GetVulkanImageView();

        descriptorImageInfos[binding] = imageInfo;
        WriteImage(binding, &descriptorImageInfos[binding]);
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
        vkUpdateDescriptorSets(VulkanCore::GetLogicalDevice(), writeDescriptorSets.size(), writeSets.data(), 0, nullptr);
    }

    /* --- DESTRUCTOR --- */

    DescriptorSet::~DescriptorSet()
    {

    }

    // ********************* Bindless Descriptor Set ********************* \\

    /* --- CONSTRUCTORS --- */

    BindlessDescriptorSet::BindlessDescriptorSet(const std::vector<uint32_t> &givenBindings, std::shared_ptr<DescriptorPool> &givenDescriptorPool)
        : boundBindings(givenBindings), descriptorPool(givenDescriptorPool), descriptorSetLayout(givenDescriptorPool->GetDescriptorSetLayout())
    {
        // Check if the current binding is not available
        for (const auto &binding : givenBindings)
        {
            ASSERT_ERROR_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [" + std::to_string(binding) + "]");
        }

        // Create descriptor set to pool
        descriptorPool->AllocateBindlessDescriptorSet(givenBindings, this->vkDescriptorSet);
    }

    std::shared_ptr<BindlessDescriptorSet> BindlessDescriptorSet::Build(const std::vector<uint32_t> &givenBindings, std::shared_ptr<DescriptorPool> &givenDescriptorPool)
    {
        return std::make_shared<BindlessDescriptorSet>(givenBindings, givenDescriptorPool);
    }

    /* --- GETTER METHODS --- */

    bool BindlessDescriptorSet::IsBindingConfigured(const uint32_t binding) const
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

    uint32_t BindlessDescriptorSet::GetFirstFreeIndex(const uint32_t binding) const
    {
        ASSERT_ERROR_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [" + std::to_string(binding) + "]");

        for (uint32_t i = 0; i < descriptorSetLayout->bindings[binding].arraySize; i++)
        {
            if (!IsIndexAllocated(binding, i))
            {
                return i;
            }
        }

        ASSERT_ERROR("Descriptor set is full");
        return 0;
    }

    bool BindlessDescriptorSet::IsIndexAllocated(const uint32_t binding, const uint32_t arrayIndex) const
    {
        return descriptorInfos.find(binding) != descriptorInfos.end() && descriptorInfos.at(binding).find(arrayIndex) != descriptorInfos.at(binding).end() && descriptorInfos.at(binding).at(arrayIndex).allocatedOrReserved;
    }

    /* --- SETTER METHODS --- */

    void BindlessDescriptorSet::FreeIndex(const uint32_t binding, const uint32_t arrayIndex, const bool reallocate)
    {
        ASSERT_ERROR_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [" + std::to_string(binding) + "]");

        ASSERT_ERROR_IF(!IsBindingConfigured(binding), "Descriptor set is not configured to work with the specified binding: [" + std::to_string(binding) + "]");

        if (arrayIndex >= descriptorSetLayout->bindings[binding].arraySize)
        {
            ASSERT_WARNING("Array index [" + std::to_string(arrayIndex) + "] is outside of the bounds of descriptor set array. Action suspended");
            return;
        }

        descriptorInfos[binding].erase(arrayIndex);

        if (reallocate)
        {
            Allocate();
        }
    }

    uint32_t BindlessDescriptorSet::ReserveIndex(const uint32_t binding, int arrayIndex)
    {
        ASSERT_ERROR_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [" + std::to_string(binding) + "]");

        ASSERT_ERROR_IF(!IsBindingConfigured(binding), "Descriptor set is not configured to work with the specified binding: [" + std::to_string(binding) + "]");

        if (arrayIndex == -1)
        {
            arrayIndex = GetFirstFreeIndex(binding);
        }
        else
        {
            if (arrayIndex >= descriptorSetLayout->bindings[binding].arraySize)
            {
                ASSERT_WARNING("Array index [" + std::to_string(arrayIndex) + "] is outside of the bounds of descriptor set array. Action suspended");
                return arrayIndex;
            }
        }

        descriptorInfos[binding][arrayIndex].allocatedOrReserved = true;

        return arrayIndex;
    }

    uint32_t BindlessDescriptorSet::WriteBuffer(const uint32_t binding, const std::unique_ptr<Buffer> &buffer, const bool overwrite, int arrayIndex)
    {
        ASSERT_ERROR_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [" + std::to_string(binding) + "]");

        ASSERT_ERROR_IF(!IsBindingConfigured(binding), "Descriptor set is not configured to work with the specified binding: [" + std::to_string(binding) + "]");

        if (arrayIndex == -1)
        {
            arrayIndex = GetFirstFreeIndex(binding);
        }

        if (!overwrite && descriptorInfos[binding][arrayIndex].allocatedOrReserved)
        {
            return arrayIndex;
        }

        VkDescriptorSetLayoutBinding bindingDescription = descriptorSetLayout->bindings[binding].bindingInfo;

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.range = buffer->GetMemorySize();
        bufferInfo.offset = buffer->GetOffset();
        bufferInfo.buffer = buffer->GetVulkanBuffer();

        descriptorInfos[binding][arrayIndex].bufferInfo = bufferInfo;
        descriptorInfos[binding][arrayIndex].allocatedOrReserved = true;

        // Create write descriptor
        VkWriteDescriptorSet writeDescriptor{};
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.descriptorType = bindingDescription.descriptorType;
        writeDescriptor.dstBinding = binding;
        writeDescriptor.dstArrayElement = arrayIndex;
        writeDescriptor.dstSet = this->vkDescriptorSet;
        writeDescriptor.pBufferInfo = &descriptorInfos[binding][arrayIndex].bufferInfo;
        writeDescriptor.descriptorCount = 1;

        // Add write descriptor to the list
        writeDescriptorSets.push_back(writeDescriptor);

        return arrayIndex;
    }

    uint32_t BindlessDescriptorSet::WriteImage(const uint32_t binding, const VkDescriptorImageInfo *imageInfo, const bool overwrite, int arrayIndex)
    {
        ASSERT_ERROR_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [" + std::to_string(binding) + "]");

        ASSERT_ERROR_IF(!IsBindingConfigured(binding), "Descriptor set is not configured to work with the specified binding: [" + std::to_string(binding) + "]");

        if (arrayIndex == -1)
        {
            arrayIndex = GetFirstFreeIndex(binding);
        }

        if (!overwrite && descriptorInfos[binding][arrayIndex].allocatedOrReserved)
        {
            return arrayIndex;
        }

        descriptorInfos[binding][arrayIndex].allocatedOrReserved = true;

        VkDescriptorSetLayoutBinding bindingDescription = descriptorSetLayout->bindings[binding].bindingInfo;

        // Create write descriptor
        VkWriteDescriptorSet writeDescriptor{};
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.descriptorType = bindingDescription.descriptorType;
        writeDescriptor.dstBinding = binding;
        writeDescriptor.dstArrayElement = arrayIndex;
        writeDescriptor.dstSet = this->vkDescriptorSet;
        writeDescriptor.pImageInfo = imageInfo;
        writeDescriptor.descriptorCount = 1;

        // Add write descriptor to the list
        writeDescriptorSets.push_back(writeDescriptor);

        return arrayIndex;
    }

    uint32_t BindlessDescriptorSet::WriteTexture(const uint32_t binding, const std::shared_ptr<Texture> &texture, const bool overwrite, int arrayIndex)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = texture->GetVulkanSampler();
        imageInfo.imageLayout = texture->GetImage()->GetLayout();
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
        vkUpdateDescriptorSets(VulkanCore::GetLogicalDevice(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
    }

    /* --- DESTRUCTOR --- */

    BindlessDescriptorSet::~BindlessDescriptorSet()
    {

    }
}
