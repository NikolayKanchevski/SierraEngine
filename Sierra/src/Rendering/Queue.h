//
// Created by Nikolay Kanchevski on 13.10.24.
//
#pragma once

#include "RenderingResource.h"

#include "CommandBuffer.h"

namespace Sierra
{

    enum class QueueOperations : uint8
    {
        None                = 0x0000,
        Graphics            = 0x0001,
        Compute             = 0x0002,
        Transfer            = 0x0004,
        All                 = Graphics | Compute | Transfer
    };
    SR_DEFINE_ENUM_FLAG_OPERATORS(QueueOperations);

    enum class QueuePriority : bool
    {
        LeastUsed,
        Dedicated
    };

    struct QueueCreateInfo
    {
        std::string_view name = "Queue";
        QueueOperations operations = QueueOperations::All;
        QueuePriority priority = QueuePriority::LeastUsed;
    };

    struct AAAA
    {
        CommandBuffer& c;
    };

    class SIERRA_API Queue : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::unique_ptr<CommandBuffer> CreateCommandBuffer(const CommandBufferCreateInfo& createInfo) const = 0;
        void SubmitCommandBuffer(const CommandBuffer& commandBuffer) const;
        virtual void SubmitCommandBuffer(const CommandBuffer& commandBuffer, std::span<const std::reference_wrapper<const CommandBuffer>> commandBuffersToWait) const;
        virtual void WaitForCommandBuffer(const CommandBuffer& commandBuffer) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual QueueOperations GetOperations() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        Queue(const Queue&) = delete;
        Queue& operator=(const Queue&) = delete;

        /* --- MOVE SEMANTICS --- */
        Queue(Queue&&) = delete;
        Queue& operator=(Queue&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Queue() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit Queue(const QueueCreateInfo& createInfo);

    };

}