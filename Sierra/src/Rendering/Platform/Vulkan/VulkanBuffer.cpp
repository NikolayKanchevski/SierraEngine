//
// Created by Nikolay Kanchevski on 5.12.23.
//

#include "VulkanBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanBuffer::VulkanBuffer(const VulkanDevice &device, const BufferCreateInfo &createInfo)
        : Buffer(createInfo), device(device), name(createInfo.name), usageFlags(BufferUsageToVkBufferUsageFlags(createInfo.usage)), memorySize(createInfo.memorySize)
    {
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
        const VkResult result = vmaCreateBuffer(device.GetMemoryAllocator(), &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, nullptr);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Failed to create buffer [{0}]! Error code: {1}.", name, static_cast<int32>(result));
        device.SetResourceName(buffer, VK_OBJECT_TYPE_BUFFER, name);

        // Map and reset memory if CPU-visible
        if (createInfo.memoryLocation == BufferMemoryLocation::CPU)
        {
            vmaMapMemory(device.GetMemoryAllocator(), allocation, &memory);
            std::memset(memory, 0, createInfo.memorySize);
        }
    }

    /* --- POLLING METHODS --- */

    void VulkanBuffer::CopyFromMemory(const void* memoryPointer, uint64 memoryByteSize, const uint64 sourceByteOffset, const uint64 destinationByteOffset)
    {
        memoryByteSize = memoryByteSize != 0 ? memoryByteSize : GetMemorySize();
        SR_ERROR_IF(destinationByteOffset + memoryByteSize > GetMemorySize(), "[Vulkan]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, to buffer [{2}], as the resulting memory space of a total of [{3}] bytes is bigger than the size of the buffer - [{4}]!", memoryByteSize, destinationByteOffset, name, destinationByteOffset + memoryByteSize, GetMemorySize());

        std::memcpy(reinterpret_cast<uint8*>(memory) + destinationByteOffset, reinterpret_cast<const uint8*>(memoryPointer) + sourceByteOffset, memoryByteSize);
        vmaFlushAllocation(device.GetMemoryAllocator(), allocation, destinationByteOffset, memoryByteSize);
    }

    /* --- DESTRUCTOR --- */

    VulkanBuffer::~VulkanBuffer()
    {
        if (allocation != VK_NULL_HANDLE && memory != nullptr) vmaUnmapMemory(device.GetMemoryAllocator(), allocation);
        vmaDestroyBuffer(device.GetMemoryAllocator(), buffer, allocation);
    }

    /* --- CONVERSIONS --- */

    VkBufferUsageFlags VulkanBuffer::BufferUsageToVkBufferUsageFlags(const BufferUsage bufferType)
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

    VmaMemoryUsage VulkanBuffer::BufferMemoryLocationToVmaMemoryUsage(const BufferMemoryLocation memoryLocation)
    {
        switch (memoryLocation)
        {
            case BufferMemoryLocation::CPU:       return VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
            case BufferMemoryLocation::GPU:       return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            default:                              break;
        }
        
        return VMA_MEMORY_USAGE_AUTO;
    }

}