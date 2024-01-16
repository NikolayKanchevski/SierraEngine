//
// Created by Nikolay Kanchevski on 5.12.23.
//

#pragma once

#include "../../Buffer.h"
#include "VulkanResource.h"

#include <vk_mem_alloc.h>
#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanBuffer final : public Buffer, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanBuffer(const VulkanDevice &device, const BufferCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void CopyFromMemory(const void* memoryPointer, uint64 memoryRange = 0, uint64 sourceOffset = 0, uint64 destinationOffset = 0) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkBuffer GetVulkanBuffer() const { return buffer; }
        [[nodiscard]] inline VkBufferUsageFlags GetVulkanUsageFlags() const { return usageFlags; }

        /* --- DESTRUCTOR --- */
        ~VulkanBuffer() override;

        /* --- CONVERSIONS --- */
        static VkBufferUsageFlags BufferUsageToVkBufferUsageFlags(BufferUsage bufferType);
        static VmaMemoryUsage BufferMemoryLocationToVmaMemoryUsage(BufferMemoryLocation memoryLocation);

    private:
        const VulkanDevice &device;

        VkBuffer buffer = VK_NULL_HANDLE;
        VkBufferUsageFlags usageFlags = 0;
        VmaAllocation allocation = VK_NULL_HANDLE;

    };

}
