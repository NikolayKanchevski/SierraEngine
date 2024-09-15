//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include "../../Device.h"
#include "MetalResource.h"

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLDevice = void;
        using MTLCommandQueue = void;

        using MTLSharedEvent = void;
        using MTLCommandBuffer = void;
    }
#endif

namespace Sierra
{

    class SIERRA_API MetalDevice final : public Device, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalDevice();

        /* --- POLLING METHODS --- */
        void SubmitCommandBuffer(const CommandBuffer& commandBuffer, std::span<const CommandBuffer*> commandBuffersToWait = { }) const override;
        void WaitForCommandBuffer(const CommandBuffer& commandBuffer) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const override;

        [[nodiscard]] bool IsImageFormatSupported(ImageFormat format, ImageUsage usage) const override;
        [[nodiscard]] bool IsImageSamplingSupported(ImageSampling sampling) const override;
        [[nodiscard]] bool IsSamplerAnisotropySupported(SamplerAnisotropy anisotropy) const override;

        [[nodiscard]] id<MTLDevice> GetMetalDevice() const { return device; }
        [[nodiscard]] id<MTLCommandQueue> GetCommandQueue() const { return commandQueue; }

        [[nodiscard]] id<MTLSharedEvent> GetSharedSignalSemaphore() const { return sharedSignalSemaphore; }
        [[nodiscard]] uint64 GetNewSignalValue() const { lastReservedSignalValue++; return lastReservedSignalValue; }

        /* --- SETTER METHODS --- */
        void SetResourceName(MTLHandle resource, std::string_view name) const;

        /* --- CONSTANTS --- */
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_INDEX                        = 0;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_UNIFORM_BUFFER_INDEX         = 0;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_STORAGE_BUFFER_INDEX         = 1;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_SAMPLED_IMAGE_INDEX          = 2;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_STORAGE_IMAGE_INDEX          = 3;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_SAMPLER_INDEX                = 4;

        constexpr static uint32 PUSH_CONSTANT_INDEX = 1;
        constexpr static uint32 VERTEX_BUFFER_INDEX = 30;

        /* --- DESTRUCTOR --- */
        ~MetalDevice() override;

    private:
        std::string deviceName;

        id<MTLDevice> device = nil;
        id<MTLCommandQueue> commandQueue = nil;

        mutable uint64 lastReservedSignalValue = 0;
        id<MTLSharedEvent> sharedSignalSemaphore = nil;

        struct CommandBufferQueueEntry
        {
            id<MTLCommandBuffer> commandBuffer = nil;
            uint32 counter = 0;
        };
        inline static std::deque<CommandBufferQueueEntry> commandBufferQueue;

    };

}
