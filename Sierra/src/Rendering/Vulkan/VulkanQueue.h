//
// Created by Nikolay Kanchevski on 13.10.24.
//
#pragma once

#include "../Queue.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanQueue final : public Queue, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanQueue(const VulkanDevice& device, const QueueCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<CommandBuffer> CreateCommandBuffer(const CommandBufferCreateInfo& createInfo) const noexcept override;
        void SubmitCommandBuffer(const CommandBuffer& commandBuffer, std::span<const std::reference_wrapper<const CommandBuffer>> commandBuffersToWait) const override;
        void WaitForCommandBuffer(const CommandBuffer& commandBuffer) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }
        [[nodiscard]] QueueOperations GetOperations() const noexcept override { return description->operations; }

        [[nodiscard]] uint32 GetFamily() const noexcept { return description->family; }
        [[nodiscard]] const VulkanDevice& GetDevice() const noexcept { return device; }

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator==(const VulkanQueue& other) noexcept { return description->family == other.description->family; }
        [[nodiscard]] bool operator!=(const VulkanQueue& other) noexcept { return !(*this == other); }

        /* --- COPY SEMANTICS --- */
        VulkanQueue(const VulkanQueue&) = delete;
        VulkanQueue& operator=(const VulkanQueue&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanQueue(VulkanQueue&&) = delete;
        VulkanQueue& operator=(VulkanQueue&&) = delete;

        /* --- DESTRUCTOR --- */
        ~VulkanQueue() noexcept override = default;

    private:
        const VulkanDevice& device;
        const std::string name;

        VkQueue queue = VK_NULL_HANDLE;
        std::shared_ptr<VulkanQueueDescription> description;

    };

}