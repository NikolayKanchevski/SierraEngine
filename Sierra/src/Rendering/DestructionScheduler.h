//
// Created by Nikolay Kanchevski on 27.10.25.
//

#pragma once

#include "RenderingResource.h"

#include "Buffer.h"
#include "Image.h"

namespace Sierra
{

    struct DestructionSchedulerCreateInfo
    {
        std::string_view name = "Destruction Scheduler";
    };

    class SIERRA_API DestructionScheduler : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        // NOTE: Queued resources are destroyed once all work submitted prior to the following flush completes, so they must only be used by command buffers submitted before it (which is
        // the case when flushing once per frame, before recording it, as long as command buffers recorded while resources get queued are submitted within the same frame)
        virtual void QueueResource(std::unique_ptr<RenderingResource>&& resource) = 0;
        virtual void Flush() = 0;

        template<RenderingResourceType... ResourceTypes>
        void QueueResources(std::unique_ptr<ResourceTypes>&&... resources)
        {
            (QueueResource(std::move(resources)), ...);
        }

        /* --- COPY SEMANTICS --- */
        DestructionScheduler(const DestructionScheduler&) = delete;
        DestructionScheduler& operator=(const DestructionScheduler&) = delete;

        /* --- DESTRUCTOR --- */
        ~DestructionScheduler() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit DestructionScheduler(const DestructionSchedulerCreateInfo& createInfo);

        /* --- MOVE SEMANTICS --- */
        DestructionScheduler(DestructionScheduler&&) noexcept = default;
        DestructionScheduler& operator=(DestructionScheduler&&) noexcept = default;

    };

}