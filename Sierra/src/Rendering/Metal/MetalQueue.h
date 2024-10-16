//
// Created by Nikolay Kanchevski on 15.10.24.
//
#pragma once

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLCommandQueue = void;
    }
#endif

#include "../Queue.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalQueue final : public Queue, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalQueue(const MetalDevice& device, const QueueCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<CommandBuffer> CreateCommandBuffer(const CommandBufferCreateInfo& createInfo) const noexcept override;
        void SubmitCommandBuffer(const CommandBuffer& commandBuffer, std::span<const std::reference_wrapper<const CommandBuffer>> commandBuffersToWait) const override;
        void WaitForCommandBuffer(const CommandBuffer& commandBuffer) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }
        [[nodiscard]] QueueOperations GetOperations() const noexcept override { return QueueOperations::All; }

        [[nodiscard]] id<MTLCommandQueue> GetMetalCommandQueue() const noexcept { return commandQueue; }
        [[nodiscard]] const MetalDevice& GetDevice() const noexcept { return device; }

        /* --- COPY SEMANTICS --- */
        MetalQueue(const MetalQueue&) = delete;
        MetalQueue& operator=(const MetalQueue&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalQueue(MetalQueue&&) = delete;
        MetalQueue& operator=(MetalQueue&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MetalQueue() noexcept override;

    private:
        const MetalDevice& device;
        const std::string name;

        id<MTLCommandQueue> commandQueue = nil;

    };

}