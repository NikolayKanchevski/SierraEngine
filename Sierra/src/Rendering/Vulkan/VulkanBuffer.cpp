//
// Created by Nikolay Kanchevski on 5.12.23.
//

#include "VulkanBuffer.h"

#include "VulkanErrorHandler.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */

    VkBufferUsageFlags BufferUsageToVkBufferUsageFlags(const BufferUsage bufferType) noexcept
    {
        VkBufferUsageFlags usageFlags = 0;
        if (bufferType & BufferUsage::SourceMemory)              usageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        if (bufferType & BufferUsage::DestinationMemory)         usageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        if (bufferType & BufferUsage::Uniform)                   usageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if (bufferType & BufferUsage::Storage)                   usageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        if (bufferType & BufferUsage::Index)                     usageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (bufferType & BufferUsage::Vertex)                    usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        return usageFlags;
    }

    VmaMemoryUsage BufferMemoryLocationToVmaMemoryUsage(const BufferMemoryLocation memoryLocation) noexcept
    {
        switch (memoryLocation)
        {
            case BufferMemoryLocation::CPU:       return VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
            case BufferMemoryLocation::GPU:       return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            default:                              break;
        }

        return VMA_MEMORY_USAGE_AUTO;
    }

    /* --- CONSTRUCTORS --- */

    VulkanBuffer::VulkanBuffer(const VulkanDevice& device, const BufferCreateInfo& createInfo)
        : Buffer(createInfo), device(device), name(createInfo.name), usageFlags(BufferUsageToVkBufferUsageFlags(createInfo.usage)), memorySize(createInfo.memorySize)
    {
        SR_THROW_IF(createInfo.usage & BufferUsage::Uniform && createInfo.memorySize > device.GetLimits().maxUniformBufferSize, ValueOutOfRangeError(SR_FORMAT("Cannot create buffer [{0}], as specified memory size is greater than device [{1}]'s max uniform buffer size - use Device::GetLimits() to query limits", name, device.GetName()), createInfo.memorySize, size(0), device.GetLimits().maxUniformBufferSize));
        SR_THROW_IF(createInfo.usage & BufferUsage::Storage && createInfo.memorySize > device.GetLimits().maxStorageBufferSize, ValueOutOfRangeError(SR_FORMAT("Cannot create buffer [{0}], as specified memory size is greater than device [{1}]'s max storage buffer size - use Device::GetLimits() to query limits", name, device.GetName()), createInfo.memorySize, size(0), device.GetLimits().maxUniformBufferSize));

        // Set up buffer create info
        const VkBufferCreateInfo bufferCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = createInfo.memorySize,
            .usage = usageFlags,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        // Set up buffer allocation info
        const VmaAllocationCreateInfo allocationCreateInfo
        {
            .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
            .usage = BufferMemoryLocationToVmaMemoryUsage(createInfo.memoryLocation),
            .memoryTypeBits = std::numeric_limits<uint32>::max(),
            .priority = 0.5f
        };

        // Create and allocate buffer
        const VkResult result = vmaCreateBuffer(device.GetVulkanMemoryAllocator(), &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, nullptr);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create buffer [{0}]", name));
        device.SetResourceName(buffer, VK_OBJECT_TYPE_BUFFER, name);

        // Map and reset memory if CPU-visible
        if (createInfo.memoryLocation == BufferMemoryLocation::CPU)
        {
            vmaMapMemory(device.GetVulkanMemoryAllocator(), allocation, &memory);
            std::memset(memory, 0, createInfo.memorySize);
        }
    }

    /* --- POLLING METHODS --- */

    void VulkanBuffer::Write(const void* memoryPointer, const size sourceOffset, const size destinationOffset, const size memoryPointerSize)
    {
        Buffer::Write(memoryPointer, sourceOffset, destinationOffset, memoryPointerSize);

        std::memcpy(reinterpret_cast<uint8*>(memory) + destinationOffset, reinterpret_cast<const uint8*>(memoryPointer) + sourceOffset, memoryPointerSize);
        vmaFlushAllocation(device.GetVulkanMemoryAllocator(), allocation, destinationOffset, memoryPointerSize);
    }

    /* --- DESTRUCTOR --- */

    VulkanBuffer::~VulkanBuffer() noexcept
    {
        if (memory != nullptr) vmaUnmapMemory(device.GetVulkanMemoryAllocator(), allocation);
        vmaDestroyBuffer(device.GetVulkanMemoryAllocator(), buffer, allocation);
    }

}