//
// Created by Nikolay Kanchevski on 13.10.24.
//
#include "Queue.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Queue::Queue(const QueueCreateInfo& createInfo)
    {
        SR_THROW_IF(createInfo.name.empty(), InvalidValueError("Cannot create queue, as specified name must not be empty"));
        SR_THROW_IF(createInfo.operations == QueueOperations::None, InvalidValueError(SR_FORMAT("Cannot create queue [{0}], as specified operations must not be [QueueOperations::None]", createInfo.name)));
    }

    /* --- POLLING METHODS --- */

    void Queue::SubmitCommandBuffer(const CommandBuffer& commandBuffer) const
    {
        SubmitCommandBuffer(commandBuffer, { });
    }

    void Queue::SubmitCommandBuffer(const Sierra::CommandBuffer& commandBuffer, const std::span<const std::reference_wrapper<const CommandBuffer>> commandBuffersToWaitFor) const
    {

    }

    void Queue::WaitForCommandBuffer(const CommandBuffer& commandBuffer) const
    {

    }

}