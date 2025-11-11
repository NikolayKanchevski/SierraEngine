//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include "RenderingResource.h"

#include "Image.h"
#include "Queue.h"
#include "Buffer.h"
#include "Shader.h"
#include "Sampler.h"
#include "Swapchain.h"
#include "RenderPass.h"
#include "Framebuffer.h"
#include "ResourceTable.h"
#include "ComputePipeline.h"
#include "../Core/Version.h"
#include "GraphicsPipeline.h"
#include "DestructionScheduler.h"

namespace Sierra
{

    struct DeviceLimits
    {
        uint64 maxUniformBufferSize = 0;
        uint64 maxStorageBufferSize = 0;
        uint32 maxLineImageDimensions = 0;
        uint32 maxPlaneImageDimensions = 0;
        uint32 maxVolumeImageDimensions = 0;
        uint32 maxCubeImageDimensions = 0;
        uint32 resourceTableUniformBufferCapacity = 0;
        uint32 resourceTableStorageBufferCapacity = 0;
        uint32 resourceTableSampledImageCapacity = 0;
        uint32 resourceTableStorageImageCapacity = 0;
        uint32 resourceTableSamplerCapacity = 0;
        uint32 maxFramebufferWidth = 0;
        uint32 maxFramebufferHeight = 0;
        Vector3UInt maxWorkGroupSize = { 0, 0, 0 };
        ImageSampling highestImageSampling = ImageSampling::x1;
        SamplerAnisotropy highestSamplerAnisotropy = SamplerAnisotropy::x1;
    };

    struct DeviceRequirements
    {
        DeviceLimits limits = { };
    };

    struct DeviceCreateInfo
    {
        std::string_view name = "Device";
        DeviceRequirements requirements = { };
    };

    class SIERRA_API Device : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::unique_ptr<Buffer> CreateBuffer(const BufferCreateInfo& createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<Image> CreateImage(const ImageCreateInfo& createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<Sampler> CreateSampler(const SamplerCreateInfo& createInfo) const = 0;

        [[nodiscard]] virtual std::unique_ptr<RenderPass> CreateRenderPass(const RenderPassCreateInfo& createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<Framebuffer> CreateFramebuffer(const FramebufferCreateInfo& createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<Swapchain> CreateSwapchain(const SwapchainCreateInfo& createInfo) const = 0;

        [[nodiscard]] virtual std::unique_ptr<Shader> CreateShader(const ShaderCreateInfo& createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) const = 0;

        [[nodiscard]] virtual std::unique_ptr<ResourceTable> CreateResourceTable(const ResourceTableCreateInfo& createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<Queue> CreateQueue(const QueueCreateInfo& createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<DestructionScheduler> CreateDestructionScheduler(const DestructionSchedulerCreateInfo& createInfo) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual std::string_view GetHardwareName() const noexcept = 0;
        [[nodiscard]] virtual Version GetBackendVersion() const noexcept = 0;
        [[nodiscard]] virtual Version GetDriverVersion() const noexcept = 0;
        [[nodiscard]] virtual DeviceLimits GetLimits() const noexcept = 0;

        [[nodiscard]] virtual bool IsImageFormatSupported(ImageFormat format, ImageUsage usage) const noexcept = 0;
        [[nodiscard]] std::optional<ImageFormat> GetSupportedImageFormat(ImageFormat preferredFormat, ImageUsage usage) const noexcept;

        [[nodiscard]] virtual bool IsImageSamplingSupported(ImageSampling sampling) const noexcept = 0;
        [[nodiscard]] ImageSampling GetHighestImageSamplingSupported() const noexcept;

        [[nodiscard]] virtual bool IsSamplerAnisotropySupported(SamplerAnisotropy anisotropy) const noexcept = 0;
        [[nodiscard]] SamplerAnisotropy GetHighestSamplerAnisotropySupported() const noexcept;

        /* --- CONSTANTS --- */
        constexpr static uint8 MAX_PUSH_CONSTANT_SIZE = 128;

        /* --- COPY SEMANTICS --- */
        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;

        /* --- DESTRUCTOR --- */
        ~Device() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit Device(const DeviceCreateInfo& createInfo);

        /* --- MOVE SEMANTICS --- */
        Device(Device&&) noexcept = default;
        Device& operator=(Device&&) noexcept = default;

    };

}
