//
// Created by Nikolay Kanchevski on 5.12.23.
//

#pragma once

#include "../../Buffer.h"
#include "VulkanResource.h"

#include <vk_mem_alloc.h>
#include "VulkanDevice.h"
#include "../../../Engine/MemoryObject.h"

namespace Sierra
{

    class SIERRA_API VulkanBuffer final : public Buffer, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanBuffer(const VulkanDevice &device, const BufferCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void CopyFromMemory(const void* memoryPointer, uint64 memorySize = 0, uint64 sourceOffset = 0, uint64 destinationOffset = 0) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkBuffer GetVulkanBuffer() const { return buffer; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

        /* --- CONVERSIONS --- */
        static VkBufferUsageFlags BufferUsageToVkBufferUsageFlags(BufferUsage bufferType);
        static VmaMemoryUsage BufferMemoryLocationToVmaMemoryUsage(BufferMemoryLocation memoryLocation);

    private:
        const VulkanDevice &device;

        MemoryObject data;
        VkBuffer buffer = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;

    };

}
