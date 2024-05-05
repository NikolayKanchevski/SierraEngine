//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include "../../Device.h"
#include "MetalResource.h"

#if !defined(__OBJC__)
    // This ugly bit is required, so that MetalDevice can be included in RenderingcContext.cpp which is C++ implemented
    namespace Sierra
    {
        #define nil { }
        template<typename T>
        struct id { volatile T* data = nil; };

        using MTLDevice = void;
        using MTLCommandQueue = void;

        using MTLSharedEvent = void;
        using MTLCommandBuffer = void;

        using MTLBuffer = void;
        using MTLTexture = void;
    }
#endif

namespace Sierra
{

    class SIERRA_API MetalDevice final : public Device, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MetalDevice(const DeviceCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void SubmitCommandBuffer(std::unique_ptr<CommandBuffer> &commandBuffer,  const std::span<const std::reference_wrapper<std::unique_ptr<CommandBuffer>>> &commandBuffersToWait = { }) const override;
        void WaitForCommandBuffer(const std::unique_ptr<CommandBuffer> &commandBuffer) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline std::string_view GetDeviceName() const override { return deviceName; }

        [[nodiscard]] bool IsImageFormatSupported(ImageFormat format, ImageUsage usage) const override;
        [[nodiscard]] bool IsImageSamplingSupported(ImageSampling sampling) const override;
        [[nodiscard]] bool IsSamplerAnisotropySupported(SamplerAnisotropy anisotropy) const override;

        [[nodiscard]] inline id<MTLDevice> GetMetalDevice() const { return device; }
        [[nodiscard]] inline id<MTLCommandQueue> GetCommandQueue() const { return commandQueue; }

        [[nodiscard]] inline id<MTLSharedEvent> GetSharedSignalSemaphore() const { return sharedSignalSemaphore; }
        [[nodiscard]] inline uint64 GetNewSignalValue() const { lastReservedSignalValue++; return lastReservedSignalValue; }

        /* --- SETTER METHODS --- */
        template<typename T>
        inline void SetResourceName(T* resource, const std::string_view name) const
        {
            #if defined(__OBJC__) && SR_ENABLE_LOGGING
                NSString* const label = [[NSString alloc] initWithCString: name.data() encoding: NSASCIIStringEncoding];
                [resource setLabel: label];
                [label release];
            #endif
        }

        /* --- CONSTANTS --- */
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_INDEX                        = 0;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_UNIFORM_BUFFER_INDEX         = 0;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_STORAGE_BUFFER_INDEX         = 1;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_SAMPLED_IMAGE_INDEX          = 2;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_STORAGE_IMAGE_INDEX          = 3;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_SAMPLER_INDEX                = 4;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_INDEX_COUNT                  = 5;

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
        static inline std::deque<CommandBufferQueueEntry> commandBufferQueue;

    };

}
