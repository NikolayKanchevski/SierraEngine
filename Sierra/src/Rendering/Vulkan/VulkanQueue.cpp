//
// Created by Nikolay Kanchevski on 13.10.24.
//

#include "VulkanQueue.h"

#include "VulkanCommandBuffer.h"
#include "VulkanErrorHandler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanQueue::VulkanQueue(const VulkanDevice& givenDevice, const QueueCreateInfo& createInfo)
        : Queue(createInfo), name(createInfo.name), device(&givenDevice)
    {
        switch (createInfo.priority)
        {
            case QueuePriority::Dedicated:
            {
                for (const std::shared_ptr<VulkanQueueDescription>& queueDescription : device->GetQueueDescriptions())
                {
                    if (queueDescription.use_count() == 0 && queueDescription->operations & createInfo.operations)
                    {
                        description = queueDescription;
                        device->GetFunctionTable().vkGetDeviceQueue(device->GetVulkanDevice(), description->family, 0, &queue);
                        break;
                    }
                }
                if (queue != VK_NULL_HANDLE) break;

                // NOTE: We specifically do not break here, such that if a dedicated queue was not found we search for any supporting requested operations
                [[fallthrough]];
            }
            case QueuePriority::LeastUsed:
            {
                const std::shared_ptr<VulkanQueueDescription>* leastUsedQueueDescription = nullptr;
                for (const std::shared_ptr<VulkanQueueDescription>& queueDescription : device->GetQueueDescriptions())
                {
                    if ((leastUsedQueueDescription == nullptr || queueDescription.use_count() < leastUsedQueueDescription->use_count()) && queueDescription->operations & createInfo.operations)
                    {
                        leastUsedQueueDescription = &queueDescription;
                    }
                }

                SR_THROW_IF(leastUsedQueueDescription == nullptr, UnsupportedFeatureError(SR_FORMAT("Cannot create queue [{0}], as device [{1}] does not support any queues with specified operations", createInfo.name, createInfo.name)));

                description = *leastUsedQueueDescription;
                device->GetFunctionTable().vkGetDeviceQueue(device->GetVulkanDevice(), description->family, 0, &queue);
                break;
            }
        }
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<CommandBuffer> VulkanQueue::CreateCommandBuffer(const CommandBufferCreateInfo& createInfo) const noexcept
    {
        return std::make_unique<VulkanCommandBuffer>(*this, createInfo);
    }

    void VulkanQueue::SubmitCommandBuffer(const CommandBuffer& commandBuffer, const std::span<const std::reference_wrapper<const CommandBuffer>> commandBuffersToWait) const
    {
        SR_THROW_IF(commandBuffer.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot submit command buffer [{0}] to queue [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", commandBuffer.GetName(), GetName())));
        const VulkanCommandBuffer& vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(commandBuffer);

        Queue::SubmitCommandBuffer(vulkanCommandBuffer, commandBuffersToWait);
        SR_THROW_IF((description->operations | vulkanCommandBuffer.GetOperations()) != description->operations, InvalidOperationError(SR_FORMAT("Cannot submit command buffer [{0}] to queue [{1}], as it does not support all recoded operations within the command buffer", vulkanCommandBuffer.GetName(), GetName())));

        SR_THROW_IF(vulkanCommandBuffer.GetQueueFamily() != GetFamily(), InvalidOperationError(SR_FORMAT("Cannot submit command buffer [{0}] to queue [{1}], as it was created for a queue of a different family", vulkanCommandBuffer.GetName(), GetName())));

        // Swapchain images used within it must have been acquired first, and only the latest point of every semaphore needs to be waited for
        std::vector<VulkanSemaphoreValue> semaphoresToWait(vulkanCommandBuffer.GetAcquisitionSemaphoreValues().begin(), vulkanCommandBuffer.GetAcquisitionSemaphoreValues().end());
        for (const std::reference_wrapper<const CommandBuffer> commandBufferToWaitReference : commandBuffersToWait)
        {
            const CommandBuffer& commandBufferToWait = commandBufferToWaitReference;

            SR_THROW_IF(commandBufferToWait.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot wait for command buffer [{0}] prior to submitting command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", commandBufferToWait.GetName(), GetName())));
            const VulkanCommandBuffer& vulkanCommandBufferToWait = static_cast<const VulkanCommandBuffer&>(commandBufferToWait);

            SR_THROW_IF(&vulkanCommandBufferToWait == &vulkanCommandBuffer, InvalidValueError(SR_FORMAT("Cannot wait for the same command buffer [{0}] prior to submitting it", vulkanCommandBuffer.GetName())));

            // Command buffers which have never been submitted have nothing to wait for
            const VulkanSemaphoreValue& completionSemaphoreValue = vulkanCommandBufferToWait.GetCompletionSemaphoreValue();
            if (completionSemaphoreValue.semaphore == VK_NULL_HANDLE) continue;

            const auto iterator = std::ranges::find_if(semaphoresToWait, [&completionSemaphoreValue](const VulkanSemaphoreValue& semaphoreValue) -> bool { return semaphoreValue.semaphore == completionSemaphoreValue.semaphore; });
            if (iterator == semaphoresToWait.end()) semaphoresToWait.emplace_back(completionSemaphoreValue);
            else iterator->value = glm::max(iterator->value, completionSemaphoreValue.value);
        }

        // Submit command buffer, which gives it the point of the timeline of the queue to wait for to know it has completed
        const VkCommandBuffer vkCommandBuffer = vulkanCommandBuffer.GetVulkanCommandBuffer();
        VulkanSemaphoreValue completionSemaphoreValue = { };
        const VkResult result = device->SubmitToQueue({ .family = GetFamily(), .semaphoresToWait = semaphoresToWait, .commandBuffers = { &vkCommandBuffer, 1 } }, completionSemaphoreValue);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not submit command buffer [{0}] to queue [{1}]", vulkanCommandBuffer.GetName(), GetName()));

        vulkanCommandBuffer.completionSemaphoreValue = completionSemaphoreValue;
        device->SetResourceName(vkCommandBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER, SR_FORMAT("{0} - {1}", vulkanCommandBuffer.GetName(), completionSemaphoreValue.value));
    }

    void VulkanQueue::WaitForCommandBuffer(const CommandBuffer& commandBuffer) const
    {
        SR_THROW_IF(commandBuffer.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot wait for command buffer [{0}] in device [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", commandBuffer.GetName(), GetName())));
        const VulkanCommandBuffer& vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(commandBuffer);

        Queue::WaitForCommandBuffer(vulkanCommandBuffer);

        // Command buffers which have never been submitted have nothing to wait for (and ones which have been begun again since were waited for prior to that, as they must not be pending to be begun)
        const VulkanSemaphoreValue& completionSemaphoreValue = vulkanCommandBuffer.GetCompletionSemaphoreValue();
        if (completionSemaphoreValue.semaphore == VK_NULL_HANDLE) return;

        // Set up wait info
        const VkSemaphoreWaitInfo waitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
            .semaphoreCount = 1,
            .pSemaphores = &completionSemaphoreValue.semaphore,
            .pValues = &completionSemaphoreValue.value
        };

        // Wait for semaphore
        const VkResult result = device->GetFunctionTable().vkWaitSemaphores(device->GetVulkanDevice(), &waitInfo, std::numeric_limits<uint64>::max());
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not wait for command buffer [{0}] to complete", vulkanCommandBuffer.GetName()));
    }
}