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

    DescriptorSetLayout::DescriptorSetLayout(const std::unordered_map<uint, DescriptorSetLayoutBinding>& givenBindings)
            : bindings(givenBindings)
    {
        // Create a pointer to layout binding array
        VkDescriptorSetLayoutBinding* layoutBindings = new VkDescriptorSetLayoutBinding[givenBindings.size()];

        VkDescriptorBindingFlags* bindingFlags = new VkDescriptorBindingFlags[bindings.size()];

        // Foreach pair in the provided tuple retrieve the created set layout binding
        uint i = 0;
        for (const auto pair : givenBindings)
        {
            layoutBindings[i] = pair.second.bindingInfo;
            bindingFlags[i] = pair.second.bindingFlags;

            i++;
        }

        // Set up the layout creation info
        VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount = static_cast<uint>(givenBindings.size());
        layoutCreateInfo.pBindings = layoutBindings;

        VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo{};
        if (VK::GetDevice()->GetDescriptorIndexingSupported())
        {
            bindingFlagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
            bindingFlagsCreateInfo.bindingCount = bindings.size();
            bindingFlagsCreateInfo.pBindingFlags = bindingFlags;
            layoutCreateInfo.pNext = &bindingFlagsCreateInfo;
        }

        // Create the Vulkan descriptor set layout
        VK_ASSERT(
            vkCreateDescriptorSetLayout(VK::GetLogicalDevice(), &layoutCreateInfo, nullptr, &vkDescriptorSetLayout),
            fmt::format("Failed to create descriptor layout with [{0}] binging(s)", layoutCreateInfo.bindingCount)
        );

        delete[] layoutBindings;
        delete[] bindingFlags;
    }

    SharedPtr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const
    {
        // Create the descriptor set layout
        return std::make_shared<DescriptorSetLayout>(this->bindings);
    }

    /* --- SETTER METHODS --- */

    DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::SetShaderStages(const ShaderType &givenShaderStages)
    {
        this->shaderStages = givenShaderStages;
        return *this;
    }

    DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::AddBinding(const uint binding, const DescriptorType descriptorType, const ShaderType givenShaderStages, const VkDescriptorBindingFlags bindingFlags, const uint arraySize, const VkSampler *immutableSamplers)
    {
        ASSERT_ERROR_IF(givenShaderStages == ShaderType::NONE, "No shader stages specified for descriptor set layout");

        ASSERT_ERROR_FORMATTED_IF(bindings.count(binding) != 0, "Binding [{0}] already in use by a [{1}] descriptor", binding, bindings[binding].bindingInfo.descriptorType);

        // Set up the layout binding info
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = (VkDescriptorType) descriptorType;
        layoutBinding.descriptorCount = arraySize;
        layoutBinding.stageFlags = (VkShaderStageFlagBits) givenShaderStages;
        layoutBinding.pImmutableSamplers = immutableSamplers;

        // Add the binding info to the tuple list
        this->bindings[binding] = { layoutBinding, bindingFlags, arraySize };

        return *this;
    }

    DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::AddBinding(const uint binding, const DescriptorType descriptorType, const VkDescriptorBindingFlags bindingFlags, const uint arraySize, const VkSampler *immutableSamplers)
    {
        AddBinding(binding, descriptorType, (ShaderType) this->shaderStages, bindingFlags, arraySize, immutableSamplers);

        return *this;
    }

    /* --- DESTRUCTOR --- */

    void DescriptorSetLayout::Destroy()
    {
        // Destroy the Vulkan descriptor set
        vkDestroyDescriptorSetLayout(VK::GetLogicalDevice(), vkDescriptorSetLayout, nullptr);
    }

    // ********************* Descriptor Pool ********************* \\

    /* --- SETTER METHODS --- */

    DescriptorPool::Builder &DescriptorPool::Builder::AddPoolSize(const DescriptorType descriptorType)
    {
        // Add the pool size to the list of pool sizes
        this->poolSizes.push_back({ (VkDescriptorType) descriptorType, maxSets });
        return *this;
    }

    DescriptorPool::Builder &DescriptorPool::Builder::SetPoolFlags(const VkDescriptorPoolCreateFlags givenPoolCreateFlags)
    {
        // Set the pool creation flags
        this->poolCreateFlags = givenPoolCreateFlags;
        return *this;
    }

    DescriptorPool::Builder &DescriptorPool::Builder::SetMaxSets(const uint givenMaxSets)
    {
        // Set the max set value
        this->maxSets = givenMaxSets;
        return *this;
    }

    void DescriptorPool::AllocateDescriptorSet(const SharedPtr<DescriptorSetLayout> &descriptorSetLayout, VkDescriptorSet &descriptorSet)
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
            vkAllocateDescriptorSets(VK::GetLogicalDevice(), &allocateInfo, &descriptorSet),
            "Failed to allocate descriptor set"
        );
    }

    void DescriptorPool::AllocateBindlessDescriptorSet(const std::vector<uint> &bindings, const SharedPtr<DescriptorSetLayout> &descriptorSetLayout, VkDescriptorSet &descriptorSet)
    {
        VkDescriptorSetVariableDescriptorCountAllocateInfo variableDescriptorCountAllocateInfo{};
        variableDescriptorCountAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;

        uint* descriptorCounts = new uint[bindings.size()];
        for (uint i = bindings.size(); i--;)
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
            vkAllocateDescriptorSets(VK::GetLogicalDevice(), &allocateInfo, &descriptorSet),
            "Failed to allocate descriptor set"
        );

        delete[] descriptorCounts;
    }

    /* --- CONSTRUCTORS --- */

    UniquePtr<DescriptorPool> DescriptorPool::Builder::Build()
    {
        // Create the descriptor pool
        return std::make_unique<DescriptorPool>(this->maxSets, this->poolCreateFlags, this->poolSizes);
    }

    DescriptorPool::DescriptorPool(uint givenMaxSets, VkDescriptorPoolCreateFlags givenPoolCreateFlags, std::vector<VkDescriptorPoolSize> givenPoolSizes)
    {
        // Set up the descriptor pool creation info
        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = static_cast<uint>(givenPoolSizes.size());
        descriptorPoolCreateInfo.maxSets = givenMaxSets;
        descriptorPoolCreateInfo.flags = givenPoolCreateFlags;
        descriptorPoolCreateInfo.pPoolSizes = givenPoolSizes.data();

        // Create the Vulkan descriptor pool
        VK_ASSERT(
            vkCreateDescriptorPool(VK::GetLogicalDevice(), &descriptorPoolCreateInfo, nullptr, &vkDescriptorPool),
            fmt::format("Failed to create descriptor pool with [{0}] max sets and [{1}] pool sizes", givenMaxSets, descriptorPoolCreateInfo.poolSizeCount)
        );
    }

    /* --- DESTRUCTOR --- */

    void DescriptorPool::Destroy()
    {
        // Destroy the Vulkan descriptor pool
        vkDestroyDescriptorPool(VK::GetLogicalDevice(), this->vkDescriptorPool, nullptr);
    }

    // ********************* Descriptor Set ********************* \\

    /* --- CONSTRUCTORS --- */

    DescriptorSet::DescriptorSet(SharedPtr<DescriptorSetLayout> &descriptorSetLayout)
        : descriptorSetLayout(descriptorSetLayout)
    {
        // Create descriptor set to pool
        VK::GetDescriptorPool()->AllocateDescriptorSet(descriptorSetLayout, this->vkDescriptorSet);
    }

    UniquePtr<DescriptorSet> DescriptorSet::Build(SharedPtr<DescriptorSetLayout> &givenDescriptorSetLayout)
    {
        return std::make_unique<DescriptorSet>(givenDescriptorSetLayout);
    }

    /* --- SETTER METHODS --- */

    void DescriptorSet::WriteBuffer(uint binding, const Buffer *buffer)
    {
        // Check if the current binding is not available
        ASSERT_ERROR_FORMATTED_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [{0}]", binding);

        // Get the binding description and check if it expects more than 1 descriptors
        VkDescriptorSetLayoutBinding bindingDescription = descriptorSetLayout->bindings[binding].bindingInfo;

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.offset = 0;
        bufferInfo.range = buffer->GetMemorySize();
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

    void DescriptorSet::WriteBuffer(const uint binding, const UniquePtr<Buffer> &buffer)
    {
        WriteBuffer(binding, buffer.get());
    }

    void DescriptorSet::WriteBuffer(const uint binding, const SharedPtr<Buffer> &buffer)
    {
        WriteBuffer(binding, buffer.get());
    }

    void DescriptorSet::WriteImage(const uint binding, const UniquePtr<Image> &image, const UniquePtr<Sampler> &sampler, const ImageLayout imageLayout)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = sampler->GetVulkanSampler();
        imageInfo.imageView = image->GetVulkanImageView();
        imageInfo.imageLayout = (VkImageLayout) imageLayout;

        descriptorImageInfos[binding] = imageInfo;
        WriteImage(binding, &descriptorImageInfos[binding]);
    }

    void DescriptorSet::WriteImage(const uint binding, const VkDescriptorImageInfo *imageInfo)
    {
        // Check if the current binding is not available
        ASSERT_ERROR_FORMATTED_IF(!descriptorSetLayout->IsBindingPresent(binding), "Descriptor set layout does not contain the specified binding: [{0}]", binding);

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

    void DescriptorSet::WriteTexture(const uint binding, const SharedPtr<Texture> &texture)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = texture->GetVulkanSampler();
        imageInfo.imageLayout = (VkImageLayout) texture->GetImage()->GetLayout();
        imageInfo.imageView = texture->GetImage()->GetVulkanImageView();

        descriptorImageInfos[binding] = imageInfo;
        WriteImage(binding, &descriptorImageInfos[binding]);
    }

    void DescriptorSet::WriteCubemap(const uint binding, const UniquePtr<Cubemap> &cubemap)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = cubemap->GetSampler()->GetVulkanSampler();
        imageInfo.imageLayout = (VkImageLayout) cubemap->GetImage()->GetLayout();
        imageInfo.imageView = cubemap->GetImage()->GetVulkanImageView();

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
        vkUpdateDescriptorSets(VK::GetLogicalDevice(), writeDescriptorSets.size(), writeSets.data(), 0, nullptr);
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
        VK::GetDescriptorPool()->AllocateBindlessDescriptorSet(givenBindings, descriptorSetLayout, this->vkDescriptorSet);
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

        VkDescriptorSetLayoutBinding bindingDescription = descriptorSetLayout->bindings[binding].bindingInfo;

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.offset = 0;
        bufferInfo.range = buffer->GetMemorySize();
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
