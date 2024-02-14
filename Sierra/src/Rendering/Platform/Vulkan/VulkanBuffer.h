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
        void CopyFromMemory(const void* memoryPointer, uint64 memoryRange = 0, uint64 sourceByteOffset = 0, uint64 destinationByteOffset = 0) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const void* GetData() const override { SR_ERROR_IF(memoryLocation != BufferMemoryLocation::CPU, "[Vulkan]: Cannot get data pointer of buffer [{0}], as it is not CPU-visible!", GetName()); return data; }
        [[nodiscard]] inline uint64 GetMemorySize() const override { return memorySize; }
        [[nodiscard]] inline BufferMemoryLocation GetMemoryLocation() const override { return memoryLocation; }

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

        void* data = nullptr;
        uint64 memorySize = 0;
        BufferMemoryLocation memoryLocation = BufferMemoryLocation::CPU;

    };

}
