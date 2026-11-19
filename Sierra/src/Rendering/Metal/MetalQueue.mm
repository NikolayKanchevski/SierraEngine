//
// Created by Nikolay Kanchevski on 15.10.24.
//

#include "MetalQueue.h"

#include "MetalCommandBuffer.h"
#include "../DeviceErrors.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalQueue::MetalQueue(const MetalDevice& givenDevice, const QueueCreateInfo& createInfo)
        : MetalResource(createInfo.name), Queue(createInfo), device(&givenDevice), semaphoreListener([[MTLSharedEventListener alloc] initWithDispatchQueue: dispatch_get_global_queue(QOS_CLASS_USER_INTERACTIVE, 0)])
    {
        SR_THROW_IF(semaphoreListener == nil, UnknownDeviceError(SR_FORMAT("Could not create queue [{0}], as creation of semaphore listener failed", GetName())));
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<CommandBuffer> MetalQueue::CreateCommandBuffer(const CommandBufferCreateInfo& createInfo) const noexcept
    {
        return std::make_unique<MetalCommandBuffer>(*this, createInfo);
    }

    void MetalQueue::SubmitCommandBuffer(const CommandBuffer& commandBuffer, std::span<const std::reference_wrapper<const CommandBuffer>> commandBuffersToWait) const
    {
        SR_THROW_IF(commandBuffer.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot submit command buffer [{0}] to device [{1}], as its backend type differs from [RenderingBackendType::Metal]", commandBuffer.GetName(), GetName())));
        const MetalCommandBuffer& metalCommandBuffer = static_cast<const MetalCommandBuffer&>(commandBuffer);

        Queue::SubmitCommandBuffer(metalCommandBuffer, commandBuffersToWait);

        // Determine what value to wait for
        uint64 waitValue = 0;
        for (const std::reference_wrapper<const CommandBuffer> commandBufferToWaitReference : commandBuffersToWait)
        {
            const CommandBuffer& commandBufferToWait = commandBufferToWaitReference;

            SR_THROW_IF(commandBufferToWait.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot wait for command buffer [{0}] prior to submitting command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", commandBufferToWait.GetName(), GetName())));
            const MetalCommandBuffer& metalCommandBufferToWait = static_cast<const MetalCommandBuffer&>(commandBufferToWait);

            SR_THROW_IF(&metalCommandBufferToWait == &metalCommandBuffer, InvalidValueError(SR_FORMAT("Cannot wait for the same command buffer [{0}] prior to submitting it", metalCommandBuffer.GetName())));
            waitValue = glm::max(waitValue, metalCommandBufferToWait.GetCompletionSemaphoreSignalValue()); // Command buffers which have never been submitted have a value of 0, which needs no waiting for
        }

        // Submit command buffer, which gives it the value to wait for to know it has completed
        metalCommandBuffer.completionSemaphoreSignalValue = device->CommitCommandBuffer(metalCommandBuffer.GetMetalCommandBuffer(), waitValue);
    }

    void MetalQueue::WaitForCommandBuffer(const CommandBuffer& commandBuffer) const
    {
        SR_THROW_IF(commandBuffer.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot wait for command buffer [{0}] in device [{1}], as its backend type differs from [RenderingBackendType::Metal]", commandBuffer.GetName(), GetName())));
        const MetalCommandBuffer& metalCommandBuffer = static_cast<const MetalCommandBuffer&>(commandBuffer);

        Queue::WaitForCommandBuffer(metalCommandBuffer);
        const uint64 waitValue = metalCommandBuffer.GetCompletionSemaphoreSignalValue();

        // If command buffer has already been completed, there is nothing to wait for
        const id<MTLSharedEvent> semaphore = device->GetSemaphore();
        if (semaphore.signaledValue >= waitValue) return;

        // Otherwise, have the semaphore notify us once it reaches the completion value of the command buffer
        const dispatch_semaphore_t completionSemaphore = dispatch_semaphore_create(0);
        [semaphore notifyListener: semaphoreListener atValue: waitValue block: ^(id<MTLSharedEvent>, uint64)
        {
            dispatch_semaphore_signal(completionSemaphore);
        }];

        // Put thread to sleep until then (value is checked once more, in case it was reached right before the notification was registered)
        if (semaphore.signaledValue < waitValue) dispatch_semaphore_wait(completionSemaphore, DISPATCH_TIME_FOREVER);
        dispatch_release(completionSemaphore);
    }

    /* --- DESTRUCTOR --- */

    MetalQueue::~MetalQueue() noexcept
    {
        [semaphoreListener release];
    }

}