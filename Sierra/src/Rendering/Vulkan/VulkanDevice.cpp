//
// Created by Nikolay Kanchevski on 10.09.23.
//

#include "VulkanDevice.h"

#define VMA_IMPLEMENTATION
#ifdef VMA_STATS_STRING_ENABLED
    #undef VMA_STATS_STRING_ENABLED
    #define VMA_STATS_STRING_ENABLED 0
#endif
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include <vk_mem_alloc.h>

#include "VulkanImage.h"
#include "VulkanQueue.h"
#include "VulkanBuffer.h"
#include "VulkanShader.h"
#include "VulkanSampler.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanErrorHandler.h"
#include "VulkanResourceTable.h"
#include "VulkanComputePipeline.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanDestructionScheduler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanDevice::VulkanDevice(const VulkanInstance& givenInstance, VkPhysicalDevice physicalDevice, const std::span<const char*> extensions, std::span<const VulkanQueueDescription> givenQueueDescriptions, const void* pNext, const DeviceCreateInfo& createInfo)
        : Device(createInfo), name(createInfo.name), instance(&givenInstance), physicalDevice(physicalDevice)
    {
        SR_THROW_IF(physicalDevice == VK_NULL_HANDLE, InvalidValueError(SR_FORMAT("Cannot create Vulkan device [{0}], as specified physical device must not be null", createInfo.name)));

        // Define queue data
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.reserve(givenQueueDescriptions.size());
        queueDescriptions.reserve(givenQueueDescriptions.size());

        // Set up queue data
        constexpr float32 QUEUE_PRIORITY = 1.0f;
        for (const VulkanQueueDescription& queueDescription : givenQueueDescriptions)
        {
            const VkDeviceQueueCreateInfo queueCreateInfo
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = queueDescription.family,
                .queueCount = 1,
                .pQueuePriorities = &QUEUE_PRIORITY
            };

            queueCreateInfos.emplace_back(queueCreateInfo);
            queueDescriptions.emplace_back(std::make_shared<VulkanQueueDescription>(queueDescription));
        }

        const VkDeviceCreateInfo deviceCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = pNext,
            .queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size()),
            .pQueueCreateInfos = queueCreateInfos.data(),
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
            .pEnabledFeatures = nullptr
        };

        // Create device
        VkResult result = instance->GetFunctionTable().vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create device [{0}]", createInfo.name));
        SetResourceName(device, VK_OBJECT_TYPE_DEVICE, SR_FORMAT("Logical device of device [{0}]", createInfo.name));

        // Retrieve hardware name
        {
            VkPhysicalDeviceProperties physicalDeviceProperties = { };
            instance->GetFunctionTable().vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

            hardwareName = physicalDeviceProperties.deviceName;
            SetResourceName(physicalDevice, VK_OBJECT_TYPE_PHYSICAL_DEVICE, hardwareName);

            vulkanVersion = Version({ static_cast<uint8>(VK_VERSION_MAJOR(physicalDeviceProperties.apiVersion)), static_cast<uint8>(VK_VERSION_MINOR(physicalDeviceProperties.apiVersion)), static_cast<uint8>(VK_VERSION_PATCH(physicalDeviceProperties.apiVersion)) });
            driverVersion = Version({ static_cast<uint8>(VK_VERSION_MAJOR(physicalDeviceProperties.driverVersion)), static_cast<uint8>(VK_VERSION_MINOR(physicalDeviceProperties.driverVersion)), static_cast<uint8>(VK_VERSION_PATCH(physicalDeviceProperties.driverVersion)) });
        }

        // Save loaded extensions
        loadedExtensions.reserve(extensions.size());
        for (const char* extension : extensions)
        {
            loadedExtensions.emplace_back(std::hash<std::string_view>{}(extension));
        }

        // Load function pointers
        functionTable = LoadVulkanDeviceFunctionTable(device);

        const VmaVulkanFunctions vulkanFunctions
        {
            .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
            .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
            .vkGetPhysicalDeviceProperties = instance->GetFunctionTable().vkGetPhysicalDeviceProperties,
            .vkGetPhysicalDeviceMemoryProperties = instance->GetFunctionTable().vkGetPhysicalDeviceMemoryProperties,
            .vkAllocateMemory = functionTable.vkAllocateMemory,
            .vkFreeMemory = functionTable.vkFreeMemory,
            .vkMapMemory = functionTable.vkMapMemory,
            .vkUnmapMemory = functionTable.vkUnmapMemory,
            .vkFlushMappedMemoryRanges = functionTable.vkFlushMappedMemoryRanges,
            .vkInvalidateMappedMemoryRanges = functionTable.vkInvalidateMappedMemoryRanges,
            .vkBindBufferMemory = functionTable.vkBindBufferMemory,
            .vkBindImageMemory = functionTable.vkBindImageMemory,
            .vkGetBufferMemoryRequirements = functionTable.vkGetBufferMemoryRequirements,
            .vkGetImageMemoryRequirements = functionTable.vkGetImageMemoryRequirements,
            .vkCreateBuffer = functionTable.vkCreateBuffer,
            .vkDestroyBuffer = functionTable.vkDestroyBuffer,
            .vkCreateImage = functionTable.vkCreateImage,
            .vkDestroyImage = functionTable.vkDestroyImage,
            .vkCmdCopyBuffer = functionTable.vkCmdCopyBuffer,
            .vkGetBufferMemoryRequirements2KHR = functionTable.vkGetBufferMemoryRequirements2 != nullptr ? functionTable.vkGetBufferMemoryRequirements2 : functionTable.vkGetBufferMemoryRequirements2KHR,
            .vkGetImageMemoryRequirements2KHR = functionTable.vkGetImageMemoryRequirements2 != nullptr ? functionTable.vkGetImageMemoryRequirements2 : functionTable.vkGetImageMemoryRequirements2KHR,
            .vkBindBufferMemory2KHR = functionTable.vkBindBufferMemory2 != nullptr ? functionTable.vkBindBufferMemory2 : functionTable.vkBindBufferMemory2KHR,
            .vkBindImageMemory2KHR = functionTable.vkBindImageMemory2 != nullptr ? functionTable.vkBindImageMemory2 : functionTable.vkBindImageMemory2KHR,
            .vkGetPhysicalDeviceMemoryProperties2KHR = instance->GetFunctionTable().vkGetPhysicalDeviceMemoryProperties2 != nullptr ? instance->GetFunctionTable().vkGetPhysicalDeviceMemoryProperties2 : instance->GetFunctionTable().vkGetPhysicalDeviceMemoryProperties2KHR,
            .vkGetDeviceBufferMemoryRequirements = functionTable.vkGetDeviceBufferMemoryRequirements,
            .vkGetDeviceImageMemoryRequirements = functionTable.vkGetDeviceImageMemoryRequirements,
            #if SR_PLATFORM_WINDOWS
                .vkGetMemoryWin32HandleKHR = functionTable.vkGetMemoryWin32HandleKHR,
            #endif
            .vkGetPhysicalDeviceProperties2KHR = instance->GetFunctionTable().vkGetPhysicalDeviceProperties2 != nullptr ? instance->GetFunctionTable().vkGetPhysicalDeviceProperties2 : instance->GetFunctionTable().vkGetPhysicalDeviceProperties2KHR
        };

        // Set up allocator create info
        const Version version = instance->GetBackendVersion();
        const VmaAllocatorCreateInfo vmaCreteInfo
        {
            .flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT,
            .physicalDevice = physicalDevice,
            .device = device,
            .pVulkanFunctions = &vulkanFunctions,
            .instance = instance->GetVulkanInstance(),
            .vulkanApiVersion = VK_MAKE_API_VERSION(0, version.GetMajor(), version.GetMinor(), version.GetPatch())
        };

        // Create allocator
        result = vmaCreateAllocator(&vmaCreteInfo, &vmaAllocator);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Cannot create Vulkan device [{0}], as creation of memory allocator failed", createInfo.name));

        // Create global timeline semaphore
        {
            SR_THROW_IF(!IsExtensionLoaded(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME), UnsupportedFeatureError(SR_FORMAT("Cannot create device Vulkan device [{0}], as it does not support the [{1}] extension", createInfo.name, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)));

            // Set up semaphore type
            constexpr VkSemaphoreTypeCreateInfo semaphoreTypeCreateInfo
            {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
                .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
                .initialValue = 0
            };

            // Set up shared semaphore create info
            const VkSemaphoreCreateInfo semaphoreCreateInfo
            {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                .pNext = &semaphoreTypeCreateInfo
            };

            // Create shared fence
            result = functionTable.vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore);
            if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Cannot create Vulkan device [{0}], as creation of timeline semaphore failed", createInfo.name));
            SetResourceName(semaphore, VK_OBJECT_TYPE_SEMAPHORE, SR_FORMAT("Semaphore of device [{0}]", createInfo.name));
        }

        // Create global (bindless) descriptor set layout
        {
            SR_THROW_IF(!IsExtensionLoaded(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME), UnsupportedFeatureError(SR_FORMAT("Cannot create device Vulkan device [{0}], as it does not support the [{1}] extension", createInfo.name, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)));

            // Retrieve descriptor indexing properties
            VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
            GetPhysicalDeviceProperties2(&descriptorIndexingProperties);

            // Set up bindings (one for each resource type)
            const std::array descriptorSetBindings
            {
                VkDescriptorSetLayoutBinding {
                    .binding = BINDLESS_UNIFORM_BUFFER_BINDING,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptorCount = glm::min(MAX_UNIFORM_BUFFERS_PER_RESOURCE_TABLE, descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindUniformBuffers),
                    .stageFlags = VK_SHADER_STAGE_ALL,
                    .pImmutableSamplers = nullptr
                },
                VkDescriptorSetLayoutBinding {
                    .binding = BINDLESS_STORAGE_BUFFER_BINDING,
                    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    .descriptorCount = glm::min(MAX_STORAGE_BUFFERS_PER_RESOURCE_TABLE, descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageBuffers),
                    .stageFlags = VK_SHADER_STAGE_ALL,
                    .pImmutableSamplers = nullptr
                },
                VkDescriptorSetLayoutBinding {
                    .binding = BINDLESS_SAMPLED_IMAGE_BINDING,
                    .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                    .descriptorCount = glm::min(MAX_SAMPLED_IMAGES_PER_RESOURCE_TABLE, descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSampledImages),
                    .stageFlags = VK_SHADER_STAGE_ALL,
                    .pImmutableSamplers = nullptr
                },
                VkDescriptorSetLayoutBinding {
                    .binding = BINDLESS_STORAGE_IMAGE_BINDING,
                    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                    .descriptorCount = glm::min(MAX_STORAGE_IMAGES_PER_RESOURCE_TABLE, descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageImages),
                    .stageFlags = VK_SHADER_STAGE_ALL,
                    .pImmutableSamplers = nullptr,
                },
                VkDescriptorSetLayoutBinding {
                    .binding = BINDLESS_SAMPLER_BINDING,
                    .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
                    .descriptorCount = glm::min(MAX_SAMPLERS_PER_RESOURCE_TABLE, descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSamplers),
                    .stageFlags = VK_SHADER_STAGE_ALL,
                    .pImmutableSamplers = nullptr
                }
            };

            // Set up flags
            constexpr std::array<VkDescriptorBindingFlagsEXT, descriptorSetBindings.size()> BINDING_FLAGS
            {
                VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | (SR_PLATFORM_APPLE * VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT),
                VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | (SR_PLATFORM_APPLE * VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT),
                VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | (SR_PLATFORM_APPLE * VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT),
                VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | (SR_PLATFORM_APPLE * VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT),
                VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | (SR_PLATFORM_APPLE * VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT)
            };

            // Set up flags create info
            const VkDescriptorSetLayoutBindingFlagsCreateInfoEXT bindingFlagsCreateInfo
            {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT,
                .bindingCount = BINDING_FLAGS.size(),
                .pBindingFlags = BINDING_FLAGS.data()
            };

            // Set up layout create info
            const VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo
            {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .pNext = &bindingFlagsCreateInfo,
                .flags = SR_PLATFORM_APPLE * VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
                .bindingCount = descriptorSetBindings.size(),
                .pBindings = descriptorSetBindings.data()
            };

            // Create bindless descriptor set layout
            result = functionTable.vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
            if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Cannot create Vulkan device [{0}], as creation of descriptor set layout failed", createInfo.name));
            SetResourceName(descriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, SR_FORMAT("Descriptor set layout of device [{0}]", createInfo.name));
        }

        // Create all pipeline layout variants in advance
        {
            // Set up pipeline layout create info (push constant ranges are to be updated)
            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                .setLayoutCount = 1,
                .pSetLayouts = &descriptorSetLayout,
                .pPushConstantRanges = nullptr
            };

            // Set up push constant range (size is to be updated)
            VkPushConstantRange pushConstantRange
            {
                .stageFlags = VK_SHADER_STAGE_ALL,
                .offset = 0,
                .size = 0
            };

            // Create layouts
            pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
            for (size i = 0; i < pipelineLayouts.size(); i++)
            {
                pushConstantRange.size = i * 4;
                pipelineLayoutCreateInfo.pushConstantRangeCount = i != 0;
                pipelineLayoutCreateInfo.pPushConstantRanges = reinterpret_cast<const VkPushConstantRange*>((i != 0) * std::uintptr_t(&pushConstantRange));

                result = functionTable.vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayouts[i]);
                if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Cannot create Vulkan device [{0}], as creation of pipeline layout [{1}] failed", createInfo.name, i));
                SetResourceName(pipelineLayouts[i], VK_OBJECT_TYPE_PIPELINE_LAYOUT, SR_FORMAT("Pipeline layout [{0}] of device [{1}]", i, createInfo.name));
            }
        }
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Buffer> VulkanDevice::CreateBuffer(const BufferCreateInfo& createInfo) const
    {
        return std::make_unique<VulkanBuffer>(*this, createInfo);
    }

    std::unique_ptr<Image> VulkanDevice::CreateImage(const ImageCreateInfo& createInfo) const
    {
        return std::make_unique<VulkanImage>(*this, createInfo);
    }

    std::unique_ptr<Sampler> VulkanDevice::CreateSampler(const SamplerCreateInfo& createInfo) const
    {
        return std::make_unique<VulkanSampler>(*this, createInfo);
    }

    std::unique_ptr<RenderPass> VulkanDevice::CreateRenderPass(const RenderPassCreateInfo& createInfo) const
    {
        return std::make_unique<VulkanRenderPass>(*this, createInfo);
    }

    std::unique_ptr<Framebuffer> VulkanDevice::CreateFramebuffer(const FramebufferCreateInfo& createInfo) const
    {
        return std::make_unique<VulkanFramebuffer>(*this, createInfo);
    }

    std::unique_ptr<Swapchain> VulkanDevice::CreateSwapchain(const SwapchainCreateInfo& createInfo) const
    {
        return std::make_unique<VulkanSwapchain>(*instance, *this, createInfo);
    }

    std::unique_ptr<Shader> VulkanDevice::CreateShader(const ShaderCreateInfo& createInfo) const
    {
        return std::make_unique<VulkanShader>(*this, createInfo);
    }

    std::unique_ptr<GraphicsPipeline> VulkanDevice::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) const
    {
        return std::make_unique<VulkanGraphicsPipeline>(*this, createInfo);
    }

    std::unique_ptr<ComputePipeline> VulkanDevice::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) const
    {
        return std::make_unique<VulkanComputePipeline>(*this, createInfo);
    }

    std::unique_ptr<ResourceTable> VulkanDevice::CreateResourceTable(const ResourceTableCreateInfo& createInfo) const
    {
        return std::make_unique<VulkanResourceTable>(*this, createInfo);
    }

    std::unique_ptr<Queue> VulkanDevice::CreateQueue(const QueueCreateInfo& createInfo) const
    {
        return std::make_unique<VulkanQueue>(*this, createInfo);
    }

    std::unique_ptr<Sierra::DestructionScheduler> VulkanDevice::CreateDestructionScheduler(const DestructionSchedulerCreateInfo& createInfo) const
    {
        return std::make_unique<VulkanDestructionScheduler>(*this, createInfo);
    }

    /* --- GETTER METHODS --- */

    DeviceLimits VulkanDevice::GetLimits() const noexcept
    {
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptorIndexingProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        VkPhysicalDeviceProperties2 physicalDeviceProperties = GetPhysicalDeviceProperties2(&descriptorIndexingProperties);

        return
        {
            .maxUniformBufferSize = physicalDeviceProperties.properties.limits.maxUniformBufferRange,
            .maxStorageBufferSize = physicalDeviceProperties.properties.limits.maxStorageBufferRange,
            .maxLineImageDimensions = physicalDeviceProperties.properties.limits.maxImageDimension1D,
            .maxPlaneImageDimensions = physicalDeviceProperties.properties.limits.maxImageDimension2D,
            .maxVolumeImageDimensions = physicalDeviceProperties.properties.limits.maxImageDimension3D,
            .maxCubeImageDimensions = physicalDeviceProperties.properties.limits.maxImageDimensionCube,
            .resourceTableUniformBufferCapacity = glm::min(descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindUniformBuffers, MAX_UNIFORM_BUFFERS_PER_RESOURCE_TABLE),
            .resourceTableStorageBufferCapacity = glm::min(descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageBuffers, MAX_STORAGE_BUFFERS_PER_RESOURCE_TABLE),
            .resourceTableSampledImageCapacity = glm::min(descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSampledImages, MAX_SAMPLED_IMAGES_PER_RESOURCE_TABLE),
            .resourceTableStorageImageCapacity = glm::min(descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageImages, MAX_STORAGE_IMAGES_PER_RESOURCE_TABLE),
            .resourceTableSamplerCapacity = glm::min(descriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSamplers, MAX_SAMPLERS_PER_RESOURCE_TABLE),
            .maxFramebufferWidth = physicalDeviceProperties.properties.limits.maxFramebufferWidth,
            .maxFramebufferHeight = physicalDeviceProperties.properties.limits.maxFramebufferHeight,
            .maxWorkGroupSize = { physicalDeviceProperties.properties.limits.maxComputeWorkGroupSize[0], physicalDeviceProperties.properties.limits.maxComputeWorkGroupSize[1], physicalDeviceProperties.properties.limits.maxComputeWorkGroupSize[2] },
            .highestImageSampling = GetHighestImageSamplingSupported(),
            .highestSamplerAnisotropy = GetHighestSamplerAnisotropySupported()
        };
    }

    bool VulkanDevice::IsImageFormatSupported(const ImageFormat format, const ImageUsage usage) const noexcept
    {
        // Get format properties
        VkFormatProperties formatProperties = { };
        instance->GetFunctionTable().vkGetPhysicalDeviceFormatProperties(physicalDevice, ImageFormatToVkFormat(format), &formatProperties);

        // Check support
        if (usage & ImageUsage::SourceMemory        && !(formatProperties.linearTilingFeatures  & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT               )) return false;
        if (usage & ImageUsage::DestinationMemory   && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_DST_BIT               )) return false;
        if (usage & ImageUsage::Storage             && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT              )) return false;
        if (usage & ImageUsage::Sample              && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT              )) return false;
        if (usage & ImageUsage::Filter              && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) return false;
        if (usage & ImageUsage::ColorAttachment     && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT           )) return false;
        if (usage & ImageUsage::DepthAttachment     && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT   )) return false;
        if (usage & ImageUsage::InputAttachment     && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT              )) return false;
        return true;
    }

    bool VulkanDevice::IsImageSamplingSupported(const ImageSampling sampling) const noexcept
    {
        const VkPhysicalDeviceProperties physicalDeviceProperties = GetPhysicalDeviceProperties();
        return physicalDeviceProperties.limits.framebufferDepthSampleCounts & ImageSamplingToVkSampleCountFlags(sampling) && physicalDeviceProperties.limits.sampledImageDepthSampleCounts & ImageSamplingToVkSampleCountFlags(sampling);
    }

    bool VulkanDevice::IsSamplerAnisotropySupported(const SamplerAnisotropy anisotropy) const noexcept
    {
        const VkPhysicalDeviceProperties physicalDeviceProperties = GetPhysicalDeviceProperties();
        if (!GetPhysicalDeviceFeatures().samplerAnisotropy) return anisotropy == SamplerAnisotropy::x1;

        switch (anisotropy)
        {
            case SamplerAnisotropy::x64:    return physicalDeviceProperties.limits.maxSamplerAnisotropy >= 64.0f;
            case SamplerAnisotropy::x32:    return physicalDeviceProperties.limits.maxSamplerAnisotropy >= 32.0f;
            case SamplerAnisotropy::x16:    return physicalDeviceProperties.limits.maxSamplerAnisotropy >= 16.0f;
            case SamplerAnisotropy::x8:     return physicalDeviceProperties.limits.maxSamplerAnisotropy >= 8.0f;
            case SamplerAnisotropy::x4:     return physicalDeviceProperties.limits.maxSamplerAnisotropy >= 4.0f;
            case SamplerAnisotropy::x2:     return physicalDeviceProperties.limits.maxSamplerAnisotropy >= 2.0f;
            case SamplerAnisotropy::x1:     break;
        }

        return true;
    }

    VkPhysicalDeviceProperties VulkanDevice::GetPhysicalDeviceProperties() const noexcept
    {
        VkPhysicalDeviceProperties physicalDeviceProperties = { };
        instance->GetFunctionTable().vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
        return physicalDeviceProperties;
    }

    VkPhysicalDeviceProperties2 VulkanDevice::GetPhysicalDeviceProperties2(void* pNext) const noexcept
    {
        VkPhysicalDeviceProperties2 physicalDeviceProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR, .pNext = pNext };
        instance->GetFunctionTable().vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties);
        return physicalDeviceProperties;
    }

    VkPhysicalDeviceFeatures VulkanDevice::GetPhysicalDeviceFeatures() const noexcept
    {
        VkPhysicalDeviceFeatures physicalDeviceFeatures = { };
        instance->GetFunctionTable().vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
        return physicalDeviceFeatures;
    }

    VkPhysicalDeviceFeatures2 VulkanDevice::GetPhysicalDeviceFeatures2(void* const pNext) const noexcept
    {
        VkPhysicalDeviceFeatures2 physicalDeviceFeatures = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR, .pNext = pNext };
        instance->GetFunctionTable().vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures);
        return physicalDeviceFeatures;
    }

    /* --- SETTER METHODS --- */

    void VulkanDevice::SetResourceName(VkHandle64 object, const VkObjectType type, const std::string_view resourceName) const noexcept
    {
        SetResourceName(reinterpret_cast<VkHandle32>(object), type, resourceName);
    }

    void VulkanDevice::SetResourceName(const VkHandle32 object, const VkObjectType type, const std::string_view resourceName) const noexcept
    {
        if (!instance->IsExtensionLoaded(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) return;

        // Set up object name info
        const VkDebugUtilsObjectNameInfoEXT objectNameInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .objectType = type,
            // ReSharper disable once CppCStyleCast
            .objectHandle = (uint64) object, // We use a C-style cast, as VkObject can be different things on different platforms
            .pObjectName = resourceName.data()
        };

        // Assign resource name
        instance->GetFunctionTable().vkSetDebugUtilsObjectNameEXT(device, &objectNameInfo);
    }

    /* --- OPERATORS --- */

    bool VulkanDevice::operator==(const VulkanDevice& other) const noexcept
    {
        VkPhysicalDeviceProperties thisPhysicalDeviceProperties = { };
        instance->GetFunctionTable().vkGetPhysicalDeviceProperties(physicalDevice, &thisPhysicalDeviceProperties);

        VkPhysicalDeviceProperties otherPhysicalDeviceProperties = { };
        other.instance->GetFunctionTable().vkGetPhysicalDeviceProperties(other.physicalDevice, &otherPhysicalDeviceProperties);

        return thisPhysicalDeviceProperties.deviceID == otherPhysicalDeviceProperties.deviceID;
    }

    bool VulkanDevice::operator!=(const VulkanDevice& other) noexcept
    {
        return !(*this == other);
    }

    /* --- DESTRUCTOR --- */

    VulkanDevice::~VulkanDevice() noexcept
    {
        for (const VkPipelineLayout& pipelineLayout : pipelineLayouts) functionTable.vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        functionTable.vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

        functionTable.vkDestroySemaphore(device, semaphore, nullptr);

        vmaDestroyAllocator(vmaAllocator);
        functionTable.vkDestroyDevice(device, nullptr);
    }

}
