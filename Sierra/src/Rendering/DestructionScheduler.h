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
        virtual ~DestructionScheduler() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit DestructionScheduler(const DestructionSchedulerCreateInfo& createInfo);

        /* --- MOVE SEMANTICS --- */
        DestructionScheduler(DestructionScheduler&&) noexcept = default;
        DestructionScheduler& operator=(DestructionScheduler&&) noexcept = default;

    };

}