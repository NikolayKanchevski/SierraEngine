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

        struct ResourceQueueEntry
        {
            uint64 waitValue = 0;
            std::unique_ptr<RenderingResource> resource = nullptr;
        };
        std::deque<ResourceQueueEntry> resourceQueue = { };

    };

}