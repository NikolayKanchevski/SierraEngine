//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLDevice = void;
        using MTLCommandQueue = void;

        using MTLSharedEvent = void;
        using MTLCommandBuffer = void;
    }
#endif

#include "../Device.h"
#include "MetalResource.h"

#include "MetalContext.h"

namespace Sierra
{

    class SIERRA_API MetalDevice final : public Device, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalDevice(const MetalContext& context, id<MTLDevice> device, const DeviceCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Buffer> CreateBuffer(const BufferCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<Image> CreateImage(const ImageCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<Sampler> CreateSampler(const SamplerCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<RenderPass> CreateRenderPass(const RenderPassCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<Swapchain> CreateSwapchain(const SwapchainCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<Shader> CreateShader(const ShaderCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<ResourceTable> CreateResourceTable(const ResourceTableCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<Queue> CreateQueue(const QueueCreateInfo& createInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }
        [[nodiscard]] std::string_view GetHardwareName() const noexcept override;

        [[nodiscard]] Version GetBackendVersion() const noexcept override { return context.GetBackendVersion(); }
        [[nodiscard]] Version GetDriverVersion() const noexcept override { return driverVersion; }
        [[nodiscard]] DeviceLimits GetLimits() const noexcept override;

        [[nodiscard]] bool IsImageFormatSupported(ImageFormat format, ImageUsage usage) const noexcept override;
        [[nodiscard]] bool IsImageSamplingSupported(ImageSampling sampling) const noexcept override;
        [[nodiscard]] bool IsSamplerAnisotropySupported(SamplerAnisotropy anisotropy) const noexcept override;

        [[nodiscard]] id<MTLDevice> GetMetalDevice() const noexcept { return device; }

        [[nodiscard]] id<MTLSharedEvent> GetSemaphore() const noexcept { return semaphore; }
        [[nodiscard]] uint64 GetNewSemaphoreSignalValue() const noexcept { return ++lastReservedSemaphoreSignalValue; }

        /* --- SETTER METHODS --- */
        void SetResourceName(const MTLResource* resource, std::string_view name) const noexcept;

        /* --- CONSTANTS --- */
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_INDEX                        = 0;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_UNIFORM_BUFFER_INDEX         = 0;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_STORAGE_BUFFER_INDEX         = 1;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_SAMPLED_IMAGE_INDEX          = 2;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_STORAGE_IMAGE_INDEX          = 3;
        constexpr static uint32 BINDLESS_ARGUMENT_BUFFER_SAMPLER_INDEX                = 4;

        constexpr static uint32 PUSH_CONSTANT_INDEX = 1;
        constexpr static uint32 VERTEX_BUFFER_INDEX = 30;

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator==(const MetalDevice& other) noexcept;
        [[nodiscard]] bool operator!=(const MetalDevice& other) noexcept;

        /* --- COPY SEMANTICS --- */
        MetalDevice(const MetalDevice&) = delete;
        MetalDevice& operator=(const MetalDevice&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalDevice(MetalDevice&&) = delete;
        MetalDevice& operator=(MetalDevice&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MetalDevice() noexcept override;

    private:
        const MetalContext& context;
        const std::string name;

        const id<MTLDevice> device = nil;
        Version driverVersion = Version({ 1, 0, 0 });

        mutable std::atomic<uint64> lastReservedSemaphoreSignalValue = 0;
        id<MTLSharedEvent> semaphore = nil;

        // NOTE: These must match the values in specified in https://github.com/NikolayKanchevski/ShaderConnect/blob/sierra/src/Platform/MetalSL/MetalSLShaderCompiler.cpp#L104
        constexpr static uint32 RESOURCE_TABLE_UNIFORM_BUFFER_CAPACITY         = 8192;
        constexpr static uint32 RESOURCE_TABLE_STORAGE_BUFFER_CAPACITY         = 8192;
        constexpr static uint32 RESOURCE_TABLE_SAMPLED_IMAGE_CAPACITY          = 8192;
        constexpr static uint32 RESOURCE_TABLE_STORAGE_IMAGE_CAPACITY          = 8192;
        constexpr static uint32 RESOURCE_TABLE_SAMPLER_CAPACITY                = 1024;

    };

}
