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
        std::string_view name = "Device";
    };

    class SIERRA_API Device : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void SubmitCommandBuffer(std::unique_ptr<CommandBuffer> &commandBuffer,  const std::span<const std::reference_wrapper<std::unique_ptr<CommandBuffer>>> &commandBuffersToWait = { }) const = 0;
        virtual void WaitForCommandBuffer(const std::unique_ptr<CommandBuffer> &commandBuffer) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual std::string_view GetDeviceName() const = 0;

        [[nodiscard]] virtual bool IsImageFormatSupported(ImageFormat format, ImageUsage usage) const = 0;
        [[nodiscard]] std::optional<ImageFormat> GetSupportedImageFormat(ImageFormat preferredFormat, ImageUsage usage) const;

        [[nodiscard]] virtual bool IsImageSamplingSupported(ImageSampling sampling) const = 0;
        [[nodiscard]] ImageSampling GetHighestImageSamplingSupported() const;

        [[nodiscard]] virtual bool IsSamplerAnisotropySupported(SamplerAnisotropy anisotropy) const = 0;
        [[nodiscard]] SamplerAnisotropy GetHighestSamplerAnisotropySupported() const;

        /* --- CONSTANTS --- */
        constexpr static uint16 MAX_PUSH_CONSTANT_SIZE = 128;
        constexpr static uint32 MAX_UNIFORM_BUFFERS_PER_RESOURCE_TABLE = 512'000;
        constexpr static uint32 MAX_STORAGE_BUFFERS_PER_RESOURCE_TABLE = 512'000;
        constexpr static uint32 MAX_SAMPLED_IMAGES_PER_RESOURCE_TABLE = 512'000;
        constexpr static uint32 MAX_STORAGE_IMAGES_PER_RESOURCE_TABLE = 512'000;
        constexpr static uint32 MAX_SAMPLERS_PER_RESOURCE_TABLE = 32'000;

        /* --- OPERATORS --- */
        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Device() = default;

    protected:
        explicit Device(const DeviceCreateInfo &createInfo);

    };

}
