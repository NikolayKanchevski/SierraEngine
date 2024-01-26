//
// Created by Nikolay Kanchevski on 5.12.23.
//

#include "VulkanBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanBuffer::VulkanBuffer(const VulkanDevice &device, const BufferCreateInfo &createInfo)
        : Buffer(createInfo), VulkanResource(createInfo.name), device(device), usageFlags(BufferUsageToVkBufferUsageFlags(createInfo.usage)), memorySize(createInfo.memorySize), memoryLocation(createInfo.memoryLocation)
    {
        // Set up buffer create info
        VkBufferCreateInfo bufferCreateInfo = { };
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = createInfo.memorySize;
        bufferCreateInfo.usage = usageFlags;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Set up buffer allocation info
        VmaAllocationCreateInfo allocationCreateInfo = { };
        allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
        allocationCreateInfo.usage = BufferMemoryLocationToVmaMemoryUsage(createInfo.memoryLocation);
        allocationCreateInfo.memoryTypeBits = std::numeric_limits<uint32>::max();
        allocationCreateInfo.memoryTypeBits = 0;
        allocationCreateInfo.priority = 0.5f;

        // Create and allocate buffer
        const VkResult result = vmaCreateBuffer(device.GetMemoryAllocator(), &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, nullptr);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Failed to create buffer [{0}]! Error code: {1}.", GetName(), result);

        // Map and reset memory if CPU-visible
        if (createInfo.memoryLocation == BufferMemoryLocation::CPU)
        {
            vmaMapMemory(device.GetMemoryAllocator(), allocation, &data);
            std::memset(data, 0, createInfo.memorySize);
        }
    }

    /* --- POLLING METHODS --- */

    void VulkanBuffer::CopyFromMemory(const void* memoryPointer, uint64 memoryRange, const uint64 sourceOffset, const uint64 destinationOffset)
    {
        memoryRange = memoryRange != 0 ? memoryRange : GetMemorySize();
        SR_ERROR_IF(memoryLocation != BufferMemoryLocation::CPU, "[Vulkan]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, to buffer [{2}], as it is not CPU-visible!", memoryRange, destinationOffset, GetName());
        SR_ERROR_IF(destinationOffset + memoryRange > GetMemorySize(), "[Vulkan]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, to buffer [{2}], as the resulting memory space of a total of [{3}] bytes is bigger than the size of the buffer - [{4}]!", memoryRange, destinationOffset, GetName(), destinationOffset + memoryRange, GetMemorySize());

        std::memcpy(reinterpret_cast<char*>(data) + destinationOffset, reinterpret_cast<const char*>(memoryPointer) + sourceOffset, memoryRange);
        vmaFlushAllocation(device.GetMemoryAllocator(), allocation, destinationOffset, memoryRange);
    }

    /* --- DESTRUCTOR --- */

    VulkanBuffer::~VulkanBuffer()
    {
        if (memoryLocation == BufferMemoryLocation::CPU) vmaUnmapMemory(device.GetMemoryAllocator(), allocation);
        vmaDestroyBuffer(device.GetMemoryAllocator(), buffer, allocation);
    }

    /* --- CONVERSIONS --- */

    VkBufferUsageFlags VulkanBuffer::BufferUsageToVkBufferUsageFlags(const BufferUsage bufferType)
    {
        VkBufferUsageFlags usageFlags = 0;
        if (bufferType & BufferUsage::SourceTransfer)            usageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        if (bufferType & BufferUsage::DestinationTransfer)       usageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
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