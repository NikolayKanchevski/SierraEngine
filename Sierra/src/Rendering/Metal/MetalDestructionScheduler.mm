//
// Created by Nikolay Kanchevski on 30.10.25.
//

#include "MetalDestructionScheduler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalDestructionScheduler::MetalDestructionScheduler(const MetalDevice& device, const DestructionSchedulerCreateInfo& createInfo)
        : MetalResource(createInfo.name), DestructionScheduler(createInfo), device(&device)
    {

    }

    /* --- POLLING METHODS --- */

    void MetalDestructionScheduler::QueueResource(std::unique_ptr<RenderingResource>&& resource)
    {
        SR_THROW_IF(resource == nullptr, InvalidValueError(SR_FORMAT("Cannot queue invalid resource [nullptr] for destruction within destruction scheduler [{0}]", GetName())));
        SR_THROW_IF(resource->GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot queue resource [{0}] for destruction within destruction scheduler [{1}], as its backend type differs from [RenderingBackendType::Metal]", resource->GetName(), GetName())));

        ResourceQueueEntry entry
        {
            .waitValue = device->GetNewSemaphoreSignalValue(),
            .resource = std::move(resource)
        };

        resourceQueue.emplace_back(std::move(entry));
    }

    void MetalDestructionScheduler::Flush()
    {
        const uint64 currentValue = device->GetSemaphore().signaledValue;
        while (!resourceQueue.empty() && resourceQueue.front().waitValue <= currentValue)
        {
            resourceQueue.pop_front();
        }
    }

}