//
// Created by Nikolay Kanchevski on 27.10.25.
//

#pragma once

#include "VulkanResource.h"
#include "../DestructionScheduler.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanDestructionScheduler final : public DestructionScheduler, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit VulkanDestructionScheduler(const VulkanDevice& device, const DestructionSchedulerCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void QueueResource(std::unique_ptr<RenderingResource>&& resource) override;
        void Flush() override;

        /* --- COPY SEMANTICS --- */
        VulkanDestructionScheduler(const VulkanDestructionScheduler&) = delete;
        VulkanDestructionScheduler& operator=(const VulkanDestructionScheduler&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanDestructionScheduler(VulkanDestructionScheduler&&) noexcept = default;
        VulkanDestructionScheduler& operator=(VulkanDestructionScheduler&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~VulkanDestructionScheduler() noexcept override = default;

    private:
        const VulkanDevice* device;

        struct ResourceQueueEntry
        {
            uint64 waitValue = 0;
            std::unique_ptr<RenderingResource> resource = nullptr;
        };
        std::deque<ResourceQueueEntry> resourceQueue = { };
    };

}