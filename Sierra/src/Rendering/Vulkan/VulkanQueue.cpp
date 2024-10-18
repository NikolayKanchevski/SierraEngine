//
// Created by Nikolay Kanchevski on 13.10.24.
//

#include "VulkanQueue.h"

#include "VulkanCommandBuffer.h"
#include "VulkanErrorHandler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanQueue::VulkanQueue(const VulkanDevice& device, const QueueCreateInfo& createInfo)
        : Queue(createInfo), device(device), name(createInfo.name)
    {
        switch (createInfo.priority)
        {
            case QueuePriority::Dedicated:
            {
                auto suitableQueueDescriptions = device.GetQueueDescriptions() | std::ranges::views::filter([createInfo](const std::shared_ptr<VulkanQueueDescription>& queueDescription) -> bool { return queueDescription->operations == createInfo.operations; });
                if (!suitableQueueDescriptions.empty())
                {
                    auto iterator = std::ranges::min_element(suitableQueueDescriptions, [](const std::shared_ptr<VulkanQueueDescription>& left, const std::shared_ptr<VulkanQueueDescription>& right) -> bool { return left.use_count() < right.use_count(); });
                    description = *iterator;

                    device.GetFunctionTable().vkGetDeviceQueue(device.GetVulkanDevice(), description->family, 0, &queue);
                    break;
                }

                // NOTE: We specifically do not break here, such that if a dedicated queue was not found we search for any supporting requested operations
                [[fallthrough]];
            }
            case QueuePriority::LeastUsed:
            {
                auto suitableQueueDescriptions = device.GetQueueDescriptions() | std::ranges::views::filter([createInfo](const std::shared_ptr<VulkanQueueDescription>& queueDescription) -> bool { return queueDescription->operations & createInfo.operations; });
                SR_THROW_IF(suitableQueueDescriptions.empty(), UnsupportedFeatureError(SR_FORMAT("Cannot create queue [{0}], as device [{1}] does not support amy queues of specified type", createInfo.name, name)));

                auto iterator = std::ranges::min_element(suitableQueueDescriptions, [](const std::shared_ptr<VulkanQueueDescription>& left, const std::shared_ptr<VulkanQueueDescription>& right) -> bool { return left.use_count() < right.use_count(); });
                description = *iterator;

                device.GetFunctionTable().vkGetDeviceQueue(device.GetVulkanDevice(), description->family, 0, &queue);
                break;
            }
        }
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<CommandBuffer> VulkanQueue::CreateCommandBuffer(const CommandBufferCreateInfo& createInfo) const noexcept
    {
        return std::make_unique<VulkanCommandBuffer>(*this, createInfo);
    }

    void VulkanQueue::SubmitCommandBuffer(const CommandBuffer& commandBuffer, std::span<const std::reference_wrapper<const CommandBuffer>> commandBuffersToWait) const
    {
        SR_THROW_IF(commandBuffer.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot submit command buffer [{0}] to queue [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", commandBuffer.GetName(), GetName())));
        const VulkanCommandBuffer& vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(commandBuffer);

        Queue::SubmitCommandBuffer(vulkanCommandBuffer, commandBuffersToWait);
        SR_THROW_IF(!(vulkanCommandBuffer.GetOperations() & description->operations), InvalidOperationError(SR_FORMAT("Cannot submit command buffer [{0}] to queue [{1}], as it does not support all recoded operations within the command buffer", vulkanCommandBuffer.GetName(), name)));

        // Determine what value to wait for
        uint64 waitValue = 0;
        for (const std::reference_wrapper<const CommandBuffer> commandBufferToWaitReference : commandBuffersToWait)
        {
            const CommandBuffer& commandBufferToWait = commandBufferToWaitReference;

            SR_THROW_IF(commandBufferToWait.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot wait for command buffer [{0}] prior to submitting command buffer [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", commandBufferToWait.GetName(), name)));
            const VulkanCommandBuffer& vulkanCommandBufferToWait = static_cast<const VulkanCommandBuffer&>(commandBufferToWait);

            SR_THROW_IF(&vulkanCommandBufferToWait == &vulkanCommandBuffer, InvalidValueError(SR_FORMAT("Cannot wait for the same command buffer [{0}] prior to submitting it", vulkanCommandBuffer.GetName())));
            waitValue = glm::max(waitValue, vulkanCommandBufferToWait.GetCompletionSemaphoreSignalValue());
        }

        // Set up semaphore submit info
        const uint64 signalValue = vulkanCommandBuffer.GetCompletionSemaphoreSignalValue();
        const VkTimelineSemaphoreSubmitInfo semaphoreSubmitInfo
        {
            .sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
            .waitSemaphoreValueCount = 1,
            .pWaitSemaphoreValues = &waitValue,
            .signalSemaphoreValueCount = 1,
            .pSignalSemaphoreValues = &signalValue
        };

        VkCommandBuffer vkCommandBuffer = vulkanCommandBuffer.GetVulkanCommandBuffer();
        constexpr VkPipelineStageFlags WAIT_STAGE = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

        // Set up submit info
        VkSemaphore generalTimelineSemaphore = device.GetSemaphore();
        const VkSubmitInfo submitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = &semaphoreSubmitInfo,
            .pWaitSemaphores = &generalTimelineSemaphore,
            .pWaitDstStageMask = &WAIT_STAGE,
            .commandBufferCount = 1,
            .pCommandBuffers = &vkCommandBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &generalTimelineSemaphore
        };

        // Submit command buffer
        const VkResult result = device.GetFunctionTable().vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not submit command buffer [{0}] to queue [{1}]", vulkanCommandBuffer.GetName(), name));
    }

    void VulkanQueue::WaitForCommandBuffer(const CommandBuffer& commandBuffer) const
    {
        SR_THROW_IF(commandBuffer.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot wait for command buffer [{0}] in device [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", commandBuffer.GetName(), GetName())));
        const VulkanCommandBuffer& vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(commandBuffer);

        Queue::WaitForCommandBuffer(vulkanCommandBuffer);
        const uint64 waitValue = vulkanCommandBuffer.GetCompletionSemaphoreSignalValue();

        // Set up wait info
        VkSemaphore generalTimelineSemaphore = device.GetSemaphore();
        const VkSemaphoreWaitInfo waitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
            .semaphoreCount = 1,
            .pSemaphores = &generalTimelineSemaphore,
            .pValues = &waitValue
        };

        // Wait for semaphore
        device.GetFunctionTable().vkWaitSemaphores(device.GetVulkanDevice(), &waitInfo, std::numeric_limits<uint64>::max());
    }

}