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

#include "MetalInstance.h"

namespace Sierra
{

    class SIERRA_API MetalDevice final : public Device, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalDevice(const MetalInstance& givenContext, id<MTLDevice> device, const DeviceCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Buffer> CreateBuffer(const BufferCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<Image> CreateImage(const ImageCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<Sampler> CreateSampler(const SamplerCreateInfo& createInfo) const override;

        [[nodiscard]] std::unique_ptr<RenderPass> CreateRenderPass(const RenderPassCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<Framebuffer> CreateFramebuffer(const FramebufferCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<Swapchain> CreateSwapchain(const SwapchainCreateInfo& createInfo) const override;

        [[nodiscard]] std::unique_ptr<Shader> CreateShader(const ShaderCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) const override;

        [[nodiscard]] std::unique_ptr<ResourceTable> CreateResourceTable(const ResourceTableCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<Queue> CreateQueue(const QueueCreateInfo& createInfo) const override;
        [[nodiscard]] std::unique_ptr<DestructionScheduler> CreateDestructionScheduler(const DestructionSchedulerCreateInfo& createInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetHardwareName() const noexcept override;

        [[nodiscard]] Version GetBackendVersion() const noexcept override { return context->GetBackendVersion(); }
        [[nodiscard]] Version GetDriverVersion() const noexcept override { return driverVersion; }
        [[nodiscard]] DeviceLimits GetLimits() const noexcept override;

        [[nodiscard]] bool IsImageFormatSupported(ImageFormat format, ImageUsage usage) const noexcept override;
        [[nodiscard]] bool IsImageSamplingSupported(ImageSampling sampling) const noexcept override;
        [[nodiscard]] bool IsSamplerAnisotropySupported(SamplerAnisotropy anisotropy) const noexcept override;

        [[nodiscard]] id<MTLDevice> GetMetalDevice() const noexcept { return device; }

        // NOTE: Every queue is the command queue of the device (just as every Vulkan queue of a family is its first one), so command buffers are all committed through here, where each signals
        // the semaphore with a value greater than any before it (returned here, as what to wait for to know it has completed), so that values are signalled in the order of committing
        [[nodiscard]] uint64 CommitCommandBuffer(id<MTLCommandBuffer> commandBuffer, uint64 semaphoreValueToWait) const;

        [[nodiscard]] id<MTLCommandQueue> GetMetalCommandQueue() const noexcept { return commandQueue; }
        [[nodiscard]] id<MTLSharedEvent> GetSemaphore() const noexcept { return semaphore; }
        [[nodiscard]] uint64 GetLastSemaphoreSignalValue() const; // As of the last commit

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
        MetalDevice(MetalDevice&&) noexcept = default;
        MetalDevice& operator=(MetalDevice&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~MetalDevice() noexcept override;

    private:
        const MetalInstance* context = nullptr;

        id<MTLDevice> device = nil;
        Version driverVersion = Version({ 1, 0, 0 });

        id<MTLCommandQueue> commandQueue = nil;
        id<MTLSharedEvent> semaphore = nil;
        mutable uint64 lastSemaphoreSignalValue = 0;
        std::unique_ptr<std::mutex> commitMutex = std::make_unique<std::mutex>(); // Guards the last signal value (and is kept behind a pointer so that the device stays movable)

        // NOTE: These must match the argument buffer layout shaders are translated to Metal with (see SierraEngineEditor/src/Assets/Shaders/Compilers/SlangShaderCompiler.cpp)
        constexpr static uint32 RESOURCE_TABLE_UNIFORM_BUFFER_CAPACITY         = 8192;
        constexpr static uint32 RESOURCE_TABLE_STORAGE_BUFFER_CAPACITY         = 8192;
        constexpr static uint32 RESOURCE_TABLE_SAMPLED_IMAGE_CAPACITY          = 8192;
        constexpr static uint32 RESOURCE_TABLE_STORAGE_IMAGE_CAPACITY          = 8192;
        constexpr static uint32 RESOURCE_TABLE_SAMPLER_CAPACITY                = 1024;

    };

}
