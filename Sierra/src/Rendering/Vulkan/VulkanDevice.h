//
// Created by Nikolay Kanchevski on 10.09.23.
//

#pragma once

#include <vk_mem_alloc.h>

#include "../Device.h"
#include "VulkanResource.h"

#include "VulkanInstance.h"
#include "VulkanDeviceFunctionTable.h"

namespace Sierra
{

    struct VulkanQueueDescription
    {
        uint32 family = 0;
        QueueOperations operations = QueueOperations::None;
    };

    class SIERRA_API VulkanDevice final : public Device, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanDevice(const VulkanInstance& instance, VkPhysicalDevice physicalDevice, std::span<const char*> extensions, std::span<const VulkanQueueDescription> queueDescriptions, const void* pNext, const DeviceCreateInfo& createInfo);

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
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }
        [[nodiscard]] std::string_view GetHardwareName() const noexcept override { return hardwareName; }

        [[nodiscard]] Version GetBackendVersion() const noexcept override { return vulkanVersion; }
        [[nodiscard]] Version GetDriverVersion() const noexcept override { return driverVersion; }
        [[nodiscard]] DeviceLimits GetLimits() const noexcept override;

        [[nodiscard]] bool IsImageFormatSupported(ImageFormat format, ImageUsage usage) const noexcept override;
        [[nodiscard]] bool IsImageSamplingSupported(ImageSampling sampling) const noexcept override;
        [[nodiscard]] bool IsSamplerAnisotropySupported(SamplerAnisotropy anisotropy) const noexcept override;

        [[nodiscard]] const VulkanDeviceFunctionTable& GetFunctionTable() const noexcept { return functionTable; }
        [[nodiscard]] bool IsExtensionLoaded(const std::string_view extensionName) const noexcept { return std::find(loadedExtensions.begin(), loadedExtensions.end(), std::hash<std::string_view>{}(extensionName.data())) != loadedExtensions.end(); }

        [[nodiscard]] VkPhysicalDevice GetVulkanPhysicalDevice() const noexcept { return physicalDevice; }
        [[nodiscard]] VkDevice GetVulkanDevice() const noexcept { return device; }

        [[nodiscard]] VmaAllocator GetVulkanMemoryAllocator() const noexcept { return vmaAllocator; }
        [[nodiscard]] std::span<const std::shared_ptr<VulkanQueueDescription>> GetQueueDescriptions() const noexcept { return queueDescriptions; }

        [[nodiscard]] VkSemaphore GetSemaphore() const noexcept { return semaphore; }
        [[nodiscard]] uint64 GetLastSemaphoreSignalValue() const noexcept { return lastReservedSemaphoreSignalValue; }
        [[nodiscard]] uint64 GetNewSemaphoreSignalValue() const noexcept { return ++lastReservedSemaphoreSignalValue; }

        [[nodiscard]] VkDescriptorSetLayout GetDescriptorSetLayout() const noexcept { return descriptorSetLayout; }
        [[nodiscard]] VkPipelineLayout GetPipelineLayout(const uint16 pushConstantSize) const { return pipelineLayouts[pushConstantSize / 4]; }

        [[nodiscard]] VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const noexcept;
        VkPhysicalDeviceProperties2 GetPhysicalDeviceProperties2(void* pNext = nullptr) const noexcept;

        [[nodiscard]] VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures() const noexcept;
        VkPhysicalDeviceFeatures2 GetPhysicalDeviceFeatures2(void* pNext = nullptr) const noexcept;

        /* --- SETTER METHODS --- */
        void SetResourceName(VkHandle64 object, VkObjectType type, std::string_view name) const noexcept;
        void SetResourceName(VkHandle32 object, VkObjectType type, std::string_view name) const noexcept;

        /* --- CONSTANTS --- */
        constexpr static uint32 BINDLESS_UNIFORM_BUFFER_BINDING         = 0;
        constexpr static uint32 BINDLESS_STORAGE_BUFFER_BINDING         = 1;
        constexpr static uint32 BINDLESS_SAMPLED_IMAGE_BINDING          = 2;
        constexpr static uint32 BINDLESS_STORAGE_IMAGE_BINDING          = 3;
        constexpr static uint32 BINDLESS_SAMPLER_BINDING                = 4;

        constexpr static uint32 MAX_UNIFORM_BUFFERS_PER_RESOURCE_TABLE  = 8192;
        constexpr static uint32 MAX_STORAGE_BUFFERS_PER_RESOURCE_TABLE  = 8192;
        constexpr static uint32 MAX_SAMPLED_IMAGES_PER_RESOURCE_TABLE   = 8192;
        constexpr static uint32 MAX_STORAGE_IMAGES_PER_RESOURCE_TABLE   = 8192;
        constexpr static uint32 MAX_SAMPLERS_PER_RESOURCE_TABLE         = 8192;

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator==(const VulkanDevice& other) const noexcept;
        [[nodiscard]] bool operator!=(const VulkanDevice& other) noexcept;

        /* --- COPY SEMANTICS --- */
        VulkanDevice(const VulkanDevice&) = delete;
        VulkanDevice& operator=(const VulkanDevice&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanDevice(VulkanDevice&&) noexcept = default;
        VulkanDevice& operator=(VulkanDevice&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~VulkanDevice() noexcept override;

    private:
        std::string name = { };
        const VulkanInstance* instance = nullptr;

        std::string hardwareName;
        Version vulkanVersion = Version({ 1, 0, 0 });
        Version driverVersion = Version({ 1, 0, 0 });

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;

        VmaAllocator vmaAllocator = VK_NULL_HANDLE;
        std::vector<std::shared_ptr<VulkanQueueDescription>> queueDescriptions = { };

        VulkanDeviceFunctionTable functionTable = { };
        std::vector<size> loadedExtensions = { };

        VkSemaphore semaphore = VK_NULL_HANDLE;
        mutable uint64 lastReservedSemaphoreSignalValue = 0;

        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
        std::array<VkPipelineLayout, MAX_PUSH_CONSTANT_SIZE / 4 + 1> pipelineLayouts = { };

        template<typename T>
        struct DestructionQueueEntry
        {
            uint64 waitValue = 0;
            std::unique_ptr<T> resource = nullptr;
        };

        std::deque<DestructionQueueEntry<Buffer>> bufferQueue = { };
        std::deque<DestructionQueueEntry<Image>> imageQueue = { };

    };

}
