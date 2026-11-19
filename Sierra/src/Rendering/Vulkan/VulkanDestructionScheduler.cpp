//
// Created by Nikolay Kanchevski on 27.10.25.
//

#include "VulkanDestructionScheduler.h"

#include "VulkanBuffer.h"
#include "VulkanErrorHandler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanDestructionScheduler::VulkanDestructionScheduler(const VulkanDevice& device, const DestructionSchedulerCreateInfo& createInfo)
        : DestructionScheduler(createInfo), name(createInfo.name), device(&device)
    {

    }

    /* --- POLLING METHODS --- */

    void VulkanDestructionScheduler::QueueResource(std::unique_ptr<RenderingResource>&& resource)
    {
        SR_THROW_IF(resource == nullptr, InvalidValueError(SR_FORMAT("Cannot queue invalid resource [nullptr] for destruction within destruction scheduler [{0}]", GetName())));
        SR_THROW_IF(resource->GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot queue resource [{0}] for destruction within destruction scheduler [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", resource->GetName(), GetName())));

        // NOTE: Command buffers only get their completion values once submitted, so the one being recorded right now (which may well use the resource) has none yet, which is why it is only scheduled on the next flush
        unscheduledResources.emplace_back(std::move(resource));
    }

    void VulkanDestructionScheduler::Flush()
    {
        // Destroy resources whose every semaphore has reached its value (batches are in the order they were scheduled in, so values within each only ever grow)
        while (!scheduledResourceBatches.empty())
        {
            const ResourceBatch& resourceBatch = scheduledResourceBatches.front();
            const bool completed = std::ranges::all_of(resourceBatch.semaphoreValuesToWait, [this](const VulkanSemaphoreValue& semaphoreValue) -> bool
            {
                uint64 currentValue = 0;
                const VkResult result = device->GetFunctionTable().vkGetSemaphoreCounterValue(device->GetVulkanDevice(), semaphoreValue.semaphore, &currentValue);
                if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not flush destruction scheduler [{0}], as reading the value of a semaphore failed", GetName()));

                return currentValue >= semaphoreValue.value;
            });

            if (!completed) break;
            scheduledResourceBatches.pop_front();
        }

        // Resources queued since the last flush were used by command buffers submitted since (which flushes must therefore happen after, as they do between frames), so they are destroyed once everything submitted so far completes
        if (!unscheduledResources.empty())
        {
            scheduledResourceBatches.push_back({ .semaphoreValuesToWait = device->GetLastQueueSignalledSemaphoreValues(), .resources = std::move(unscheduledResources) });
            unscheduledResources.clear();
        }
    }

}