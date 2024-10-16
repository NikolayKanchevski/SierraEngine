//
// Created by Nikolay Kanchevski on 15.10.24.
//

#include "MetalQueue.h"

#include "MetalCommandBuffer.h"
#include "../DeviceErrors.h"

namespace Sierra
{

    namespace
    {
        struct CommandBufferQueueEntry
        {
            id<MTLCommandBuffer> commandBuffer = nil;
            uint32 counter = 0;
        };
        std::deque<CommandBufferQueueEntry> commandBufferQueue = { };
    }

    /* --- CONSTRUCTORS --- */

    MetalQueue::MetalQueue(const MetalDevice& device, const QueueCreateInfo& createInfo)
        : Queue(createInfo), device(device), name(createInfo.name), commandQueue([device.GetMetalDevice() newCommandQueue])
    {
        SR_THROW_IF(commandQueue == nil, UnknownDeviceError(SR_FORMAT("Could not create device [{0}], as creation of command queue failed", GetName())));
        device.SetResourceName(commandQueue, SR_FORMAT("Command queue of queue [{0}]", name));
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

        // If we do not need any manual synchronization, directly submit command buffer
        if (commandBuffersToWait.empty())
        {
            [metalCommandBuffer.GetMetalCommandBuffer() encodeSignalEvent: device.GetSemaphore() value: metalCommandBuffer.GetCompletionSemaphoreSignalValue()];
            [metalCommandBuffer.GetMetalCommandBuffer() commit];
            return;
        }

        // Try to find the semaphore counter of the command buffer
        auto iterator = std::ranges::find_if(commandBufferQueue, [&metalCommandBuffer](const CommandBufferQueueEntry& item) -> bool { return item.commandBuffer == metalCommandBuffer.GetMetalCommandBuffer(); });
        if (iterator == commandBufferQueue.end())
        {
            commandBufferQueue.push_back({ .commandBuffer = metalCommandBuffer.GetMetalCommandBuffer(), .counter = static_cast<uint32>(commandBuffersToWait.size()) });
            iterator = commandBufferQueue.end() - 1;
        }

        // Decrement semaphore counter after every command buffer to wait on until it reaches 0, then submit
        for (const std::reference_wrapper<const CommandBuffer> commandBufferToWaitReference : commandBuffersToWait)
        {
            const CommandBuffer& commandBufferToWait = commandBufferToWaitReference;

            SR_THROW_IF(commandBufferToWait.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot wait for command buffer [{0}] prior to submitting command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", commandBufferToWait.GetName(), name)));
            const MetalCommandBuffer& metalCommandBufferToWait = static_cast<const MetalCommandBuffer&>(commandBufferToWait);

            SR_THROW_IF(&metalCommandBufferToWait == &metalCommandBuffer, InvalidValueError(SR_FORMAT("Cannot wait for the same command buffer [{0}] prior to submitting it", metalCommandBuffer.GetName())));
            [metalCommandBufferToWait.GetMetalCommandBuffer() addCompletedHandler: ^(id<MTLCommandBuffer>)
            {
                auto semaphoreIterator = std::ranges::find_if(commandBufferQueue, [&metalCommandBuffer](const CommandBufferQueueEntry& item) -> bool { return item.commandBuffer == metalCommandBuffer.GetMetalCommandBuffer(); });
                if (semaphoreIterator->counter--; semaphoreIterator->counter == 0)
                {
                    [semaphoreIterator->commandBuffer encodeSignalEvent: device.GetSemaphore() value: metalCommandBuffer.GetCompletionSemaphoreSignalValue()];
                    [semaphoreIterator->commandBuffer commit];
                    commandBufferQueue.erase(semaphoreIterator);
                }
            }];
        }
    }

    void MetalQueue::WaitForCommandBuffer(const CommandBuffer& commandBuffer) const
    {
        SR_THROW_IF(commandBuffer.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot wait for command buffer [{0}] in device [{1}], as its backend type differs from [RenderingBackendType::Metal]", commandBuffer.GetName(), GetName())));
        const MetalCommandBuffer& metalCommandBuffer = static_cast<const MetalCommandBuffer&>(commandBuffer);

        Queue::WaitForCommandBuffer(metalCommandBuffer);
        const uint64 waitValue = metalCommandBuffer.GetCompletionSemaphoreSignalValue();

        // Wait for completion
        while (device.GetSemaphore().signaledValue < waitValue);
    }

    /* --- DESTRUCTOR --- */

    MetalQueue::~MetalQueue() noexcept
    {
        [commandQueue release];
    }

}