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

    struct VulkanSemaphoreValue
    {
        VkSemaphore semaphore = VK_NULL_HANDLE;
        uint64 value = 0; // Ignored for binary semaphores
    };

    struct VulkanQueueSubmitInfo
    {
        uint32 family = 0;
        std::span<const VulkanSemaphoreValue> semaphoresToWait = { }; // Waited for prior to any work of the submission
        std::span<const VkCommandBuffer> commandBuffers = { };
        std::span<const VkSemaphore> binarySemaphoresToSignal = { }; // Signalled alongside the timeline semaphore of the family
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

        // NOTE: Every queue of a family is the first VkQueue of it, so all submissions (and presentations) to it go through here, where they are externally synchronized (as Vulkan
        // requires), and where each signals the timeline semaphore of the family with a value greater than any before it (output here, as what to wait for to know it has completed)
        [[nodiscard]] VkResult SubmitToQueue(const VulkanQueueSubmitInfo& submitInfo, VulkanSemaphoreValue& outSignalledSemaphoreValue) const;
        [[nodiscard]] VkResult PresentToQueue(uint32 family, const VkPresentInfoKHR& presentInfo) const;
        void WaitForQueuesIdle() const;

        [[nodiscard]] VkQueue GetQueue(uint32 family) const;
        [[nodiscard]] std::vector<VulkanSemaphoreValue> GetLastQueueSignalledSemaphoreValues() const; // Of the timeline semaphore of every family, as of the last submission to it

        [[nodiscard]] VkDescriptorSetLayout GetDescriptorSetLayout() const noexcept { return descriptorSetLayout; }
        [[nodiscard]] VkPipelineLayout GetPipelineLayout(const uint16 pushConstantSize) const { return pipelineLayouts[pushConstantSize / 4]; }

        [[nodiscard]] VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const noexcept;
        VkPhysicalDeviceProperties2 GetPhysicalDeviceProperties2(void* pNext = nullptr) const noexcept;

        [[nodiscard]] VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures() const noexcept;
        VkPhysicalDeviceFeatures2 GetPhysicalDeviceFeatures2(void* pNext = nullptr) const noexcept;

        [[nodiscard]] VkQueueFamilyProperties GetQueueFamilyProperties(uint32 family) const noexcept;

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

        struct QueueTimeline
        {
            uint32 family = 0;
            VkQueue queue = VK_NULL_HANDLE;

            VkSemaphore semaphore = VK_NULL_HANDLE; // Signalled by every submission to the queue, with values increasing in the order they are submitted in
            uint64 lastSignalValue = 0;
            std::mutex mutex = { }; // Guards the queue (which must be externally synchronized) and the last signal value
        };
        std::vector<std::unique_ptr<QueueTimeline>> queueTimelines = { };
        [[nodiscard]] QueueTimeline& GetQueueTimeline(uint32 family) const;

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
