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
        if (this->shaderStages == -1)
        {
            Debugger::ThrowError("No shader stages specified for descriptor set layout");
        }

        if (bindings.count(binding) != 0)
        {
            Debugger::ThrowError("Binding [" + std::to_string(binding) + "] already in use by a [" + std::to_string(bindings[binding].bindingInfo.descriptorType) + "] descriptor");
        }

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
        std::vector<VkDescriptorSetLayoutBinding> layoutBindings(givenBindings.size());

        auto* bindingFlags = new VkDescriptorBindingFlags[bindings.size()];

        // Foreach pair in the provided tuple retrieve the created set layout binding
        for (const auto pair : givenBindings)
        {
            layoutBindings[pair.first] = pair.second.bindingInfo;
            bindingFlags[pair.first] = pair.second.bindingFlags;
        }

        // Set up the layout creation info
        VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount = static_cast<uint32_t>(givenBindings.size());
        layoutCreateInfo.pBindings = layoutBindings.data();

        if (DescriptorInfo::DESCRIPTOR_INDEXING_SUPPORTED)
        {
            VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo{};
            bindingFlagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
            bindingFlagsCreateInfo.bindingCount = bindings.size();
            bindingFlagsCreateInfo.pBindingFlags = bindingFlags;
            layoutCreateInfo.pNext = &bindingFlagsCreateInfo;
        }

        // Create the Vulkan descriptor set layout
        Debugger::CheckResults(
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

    DescriptorPool::Builder &DescriptorPool::Builder::AddPoolSize(const VkDescriptorType descriptorType, const uint32_t count)
    {
        // Add the pool size to the list of pool sizes
        this->poolSizes.push_back({ descriptorType, count });
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
        Debugger::CheckResults(
                vkAllocateDescriptorSets(VulkanCore::GetLogicalDevice(), &allocateInfo, &descriptorSet),
                "Failed to allocate descriptor set"
        );
    }

    void DescriptorPool::AllocateBindlessDescriptorSet(const std::vector<uint32_t> &bindings, VkDescriptorSet &descriptorSet)
    {
        VkDescriptorSetVariableDescriptorCountAllocateInfo variableDescriptorCountAllocateInfo{};
        variableDescriptorCountAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;

        uint32_t descriptorCounts[bindings.size()];
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
        allocateInfo.pSetLayouts = &vkDescriptorSetLayout;
        allocateInfo.descriptorSetCount = 1;
        allocateInfo.pNext = &variableDescriptorCountAllocateInfo;

        // Create the Vulkan descriptor set
        Debugger::CheckResults(
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
        Debugger::CheckResults(
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

    /* --- SETTER METHODS --- */

    void DescriptorSet::WriteBuffer(const uint32_t binding, const std::unique_ptr<Buffer> &buffer)
    {
        // Check if the current binding is not available
        if (descriptorSetLayout->bindings.count(binding) == 0)
        {
            Debugger::ThrowError("Descriptor set layout does not contain the specified binding: [" + std::to_string(binding) + "]");
        }

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
        writeDescriptorSets.push_back(writeDescriptor);
    }

    void DescriptorSet::WriteImage(const uint32_t binding, const VkDescriptorImageInfo *imageInfo)
    {
        // Check if the current binding is not available
        if (descriptorSetLayout->bindings.count(binding) == 0)
        {
            Debugger::ThrowError("Descriptor set layout does not contain the specified binding: [" + std::to_string(binding) + "]");
        }

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
        writeDescriptorSets.push_back(writeDescriptor);
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
        vkUpdateDescriptorSets(VulkanCore::GetLogicalDevice(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
    }

    /* --- DESTRUCTOR --- */

    DescriptorSet::~DescriptorSet()
    {

    }

    // ********************* Bindless Descriptor Set ********************* \\

    /* --- CONSTRUCTORS --- */

    BindlessDescriptorSet::BindlessDescriptorSet(const std::vector<uint32_t> &givenBindings, const uint32_t givenDescriptorType, std::shared_ptr<DescriptorPool> &givenDescriptorPool)
        : descriptorType(givenDescriptorType), descriptorPool(givenDescriptorPool), descriptorSetLayout(givenDescriptorPool->GetDescriptorSetLayout())
    {
        // Check if the current binding is not available
        // TODO: This
        for (const auto &binding : givenBindings)
        {
            if (descriptorSetLayout->bindings.count(binding) == 0)
            {
                Debugger::ThrowError("Descriptor set layout does not contain the specified binding: [" + std::to_string(binding) + "]");
            }
        }

        // Create descriptor set to pool
        descriptorPool->AllocateBindlessDescriptorSet(givenBindings, this->vkDescriptorSet);
    }

    std::unique_ptr<BindlessDescriptorSet> BindlessDescriptorSet::Build(const std::vector<uint32_t> &givenBindings, const uint32_t givenDescriptorType, std::shared_ptr<DescriptorPool> &givenDescriptorPool)
    {
        return std::make_unique<BindlessDescriptorSet>(givenBindings, givenDescriptorType, givenDescriptorPool);
    }

    /* --- SETTER METHODS --- */

    void BindlessDescriptorSet::WriteBuffer(const uint32_t binding, const std::unique_ptr<Buffer> &buffer, const uint32_t arrayIndex)
    {
        if (descriptorType != DESCRIPTOR_TYPE_BUFFER_TRANSFER)
        {
            Debugger::ThrowError("Cannot write buffers to a bindless descriptor of type [" + DescriptorTypeToString(descriptorType) + "]. Make sure it has its type set to [DESCRIPTOR_TYPE_BUFFER_TRANSFER]");
        }

        // Get the binding description and check if it expects more than 1 descriptors
        VkDescriptorSetLayoutBinding bindingDescription = descriptorSetLayout->bindings[binding].bindingInfo;

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.range = buffer->GetMemorySize();
        bufferInfo.offset = buffer->GetOffset();
        bufferInfo.buffer = buffer->GetVulkanBuffer();
        descriptorBufferInfos[binding][arrayIndex] = bufferInfo;

        // Create write descriptor
        VkWriteDescriptorSet writeDescriptor{};
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.descriptorType = bindingDescription.descriptorType;
        writeDescriptor.dstBinding = binding;
        writeDescriptor.dstArrayElement = arrayIndex;
        writeDescriptor.dstSet = this->vkDescriptorSet;
        writeDescriptor.pBufferInfo = &descriptorBufferInfos[binding][arrayIndex];
        writeDescriptor.descriptorCount = 1;

        // Add write descriptor to the list
        writeDescriptorSets.push_back(writeDescriptor);
    }

    void BindlessDescriptorSet::WriteImage(const uint32_t binding, const VkDescriptorImageInfo *imageInfo, const uint32_t arrayIndex)
    {
        if (descriptorType != DESCRIPTOR_TYPE_IMAGE_TRANSFER)
        {
            Debugger::ThrowError("Cannot write images to a bindless descriptor of type [" + DescriptorTypeToString(descriptorType) + "]. Make sure it has its type set to [DESCRIPTOR_TYPE_IMAGE_TRANSFER]");
        }

        // Get the binding description and check if it expects more than 1 descriptors
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
    }

    void BindlessDescriptorSet::WriteTexture(const uint32_t binding, const std::shared_ptr<Texture> &texture, const uint32_t arrayIndex)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = texture->GetVulkanSampler();
        imageInfo.imageLayout = texture->GetImage()->GetLayout();
        imageInfo.imageView = texture->GetImage()->GetVulkanImageView();

        descriptorImageInfos[binding][arrayIndex] = imageInfo;

        WriteImage(binding, &descriptorImageInfos[binding][arrayIndex], arrayIndex);
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
