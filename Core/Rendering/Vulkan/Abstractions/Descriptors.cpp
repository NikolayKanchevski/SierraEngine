//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include <vector>

#include "Descriptors.h"
#include "../VulkanDebugger.h"
#include "../VulkanCore.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    // ********************* Descriptor Set Layout ********************* \\

    /* --- SETTER METHODS --- */

    DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::AddBinding(const uint32_t binding, const VkDescriptorType descriptorType, const VkShaderStageFlags shaderStages, const uint32_t descriptorCount, const VkSampler *immutableSamplers)
    {
        if (bindings.count(binding) != 0)
        {
            VulkanDebugger::ThrowError("Binding [" + std::to_string(binding) + "] already in use by a [" + std::to_string(bindings[binding].descriptorType) + "] descriptor");
        }

        // Set up the layout binding info
        VkDescriptorSetLayoutBinding layoutBinding;
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = descriptorCount;
        layoutBinding.stageFlags = shaderStages;
        layoutBinding.pImmutableSamplers = immutableSamplers;

        // Add the binding info to the tuple list
        this->bindings[binding] = layoutBinding;

        return *this;
    }

    /* --- CONSTRUCTORS --- */

    std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const
    {
        // Create the descriptor set layout
        return std::make_unique<DescriptorSetLayout>(this->bindings);
    }

    DescriptorSetLayout::DescriptorSetLayout(const std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>& givenBindings)
        : bindings(givenBindings)
    {
        // Create a pointer to layout binding array
        std::vector<VkDescriptorSetLayoutBinding> layoutBindings(givenBindings.size());

        // Foreach pair in the provided tuple retrieve the created set layout binding
        for (auto pair : givenBindings)
        {
            layoutBindings.push_back(pair.second);
        }

        // Set up the layout creation info
        VkDescriptorSetLayoutCreateInfo layoutCreateInfo;
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount = static_cast<uint32_t>(givenBindings.size());
        layoutCreateInfo.pBindings = layoutBindings.data();

        // Create the Vulkan descriptor set layout
        VulkanDebugger::CheckResults(
            vkCreateDescriptorSetLayout(VulkanCore::logicalDevice, &layoutCreateInfo, nullptr, &vkDescriptorSetLayout),
            "Failed to create descriptor layout with [" + std::to_string(layoutCreateInfo.bindingCount) + "] binging(s)"
        );
    }

    /* --- DESTRUCTOR --- */

    DescriptorSetLayout::~DescriptorSetLayout()
    {
        // Destroy the Vulkan descriptor set
        vkDestroyDescriptorSetLayout(VulkanCore::logicalDevice, vkDescriptorSetLayout, nullptr);
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

    void Sierra::Core::Rendering::Vulkan::Abstractions::DescriptorPool::AllocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptorSet)
    {
        // Set up the allocation info
        VkDescriptorSetAllocateInfo allocateInfo;
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = this->vkDescriptorPool;
        allocateInfo.pSetLayouts = &descriptorSetLayout;
        allocateInfo.descriptorSetCount = 1;

        // Create the Vulkan descriptor set
        VulkanDebugger::CheckResults(
            vkAllocateDescriptorSets(VulkanCore::logicalDevice, &allocateInfo, nullptr),
            "Failed to allocate descriptor set"
        );
    }

    /* --- CONSTRUCTORS --- */

    std::unique_ptr<DescriptorPool> DescriptorPool::Builder::Build() const
    {
        // Create the descriptor pool
        return std::make_unique<DescriptorPool>(this->maxSets, this->poolCreateFlags, this->poolSizes);
    }

    DescriptorPool::DescriptorPool(uint32_t givenMaxSets, VkDescriptorPoolCreateFlags givenPoolCreateFlags, std::vector<VkDescriptorPoolSize> givenPoolSizes)
    {
        // Set up the descriptor pool creation info
        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(givenPoolSizes.size());
        descriptorPoolCreateInfo.maxSets = givenMaxSets;
        descriptorPoolCreateInfo.flags = givenPoolCreateFlags;
        descriptorPoolCreateInfo.pPoolSizes = givenPoolSizes.data();

        // Create the Vulkan descriptor pool
        VulkanDebugger::CheckResults(
            vkCreateDescriptorPool(VulkanCore::logicalDevice, &descriptorPoolCreateInfo, nullptr, &vkDescriptorPool),
            "Failed to create descriptor pool with [" + std::to_string(givenMaxSets) + "] max sets and [" + std::to_string(descriptorPoolCreateInfo.poolSizeCount) + "] pool sizes"
        );
    }

    /* --- DESTRUCTOR --- */

    DescriptorPool::~DescriptorPool()
    {
        // Destroy the Vulkan descriptor pool
        vkDestroyDescriptorPool(VulkanCore::logicalDevice, this->vkDescriptorPool, nullptr);
    }

    // ********************* Descriptor Writer ********************* \\

    /* --- SETTER METHODS --- */

    DescriptorWriter &DescriptorWriter::WriteBuffer(const uint32_t binding, const VkDescriptorBufferInfo bufferInfo)
    {
        // Check if the current binding is not available
        if (descriptorSetLayout.bindings.count(binding) == 0)
        {
            VulkanDebugger::ThrowError("Descriptor set layout does not contain the specified binding: [" + std::to_string(binding) + "]");
        }

        // Get the binding description and check if it expects more than 1 descriptors
        VkDescriptorSetLayoutBinding bindingDescription = descriptorSetLayout.bindings[binding];
        if (bindingDescription.descriptorCount != 1)
        {
            VulkanDebugger::ThrowError("Trying to bind [" + std::to_string(bindingDescription.descriptorCount) + "] descriptors while only 1 at a time is supported");
        }

        // Create write descriptor
        VkWriteDescriptorSet writeDescriptor;
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.descriptorType = bindingDescription.descriptorType;
        writeDescriptor.dstBinding = binding;
        writeDescriptor.pBufferInfo = &bufferInfo;
        writeDescriptor.descriptorCount = 1;

        // Add write descriptor to the list
        writeDescriptorSets.push_back(writeDescriptor);

        return *this;
    }

    DescriptorWriter &DescriptorWriter::WriteImage(const uint32_t binding, const VkDescriptorImageInfo imageInfo)
    {
        // Check if the current binding is not available
        if (descriptorSetLayout.bindings.count(binding) == 0)
        {
            VulkanDebugger::ThrowError("Descriptor set layout does not contain the specified binding: [" + std::to_string(binding) + "]");
        }

        // Get the binding description and check if it expects more than 1 descriptors
        VkDescriptorSetLayoutBinding bindingDescription = descriptorSetLayout.bindings[binding];
        if (bindingDescription.descriptorCount != 1)
        {
            VulkanDebugger::ThrowError("Trying to bind [" + std::to_string(bindingDescription.descriptorCount) + "] descriptors while only 1 at a time is supported");
        }

        // Create write descriptor
        VkWriteDescriptorSet writeDescriptor;
        writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptor.descriptorType = bindingDescription.descriptorType;
        writeDescriptor.dstBinding = binding;
        writeDescriptor.pImageInfo = &imageInfo;
        writeDescriptor.descriptorCount = 1;

        // Add write descriptor to the list
        writeDescriptorSets.push_back(writeDescriptor);

        return *this;
    }

    /* --- CONSTRUCTORS --- */

    void DescriptorWriter::Build(VkDescriptorSet &descriptorSet)
    {
        // Allocate descriptor set to pool
        descriptorPool.AllocateDescriptorSet(descriptorSetLayout.GetVulkanDescriptorSetLayout(), descriptorSet);

        // Update the descriptor sets
        Overwrite(descriptorSet);
    }

    DescriptorWriter::DescriptorWriter(DescriptorSetLayout &givenDescriptorSetLayout, DescriptorPool &givenDescriptorPool)
        : descriptorSetLayout(givenDescriptorSetLayout), descriptorPool(givenDescriptorPool)
    {

    }

    void DescriptorWriter::Overwrite(VkDescriptorSet &descriptorSet)
    {
        for (size_t i = writeDescriptorSets.size(); i--;)
        {
            writeDescriptorSets[i].dstSet = descriptorSet;
        }

        vkUpdateDescriptorSets(VulkanCore::logicalDevice, writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
    }
}
