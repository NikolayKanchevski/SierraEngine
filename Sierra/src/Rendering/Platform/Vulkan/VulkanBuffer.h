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
        VulkanBuffer(const VulkanDevice& device, const BufferCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void CopyFromMemory(const void* memory, size memorySize = 0,  size sourceOffset = 0,  size offset = 0) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const override { return name; }

        [[nodiscard]] std::span<const uint8> GetMemory() const override { return { reinterpret_cast<const uint8*>(memory), memorySize }; }
        [[nodiscard]] uint64 GetMemorySize() const override { return memorySize; }

        [[nodiscard]] VkBuffer GetVulkanBuffer() const { return buffer; }
        [[nodiscard]] VkBufferUsageFlags GetVulkanUsageFlags() const { return usageFlags; }

        /* --- DESTRUCTOR --- */
        ~VulkanBuffer() override;

        /* --- CONVERSIONS --- */
        static VkBufferUsageFlags BufferUsageToVkBufferUsageFlags(BufferUsage bufferType);
        static VmaMemoryUsage BufferMemoryLocationToVmaMemoryUsage(BufferMemoryLocation memoryLocation);

    private:
        const VulkanDevice& device;

        std::string name;
        VkBuffer buffer = VK_NULL_HANDLE;
        VkBufferUsageFlags usageFlags = 0;
        VmaAllocation allocation = VK_NULL_HANDLE;

        void* memory = nullptr;
        size memorySize = 0;

    };

}
