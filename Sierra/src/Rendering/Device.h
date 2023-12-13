//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include "RenderingResource.h"

#include "Image.h"
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
        virtual void SubmitCommandBuffer(const std::unique_ptr<CommandBuffer> &commandBuffer) const = 0;
        virtual void SubmitAndWaitCommandBuffer(const std::unique_ptr<CommandBuffer> &commandBuffer) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const char* GetDeviceName() const = 0;

        [[nodiscard]] ImageFormat GetSupportedImageFormat(ImageChannels preferredChannels, ImageMemoryType preferredMemoryType, ImageUsage usage) const;
        [[nodiscard]] virtual bool IsImageConfigurationSupported(ImageFormat format, ImageUsage usage) const = 0;

        [[nodiscard]] virtual bool IsColorSamplingSupported(ImageSampling sampling) const = 0;
        [[nodiscard]] virtual bool IsDepthSamplingSupported(ImageSampling sampling) const = 0;

        [[nodiscard]] virtual ImageSampling GetHighestColorSampling() const = 0;
        [[nodiscard]] virtual ImageSampling GetHighestDepthSampling() const = 0;

    protected:
        explicit Device(const DeviceCreateInfo &createInfo);

    };

}
