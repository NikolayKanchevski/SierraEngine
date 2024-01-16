//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include "../../Device.h"
#include "MetalResource.h"

namespace Sierra
{

    class SIERRA_API MetalDevice final : public Device, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MetalDevice(const DeviceCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void SubmitCommandBuffer(std::unique_ptr<CommandBuffer> &commandBuffer) const override;
        void SubmitAndWaitCommandBuffer(std::unique_ptr<CommandBuffer> &commandBuffer) const override;
        void WaitUntilIdle() const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const std::string& GetDeviceName() const override { return deviceName; }

        [[nodiscard]] bool IsImageConfigurationSupported(ImageFormat format, ImageUsage usage) const override;
        [[nodiscard]] bool IsImageSamplingSupported(ImageSampling sampling) const override;
        [[nodiscard]] ImageSampling GetHighestImageSamplingSupported() const override;

        [[nodiscard]] inline MTL::Device* GetMetalDevice() const { return device; }
        [[nodiscard]] inline MTL::CommandQueue* GetCommandQueue() const { return commandQueue; }

        /* --- DESTRUCTOR --- */
        ~MetalDevice() override;

    private:
        MTL::Device* device = nullptr;
        std::string deviceName;

        MTL::CommandQueue* commandQueue = nullptr;
        dispatch_semaphore_t sharedCommandBufferSemaphore = nullptr;

    };

}
