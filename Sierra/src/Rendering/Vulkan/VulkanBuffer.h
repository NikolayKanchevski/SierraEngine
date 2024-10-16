//
// Created by Nikolay Kanchevski on 5.12.23.
//

#pragma once

#include "../Buffer.h"
#include "VulkanResource.h"

#include <vk_mem_alloc.h>
#include "VulkanDevice.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API VkBufferUsageFlags BufferUsageToVkBufferUsageFlags(BufferUsage bufferType) noexcept;
    [[nodiscard]] SIERRA_API VmaMemoryUsage BufferMemoryLocationToVmaMemoryUsage(BufferMemoryLocation memoryLocation) noexcept;

    class SIERRA_API VulkanBuffer final : public Buffer, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanBuffer(const VulkanDevice& device, const BufferCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Write(const void* memory, size sourceOffset, size destinationOffset, size memorySize) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }

        [[nodiscard]] void* GetMemory() const noexcept override { return memory; }
        [[nodiscard]] size GetMemorySize() const noexcept override { return memorySize; }

        [[nodiscard]] VkBuffer GetVulkanBuffer() const noexcept { return buffer; }
        [[nodiscard]] VkBufferUsageFlags GetVulkanUsageFlags() const noexcept { return usageFlags; }

        /* --- COPY SEMANTICS --- */
        VulkanBuffer(const VulkanBuffer&) = delete;
        VulkanBuffer& operator=(const VulkanBuffer&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanBuffer(VulkanBuffer&&) = delete;
        VulkanBuffer& operator=(VulkanBuffer&&) = delete;

        /* --- DESTRUCTOR --- */
        ~VulkanBuffer() noexcept override;

    private:
        const VulkanDevice& device;
        const std::string name;

        VkBuffer buffer = VK_NULL_HANDLE;
        VkBufferUsageFlags usageFlags = 0;
        VmaAllocation allocation = VK_NULL_HANDLE;

        void* memory = nullptr;
        size memorySize = 0;

    };

}
