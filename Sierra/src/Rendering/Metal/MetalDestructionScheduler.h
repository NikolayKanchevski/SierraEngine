//
// Created by Nikolay Kanchevski on 30.10.25.
//

#pragma once

#include "../DestructionScheduler.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalDestructionScheduler final : public DestructionScheduler, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MetalDestructionScheduler(const MetalDevice& device, const DestructionSchedulerCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void QueueResource(std::unique_ptr<RenderingResource>&& resource) override;
        void Flush() override;

        /* --- COPY SEMANTICS --- */
        MetalDestructionScheduler(const MetalDestructionScheduler&) = delete;
        MetalDestructionScheduler& operator=(const MetalDestructionScheduler&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalDestructionScheduler(MetalDestructionScheduler&&) noexcept = default;
        MetalDestructionScheduler& operator=(MetalDestructionScheduler&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~MetalDestructionScheduler() noexcept override = default;

    private:
        const MetalDevice* device;

        // Resources queued since the last flush, which are yet to be given the value to wait for
        std::vector<std::unique_ptr<RenderingResource>> unscheduledResources = { };

        // Resources to destroy once the semaphore of the device reaches the value it had when they were scheduled
        struct ResourceBatch
        {
            uint64 semaphoreValueToWait = 0;
            std::vector<std::unique_ptr<RenderingResource>> resources = { };
        };
        std::deque<ResourceBatch> scheduledResourceBatches = { };

    };

}