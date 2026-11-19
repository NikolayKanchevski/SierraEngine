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

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }

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
        std::string name = { };
        const VulkanDevice* device;

        // Resources queued since the last flush, which are yet to be given the points to wait for
        std::vector<std::unique_ptr<RenderingResource>> unscheduledResources = { };

        // Resources to destroy once the timeline semaphore of every queue reaches the value it had when they were scheduled
        struct ResourceBatch
        {
            std::vector<VulkanSemaphoreValue> semaphoreValuesToWait = { };
            std::vector<std::unique_ptr<RenderingResource>> resources = { };
        };
        std::deque<ResourceBatch> scheduledResourceBatches = { };
    };

}