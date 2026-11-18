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
        bool Write(uint64 destinationOffset, std::span<const uint8> buffer) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }
        [[nodiscard]] uint64 GetMemorySize() const noexcept override { return memorySize; }

        [[nodiscard]] VkBuffer GetVulkanBuffer() const noexcept { return buffer; }

        /* --- COPY SEMANTICS --- */
        VulkanBuffer(const VulkanBuffer&) = delete;
        VulkanBuffer& operator=(const VulkanBuffer&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanBuffer(VulkanBuffer&&) noexcept = default;
        VulkanBuffer& operator=(VulkanBuffer&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~VulkanBuffer() noexcept override;

    private:
        std::string name = { };
        const VulkanDevice* device = nullptr;

        VkBuffer buffer = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;

        void* mappedMemory = nullptr;
        size memorySize = 0;

    };

}
