//
// Created by Nikolay Kanchevski on 9.03.24.
//

#include "VulkanResourceTable.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanSampler.h"
#include "VulkanResultHandler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanResourceTable::VulkanResourceTable(const VulkanDevice& device, const ResourceTableCreateInfo& createInfo)
        : ResourceTable(createInfo), device(device), name(createInfo.name)
    {
        SR_THROW_IF(!device.IsExtensionLoaded(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME), UnsupportedFeatureError(SR_FORMAT("Cannot create resource table [{0}] from device [{1}]", name, device.GetName())));

        // Retrieve descriptor indexing properties
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);

        // Set up pool sizes
        const std::array poolSizes
        {
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = GetUniformBufferCapacity(),
            },
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = GetStorageBufferCapacity(),
            },
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = GetSampledImageCapacity(),
            },
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .descriptorCount = GetStorageImageCapacity(),
            },
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_SAMPLER,
                .descriptorCount = GetSamplerCapacity(),
            }
        };

        // Set up pool create info
        const VkDescriptorPoolCreateInfo descriptorPoolCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .flags = SR_PLATFORM_APPLE * VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
            .maxSets = 1,
            .poolSizeCount = poolSizes.size(),
            .pPoolSizes = poolSizes.data()
        };

        // Create descriptor pool
        VkResult result = device.GetFunctionTable().vkCreateDescriptorPool(device.GetVulkanDevice(), &descriptorPoolCreateInfo, nullptr, &descriptorPool);
        if (result != VK_SUCCESS) HandleVulkanResult(result, SR_FORMAT("Could not create resource table [{0}], as creation of descriptor pool failed", name));
        device.SetResourceName(descriptorPool, VK_OBJECT_TYPE_DESCRIPTOR_POOL, SR_FORMAT("Descriptor pool of resource table [{0}]", name));

        // Set up set allocate info
        VkDescriptorSetLayout descriptorSetLayout = device.GetDescriptorSetLayout();
        const VkDescriptorSetAllocateInfo descriptorSetAllocateInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = descriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &descriptorSetLayout
        };

        // Allocate descriptor set
        result = device.GetFunctionTable().vkAllocateDescriptorSets(device.GetVulkanDevice(), &descriptorSetAllocateInfo, &descriptorSet);
        if (result != VK_SUCCESS) HandleVulkanResult(result, SR_FORMAT("Could not create resource table [{0}], as creation of descriptor set failed", name));
        device.SetResourceName(descriptorSet, VK_OBJECT_TYPE_DESCRIPTOR_SET, SR_FORMAT("Descriptor set of resource table [{0}]", name));
    }

    /* --- POLLING METHODS --- */

    void VulkanResourceTable::BindUniformBuffer(const uint32 index, const Buffer& buffer, const size offset, const size memorySize)
    {
        SR_THROW_IF(buffer.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot bind uniform buffer [{0}] to resource table [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", buffer.GetName(), name)));
        const VulkanBuffer& vulkanBuffer = static_cast<const VulkanBuffer&>(buffer);

        ResourceTable::BindUniformBuffer(index, buffer, offset, memorySize);

        // Set up buffer info
        const VkDescriptorBufferInfo bufferInfo
        {
            .buffer = vulkanBuffer.GetVulkanBuffer(),
            .offset = offset,
            .range = memorySize != 0 ? memorySize : buffer.GetMemorySize()
        };

        // Set up write info
        const VkWriteDescriptorSet writeDescriptorSet
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSet,
            .dstBinding = VulkanDevice::BINDLESS_UNIFORM_BUFFER_BINDING,
            .dstArrayElement = index,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &bufferInfo
        };

        // Update descriptor set
        device.GetFunctionTable().vkUpdateDescriptorSets(device.GetVulkanDevice(), 1, &writeDescriptorSet, 0, nullptr);
    }

    void VulkanResourceTable::BindStorageBuffer(const uint32 index, const Buffer& buffer, const size offset, const size memorySize)
    {
        SR_THROW_IF(buffer.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot bind storage buffer [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", buffer.GetName(), name)));
        const VulkanBuffer& vulkanBuffer = static_cast<const VulkanBuffer&>(buffer);

        ResourceTable::BindStorageBuffer(index, buffer, offset, memorySize);

        // Set up buffer info
        const VkDescriptorBufferInfo bufferInfo
        {
            .buffer = vulkanBuffer.GetVulkanBuffer(),
            .offset = offset,
            .range = memorySize != 0 ? memorySize : buffer.GetMemorySize()
        };

        // Set up write info
        const VkWriteDescriptorSet writeDescriptorSet
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSet,
            .dstBinding = VulkanDevice::BINDLESS_STORAGE_BUFFER_BINDING,
            .dstArrayElement = index,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .pBufferInfo = &bufferInfo
        };

        // Update descriptor set
        device.GetFunctionTable().vkUpdateDescriptorSets(device.GetVulkanDevice(), 1, &writeDescriptorSet, 0, nullptr);
    }

    void VulkanResourceTable::BindSampledImage(const uint32 index, const Image& image)
    {
        SR_THROW_IF(image.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot bind sampled image [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", image.GetName(), name)));
        const VulkanImage& vulkanImage = static_cast<const VulkanImage&>(image);

        ResourceTable::BindSampledImage(index, vulkanImage);

        // Set up image info
        const VkDescriptorImageInfo imageInfo
        {
            .sampler = VK_NULL_HANDLE,
            .imageView = vulkanImage.GetVulkanImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        // Set up write info
        const VkWriteDescriptorSet writeDescriptorSet
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSet,
            .dstBinding = VulkanDevice::BINDLESS_SAMPLED_IMAGE_BINDING,
            .dstArrayElement = index,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .pImageInfo = &imageInfo
        };

        // Update descriptor set
        device.GetFunctionTable().vkUpdateDescriptorSets(device.GetVulkanDevice(), 1, &writeDescriptorSet, 0, nullptr);
    }

    void VulkanResourceTable::BindStorageImage(const uint32 index, const Image& image)
    {
        SR_THROW_IF(image.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot bind storage image [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", image.GetName(), name)));
        const VulkanImage& vulkanImage = static_cast<const VulkanImage&>(image);

        ResourceTable::BindStorageImage(index, vulkanImage);

        // Set up image info
        const VkDescriptorImageInfo imageInfo
        {
            .sampler = VK_NULL_HANDLE,
            .imageView = vulkanImage.GetVulkanImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_GENERAL
        };

        // Set up write info
        const VkWriteDescriptorSet writeDescriptorSet
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSet,
            .dstBinding = VulkanDevice::BINDLESS_STORAGE_IMAGE_BINDING,
            .dstArrayElement = index,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .pImageInfo = &imageInfo
        };

        // Update descriptor set
        device.GetFunctionTable().vkUpdateDescriptorSets(device.GetVulkanDevice(), 1, &writeDescriptorSet, 0, nullptr);
    }

    void VulkanResourceTable::BindSampler(const uint32 index, const Sampler& sampler)
    {
        SR_THROW_IF(sampler.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot bind sampler [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", sampler.GetName(), name)));
        const VulkanSampler& vulkanSampler = static_cast<const VulkanSampler&>(sampler);

        ResourceTable::BindSampler(index, sampler);

        // Set up image info
        const VkDescriptorImageInfo imageInfo
        {
            .sampler = vulkanSampler.GetVulkanSampler(),
            .imageView = VK_NULL_HANDLE,
            .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        // Set up write info
        const VkWriteDescriptorSet writeDescriptorSet
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSet,
            .dstBinding = VulkanDevice::BINDLESS_SAMPLER_BINDING,
            .dstArrayElement = index,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .pImageInfo = &imageInfo
        };

        // Update descriptor set
        device.GetFunctionTable().vkUpdateDescriptorSets(device.GetVulkanDevice(), 1, &writeDescriptorSet, 0, nullptr);
    }

    /* --- GETTER METHODS --- */

    uint32 VulkanResourceTable::GetUniformBufferCapacity() const noexcept
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return glm::min(VulkanDevice::MAX_UNIFORM_BUFFERS_PER_RESOURCE_TABLE, descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindUniformBuffers);
    }

    uint32 VulkanResourceTable::GetStorageBufferCapacity() const noexcept
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return glm::min(VulkanDevice::MAX_STORAGE_BUFFERS_PER_RESOURCE_TABLE, descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageBuffers);
    }

    uint32 VulkanResourceTable::GetSampledImageCapacity() const noexcept
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return glm::min(VulkanDevice::MAX_SAMPLED_IMAGES_PER_RESOURCE_TABLE, descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSampledImages);
    }

    uint32 VulkanResourceTable::GetStorageImageCapacity() const noexcept
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return glm::min(VulkanDevice::MAX_STORAGE_IMAGES_PER_RESOURCE_TABLE, descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageImages);
    }

    uint32 VulkanResourceTable::GetSamplerCapacity() const noexcept
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return glm::min(VulkanDevice::MAX_SAMPLERS_PER_RESOURCE_TABLE, descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSamplers);
    }

    /* --- DESTRUCTOR --- */

    VulkanResourceTable::~VulkanResourceTable() noexcept
    {
        device.GetFunctionTable().vkResetDescriptorPool(device.GetVulkanDevice(), descriptorPool, 0);
        device.GetFunctionTable().vkDestroyDescriptorPool(device.GetVulkanDevice(), descriptorPool, nullptr);
    }

}