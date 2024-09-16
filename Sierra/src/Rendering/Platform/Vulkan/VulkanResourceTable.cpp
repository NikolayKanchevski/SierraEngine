//
// Created by Nikolay Kanchevski on 9.03.24.
//

#include "VulkanResourceTable.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanSampler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanResourceTable::VulkanResourceTable(const VulkanDevice& device, const ResourceTableCreateInfo& createInfo)
        : ResourceTable(createInfo), device(device), name(createInfo.name)
    {
        SR_ERROR_IF(!device.IsExtensionLoaded(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME), "[Vulkan]: Cannot create resource table [{0}], as the provided device [{1}] does not support the {2} extension!", name, device.GetName(), VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

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
        VkResult result = device.GetFunctionTable().vkCreateDescriptorPool(device.GetLogicalDevice(), &descriptorPoolCreateInfo, nullptr, &descriptorPool);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create descriptor pool of resource table [{0}]! Error code: {1}.", name, static_cast<int32>(result));
        device.SetResourceName(descriptorPool, VK_OBJECT_TYPE_DESCRIPTOR_POOL, fmt::format("Descriptor pool of resource table [{0}]", name));

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
        result = device.GetFunctionTable().vkAllocateDescriptorSets(device.GetLogicalDevice(), &descriptorSetAllocateInfo, &descriptorSet);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not allocate descriptor set of resource table [{0}]! Error code: {1}.", name, static_cast<int32>(result));
        device.SetResourceName(descriptorSet, VK_OBJECT_TYPE_DESCRIPTOR_SET, fmt::format("Descriptor set of resource table [{0}]", name));
    }

    /* --- POLLING METHODS --- */

    void VulkanResourceTable::BindUniformBuffer(const ResourceIndex index, const Buffer& buffer, const size memorySize, const size offset)
    {
        SR_ERROR_IF(buffer.GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind uniform buffer [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", buffer.GetName(), name);
        const VulkanBuffer& vulkanBuffer = static_cast<const VulkanBuffer&>(buffer);

        if (index >= GetUniformBufferCapacity())
        {
            SR_WARNING("[Vulkan]: Cannot bind uniform buffer at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetUniformBufferCapacity() to query uniform buffer capacity.", index, name);
            return;
        }

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
        device.GetFunctionTable().vkUpdateDescriptorSets(device.GetLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);
    }

    void VulkanResourceTable::BindStorageBuffer(const ResourceIndex index, const Buffer& buffer, const size memorySize, const size offset)
    {
        SR_ERROR_IF(buffer.GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind storage buffer [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", buffer.GetName(), name);
        const VulkanBuffer& vulkanBuffer = static_cast<const VulkanBuffer&>(buffer);

        if (index >= GetStorageBufferCapacity())
        {
            SR_WARNING("[Vulkan]: Cannot bind storage buffer at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetStorageBufferCapacity() to query storage buffer capacity.", index, name);
            return;
        }

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
        device.GetFunctionTable().vkUpdateDescriptorSets(device.GetLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);
    }

    void VulkanResourceTable::BindSampledImage(const ResourceIndex index, const Image& image)
    {
        SR_ERROR_IF(image.GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind sampled image [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", image.GetName(), name);
        const VulkanImage& vulkanImage = static_cast<const VulkanImage&>(image);

        if (index >= GetSampledImageCapacity())
        {
            SR_WARNING("[Vulkan]: Cannot bind sampled image at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetSampledImageCapacity() to query sampled image capacity.", index, name);
            return;
        }

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
        device.GetFunctionTable().vkUpdateDescriptorSets(device.GetLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);
    }

    void VulkanResourceTable::BindSampler(const ResourceIndex index, const Sampler& sampler)
    {
        SR_ERROR_IF(sampler.GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind sampler [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", sampler.GetName(), name);
        const VulkanSampler& vulkanSampler = static_cast<const VulkanSampler&>(sampler);

        if (index >= GetSamplerCapacity())
        {
            SR_WARNING("[Vulkan]: Cannot bind sampler at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetSamplerCapacity() to query sampler capacity.", index, name);
            return;
        }

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
        device.GetFunctionTable().vkUpdateDescriptorSets(device.GetLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);
    }

    void VulkanResourceTable::BindStorageImage(const ResourceIndex index, const Image& image)
    {
        SR_ERROR_IF(image.GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind storage image [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", image.GetName(), name);
        const VulkanImage& vulkanImage = static_cast<const VulkanImage&>(image);

        if (index >= GetStorageImageCapacity())
        {
            SR_WARNING("[Vulkan]: Cannot bind storage image at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetStorageImageCapacity() to query storage image capacity.", index, name);
            return;
        }

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
        device.GetFunctionTable().vkUpdateDescriptorSets(device.GetLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);
    }

    /* --- GETTER METHODS --- */

    uint32 VulkanResourceTable::GetUniformBufferCapacity() const
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return glm::min(VulkanDevice::MAX_UNIFORM_BUFFERS_PER_RESOURCE_TABLE, descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindUniformBuffers);
    }

    uint32 VulkanResourceTable::GetStorageBufferCapacity() const
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return glm::min(VulkanDevice::MAX_STORAGE_BUFFERS_PER_RESOURCE_TABLE, descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageBuffers);
    }

    uint32 VulkanResourceTable::GetSampledImageCapacity() const
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return glm::min(VulkanDevice::MAX_SAMPLED_IMAGES_PER_RESOURCE_TABLE, descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSampledImages);
    }

    uint32 VulkanResourceTable::GetStorageImageCapacity() const
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return glm::min(VulkanDevice::MAX_STORAGE_IMAGES_PER_RESOURCE_TABLE, descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageImages);
    }

    uint32 VulkanResourceTable::GetSamplerCapacity() const
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return glm::min(VulkanDevice::MAX_SAMPLERS_PER_RESOURCE_TABLE, descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSamplers);
    }

    /* --- DESTRUCTOR --- */

    VulkanResourceTable::~VulkanResourceTable()
    {
        device.GetFunctionTable().vkResetDescriptorPool(device.GetLogicalDevice(), descriptorPool, 0);
        device.GetFunctionTable().vkDestroyDescriptorPool(device.GetLogicalDevice(), descriptorPool, nullptr);
    }

}