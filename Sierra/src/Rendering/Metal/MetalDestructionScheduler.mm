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

        // NOTE: Command buffers only get their completion values once submitted, so the one being recorded right now (which may well use the resource) has none yet, which is why it is only scheduled on the next flush
        unscheduledResources.emplace_back(std::move(resource));
    }

    void MetalDestructionScheduler::Flush()
    {
        // Destroy resources whose value the semaphore has reached (batches are in the order they were scheduled in, so their values only ever grow)
        const uint64 currentValue = device->GetSemaphore().signaledValue;
        while (!scheduledResourceBatches.empty() && scheduledResourceBatches.front().semaphoreValueToWait <= currentValue)
        {
            scheduledResourceBatches.pop_front();
        }

        // Resources queued since the last flush were used by command buffers submitted since (which flushes must therefore happen after, as they do between frames), so they are destroyed once everything submitted so far completes
        if (!unscheduledResources.empty())
        {
            scheduledResourceBatches.push_back({ .semaphoreValueToWait = device->GetLastSemaphoreSignalValue(), .resources = std::move(unscheduledResources) });
            unscheduledResources.clear();
        }
    }

}