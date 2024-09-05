//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include "RenderingResource.h"

#include "../Core/UUID.hpp"
#include "CommandBuffer.h"
#include "Image.h"
#include "Sampler.h"

namespace Sierra
{

    class SIERRA_API Device : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void SubmitCommandBuffer(const CommandBuffer& commandBuffer, std::span<const CommandBuffer*> commandBuffersToWait = { }) const = 0;
        virtual void WaitForCommandBuffer(const CommandBuffer& commandBuffer) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual bool IsImageFormatSupported(ImageFormat format, ImageUsage usage) const = 0;
        [[nodiscard]] std::optional<ImageFormat> GetSupportedImageFormat(ImageFormat preferredFormat, ImageUsage usage) const;

        [[nodiscard]] virtual bool IsImageSamplingSupported(ImageSampling sampling) const = 0;
        [[nodiscard]] ImageSampling GetHighestImageSamplingSupported() const;

        [[nodiscard]] virtual bool IsSamplerAnisotropySupported(SamplerAnisotropy anisotropy) const = 0;
        [[nodiscard]] SamplerAnisotropy GetHighestSamplerAnisotropySupported() const;

        /* --- CONSTANTS --- */
        constexpr static uint16 MAX_PUSH_CONSTANT_SIZE = 128;

        /* --- DESTRUCTOR --- */
        ~Device() override = default;

    protected:
        Device() = default;

    };

}
