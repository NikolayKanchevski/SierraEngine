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

    VulkanResourceTable::VulkanResourceTable(const VulkanDevice &device, const ResourceTableCreateInfo &createInfo)
        : ResourceTable(createInfo), VulkanResource(createInfo.name), device(device)
    {
        SR_ERROR_IF(!device.IsExtensionLoaded(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME), "[Vulkan]: Cannot create resource table [{0}], as the provided device [{1}] does not support the {2} extension!", GetName(), device.GetName(), VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

        // Retrieve descriptor indexing properties
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);

        // Set up pool sizes
        const std::array<VkDescriptorPoolSize, VulkanDevice::BINDLESS_BINDING_COUNT> poolSizes
        {
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = glm::min(descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindUniformBuffers, 250'000U),
            },
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = glm::min(descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageBuffers, 250'000U),
            },
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = glm::min(descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSampledImages >> 1, 250'000U),
            },
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = glm::min(descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSampledImages >> 1, 250'000U),
            },
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .descriptorCount = glm::min(descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageImages >> 1, 250'000U),
            },
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .descriptorCount = glm::min(descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageImages >> 1, 250'000U),
            },
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_SAMPLER,
                .descriptorCount = glm::min(descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSamplers, 250'000U),
            }
        };

        // Set up pool create info
        const VkDescriptorPoolCreateInfo descriptorPoolCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
            .maxSets = 1,
            .poolSizeCount = poolSizes.size(),
            .pPoolSizes = poolSizes.data()
        };

        // Create descriptor pool
        VkResult result = device.GetFunctionTable().vkCreateDescriptorPool(device.GetLogicalDevice(), &descriptorPoolCreateInfo, nullptr, &descriptorPool);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create descriptor pool of resource table [{0}]! Error code: {1}.", GetName(), result);
        device.SetObjectName(descriptorPool, VK_OBJECT_TYPE_DESCRIPTOR_POOL, "Descriptor pool of resource table [" + std::string(GetName()) + "]");

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
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not allocate descriptor set of resource table [{0}]! Error code: {1}.", GetName(), result);
        device.SetObjectName(descriptorSet, VK_OBJECT_TYPE_DESCRIPTOR_SET, "Descriptor set of resource table [" + std::string(GetName()) + "]");
    }

    /* --- POLLING METHODS --- */

    void VulkanResourceTable::BindUniformBuffer(const uint32 index, const std::unique_ptr<Buffer> &buffer, uint64 memoryRange, uint64 byteOffset)
    {
        SR_ERROR_IF(buffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind uniform buffer [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", buffer->GetName(), GetName());
        const VulkanBuffer &vulkanBuffer = static_cast<VulkanBuffer&>(*buffer);

        if (index >= GetUniformBufferCapacity())
        {
            SR_WARNING("[Vulkan]: Cannot bind uniform buffer at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetUniformBufferCapacity() to query uniform buffer capacity.", index, GetName());
            return;
        }

        // Set up buffer info
        const VkDescriptorBufferInfo bufferInfo
        {
            .buffer = vulkanBuffer.GetVulkanBuffer(),
            .offset = byteOffset,
            .range = memoryRange != 0 ? memoryRange : buffer->GetMemorySize()
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

    void VulkanResourceTable::BindStorageBuffer(const uint32 index, const std::unique_ptr<Buffer> &buffer, const uint64 memoryRange, const uint64 byteOffset)
    {
        SR_ERROR_IF(buffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind storage buffer [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", buffer->GetName(), GetName());
        const VulkanBuffer &vulkanBuffer = static_cast<VulkanBuffer&>(*buffer);

        if (index >= GetStorageBufferCapacity())
        {
            SR_WARNING("[Vulkan]: Cannot bind storage buffer at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetStorageBufferCapacity() to query storage buffer capacity.", index, GetName());
            return;
        }

        // Set up buffer info
        const VkDescriptorBufferInfo bufferInfo
        {
            .buffer = vulkanBuffer.GetVulkanBuffer(),
            .offset = byteOffset,
            .range = memoryRange != 0 ? memoryRange : buffer->GetMemorySize()
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

    void VulkanResourceTable::BindSampledImage(const uint32 index, const std::unique_ptr<Image> &image)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind sampled image [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", image->GetName(), GetName());
        const VulkanImage &vulkanImage = static_cast<VulkanImage&>(*image);

        if (index >= GetSampledImageCapacity())
        {
            SR_WARNING("[Vulkan]: Cannot bind sampled image at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetSampledImageCapacity() to query sampled image capacity.", index, GetName());
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

    void VulkanResourceTable::BindSampledCubemap(const uint32 index, const std::unique_ptr<Image> &image)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind sampled cubemap [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", image->GetName(), GetName());
        const VulkanImage &vulkanImage = static_cast<VulkanImage&>(*image);

        if (index >= GetSampledCubemapCapacity())
        {
            SR_WARNING("[Vulkan]: Cannot bind sampled cubemap at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetSampledCubemapCapacity() to query sampled cubemap capacity.", index, GetName());
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
            .dstBinding = VulkanDevice::BINDLESS_SAMPLED_CUBEMAP_BINDING,
            .dstArrayElement = index,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .pImageInfo = &imageInfo
        };

        // Update descriptor set
        device.GetFunctionTable().vkUpdateDescriptorSets(device.GetLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);
    }

    void VulkanResourceTable::BindSampler(const uint32 index, const std::unique_ptr<Sampler> &sampler)
    {
        SR_ERROR_IF(sampler->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind sampler [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", sampler->GetName(), GetName());
        const VulkanSampler &vulkanSampler = static_cast<VulkanSampler&>(*sampler);

        if (index >= GetSamplerCapacity())
        {
            SR_WARNING("[Vulkan]: Cannot bind sampler at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetSamplerCapacity() to query sampler capacity.", index, GetName());
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

    void VulkanResourceTable::BindStorageImage(const uint32 index, const std::unique_ptr<Image> &image)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind storage image [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", image->GetName(), GetName());
        const VulkanImage &vulkanImage = static_cast<VulkanImage&>(*image);

        if (index >= GetStorageImageCapacity())
        {
            SR_WARNING("[Vulkan]: Cannot bind storage image at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetStorageImageCapacity() to query storage image capacity.", index, GetName());
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

    void VulkanResourceTable::BindStorageCubemap(const uint32 index, const std::unique_ptr<Image> &image)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind storage cubemap [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", image->GetName(), GetName());
        const VulkanImage &vulkanImage = static_cast<VulkanImage&>(*image);

        if (index >= GetStorageCubemapCapacity())
        {
            SR_WARNING("[Vulkan]: Cannot bind storage cubemap at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetStorageCubemapCapacity() to query storage cubemap capacity..", index, GetName());
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
            .dstBinding = VulkanDevice::BINDLESS_STORAGE_CUBEMAP_BINDING,
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
        return descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindUniformBuffers;
    }

    uint32 VulkanResourceTable::GetStorageBufferCapacity() const
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageBuffers;
    }

    uint32 VulkanResourceTable::GetSampledImageCapacity() const
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSampledImages >> 1;
    }

    uint32 VulkanResourceTable::GetSampledCubemapCapacity() const
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSampledImages >> 1;
    }

    uint32 VulkanResourceTable::GetSamplerCapacity() const
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSamplers;
    }

    uint32 VulkanResourceTable::GetStorageImageCapacity() const
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageImages >> 1;
    }

    uint32 VulkanResourceTable::GetStorageCubemapCapacity() const
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        device.GetPhysicalDeviceProperties2(&descriptorIndexingProperties);
        return descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageImages >> 1;
    }


    /* --- DESTRUCTOR --- */

    VulkanResourceTable::~VulkanResourceTable()
    {
        device.GetFunctionTable().vkResetDescriptorPool(device.GetLogicalDevice(), descriptorPool, 0);
        device.GetFunctionTable().vkDestroyDescriptorPool(device.GetLogicalDevice(), descriptorPool, nullptr);
    }

}