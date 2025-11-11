//
// Created by Nikolay Kanchevski on 27.10.25.
//

#include "VulkanDestructionScheduler.h"

#include "VulkanBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanDestructionScheduler::VulkanDestructionScheduler(const VulkanDevice& device, const DestructionSchedulerCreateInfo& createInfo)
        : VulkanResource(createInfo.name), DestructionScheduler(createInfo), device(&device)
    {

    }

    /* --- POLLING METHODS --- */

    void VulkanDestructionScheduler::QueueResource(std::unique_ptr<RenderingResource>&& resource)
    {
        SR_THROW_IF(resource == nullptr, InvalidValueError(SR_FORMAT("Cannot queue invalid resource [nullptr] for destruction within destruction scheduler [{0}]", GetName())));
        SR_THROW_IF(resource->GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot queue resource [{0}] for destruction within destruction scheduler [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", resource->GetName(), GetName())));

        ResourceQueueEntry entry
        {
            .waitValue = device->GetLastSemaphoreSignalValue(),
            .resource = std::move(resource)
        };

        resourceQueue.emplace_back(std::move(entry));
    }

    void VulkanDestructionScheduler::Flush()
    {
        uint64 currentValue = 0;
        device->GetFunctionTable().vkGetSemaphoreCounterValue(device->GetVulkanDevice(), device->GetSemaphore(), &currentValue);

        while (!resourceQueue.empty() && resourceQueue.front().waitValue <= currentValue)
        {
            resourceQueue.pop_front();
        }
    }

}