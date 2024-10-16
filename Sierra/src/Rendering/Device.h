//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include "RenderingResource.h"

#include "Image.h"
#include "Sampler.h"
#include "CommandBuffer.h"
#include "Swapchain.h"

namespace Sierra
{

    struct DeviceCreateInfo
    {
        const std::string &name = "Device";
    };

    class SIERRA_API Device : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void SubmitCommandBuffer(std::unique_ptr<CommandBuffer> &commandBuffer,  const std::initializer_list<std::reference_wrapper<std::unique_ptr<CommandBuffer>>> &commandBuffersToWait = { }) const = 0;
        virtual void WaitForCommandBuffer(const std::unique_ptr<CommandBuffer> &commandBuffer) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const std::string& GetDeviceName() const = 0;

        [[nodiscard]] virtual bool IsImageFormatSupported(ImageFormat format, ImageUsage usage) const = 0;
        [[nodiscard]] std::optional<ImageFormat> GetSupportedImageFormat(ImageFormat preferredFormat, ImageUsage usage) const;

        [[nodiscard]] virtual bool IsImageSamplingSupported(ImageSampling sampling) const = 0;
        [[nodiscard]] ImageSampling GetHighestImageSamplingSupported() const;

        [[nodiscard]] virtual bool IsSamplerAnisotropySupported(SamplerAnisotropy anisotropy) const = 0;
        [[nodiscard]] SamplerAnisotropy GetHighestSamplerAnisotropySupported() const;

        /* --- OPERATORS --- */
        Device(const Device&) = delete;
        Device &operator=(const Device&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Device() = default;

    protected:
        explicit Device(const DeviceCreateInfo &createInfo);

    };

}
