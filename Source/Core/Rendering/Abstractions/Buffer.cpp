//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Buffer.h"

#include "../Bases/VK.h"


namespace Sierra::Rendering
{

    /* --- CONSTRUCTORS --- */

    Buffer::Buffer(const BufferCreateInfo &createInfo)
        : data(Engine::MemoryObject(createInfo.memorySize, 0)), bufferUsage(createInfo.bufferUsage)
    {
        // Set up buffer creation info
        VkBufferCreateInfo vkBufferCreateInfo{};
        vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vkBufferCreateInfo.size = data.GetMemorySize();
        vkBufferCreateInfo.usage = static_cast<VkBufferUsageFlags>(createInfo.bufferUsage);
        vkBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Set up buffer allocation info
        VmaAllocationCreateInfo allocationCreateInfo{};
        allocationCreateInfo.flags = static_cast<VmaAllocationCreateFlags>(VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        allocationCreateInfo.memoryTypeBits = 0;
        allocationCreateInfo.priority = 0.5f;

        // Create and allocate buffer
        VK_ASSERT(
            vmaCreateBuffer(VK::GetMemoryAllocator(), &vkBufferCreateInfo, &allocationCreateInfo, &vkBuffer, &vmaBufferAllocation, nullptr),
            FORMAT_STRING("Failed to create buffer with size of [{0}] for [{1}] usage", data.GetMemorySize(), VK_TO_STRING(bufferUsage, BufferUsageFlagBits))
        );

        // Map memory
        vmaMapMemory(VK::GetMemoryAllocator(), vmaBufferAllocation, &data.GetData());
        memset(data.GetData(), 0, data.GetMemorySize());
    }

    UniquePtr<Buffer> Buffer::Create(const BufferCreateInfo &createInfo)
    {
        return std::make_unique<Buffer>(createInfo);
    }

    /* --- SETTER METHODS --- */

    void Buffer::Flush()
    {
        vmaFlushAllocation(VK::GetMemoryAllocator(), vmaBufferAllocation, 0, data.GetMemorySize());
    }

    void Buffer::CopyFromPointer(const void* pointer, const uint64 size, const uint64 offset)
    {
        // Copy memory data to Vulkan buffer
        data.SetDataByOffset(pointer, size != 0 ? size : data.GetMemorySize(), offset);
        Flush();
    }

    /* --- DESTRUCTOR --- */

    void Buffer::Destroy()
    {
        // Unmap the memory
        vmaUnmapMemory(VK::GetMemoryAllocator(), vmaBufferAllocation);
        vmaDestroyBuffer(VK::GetMemoryAllocator(), vkBuffer, vmaBufferAllocation);
        vkBuffer = VK_NULL_HANDLE;
    }
}