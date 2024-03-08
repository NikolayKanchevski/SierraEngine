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
        void SubmitCommandBuffer(std::unique_ptr<CommandBuffer> &commandBuffer,  const std::initializer_list<std::reference_wrapper<std::unique_ptr<CommandBuffer>>> &commandBuffersToWait = { }) const override;
        void WaitForCommandBuffer(const std::unique_ptr<CommandBuffer> &commandBuffer) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const std::string& GetDeviceName() const override { return deviceName; }

        [[nodiscard]] bool IsImageFormatSupported(ImageFormat format, ImageUsage usage) const override;
        [[nodiscard]] bool IsImageSamplingSupported(ImageSampling sampling) const override;
        [[nodiscard]] bool IsSamplerAnisotropySupported(SamplerAnisotropy anisotropy) const override;

        [[nodiscard]] inline id<MTLDevice> GetMetalDevice() const { return device; }
        [[nodiscard]] inline id<MTLCommandQueue> GetCommandQueue() const { return commandQueue; }

        [[nodiscard]] inline id<MTLSharedEvent> GetSharedSignalSemaphore() const { return sharedSignalSemaphore; }
        [[nodiscard]] inline uint64 GetNewSignalValue() const { lastReservedSignalValue++; return lastReservedSignalValue; }

        /* --- SETTER METHODS --- */
        template<typename T>
        inline void SetResourceName(T* resource, const std::string &name) const
        {
            #if defined(__OBJC__) && SR_ENABLE_LOGGING
                NSString* const label = [[NSString alloc] initWithCString: name.c_str() encoding: NSASCIIStringEncoding];
                [resource setLabel: label];
                [label release];
            #endif
        }

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
