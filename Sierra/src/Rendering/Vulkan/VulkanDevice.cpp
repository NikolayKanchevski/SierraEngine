//
// Created by Nikolay Kanchevski on 10.09.23.
//

#include <vulkan/vulkan.h>
#include "VulkanDevice.h"

#define VMA_IMPLEMENTATION
#ifdef VMA_STATS_STRING_ENABLED
    #undef VMA_STATS_STRING_ENABLED
    #define VMA_STATS_STRING_ENABLED 0
#endif
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include <vk_mem_alloc.h>

#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanSampler.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"
#include "VulkanShader.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanComputePipeline.h"
#include "VulkanResourceTable.h"
#include "VulkanQueue.h"
#include "VulkanErrorHandler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanDevice::VulkanDevice(const Sierra::VulkanContext& context, VkPhysicalDevice physicalDevice, std::span<const char*> extensions, std::span<const VulkanQueueDescription> givenQueueDescriptions, const void* pNext, const DeviceCreateInfo& createInfo)
        : Device(createInfo), context(context), name(createInfo.name), physicalDevice(physicalDevice)
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
        VkResult result = context.GetFunctionTable().vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create device [{0}]", createInfo.name));
        SetResourceName(device, VK_OBJECT_TYPE_DEVICE, SR_FORMAT("Logical device of device [{0}]", name));

        // Retrieve hardware name
        {
            VkPhysicalDeviceProperties physicalDeviceProperties = { };
            context.GetFunctionTable().vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

            hardwareName = physicalDeviceProperties.deviceName;
            SetResourceName(physicalDevice, VK_OBJECT_TYPE_PHYSICAL_DEVICE, hardwareName);

            vulkanVersion = Version({ VK_VERSION_MAJOR(physicalDeviceProperties.apiVersion), VK_VERSION_MINOR(physicalDeviceProperties.apiVersion), VK_VERSION_PATCH(physicalDeviceProperties.apiVersion) });
            driverVersion = Version({ VK_VERSION_MAJOR(physicalDeviceProperties.driverVersion), VK_VERSION_MINOR(physicalDeviceProperties.driverVersion), VK_VERSION_PATCH(physicalDeviceProperties.driverVersion) });
        }

        // Save loaded extensions
        loadedExtensions.reserve(extensions.size());
        for (const char* extension : extensions)
        {
            loadedExtensions.emplace_back(std::hash<std::string_view>{}(extension));
        }

        #pragma region Function Pointers
            #if defined(VK_VERSION_1_0)
                functionTable.vkAllocateCommandBuffers           = reinterpret_cast<PFN_vkAllocateCommandBuffers>(vkGetDeviceProcAddr(device, "vkAllocateCommandBuffers"));
                functionTable.vkAllocateDescriptorSets           = reinterpret_cast<PFN_vkAllocateDescriptorSets>(vkGetDeviceProcAddr(device, "vkAllocateDescriptorSets"));
                functionTable.vkAllocateMemory                   = reinterpret_cast<PFN_vkAllocateMemory>(vkGetDeviceProcAddr(device, "vkAllocateMemory"));
                functionTable.vkBeginCommandBuffer               = reinterpret_cast<PFN_vkBeginCommandBuffer>(vkGetDeviceProcAddr(device, "vkBeginCommandBuffer"));
                functionTable.vkBindBufferMemory                 = reinterpret_cast<PFN_vkBindBufferMemory>(vkGetDeviceProcAddr(device, "vkBindBufferMemory"));
                functionTable.vkBindImageMemory                  = reinterpret_cast<PFN_vkBindImageMemory>(vkGetDeviceProcAddr(device, "vkBindImageMemory"));
                functionTable.vkCmdBeginQuery                    = reinterpret_cast<PFN_vkCmdBeginQuery>(vkGetDeviceProcAddr(device, "vkCmdBeginQuery"));
                functionTable.vkCmdBeginRenderPass               = reinterpret_cast<PFN_vkCmdBeginRenderPass>(vkGetDeviceProcAddr(device, "vkCmdBeginRenderPass"));
                functionTable.vkCmdBindDescriptorSets            = reinterpret_cast<PFN_vkCmdBindDescriptorSets>(vkGetDeviceProcAddr(device, "vkCmdBindDescriptorSets"));
                functionTable.vkCmdBindIndexBuffer               = reinterpret_cast<PFN_vkCmdBindIndexBuffer>(vkGetDeviceProcAddr(device, "vkCmdBindIndexBuffer"));
                functionTable.vkCmdBindPipeline                  = reinterpret_cast<PFN_vkCmdBindPipeline>(vkGetDeviceProcAddr(device, "vkCmdBindPipeline"));
                functionTable.vkCmdBindVertexBuffers             = reinterpret_cast<PFN_vkCmdBindVertexBuffers>(vkGetDeviceProcAddr(device, "vkCmdBindVertexBuffers"));
                functionTable.vkCmdBlitImage                     = reinterpret_cast<PFN_vkCmdBlitImage>(vkGetDeviceProcAddr(device, "vkCmdBlitImage"));
                functionTable.vkCmdClearAttachments              = reinterpret_cast<PFN_vkCmdClearAttachments>(vkGetDeviceProcAddr(device, "vkCmdClearAttachments"));
                functionTable.vkCmdClearColorImage               = reinterpret_cast<PFN_vkCmdClearColorImage>(vkGetDeviceProcAddr(device, "vkCmdClearColorImage"));
                functionTable.vkCmdClearDepthStencilImage        = reinterpret_cast<PFN_vkCmdClearDepthStencilImage>(vkGetDeviceProcAddr(device, "vkCmdClearDepthStencilImage"));
                functionTable.vkCmdCopyBuffer                    = reinterpret_cast<PFN_vkCmdCopyBuffer>(vkGetDeviceProcAddr(device, "vkCmdCopyBuffer"));
                functionTable.vkCmdCopyBufferToImage             = reinterpret_cast<PFN_vkCmdCopyBufferToImage>(vkGetDeviceProcAddr(device, "vkCmdCopyBufferToImage"));
                functionTable.vkCmdCopyImage                     = reinterpret_cast<PFN_vkCmdCopyImage>(vkGetDeviceProcAddr(device, "vkCmdCopyImage"));
                functionTable.vkCmdCopyImageToBuffer             = reinterpret_cast<PFN_vkCmdCopyImageToBuffer>(vkGetDeviceProcAddr(device, "vkCmdCopyImageToBuffer"));
                functionTable.vkCmdCopyQueryPoolResults          = reinterpret_cast<PFN_vkCmdCopyQueryPoolResults>(vkGetDeviceProcAddr(device, "vkCmdCopyQueryPoolResults"));
                functionTable.vkCmdDispatch                      = reinterpret_cast<PFN_vkCmdDispatch>(vkGetDeviceProcAddr(device, "vkCmdDispatch"));
                functionTable.vkCmdDispatchIndirect              = reinterpret_cast<PFN_vkCmdDispatchIndirect>(vkGetDeviceProcAddr(device, "vkCmdDispatchIndirect"));
                functionTable.vkCmdDraw                          = reinterpret_cast<PFN_vkCmdDraw>(vkGetDeviceProcAddr(device, "vkCmdDraw"));
                functionTable.vkCmdDrawIndexed                   = reinterpret_cast<PFN_vkCmdDrawIndexed>(vkGetDeviceProcAddr(device, "vkCmdDrawIndexed"));
                functionTable.vkCmdDrawIndexedIndirect           = reinterpret_cast<PFN_vkCmdDrawIndexedIndirect>(vkGetDeviceProcAddr(device, "vkCmdDrawIndexedIndirect"));
                functionTable.vkCmdDrawIndirect                  = reinterpret_cast<PFN_vkCmdDrawIndirect>(vkGetDeviceProcAddr(device, "vkCmdDrawIndirect"));
                functionTable.vkCmdEndQuery                      = reinterpret_cast<PFN_vkCmdEndQuery>(vkGetDeviceProcAddr(device, "vkCmdEndQuery"));
                functionTable.vkCmdEndRenderPass                 = reinterpret_cast<PFN_vkCmdEndRenderPass>(vkGetDeviceProcAddr(device, "vkCmdEndRenderPass"));
                functionTable.vkCmdExecuteCommands               = reinterpret_cast<PFN_vkCmdExecuteCommands>(vkGetDeviceProcAddr(device, "vkCmdExecuteCommands"));
                functionTable.vkCmdFillBuffer                    = reinterpret_cast<PFN_vkCmdFillBuffer>(vkGetDeviceProcAddr(device, "vkCmdFillBuffer"));
                functionTable.vkCmdNextSubpass                   = reinterpret_cast<PFN_vkCmdNextSubpass>(vkGetDeviceProcAddr(device, "vkCmdNextSubpass"));
                functionTable.vkCmdPipelineBarrier               = reinterpret_cast<PFN_vkCmdPipelineBarrier>(vkGetDeviceProcAddr(device, "vkCmdPipelineBarrier"));
                functionTable.vkCmdPushConstants                 = reinterpret_cast<PFN_vkCmdPushConstants>(vkGetDeviceProcAddr(device, "vkCmdPushConstants"));
                functionTable.vkCmdResetEvent                    = reinterpret_cast<PFN_vkCmdResetEvent>(vkGetDeviceProcAddr(device, "vkCmdResetEvent"));
                functionTable.vkCmdResetQueryPool                = reinterpret_cast<PFN_vkCmdResetQueryPool>(vkGetDeviceProcAddr(device, "vkCmdResetQueryPool"));
                functionTable.vkCmdResolveImage                  = reinterpret_cast<PFN_vkCmdResolveImage>(vkGetDeviceProcAddr(device, "vkCmdResolveImage"));
                functionTable.vkCmdSetBlendConstants             = reinterpret_cast<PFN_vkCmdSetBlendConstants>(vkGetDeviceProcAddr(device, "vkCmdSetBlendConstants"));
                functionTable.vkCmdSetDepthBias                  = reinterpret_cast<PFN_vkCmdSetDepthBias>(vkGetDeviceProcAddr(device, "vkCmdSetDepthBias"));
                functionTable.vkCmdSetDepthBounds                = reinterpret_cast<PFN_vkCmdSetDepthBounds>(vkGetDeviceProcAddr(device, "vkCmdSetDepthBounds"));
                functionTable.vkCmdSetEvent                      = reinterpret_cast<PFN_vkCmdSetEvent>(vkGetDeviceProcAddr(device, "vkCmdSetEvent"));
                functionTable.vkCmdSetLineWidth                  = reinterpret_cast<PFN_vkCmdSetLineWidth>(vkGetDeviceProcAddr(device, "vkCmdSetLineWidth"));
                functionTable.vkCmdSetScissor                    = reinterpret_cast<PFN_vkCmdSetScissor>(vkGetDeviceProcAddr(device, "vkCmdSetScissor"));
                functionTable.vkCmdSetStencilCompareMask         = reinterpret_cast<PFN_vkCmdSetStencilCompareMask>(vkGetDeviceProcAddr(device, "vkCmdSetStencilCompareMask"));
                functionTable.vkCmdSetStencilReference           = reinterpret_cast<PFN_vkCmdSetStencilReference>(vkGetDeviceProcAddr(device, "vkCmdSetStencilReference"));
                functionTable.vkCmdSetStencilWriteMask           = reinterpret_cast<PFN_vkCmdSetStencilWriteMask>(vkGetDeviceProcAddr(device, "vkCmdSetStencilWriteMask"));
                functionTable.vkCmdSetViewport                   = reinterpret_cast<PFN_vkCmdSetViewport>(vkGetDeviceProcAddr(device, "vkCmdSetViewport"));
                functionTable.vkCmdUpdateBuffer                  = reinterpret_cast<PFN_vkCmdUpdateBuffer>(vkGetDeviceProcAddr(device, "vkCmdUpdateBuffer"));
                functionTable.vkCmdWaitEvents                    = reinterpret_cast<PFN_vkCmdWaitEvents>(vkGetDeviceProcAddr(device, "vkCmdWaitEvents"));
                functionTable.vkCmdWriteTimestamp                = reinterpret_cast<PFN_vkCmdWriteTimestamp>(vkGetDeviceProcAddr(device, "vkCmdWriteTimestamp"));
                functionTable.vkCreateBuffer                     = reinterpret_cast<PFN_vkCreateBuffer>(vkGetDeviceProcAddr(device, "vkCreateBuffer"));
                functionTable.vkCreateBufferView                 = reinterpret_cast<PFN_vkCreateBufferView>(vkGetDeviceProcAddr(device, "vkCreateBufferView"));
                functionTable.vkCreateCommandPool                = reinterpret_cast<PFN_vkCreateCommandPool>(vkGetDeviceProcAddr(device, "vkCreateCommandPool"));
                functionTable.vkCreateComputePipelines           = reinterpret_cast<PFN_vkCreateComputePipelines>(vkGetDeviceProcAddr(device, "vkCreateComputePipelines"));
                functionTable.vkCreateDescriptorPool             = reinterpret_cast<PFN_vkCreateDescriptorPool>(vkGetDeviceProcAddr(device, "vkCreateDescriptorPool"));
                functionTable.vkCreateDescriptorSetLayout        = reinterpret_cast<PFN_vkCreateDescriptorSetLayout>(vkGetDeviceProcAddr(device, "vkCreateDescriptorSetLayout"));
                functionTable.vkCreateEvent                      = reinterpret_cast<PFN_vkCreateEvent>(vkGetDeviceProcAddr(device, "vkCreateEvent"));
                functionTable.vkCreateFence                      = reinterpret_cast<PFN_vkCreateFence>(vkGetDeviceProcAddr(device, "vkCreateFence"));
                functionTable.vkCreateFramebuffer                = reinterpret_cast<PFN_vkCreateFramebuffer>(vkGetDeviceProcAddr(device, "vkCreateFramebuffer"));
                functionTable.vkCreateGraphicsPipelines          = reinterpret_cast<PFN_vkCreateGraphicsPipelines>(vkGetDeviceProcAddr(device, "vkCreateGraphicsPipelines"));
                functionTable.vkCreateImage                      = reinterpret_cast<PFN_vkCreateImage>(vkGetDeviceProcAddr(device, "vkCreateImage"));
                functionTable.vkCreateImageView                  = reinterpret_cast<PFN_vkCreateImageView>(vkGetDeviceProcAddr(device, "vkCreateImageView"));
                functionTable.vkCreatePipelineCache              = reinterpret_cast<PFN_vkCreatePipelineCache>(vkGetDeviceProcAddr(device, "vkCreatePipelineCache"));
                functionTable.vkCreatePipelineLayout             = reinterpret_cast<PFN_vkCreatePipelineLayout>(vkGetDeviceProcAddr(device, "vkCreatePipelineLayout"));
                functionTable.vkCreateQueryPool                  = reinterpret_cast<PFN_vkCreateQueryPool>(vkGetDeviceProcAddr(device, "vkCreateQueryPool"));
                functionTable.vkCreateRenderPass                 = reinterpret_cast<PFN_vkCreateRenderPass>(vkGetDeviceProcAddr(device, "vkCreateRenderPass"));
                functionTable.vkCreateSampler                    = reinterpret_cast<PFN_vkCreateSampler>(vkGetDeviceProcAddr(device, "vkCreateSampler"));
                functionTable.vkCreateSemaphore                  = reinterpret_cast<PFN_vkCreateSemaphore>(vkGetDeviceProcAddr(device, "vkCreateSemaphore"));
                functionTable.vkCreateShaderModule               = reinterpret_cast<PFN_vkCreateShaderModule>(vkGetDeviceProcAddr(device, "vkCreateShaderModule"));
                functionTable.vkDestroyBuffer                    = reinterpret_cast<PFN_vkDestroyBuffer>(vkGetDeviceProcAddr(device, "vkDestroyBuffer"));
                functionTable.vkDestroyBufferView                = reinterpret_cast<PFN_vkDestroyBufferView>(vkGetDeviceProcAddr(device, "vkDestroyBufferView"));
                functionTable.vkDestroyCommandPool               = reinterpret_cast<PFN_vkDestroyCommandPool>(vkGetDeviceProcAddr(device, "vkDestroyCommandPool"));
                functionTable.vkDestroyDescriptorPool            = reinterpret_cast<PFN_vkDestroyDescriptorPool>(vkGetDeviceProcAddr(device, "vkDestroyDescriptorPool"));
                functionTable.vkDestroyDescriptorSetLayout       = reinterpret_cast<PFN_vkDestroyDescriptorSetLayout>(vkGetDeviceProcAddr(device, "vkDestroyDescriptorSetLayout"));
                functionTable.vkDestroyDevice                    = reinterpret_cast<PFN_vkDestroyDevice>(vkGetDeviceProcAddr(device, "vkDestroyDevice"));
                functionTable.vkDestroyEvent                     = reinterpret_cast<PFN_vkDestroyEvent>(vkGetDeviceProcAddr(device, "vkDestroyEvent"));
                functionTable.vkDestroyFence                     = reinterpret_cast<PFN_vkDestroyFence>(vkGetDeviceProcAddr(device, "vkDestroyFence"));
                functionTable.vkDestroyFramebuffer               = reinterpret_cast<PFN_vkDestroyFramebuffer>(vkGetDeviceProcAddr(device, "vkDestroyFramebuffer"));
                functionTable.vkDestroyImage                     = reinterpret_cast<PFN_vkDestroyImage>(vkGetDeviceProcAddr(device, "vkDestroyImage"));
                functionTable.vkDestroyImageView                 = reinterpret_cast<PFN_vkDestroyImageView>(vkGetDeviceProcAddr(device, "vkDestroyImageView"));
                functionTable.vkDestroyPipeline                  = reinterpret_cast<PFN_vkDestroyPipeline>(vkGetDeviceProcAddr(device, "vkDestroyPipeline"));
                functionTable.vkDestroyPipelineCache             = reinterpret_cast<PFN_vkDestroyPipelineCache>(vkGetDeviceProcAddr(device, "vkDestroyPipelineCache"));
                functionTable.vkDestroyPipelineLayout            = reinterpret_cast<PFN_vkDestroyPipelineLayout>(vkGetDeviceProcAddr(device, "vkDestroyPipelineLayout"));
                functionTable.vkDestroyQueryPool                 = reinterpret_cast<PFN_vkDestroyQueryPool>(vkGetDeviceProcAddr(device, "vkDestroyQueryPool"));
                functionTable.vkDestroyRenderPass                = reinterpret_cast<PFN_vkDestroyRenderPass>(vkGetDeviceProcAddr(device, "vkDestroyRenderPass"));
                functionTable.vkDestroySampler                   = reinterpret_cast<PFN_vkDestroySampler>(vkGetDeviceProcAddr(device, "vkDestroySampler"));
                functionTable.vkDestroySemaphore                 = reinterpret_cast<PFN_vkDestroySemaphore>(vkGetDeviceProcAddr(device, "vkDestroySemaphore"));
                functionTable.vkDestroyShaderModule              = reinterpret_cast<PFN_vkDestroyShaderModule>(vkGetDeviceProcAddr(device, "vkDestroyShaderModule"));
                functionTable.vkDeviceWaitIdle                   = reinterpret_cast<PFN_vkDeviceWaitIdle>(vkGetDeviceProcAddr(device, "vkDeviceWaitIdle"));
                functionTable.vkEndCommandBuffer                 = reinterpret_cast<PFN_vkEndCommandBuffer>(vkGetDeviceProcAddr(device, "vkEndCommandBuffer"));
                functionTable.vkFlushMappedMemoryRanges          = reinterpret_cast<PFN_vkFlushMappedMemoryRanges>(vkGetDeviceProcAddr(device, "vkFlushMappedMemoryRanges"));
                functionTable.vkFreeCommandBuffers               = reinterpret_cast<PFN_vkFreeCommandBuffers>(vkGetDeviceProcAddr(device, "vkFreeCommandBuffers"));
                functionTable.vkFreeDescriptorSets               = reinterpret_cast<PFN_vkFreeDescriptorSets>(vkGetDeviceProcAddr(device, "vkFreeDescriptorSets"));
                functionTable.vkFreeMemory                       = reinterpret_cast<PFN_vkFreeMemory>(vkGetDeviceProcAddr(device, "vkFreeMemory"));
                functionTable.vkGetBufferMemoryRequirements      = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(vkGetDeviceProcAddr(device, "vkGetBufferMemoryRequirements"));
                functionTable.vkGetDeviceMemoryCommitment        = reinterpret_cast<PFN_vkGetDeviceMemoryCommitment>(vkGetDeviceProcAddr(device, "vkGetDeviceMemoryCommitment"));
                functionTable.vkGetDeviceQueue                   = reinterpret_cast<PFN_vkGetDeviceQueue>(vkGetDeviceProcAddr(device, "vkGetDeviceQueue"));
                functionTable.vkGetEventStatus                   = reinterpret_cast<PFN_vkGetEventStatus>(vkGetDeviceProcAddr(device, "vkGetEventStatus"));
                functionTable.vkGetFenceStatus                   = reinterpret_cast<PFN_vkGetFenceStatus>(vkGetDeviceProcAddr(device, "vkGetFenceStatus"));
                functionTable.vkGetImageMemoryRequirements       = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(vkGetDeviceProcAddr(device, "vkGetImageMemoryRequirements"));
                functionTable.vkGetImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements>(vkGetDeviceProcAddr(device, "vkGetImageSparseMemoryRequirements"));
                functionTable.vkGetImageSubresourceLayout        = reinterpret_cast<PFN_vkGetImageSubresourceLayout>(vkGetDeviceProcAddr(device, "vkGetImageSubresourceLayout"));
                functionTable.vkGetPipelineCacheData             = reinterpret_cast<PFN_vkGetPipelineCacheData>(vkGetDeviceProcAddr(device, "vkGetPipelineCacheData"));
                functionTable.vkGetQueryPoolResults              = reinterpret_cast<PFN_vkGetQueryPoolResults>(vkGetDeviceProcAddr(device, "vkGetQueryPoolResults"));
                functionTable.vkGetRenderAreaGranularity         = reinterpret_cast<PFN_vkGetRenderAreaGranularity>(vkGetDeviceProcAddr(device, "vkGetRenderAreaGranularity"));
                functionTable.vkInvalidateMappedMemoryRanges     = reinterpret_cast<PFN_vkInvalidateMappedMemoryRanges>(vkGetDeviceProcAddr(device, "vkInvalidateMappedMemoryRanges"));
                functionTable.vkMapMemory                        = reinterpret_cast<PFN_vkMapMemory>(vkGetDeviceProcAddr(device, "vkMapMemory"));
                functionTable.vkMergePipelineCaches              = reinterpret_cast<PFN_vkMergePipelineCaches>(vkGetDeviceProcAddr(device, "vkMergePipelineCaches"));
                functionTable.vkQueueBindSparse                  = reinterpret_cast<PFN_vkQueueBindSparse>(vkGetDeviceProcAddr(device, "vkQueueBindSparse"));
                functionTable.vkQueueSubmit                      = reinterpret_cast<PFN_vkQueueSubmit>(vkGetDeviceProcAddr(device, "vkQueueSubmit"));
                functionTable.vkQueueWaitIdle                    = reinterpret_cast<PFN_vkQueueWaitIdle>(vkGetDeviceProcAddr(device, "vkQueueWaitIdle"));
                functionTable.vkResetCommandBuffer               = reinterpret_cast<PFN_vkResetCommandBuffer>(vkGetDeviceProcAddr(device, "vkResetCommandBuffer"));
                functionTable.vkResetCommandPool                 = reinterpret_cast<PFN_vkResetCommandPool>(vkGetDeviceProcAddr(device, "vkResetCommandPool"));
                functionTable.vkResetDescriptorPool              = reinterpret_cast<PFN_vkResetDescriptorPool>(vkGetDeviceProcAddr(device, "vkResetDescriptorPool"));
                functionTable.vkResetEvent                       = reinterpret_cast<PFN_vkResetEvent>(vkGetDeviceProcAddr(device, "vkResetEvent"));
                functionTable.vkResetFences                      = reinterpret_cast<PFN_vkResetFences>(vkGetDeviceProcAddr(device, "vkResetFences"));
                functionTable.vkSetEvent                         = reinterpret_cast<PFN_vkSetEvent>(vkGetDeviceProcAddr(device, "vkSetEvent"));
                functionTable.vkUnmapMemory                      = reinterpret_cast<PFN_vkUnmapMemory>(vkGetDeviceProcAddr(device, "vkUnmapMemory"));
                functionTable.vkUpdateDescriptorSets             = reinterpret_cast<PFN_vkUpdateDescriptorSets>(vkGetDeviceProcAddr(device, "vkUpdateDescriptorSets"));
                functionTable.vkWaitForFences                    = reinterpret_cast<PFN_vkWaitForFences>(vkGetDeviceProcAddr(device, "vkWaitForFences"));
            #endif
            #if defined(VK_VERSION_1_1)
                functionTable.vkBindBufferMemory2                 = reinterpret_cast<PFN_vkBindBufferMemory2>(vkGetDeviceProcAddr(device, "vkBindBufferMemory2"));
                functionTable.vkBindImageMemory2                  = reinterpret_cast<PFN_vkBindImageMemory2>(vkGetDeviceProcAddr(device, "vkBindImageMemory2"));
                functionTable.vkCmdDispatchBase                   = reinterpret_cast<PFN_vkCmdDispatchBase>(vkGetDeviceProcAddr(device, "vkCmdDispatchBase"));
                functionTable.vkCmdSetDeviceMask                  = reinterpret_cast<PFN_vkCmdSetDeviceMask>(vkGetDeviceProcAddr(device, "vkCmdSetDeviceMask"));
                functionTable.vkCreateDescriptorUpdateTemplate    = reinterpret_cast<PFN_vkCreateDescriptorUpdateTemplate>(vkGetDeviceProcAddr(device, "vkCreateDescriptorUpdateTemplate"));
                functionTable.vkCreateSamplerYcbcrConversion      = reinterpret_cast<PFN_vkCreateSamplerYcbcrConversion>(vkGetDeviceProcAddr(device, "vkCreateSamplerYcbcrConversion"));
                functionTable.vkDestroyDescriptorUpdateTemplate   = reinterpret_cast<PFN_vkDestroyDescriptorUpdateTemplate>(vkGetDeviceProcAddr(device, "vkDestroyDescriptorUpdateTemplate"));
                functionTable.vkDestroySamplerYcbcrConversion     = reinterpret_cast<PFN_vkDestroySamplerYcbcrConversion>(vkGetDeviceProcAddr(device, "vkDestroySamplerYcbcrConversion"));
                functionTable.vkGetBufferMemoryRequirements2      = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2>(vkGetDeviceProcAddr(device, "vkGetBufferMemoryRequirements2"));
                functionTable.vkGetDescriptorSetLayoutSupport     = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSupport>(vkGetDeviceProcAddr(device, "vkGetDescriptorSetLayoutSupport"));
                functionTable.vkGetDeviceGroupPeerMemoryFeatures  = reinterpret_cast<PFN_vkGetDeviceGroupPeerMemoryFeatures>(vkGetDeviceProcAddr(device, "vkGetDeviceGroupPeerMemoryFeatures"));
                functionTable.vkGetDeviceQueue2                   = reinterpret_cast<PFN_vkGetDeviceQueue2>(vkGetDeviceProcAddr(device, "vkGetDeviceQueue2"));
                functionTable.vkGetImageMemoryRequirements2       = reinterpret_cast<PFN_vkGetImageMemoryRequirements2>(vkGetDeviceProcAddr(device, "vkGetImageMemoryRequirements2"));
                functionTable.vkGetImageSparseMemoryRequirements2 = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements2>(vkGetDeviceProcAddr(device, "vkGetImageSparseMemoryRequirements2"));
                functionTable.vkTrimCommandPool                   = reinterpret_cast<PFN_vkTrimCommandPool>(vkGetDeviceProcAddr(device, "vkTrimCommandPool"));
                functionTable.vkUpdateDescriptorSetWithTemplate   = reinterpret_cast<PFN_vkUpdateDescriptorSetWithTemplate>(vkGetDeviceProcAddr(device, "vkUpdateDescriptorSetWithTemplate"));
            #endif
            #if defined(VK_VERSION_1_2)
                functionTable.vkCmdBeginRenderPass2                 = reinterpret_cast<PFN_vkCmdBeginRenderPass2>(vkGetDeviceProcAddr(device, "vkCmdBeginRenderPass2"));
                functionTable.vkCmdDrawIndexedIndirectCount         = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCount>(vkGetDeviceProcAddr(device, "vkCmdDrawIndexedIndirectCount"));
                functionTable.vkCmdDrawIndirectCount                = reinterpret_cast<PFN_vkCmdDrawIndirectCount>(vkGetDeviceProcAddr(device, "vkCmdDrawIndirectCount"));
                functionTable.vkCmdEndRenderPass2                   = reinterpret_cast<PFN_vkCmdEndRenderPass2>(vkGetDeviceProcAddr(device, "vkCmdEndRenderPass2"));
                functionTable.vkCmdNextSubpass2                     = reinterpret_cast<PFN_vkCmdNextSubpass2>(vkGetDeviceProcAddr(device, "vkCmdNextSubpass2"));
                functionTable.vkCreateRenderPass2                   = reinterpret_cast<PFN_vkCreateRenderPass2>(vkGetDeviceProcAddr(device, "vkCreateRenderPass2"));
                functionTable.vkGetBufferDeviceAddress              = reinterpret_cast<PFN_vkGetBufferDeviceAddress>(vkGetDeviceProcAddr(device, "vkGetBufferDeviceAddress"));
                functionTable.vkGetBufferOpaqueCaptureAddress       = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureAddress>(vkGetDeviceProcAddr(device, "vkGetBufferOpaqueCaptureAddress"));
                functionTable.vkGetDeviceMemoryOpaqueCaptureAddress = reinterpret_cast<PFN_vkGetDeviceMemoryOpaqueCaptureAddress>(vkGetDeviceProcAddr(device, "vkGetDeviceMemoryOpaqueCaptureAddress"));
                functionTable.vkGetSemaphoreCounterValue            = reinterpret_cast<PFN_vkGetSemaphoreCounterValue>(vkGetDeviceProcAddr(device, "vkGetSemaphoreCounterValue"));
                functionTable.vkResetQueryPool                      = reinterpret_cast<PFN_vkResetQueryPool>(vkGetDeviceProcAddr(device, "vkResetQueryPool"));
                functionTable.vkSignalSemaphore                     = reinterpret_cast<PFN_vkSignalSemaphore>(vkGetDeviceProcAddr(device, "vkSignalSemaphore"));
                functionTable.vkWaitSemaphores                      = reinterpret_cast<PFN_vkWaitSemaphores>(vkGetDeviceProcAddr(device, "vkWaitSemaphores"));
            #endif
            #if defined(VK_VERSION_1_3)
                functionTable.vkCmdBeginRendering                      = reinterpret_cast<PFN_vkCmdBeginRendering>(vkGetDeviceProcAddr(device, "vkCmdBeginRendering"));
                functionTable.vkCmdBindVertexBuffers2                  = reinterpret_cast<PFN_vkCmdBindVertexBuffers2>(vkGetDeviceProcAddr(device, "vkCmdBindVertexBuffers2"));
                functionTable.vkCmdBlitImage2                          = reinterpret_cast<PFN_vkCmdBlitImage2>(vkGetDeviceProcAddr(device, "vkCmdBlitImage2"));
                functionTable.vkCmdCopyBuffer2                         = reinterpret_cast<PFN_vkCmdCopyBuffer2>(vkGetDeviceProcAddr(device, "vkCmdCopyBuffer2"));
                functionTable.vkCmdCopyBufferToImage2                  = reinterpret_cast<PFN_vkCmdCopyBufferToImage2>(vkGetDeviceProcAddr(device, "vkCmdCopyBufferToImage2"));
                functionTable.vkCmdCopyImage2                          = reinterpret_cast<PFN_vkCmdCopyImage2>(vkGetDeviceProcAddr(device, "vkCmdCopyImage2"));
                functionTable.vkCmdCopyImageToBuffer2                  = reinterpret_cast<PFN_vkCmdCopyImageToBuffer2>(vkGetDeviceProcAddr(device, "vkCmdCopyImageToBuffer2"));
                functionTable.vkCmdEndRendering                        = reinterpret_cast<PFN_vkCmdEndRendering>(vkGetDeviceProcAddr(device, "vkCmdEndRendering"));
                functionTable.vkCmdPipelineBarrier2                    = reinterpret_cast<PFN_vkCmdPipelineBarrier2>(vkGetDeviceProcAddr(device, "vkCmdPipelineBarrier2"));
                functionTable.vkCmdResetEvent2                         = reinterpret_cast<PFN_vkCmdResetEvent2>(vkGetDeviceProcAddr(device, "vkCmdResetEvent2"));
                functionTable.vkCmdResolveImage2                       = reinterpret_cast<PFN_vkCmdResolveImage2>(vkGetDeviceProcAddr(device, "vkCmdResolveImage2"));
                functionTable.vkCmdSetCullMode                         = reinterpret_cast<PFN_vkCmdSetCullMode>(vkGetDeviceProcAddr(device, "vkCmdSetCullMode"));
                functionTable.vkCmdSetDepthBiasEnable                  = reinterpret_cast<PFN_vkCmdSetDepthBiasEnable>(vkGetDeviceProcAddr(device, "vkCmdSetDepthBiasEnable"));
                functionTable.vkCmdSetDepthBoundsTestEnable            = reinterpret_cast<PFN_vkCmdSetDepthBoundsTestEnable>(vkGetDeviceProcAddr(device, "vkCmdSetDepthBoundsTestEnable"));
                functionTable.vkCmdSetDepthCompareOp                   = reinterpret_cast<PFN_vkCmdSetDepthCompareOp>(vkGetDeviceProcAddr(device, "vkCmdSetDepthCompareOp"));
                functionTable.vkCmdSetDepthTestEnable                  = reinterpret_cast<PFN_vkCmdSetDepthTestEnable>(vkGetDeviceProcAddr(device, "vkCmdSetDepthTestEnable"));
                functionTable.vkCmdSetDepthWriteEnable                 = reinterpret_cast<PFN_vkCmdSetDepthWriteEnable>(vkGetDeviceProcAddr(device, "vkCmdSetDepthWriteEnable"));
                functionTable.vkCmdSetEvent2                           = reinterpret_cast<PFN_vkCmdSetEvent2>(vkGetDeviceProcAddr(device, "vkCmdSetEvent2"));
                functionTable.vkCmdSetFrontFace                        = reinterpret_cast<PFN_vkCmdSetFrontFace>(vkGetDeviceProcAddr(device, "vkCmdSetFrontFace"));
                functionTable.vkCmdSetPrimitiveRestartEnable           = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnable>(vkGetDeviceProcAddr(device, "vkCmdSetPrimitiveRestartEnable"));
                functionTable.vkCmdSetPrimitiveTopology                = reinterpret_cast<PFN_vkCmdSetPrimitiveTopology>(vkGetDeviceProcAddr(device, "vkCmdSetPrimitiveTopology"));
                functionTable.vkCmdSetRasterizerDiscardEnable          = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnable>(vkGetDeviceProcAddr(device, "vkCmdSetRasterizerDiscardEnable"));
                functionTable.vkCmdSetScissorWithCount                 = reinterpret_cast<PFN_vkCmdSetScissorWithCount>(vkGetDeviceProcAddr(device, "vkCmdSetScissorWithCount"));
                functionTable.vkCmdSetStencilOp                        = reinterpret_cast<PFN_vkCmdSetStencilOp>(vkGetDeviceProcAddr(device, "vkCmdSetStencilOp"));
                functionTable.vkCmdSetStencilTestEnable                = reinterpret_cast<PFN_vkCmdSetStencilTestEnable>(vkGetDeviceProcAddr(device, "vkCmdSetStencilTestEnable"));
                functionTable.vkCmdSetViewportWithCount                = reinterpret_cast<PFN_vkCmdSetViewportWithCount>(vkGetDeviceProcAddr(device, "vkCmdSetViewportWithCount"));
                functionTable.vkCmdWaitEvents2                         = reinterpret_cast<PFN_vkCmdWaitEvents2>(vkGetDeviceProcAddr(device, "vkCmdWaitEvents2"));
                functionTable.vkCmdWriteTimestamp2                     = reinterpret_cast<PFN_vkCmdWriteTimestamp2>(vkGetDeviceProcAddr(device, "vkCmdWriteTimestamp2"));
                functionTable.vkCreatePrivateDataSlot                  = reinterpret_cast<PFN_vkCreatePrivateDataSlot>(vkGetDeviceProcAddr(device, "vkCreatePrivateDataSlot"));
                functionTable.vkDestroyPrivateDataSlot                 = reinterpret_cast<PFN_vkDestroyPrivateDataSlot>(vkGetDeviceProcAddr(device, "vkDestroyPrivateDataSlot"));
                functionTable.vkGetDeviceBufferMemoryRequirements      = reinterpret_cast<PFN_vkGetDeviceBufferMemoryRequirements>(vkGetDeviceProcAddr(device, "vkGetDeviceBufferMemoryRequirements"));
                functionTable.vkGetDeviceImageMemoryRequirements       = reinterpret_cast<PFN_vkGetDeviceImageMemoryRequirements>(vkGetDeviceProcAddr(device, "vkGetDeviceImageMemoryRequirements"));
                functionTable.vkGetDeviceImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageSparseMemoryRequirements>(vkGetDeviceProcAddr(device, "vkGetDeviceImageSparseMemoryRequirements"));
                functionTable.vkGetPrivateData                         = reinterpret_cast<PFN_vkGetPrivateData>(vkGetDeviceProcAddr(device, "vkGetPrivateData"));
                functionTable.vkQueueSubmit2                           = reinterpret_cast<PFN_vkQueueSubmit2>(vkGetDeviceProcAddr(device, "vkQueueSubmit2"));
                functionTable.vkSetPrivateData                         = reinterpret_cast<PFN_vkSetPrivateData>(vkGetDeviceProcAddr(device, "vkSetPrivateData"));
            #endif
            #if defined(VK_AMD_buffer_marker)
                functionTable.vkCmdWriteBufferMarkerAMD = reinterpret_cast<PFN_vkCmdWriteBufferMarkerAMD>(vkGetDeviceProcAddr(device, "vkCmdWriteBufferMarkerAMD"));
            #endif
            #if defined(VK_AMD_display_native_hdr)
                functionTable.vkSetLocalDimmingAMD = reinterpret_cast<PFN_vkSetLocalDimmingAMD>(vkGetDeviceProcAddr(device, "vkSetLocalDimmingAMD"));
            #endif
            #if defined(VK_AMD_draw_indirect_count)
                functionTable.vkCmdDrawIndexedIndirectCountAMD = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCountAMD>(vkGetDeviceProcAddr(device, "vkCmdDrawIndexedIndirectCountAMD"));
                functionTable.vkCmdDrawIndirectCountAMD        = reinterpret_cast<PFN_vkCmdDrawIndirectCountAMD>(vkGetDeviceProcAddr(device, "vkCmdDrawIndirectCountAMD"));
            #endif
            #if defined(VK_AMD_shader_info)
                functionTable.vkGetShaderInfoAMD = reinterpret_cast<PFN_vkGetShaderInfoAMD>(vkGetDeviceProcAddr(device, "vkGetShaderInfoAMD"));
            #endif
            #if defined(VK_ANDROID_external_memory_android_hardware_buffer)
                functionTable.vkGetAndroidHardwareBufferPropertiesANDROID = reinterpret_cast<PFN_vkGetAndroidHardwareBufferPropertiesANDROID>(vkGetDeviceProcAddr(device, "vkGetAndroidHardwareBufferPropertiesANDROID"));
                functionTable.vkGetMemoryAndroidHardwareBufferANDROID     = reinterpret_cast<PFN_vkGetMemoryAndroidHardwareBufferANDROID>(vkGetDeviceProcAddr(device, "vkGetMemoryAndroidHardwareBufferANDROID"));
            #endif
            #if defined(VK_EXT_attachment_feedback_loop_dynamic_state)
                functionTable.vkCmdSetAttachmentFeedbackLoopEnableEXT = reinterpret_cast<PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetAttachmentFeedbackLoopEnableEXT"));
            #endif
            #if defined(VK_EXT_buffer_device_address)
                functionTable.vkGetBufferDeviceAddressEXT = reinterpret_cast<PFN_vkGetBufferDeviceAddressEXT>(vkGetDeviceProcAddr(device, "vkGetBufferDeviceAddressEXT"));
            #endif
            #if defined(VK_EXT_calibrated_timestamps)
                functionTable.vkGetCalibratedTimestampsEXT = reinterpret_cast<PFN_vkGetCalibratedTimestampsEXT>(vkGetDeviceProcAddr(device, "vkGetCalibratedTimestampsEXT"));
            #endif
            #if defined(VK_EXT_color_write_enable)
                functionTable.vkCmdSetColorWriteEnableEXT = reinterpret_cast<PFN_vkCmdSetColorWriteEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetColorWriteEnableEXT"));
            #endif
            #if defined(VK_EXT_conditional_rendering)
                functionTable.vkCmdBeginConditionalRenderingEXT = reinterpret_cast<PFN_vkCmdBeginConditionalRenderingEXT>(vkGetDeviceProcAddr(device, "vkCmdBeginConditionalRenderingEXT"));
                functionTable.vkCmdEndConditionalRenderingEXT   = reinterpret_cast<PFN_vkCmdEndConditionalRenderingEXT>(vkGetDeviceProcAddr(device, "vkCmdEndConditionalRenderingEXT"));
            #endif
            #if defined(VK_EXT_debug_marker)
                functionTable.vkCmdDebugMarkerBeginEXT      = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(vkGetDeviceProcAddr(device, "vkCmdDebugMarkerBeginEXT"));
                functionTable.vkCmdDebugMarkerEndEXT        = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>(vkGetDeviceProcAddr(device, "vkCmdDebugMarkerEndEXT"));
                functionTable.vkCmdDebugMarkerInsertEXT     = reinterpret_cast<PFN_vkCmdDebugMarkerInsertEXT>(vkGetDeviceProcAddr(device, "vkCmdDebugMarkerInsertEXT"));
                functionTable.vkDebugMarkerSetObjectNameEXT = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>(vkGetDeviceProcAddr(device, "vkDebugMarkerSetObjectNameEXT"));
                functionTable.vkDebugMarkerSetObjectTagEXT  = reinterpret_cast<PFN_vkDebugMarkerSetObjectTagEXT>(vkGetDeviceProcAddr(device, "vkDebugMarkerSetObjectTagEXT"));
            #endif
            #if defined(VK_EXT_depth_bias_control)
                functionTable.vkCmdSetDepthBias2EXT = reinterpret_cast<PFN_vkCmdSetDepthBias2EXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthBias2EXT"));
            #endif
            #if defined(VK_EXT_descriptor_buffer)
                functionTable.vkCmdBindDescriptorBufferEmbeddedSamplersEXT = reinterpret_cast<PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT>(vkGetDeviceProcAddr(device, "vkCmdBindDescriptorBufferEmbeddedSamplersEXT"));
                functionTable.vkCmdBindDescriptorBuffersEXT                = reinterpret_cast<PFN_vkCmdBindDescriptorBuffersEXT>(vkGetDeviceProcAddr(device, "vkCmdBindDescriptorBuffersEXT"));
                functionTable.vkCmdSetDescriptorBufferOffsetsEXT           = reinterpret_cast<PFN_vkCmdSetDescriptorBufferOffsetsEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDescriptorBufferOffsetsEXT"));
                functionTable.vkGetBufferOpaqueCaptureDescriptorDataEXT    = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT>(vkGetDeviceProcAddr(device, "vkGetBufferOpaqueCaptureDescriptorDataEXT"));
                functionTable.vkGetDescriptorEXT                           = reinterpret_cast<PFN_vkGetDescriptorEXT>(vkGetDeviceProcAddr(device, "vkGetDescriptorEXT"));
                functionTable.vkGetDescriptorSetLayoutBindingOffsetEXT     = reinterpret_cast<PFN_vkGetDescriptorSetLayoutBindingOffsetEXT>(vkGetDeviceProcAddr(device, "vkGetDescriptorSetLayoutBindingOffsetEXT"));
                functionTable.vkGetDescriptorSetLayoutSizeEXT              = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSizeEXT>(vkGetDeviceProcAddr(device, "vkGetDescriptorSetLayoutSizeEXT"));
                functionTable.vkGetImageOpaqueCaptureDescriptorDataEXT     = reinterpret_cast<PFN_vkGetImageOpaqueCaptureDescriptorDataEXT>(vkGetDeviceProcAddr(device, "vkGetImageOpaqueCaptureDescriptorDataEXT"));
                functionTable.vkGetImageViewOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT>(vkGetDeviceProcAddr(device, "vkGetImageViewOpaqueCaptureDescriptorDataEXT"));
                functionTable.vkGetSamplerOpaqueCaptureDescriptorDataEXT   = reinterpret_cast<PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT>(vkGetDeviceProcAddr(device, "vkGetSamplerOpaqueCaptureDescriptorDataEXT"));
            #endif
            #if defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing))
                functionTable.vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT"));
            #endif
            #if defined(VK_EXT_device_fault)
                functionTable.vkGetDeviceFaultInfoEXT = reinterpret_cast<PFN_vkGetDeviceFaultInfoEXT>(vkGetDeviceProcAddr(device, "vkGetDeviceFaultInfoEXT"));
            #endif
            #if defined(VK_EXT_discard_rectangles)
                functionTable.vkCmdSetDiscardRectangleEXT = reinterpret_cast<PFN_vkCmdSetDiscardRectangleEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDiscardRectangleEXT"));
            #endif
            #if defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2
                functionTable.vkCmdSetDiscardRectangleEnableEXT = reinterpret_cast<PFN_vkCmdSetDiscardRectangleEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDiscardRectangleEnableEXT"));
                functionTable.vkCmdSetDiscardRectangleModeEXT   = reinterpret_cast<PFN_vkCmdSetDiscardRectangleModeEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDiscardRectangleModeEXT"));
            #endif
            #if defined(VK_EXT_display_control)
                functionTable.vkDisplayPowerControlEXT  = reinterpret_cast<PFN_vkDisplayPowerControlEXT>(vkGetDeviceProcAddr(device, "vkDisplayPowerControlEXT"));
                functionTable.vkGetSwapchainCounterEXT  = reinterpret_cast<PFN_vkGetSwapchainCounterEXT>(vkGetDeviceProcAddr(device, "vkGetSwapchainCounterEXT"));
                functionTable.vkRegisterDeviceEventEXT  = reinterpret_cast<PFN_vkRegisterDeviceEventEXT>(vkGetDeviceProcAddr(device, "vkRegisterDeviceEventEXT"));
                functionTable.vkRegisterDisplayEventEXT = reinterpret_cast<PFN_vkRegisterDisplayEventEXT>(vkGetDeviceProcAddr(device, "vkRegisterDisplayEventEXT"));
            #endif
            #if defined(VK_EXT_external_memory_host)
                functionTable.vkGetMemoryHostPointerPropertiesEXT = reinterpret_cast<PFN_vkGetMemoryHostPointerPropertiesEXT>(vkGetDeviceProcAddr(device, "vkGetMemoryHostPointerPropertiesEXT"));
            #endif
            #if defined(VK_EXT_full_screen_exclusive)
                functionTable.vkAcquireFullScreenExclusiveModeEXT = reinterpret_cast<PFN_vkAcquireFullScreenExclusiveModeEXT>(vkGetDeviceProcAddr(device, "vkAcquireFullScreenExclusiveModeEXT"));
                functionTable.vkReleaseFullScreenExclusiveModeEXT = reinterpret_cast<PFN_vkReleaseFullScreenExclusiveModeEXT>(vkGetDeviceProcAddr(device, "vkReleaseFullScreenExclusiveModeEXT"));
            #endif
            #if defined(VK_EXT_hdr_metadata)
                functionTable.vkSetHdrMetadataEXT = reinterpret_cast<PFN_vkSetHdrMetadataEXT>(vkGetDeviceProcAddr(device, "vkSetHdrMetadataEXT"));
            #endif
            #if defined(VK_EXT_host_image_copy)
                functionTable.vkCopyImageToImageEXT      = reinterpret_cast<PFN_vkCopyImageToImageEXT>(vkGetDeviceProcAddr(device, "vkCopyImageToImageEXT"));
                functionTable.vkCopyImageToMemoryEXT     = reinterpret_cast<PFN_vkCopyImageToMemoryEXT>(vkGetDeviceProcAddr(device, "vkCopyImageToMemoryEXT"));
                functionTable.vkCopyMemoryToImageEXT     = reinterpret_cast<PFN_vkCopyMemoryToImageEXT>(vkGetDeviceProcAddr(device, "vkCopyMemoryToImageEXT"));
                functionTable.vkTransitionImageLayoutEXT = reinterpret_cast<PFN_vkTransitionImageLayoutEXT>(vkGetDeviceProcAddr(device, "vkTransitionImageLayoutEXT"));
            #endif
            #if defined(VK_EXT_host_query_reset)
                functionTable.vkResetQueryPoolEXT = reinterpret_cast<PFN_vkResetQueryPoolEXT>(vkGetDeviceProcAddr(device, "vkResetQueryPoolEXT"));
            #endif
            #if defined(VK_EXT_image_drm_format_modifier)
                functionTable.vkGetImageDrmFormatModifierPropertiesEXT = reinterpret_cast<PFN_vkGetImageDrmFormatModifierPropertiesEXT>(vkGetDeviceProcAddr(device, "vkGetImageDrmFormatModifierPropertiesEXT"));
            #endif
            #if defined(VK_EXT_line_rasterization)
                functionTable.vkCmdSetLineStippleEXT = reinterpret_cast<PFN_vkCmdSetLineStippleEXT>(vkGetDeviceProcAddr(device, "vkCmdSetLineStippleEXT"));
            #endif
            #if defined(VK_EXT_mesh_shader)
                functionTable.vkCmdDrawMeshTasksEXT              = reinterpret_cast<PFN_vkCmdDrawMeshTasksEXT>(vkGetDeviceProcAddr(device, "vkCmdDrawMeshTasksEXT"));
                functionTable.vkCmdDrawMeshTasksIndirectCountEXT = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectCountEXT>(vkGetDeviceProcAddr(device, "vkCmdDrawMeshTasksIndirectCountEXT"));
                functionTable.vkCmdDrawMeshTasksIndirectEXT      = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectEXT>(vkGetDeviceProcAddr(device, "vkCmdDrawMeshTasksIndirectEXT"));
            #endif
            #if defined(VK_EXT_vulkan_objects)
                functionTable.vkExportVulkanObjectsEXT = reinterpret_cast<PFN_vkExportVulkanObjectsEXT>(vkGetDeviceProcAddr(device, "vkExportVulkanObjectsEXT"));
            #endif
            #if defined(VK_EXT_multi_draw)
                functionTable.vkCmdDrawMultiEXT        = reinterpret_cast<PFN_vkCmdDrawMultiEXT>(vkGetDeviceProcAddr(device, "vkCmdDrawMultiEXT"));
                functionTable.vkCmdDrawMultiIndexedEXT = reinterpret_cast<PFN_vkCmdDrawMultiIndexedEXT>(vkGetDeviceProcAddr(device, "vkCmdDrawMultiIndexedEXT"));
            #endif
            #if defined(VK_EXT_opacity_micromap)
                functionTable.vkBuildMicromapsEXT                 = reinterpret_cast<PFN_vkBuildMicromapsEXT>(vkGetDeviceProcAddr(device, "vkBuildMicromapsEXT"));
                functionTable.vkCmdBuildMicromapsEXT              = reinterpret_cast<PFN_vkCmdBuildMicromapsEXT>(vkGetDeviceProcAddr(device, "vkCmdBuildMicromapsEXT"));
                functionTable.vkCmdCopyMemoryToMicromapEXT        = reinterpret_cast<PFN_vkCmdCopyMemoryToMicromapEXT>(vkGetDeviceProcAddr(device, "vkCmdCopyMemoryToMicromapEXT"));
                functionTable.vkCmdCopyMicromapEXT                = reinterpret_cast<PFN_vkCmdCopyMicromapEXT>(vkGetDeviceProcAddr(device, "vkCmdCopyMicromapEXT"));
                functionTable.vkCmdCopyMicromapToMemoryEXT        = reinterpret_cast<PFN_vkCmdCopyMicromapToMemoryEXT>(vkGetDeviceProcAddr(device, "vkCmdCopyMicromapToMemoryEXT"));
                functionTable.vkCmdWriteMicromapsPropertiesEXT    = reinterpret_cast<PFN_vkCmdWriteMicromapsPropertiesEXT>(vkGetDeviceProcAddr(device, "vkCmdWriteMicromapsPropertiesEXT"));
                functionTable.vkCopyMemoryToMicromapEXT           = reinterpret_cast<PFN_vkCopyMemoryToMicromapEXT>(vkGetDeviceProcAddr(device, "vkCopyMemoryToMicromapEXT"));
                functionTable.vkCopyMicromapEXT                   = reinterpret_cast<PFN_vkCopyMicromapEXT>(vkGetDeviceProcAddr(device, "vkCopyMicromapEXT"));
                functionTable.vkCopyMicromapToMemoryEXT           = reinterpret_cast<PFN_vkCopyMicromapToMemoryEXT>(vkGetDeviceProcAddr(device, "vkCopyMicromapToMemoryEXT"));
                functionTable.vkCreateMicromapEXT                 = reinterpret_cast<PFN_vkCreateMicromapEXT>(vkGetDeviceProcAddr(device, "vkCreateMicromapEXT"));
                functionTable.vkDestroyMicromapEXT                = reinterpret_cast<PFN_vkDestroyMicromapEXT>(vkGetDeviceProcAddr(device, "vkDestroyMicromapEXT"));
                functionTable.vkGetDeviceMicromapCompatibilityEXT = reinterpret_cast<PFN_vkGetDeviceMicromapCompatibilityEXT>(vkGetDeviceProcAddr(device, "vkGetDeviceMicromapCompatibilityEXT"));
                functionTable.vkGetMicromapBuildSizesEXT          = reinterpret_cast<PFN_vkGetMicromapBuildSizesEXT>(vkGetDeviceProcAddr(device, "vkGetMicromapBuildSizesEXT"));
                functionTable.vkWriteMicromapsPropertiesEXT       = reinterpret_cast<PFN_vkWriteMicromapsPropertiesEXT>(vkGetDeviceProcAddr(device, "vkWriteMicromapsPropertiesEXT"));
            #endif
            #if defined(VK_EXT_pageable_device_local_memory)
                functionTable.vkSetDeviceMemoryPriorityEXT = reinterpret_cast<PFN_vkSetDeviceMemoryPriorityEXT>(vkGetDeviceProcAddr(device, "vkSetDeviceMemoryPriorityEXT"));
            #endif
            #if defined(VK_EXT_pipeline_properties)
                functionTable.vkGetPipelinePropertiesEXT = reinterpret_cast<PFN_vkGetPipelinePropertiesEXT>(vkGetDeviceProcAddr(device, "vkGetPipelinePropertiesEXT"));
            #endif
            #if defined(VK_EXT_private_data)
                functionTable.vkCreatePrivateDataSlotEXT  = reinterpret_cast<PFN_vkCreatePrivateDataSlotEXT>(vkGetDeviceProcAddr(device, "vkCreatePrivateDataSlotEXT"));
                functionTable.vkDestroyPrivateDataSlotEXT = reinterpret_cast<PFN_vkDestroyPrivateDataSlotEXT>(vkGetDeviceProcAddr(device, "vkDestroyPrivateDataSlotEXT"));
                functionTable.vkGetPrivateDataEXT         = reinterpret_cast<PFN_vkGetPrivateDataEXT>(vkGetDeviceProcAddr(device, "vkGetPrivateDataEXT"));
                functionTable.vkSetPrivateDataEXT         = reinterpret_cast<PFN_vkSetPrivateDataEXT>(vkGetDeviceProcAddr(device, "vkSetPrivateDataEXT"));
            #endif
            #if defined(VK_EXT_sample_locations)
                functionTable.vkCmdSetSampleLocationsEXT = reinterpret_cast<PFN_vkCmdSetSampleLocationsEXT>(vkGetDeviceProcAddr(device, "vkCmdSetSampleLocationsEXT"));
            #endif
            #if defined(VK_EXT_shader_module_identifier)
                functionTable.vkGetShaderModuleCreateInfoIdentifierEXT = reinterpret_cast<PFN_vkGetShaderModuleCreateInfoIdentifierEXT>(vkGetDeviceProcAddr(device, "vkGetShaderModuleCreateInfoIdentifierEXT"));
                functionTable.vkGetShaderModuleIdentifierEXT           = reinterpret_cast<PFN_vkGetShaderModuleIdentifierEXT>(vkGetDeviceProcAddr(device, "vkGetShaderModuleIdentifierEXT"));
            #endif
            #if defined(VK_EXT_shader_object)
                functionTable.vkCmdBindShadersEXT      = reinterpret_cast<PFN_vkCmdBindShadersEXT>(vkGetDeviceProcAddr(device, "vkCmdBindShadersEXT"));
                functionTable.vkCreateShadersEXT       = reinterpret_cast<PFN_vkCreateShadersEXT>(vkGetDeviceProcAddr(device, "vkCreateShadersEXT"));
                functionTable.vkDestroyShaderEXT       = reinterpret_cast<PFN_vkDestroyShaderEXT>(vkGetDeviceProcAddr(device, "vkDestroyShaderEXT"));
                functionTable.vkGetShaderBinaryDataEXT = reinterpret_cast<PFN_vkGetShaderBinaryDataEXT>(vkGetDeviceProcAddr(device, "vkGetShaderBinaryDataEXT"));
            #endif
            #if defined(VK_EXT_swapchain_maintenance1)
                functionTable.vkReleaseSwapchainImagesEXT = reinterpret_cast<PFN_vkReleaseSwapchainImagesEXT>(vkGetDeviceProcAddr(device, "vkReleaseSwapchainImagesEXT"));
            #endif
            #if defined(VK_EXT_transform_feedback)
                functionTable.vkCmdBeginQueryIndexedEXT            = reinterpret_cast<PFN_vkCmdBeginQueryIndexedEXT>(vkGetDeviceProcAddr(device, "vkCmdBeginQueryIndexedEXT"));
                functionTable.vkCmdBeginTransformFeedbackEXT       = reinterpret_cast<PFN_vkCmdBeginTransformFeedbackEXT>(vkGetDeviceProcAddr(device, "vkCmdBeginTransformFeedbackEXT"));
                functionTable.vkCmdBindTransformFeedbackBuffersEXT = reinterpret_cast<PFN_vkCmdBindTransformFeedbackBuffersEXT>(vkGetDeviceProcAddr(device, "vkCmdBindTransformFeedbackBuffersEXT"));
                functionTable.vkCmdDrawIndirectByteCountEXT        = reinterpret_cast<PFN_vkCmdDrawIndirectByteCountEXT>(vkGetDeviceProcAddr(device, "vkCmdDrawIndirectByteCountEXT"));
                functionTable.vkCmdEndQueryIndexedEXT              = reinterpret_cast<PFN_vkCmdEndQueryIndexedEXT>(vkGetDeviceProcAddr(device, "vkCmdEndQueryIndexedEXT"));
                functionTable.vkCmdEndTransformFeedbackEXT         = reinterpret_cast<PFN_vkCmdEndTransformFeedbackEXT>(vkGetDeviceProcAddr(device, "vkCmdEndTransformFeedbackEXT"));
            #endif
            #if defined(VK_EXT_validation_cache)
                functionTable.vkCreateValidationCacheEXT  = reinterpret_cast<PFN_vkCreateValidationCacheEXT>(vkGetDeviceProcAddr(device, "vkCreateValidationCacheEXT"));
                functionTable.vkDestroyValidationCacheEXT = reinterpret_cast<PFN_vkDestroyValidationCacheEXT>(vkGetDeviceProcAddr(device, "vkDestroyValidationCacheEXT"));
                functionTable.vkGetValidationCacheDataEXT = reinterpret_cast<PFN_vkGetValidationCacheDataEXT>(vkGetDeviceProcAddr(device, "vkGetValidationCacheDataEXT"));
                functionTable.vkMergeValidationCachesEXT  = reinterpret_cast<PFN_vkMergeValidationCachesEXT>(vkGetDeviceProcAddr(device, "vkMergeValidationCachesEXT"));
            #endif
            #if defined(VK_FUCHSIA_buffer_collection)
                functionTable.vkCreateBufferCollectionFUCHSIA               = reinterpret_cast<PFN_vkCreateBufferCollectionFUCHSIA>(vkGetDeviceProcAddr(device, "vkCreateBufferCollectionFUCHSIA"));
                functionTable.vkDestroyBufferCollectionFUCHSIA              = reinterpret_cast<PFN_vkDestroyBufferCollectionFUCHSIA>(vkGetDeviceProcAddr(device, "vkDestroyBufferCollectionFUCHSIA"));
                functionTable.vkGetBufferCollectionPropertiesFUCHSIA        = reinterpret_cast<PFN_vkGetBufferCollectionPropertiesFUCHSIA>(vkGetDeviceProcAddr(device, "vkGetBufferCollectionPropertiesFUCHSIA"));
                functionTable.vkSetBufferCollectionBufferConstraintsFUCHSIA = reinterpret_cast<PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA>(vkGetDeviceProcAddr(device, "vkSetBufferCollectionBufferConstraintsFUCHSIA"));
                functionTable.vkSetBufferCollectionImageConstraintsFUCHSIA  = reinterpret_cast<PFN_vkSetBufferCollectionImageConstraintsFUCHSIA>(vkGetDeviceProcAddr(device, "vkSetBufferCollectionImageConstraintsFUCHSIA"));
            #endif
            #if defined(VK_FUCHSIA_external_memory)
                functionTable.vkGetMemoryZirconHandleFUCHSIA           = reinterpret_cast<PFN_vkGetMemoryZirconHandleFUCHSIA>(vkGetDeviceProcAddr(device, "vkGetMemoryZirconHandleFUCHSIA"));
                functionTable.vkGetMemoryZirconHandlePropertiesFUCHSIA = reinterpret_cast<PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA>(vkGetDeviceProcAddr(device, "vkGetMemoryZirconHandlePropertiesFUCHSIA"));
            #endif
            #if defined(VK_FUCHSIA_external_semaphore)
                functionTable.vkGetSemaphoreZirconHandleFUCHSIA    = reinterpret_cast<PFN_vkGetSemaphoreZirconHandleFUCHSIA>(vkGetDeviceProcAddr(device, "vkGetSemaphoreZirconHandleFUCHSIA"));
                functionTable.vkImportSemaphoreZirconHandleFUCHSIA = reinterpret_cast<PFN_vkImportSemaphoreZirconHandleFUCHSIA>(vkGetDeviceProcAddr(device, "vkImportSemaphoreZirconHandleFUCHSIA"));
            #endif
            #if defined(VK_GOOGLE_display_timing)
                functionTable.vkGetPastPresentationTimingGOOGLE = reinterpret_cast<PFN_vkGetPastPresentationTimingGOOGLE>(vkGetDeviceProcAddr(device, "vkGetPastPresentationTimingGOOGLE"));
                functionTable.vkGetRefreshCycleDurationGOOGLE   = reinterpret_cast<PFN_vkGetRefreshCycleDurationGOOGLE>(vkGetDeviceProcAddr(device, "vkGetRefreshCycleDurationGOOGLE"));
            #endif
            #if defined(VK_HUAWEI_cluster_culling_shader)
                functionTable.vkCmdDrawClusterHUAWEI         = reinterpret_cast<PFN_vkCmdDrawClusterHUAWEI>(vkGetDeviceProcAddr(device, "vkCmdDrawClusterHUAWEI"));
                functionTable.vkCmdDrawClusterIndirectHUAWEI = reinterpret_cast<PFN_vkCmdDrawClusterIndirectHUAWEI>(vkGetDeviceProcAddr(device, "vkCmdDrawClusterIndirectHUAWEI"));
            #endif
            #if defined(VK_HUAWEI_invocation_mask)
                functionTable.vkCmdBindInvocationMaskHUAWEI = reinterpret_cast<PFN_vkCmdBindInvocationMaskHUAWEI>(vkGetDeviceProcAddr(device, "vkCmdBindInvocationMaskHUAWEI"));
            #endif
            #if defined(VK_HUAWEI_subpass_shading)
                functionTable.vkCmdSubpassShadingHUAWEI                       = reinterpret_cast<PFN_vkCmdSubpassShadingHUAWEI>(vkGetDeviceProcAddr(device, "vkCmdSubpassShadingHUAWEI"));
                functionTable.vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI = reinterpret_cast<PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI>(vkGetDeviceProcAddr(device, "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI"));
            #endif
            #if defined(VK_INTEL_performance_query)
                functionTable.vkAcquirePerformanceConfigurationINTEL  = reinterpret_cast<PFN_vkAcquirePerformanceConfigurationINTEL>(vkGetDeviceProcAddr(device, "vkAcquirePerformanceConfigurationINTEL"));
                functionTable.vkCmdSetPerformanceMarkerINTEL          = reinterpret_cast<PFN_vkCmdSetPerformanceMarkerINTEL>(vkGetDeviceProcAddr(device, "vkCmdSetPerformanceMarkerINTEL"));
                functionTable.vkCmdSetPerformanceOverrideINTEL        = reinterpret_cast<PFN_vkCmdSetPerformanceOverrideINTEL>(vkGetDeviceProcAddr(device, "vkCmdSetPerformanceOverrideINTEL"));
                functionTable.vkCmdSetPerformanceStreamMarkerINTEL    = reinterpret_cast<PFN_vkCmdSetPerformanceStreamMarkerINTEL>(vkGetDeviceProcAddr(device, "vkCmdSetPerformanceStreamMarkerINTEL"));
                functionTable.vkGetPerformanceParameterINTEL          = reinterpret_cast<PFN_vkGetPerformanceParameterINTEL>(vkGetDeviceProcAddr(device, "vkGetPerformanceParameterINTEL"));
                functionTable.vkInitializePerformanceApiINTEL         = reinterpret_cast<PFN_vkInitializePerformanceApiINTEL>(vkGetDeviceProcAddr(device, "vkInitializePerformanceApiINTEL"));
                functionTable.vkQueueSetPerformanceConfigurationINTEL = reinterpret_cast<PFN_vkQueueSetPerformanceConfigurationINTEL>(vkGetDeviceProcAddr(device, "vkQueueSetPerformanceConfigurationINTEL"));
                functionTable.vkReleasePerformanceConfigurationINTEL  = reinterpret_cast<PFN_vkReleasePerformanceConfigurationINTEL>(vkGetDeviceProcAddr(device, "vkReleasePerformanceConfigurationINTEL"));
                functionTable.vkUninitializePerformanceApiINTEL       = reinterpret_cast<PFN_vkUninitializePerformanceApiINTEL>(vkGetDeviceProcAddr(device, "vkUninitializePerformanceApiINTEL"));
            #endif
            #if defined(VK_KHR_acceleration_structure)
                functionTable.vkBuildAccelerationStructuresKHR                 = reinterpret_cast<PFN_vkBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(device, "vkBuildAccelerationStructuresKHR"));
                functionTable.vkCmdBuildAccelerationStructuresIndirectKHR      = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresIndirectKHR>(vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresIndirectKHR"));
                functionTable.vkCmdBuildAccelerationStructuresKHR              = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresKHR"));
                functionTable.vkCmdCopyAccelerationStructureKHR                = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCmdCopyAccelerationStructureKHR"));
                functionTable.vkCmdCopyAccelerationStructureToMemoryKHR        = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureToMemoryKHR>(vkGetDeviceProcAddr(device, "vkCmdCopyAccelerationStructureToMemoryKHR"));
                functionTable.vkCmdCopyMemoryToAccelerationStructureKHR        = reinterpret_cast<PFN_vkCmdCopyMemoryToAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCmdCopyMemoryToAccelerationStructureKHR"));
                functionTable.vkCmdWriteAccelerationStructuresPropertiesKHR    = reinterpret_cast<PFN_vkCmdWriteAccelerationStructuresPropertiesKHR>(vkGetDeviceProcAddr(device, "vkCmdWriteAccelerationStructuresPropertiesKHR"));
                functionTable.vkCopyAccelerationStructureKHR                   = reinterpret_cast<PFN_vkCopyAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCopyAccelerationStructureKHR"));
                functionTable.vkCopyAccelerationStructureToMemoryKHR           = reinterpret_cast<PFN_vkCopyAccelerationStructureToMemoryKHR>(vkGetDeviceProcAddr(device, "vkCopyAccelerationStructureToMemoryKHR"));
                functionTable.vkCopyMemoryToAccelerationStructureKHR           = reinterpret_cast<PFN_vkCopyMemoryToAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCopyMemoryToAccelerationStructureKHR"));
                functionTable.vkCreateAccelerationStructureKHR                 = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureKHR"));
                functionTable.vkDestroyAccelerationStructureKHR                = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureKHR"));
                functionTable.vkGetAccelerationStructureBuildSizesKHR          = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureBuildSizesKHR"));
                functionTable.vkGetAccelerationStructureDeviceAddressKHR       = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureDeviceAddressKHR"));
                functionTable.vkGetDeviceAccelerationStructureCompatibilityKHR = reinterpret_cast<PFN_vkGetDeviceAccelerationStructureCompatibilityKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceAccelerationStructureCompatibilityKHR"));
                functionTable.vkWriteAccelerationStructuresPropertiesKHR       = reinterpret_cast<PFN_vkWriteAccelerationStructuresPropertiesKHR>(vkGetDeviceProcAddr(device, "vkWriteAccelerationStructuresPropertiesKHR"));
            #endif
            #if defined(VK_KHR_bind_memory2)
                functionTable.vkBindBufferMemory2KHR = reinterpret_cast<PFN_vkBindBufferMemory2KHR>(vkGetDeviceProcAddr(device, "vkBindBufferMemory2KHR"));
                functionTable.vkBindImageMemory2KHR  = reinterpret_cast<PFN_vkBindImageMemory2KHR>(vkGetDeviceProcAddr(device, "vkBindImageMemory2KHR"));
            #endif
            #if defined(VK_KHR_buffer_device_address)
                functionTable.vkGetBufferDeviceAddressKHR              = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(device, "vkGetBufferDeviceAddressKHR"));
                functionTable.vkGetBufferOpaqueCaptureAddressKHR       = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureAddressKHR>(vkGetDeviceProcAddr(device, "vkGetBufferOpaqueCaptureAddressKHR"));
                functionTable.vkGetDeviceMemoryOpaqueCaptureAddressKHR = reinterpret_cast<PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceMemoryOpaqueCaptureAddressKHR"));
            #endif
            #if defined(VK_KHR_copy_commands2)
                functionTable.vkCmdBlitImage2KHR         = reinterpret_cast<PFN_vkCmdBlitImage2KHR>(vkGetDeviceProcAddr(device, "vkCmdBlitImage2KHR"));
                functionTable.vkCmdCopyBuffer2KHR        = reinterpret_cast<PFN_vkCmdCopyBuffer2KHR>(vkGetDeviceProcAddr(device, "vkCmdCopyBuffer2KHR"));
                functionTable.vkCmdCopyBufferToImage2KHR = reinterpret_cast<PFN_vkCmdCopyBufferToImage2KHR>(vkGetDeviceProcAddr(device, "vkCmdCopyBufferToImage2KHR"));
                functionTable.vkCmdCopyImage2KHR         = reinterpret_cast<PFN_vkCmdCopyImage2KHR>(vkGetDeviceProcAddr(device, "vkCmdCopyImage2KHR"));
                functionTable.vkCmdCopyImageToBuffer2KHR = reinterpret_cast<PFN_vkCmdCopyImageToBuffer2KHR>(vkGetDeviceProcAddr(device, "vkCmdCopyImageToBuffer2KHR"));
                functionTable.vkCmdResolveImage2KHR      = reinterpret_cast<PFN_vkCmdResolveImage2KHR>(vkGetDeviceProcAddr(device, "vkCmdResolveImage2KHR"));
            #endif
            #if defined(VK_KHR_create_renderpass2)
                functionTable.vkCmdBeginRenderPass2KHR = reinterpret_cast<PFN_vkCmdBeginRenderPass2KHR>(vkGetDeviceProcAddr(device, "vkCmdBeginRenderPass2KHR"));
                functionTable.vkCmdEndRenderPass2KHR   = reinterpret_cast<PFN_vkCmdEndRenderPass2KHR>(vkGetDeviceProcAddr(device, "vkCmdEndRenderPass2KHR"));
                functionTable.vkCmdNextSubpass2KHR     = reinterpret_cast<PFN_vkCmdNextSubpass2KHR>(vkGetDeviceProcAddr(device, "vkCmdNextSubpass2KHR"));
                functionTable.vkCreateRenderPass2KHR   = reinterpret_cast<PFN_vkCreateRenderPass2KHR>(vkGetDeviceProcAddr(device, "vkCreateRenderPass2KHR"));
            #endif
            #if defined(VK_KHR_deferred_host_operations)
                functionTable.vkCreateDeferredOperationKHR            = reinterpret_cast<PFN_vkCreateDeferredOperationKHR>(vkGetDeviceProcAddr(device, "vkCreateDeferredOperationKHR"));
                functionTable.vkDeferredOperationJoinKHR              = reinterpret_cast<PFN_vkDeferredOperationJoinKHR>(vkGetDeviceProcAddr(device, "vkDeferredOperationJoinKHR"));
                functionTable.vkDestroyDeferredOperationKHR           = reinterpret_cast<PFN_vkDestroyDeferredOperationKHR>(vkGetDeviceProcAddr(device, "vkDestroyDeferredOperationKHR"));
                functionTable.vkGetDeferredOperationMaxConcurrencyKHR = reinterpret_cast<PFN_vkGetDeferredOperationMaxConcurrencyKHR>(vkGetDeviceProcAddr(device, "vkGetDeferredOperationMaxConcurrencyKHR"));
                functionTable.vkGetDeferredOperationResultKHR         = reinterpret_cast<PFN_vkGetDeferredOperationResultKHR>(vkGetDeviceProcAddr(device, "vkGetDeferredOperationResultKHR"));
            #endif
            #if defined(VK_KHR_descriptor_update_template)
                functionTable.vkCreateDescriptorUpdateTemplateKHR  = reinterpret_cast<PFN_vkCreateDescriptorUpdateTemplateKHR>(vkGetDeviceProcAddr(device, "vkCreateDescriptorUpdateTemplateKHR"));
                functionTable.vkDestroyDescriptorUpdateTemplateKHR = reinterpret_cast<PFN_vkDestroyDescriptorUpdateTemplateKHR>(vkGetDeviceProcAddr(device, "vkDestroyDescriptorUpdateTemplateKHR"));
                functionTable.vkUpdateDescriptorSetWithTemplateKHR = reinterpret_cast<PFN_vkUpdateDescriptorSetWithTemplateKHR>(vkGetDeviceProcAddr(device, "vkUpdateDescriptorSetWithTemplateKHR"));
            #endif
            #if defined(VK_KHR_device_group)
                functionTable.vkCmdDispatchBaseKHR                  = reinterpret_cast<PFN_vkCmdDispatchBaseKHR>(vkGetDeviceProcAddr(device, "vkCmdDispatchBaseKHR"));
                functionTable.vkCmdSetDeviceMaskKHR                 = reinterpret_cast<PFN_vkCmdSetDeviceMaskKHR>(vkGetDeviceProcAddr(device, "vkCmdSetDeviceMaskKHR"));
                functionTable.vkGetDeviceGroupPeerMemoryFeaturesKHR = reinterpret_cast<PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceGroupPeerMemoryFeaturesKHR"));
            #endif
            #if defined(VK_KHR_display_swapchain)
                functionTable.vkCreateSharedSwapchainsKHR = reinterpret_cast<PFN_vkCreateSharedSwapchainsKHR>(vkGetDeviceProcAddr(device, "vkCreateSharedSwapchainsKHR"));
            #endif
            #if defined(VK_KHR_draw_indirect_count)
                functionTable.vkCmdDrawIndexedIndirectCountKHR = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCountKHR>(vkGetDeviceProcAddr(device, "vkCmdDrawIndexedIndirectCountKHR"));
                functionTable.vkCmdDrawIndirectCountKHR        = reinterpret_cast<PFN_vkCmdDrawIndirectCountKHR>(vkGetDeviceProcAddr(device, "vkCmdDrawIndirectCountKHR"));
            #endif
            #if defined(VK_KHR_dynamic_rendering)
                functionTable.vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(vkGetDeviceProcAddr(device, "vkCmdBeginRenderingKHR"));
                functionTable.vkCmdEndRenderingKHR   = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(vkGetDeviceProcAddr(device, "vkCmdEndRenderingKHR"));
            #endif
            #if defined(VK_KHR_external_fence_fd)
                functionTable.vkGetFenceFdKHR    = reinterpret_cast<PFN_vkGetFenceFdKHR>(vkGetDeviceProcAddr(device, "vkGetFenceFdKHR"));
                functionTable.vkImportFenceFdKHR = reinterpret_cast<PFN_vkImportFenceFdKHR>(vkGetDeviceProcAddr(device, "vkImportFenceFdKHR"));
            #endif
            #if defined(VK_KHR_external_fence_win32)
                functionTable.vkGetFenceWin32HandleKHR    = reinterpret_cast<PFN_vkGetFenceWin32HandleKHR>(vkGetDeviceProcAddr(device, "vkGetFenceWin32HandleKHR"));
                functionTable.vkImportFenceWin32HandleKHR = reinterpret_cast<PFN_vkImportFenceWin32HandleKHR>(vkGetDeviceProcAddr(device, "vkImportFenceWin32HandleKHR"));
            #endif
            #if defined(VK_KHR_external_memory_fd)
                functionTable.vkGetMemoryFdKHR           = reinterpret_cast<PFN_vkGetMemoryFdKHR>(vkGetDeviceProcAddr(device, "vkGetMemoryFdKHR"));
                functionTable.vkGetMemoryFdPropertiesKHR = reinterpret_cast<PFN_vkGetMemoryFdPropertiesKHR>(vkGetDeviceProcAddr(device, "vkGetMemoryFdPropertiesKHR"));
            #endif
            #if defined(VK_KHR_external_memory_win32)
                functionTable.vkGetMemoryWin32HandleKHR           = reinterpret_cast<PFN_vkGetMemoryWin32HandleKHR>(vkGetDeviceProcAddr(device, "vkGetMemoryWin32HandleKHR"));
                functionTable.vkGetMemoryWin32HandlePropertiesKHR = reinterpret_cast<PFN_vkGetMemoryWin32HandlePropertiesKHR>(vkGetDeviceProcAddr(device, "vkGetMemoryWin32HandlePropertiesKHR"));
            #endif
            #if defined(VK_KHR_external_semaphore_fd)
                functionTable.vkGetSemaphoreFdKHR    = reinterpret_cast<PFN_vkGetSemaphoreFdKHR>(vkGetDeviceProcAddr(device, "vkGetSemaphoreFdKHR"));
                functionTable.vkImportSemaphoreFdKHR = reinterpret_cast<PFN_vkImportSemaphoreFdKHR>(vkGetDeviceProcAddr(device, "vkImportSemaphoreFdKHR"));
            #endif
            #if defined(VK_KHR_external_semaphore_win32)
                functionTable.vkGetSemaphoreWin32HandleKHR    = reinterpret_cast<PFN_vkGetSemaphoreWin32HandleKHR>(vkGetDeviceProcAddr(device, "vkGetSemaphoreWin32HandleKHR"));
                functionTable.vkImportSemaphoreWin32HandleKHR = reinterpret_cast<PFN_vkImportSemaphoreWin32HandleKHR>(vkGetDeviceProcAddr(device, "vkImportSemaphoreWin32HandleKHR"));
            #endif
            #if defined(VK_KHR_fragment_shading_rate)
                functionTable.vkCmdSetFragmentShadingRateKHR = reinterpret_cast<PFN_vkCmdSetFragmentShadingRateKHR>(vkGetDeviceProcAddr(device, "vkCmdSetFragmentShadingRateKHR"));
            #endif
            #if defined(VK_KHR_get_memory_requirements2)
                functionTable.vkGetBufferMemoryRequirements2KHR      = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2KHR>(vkGetDeviceProcAddr(device, "vkGetBufferMemoryRequirements2KHR"));
                functionTable.vkGetImageMemoryRequirements2KHR       = reinterpret_cast<PFN_vkGetImageMemoryRequirements2KHR>(vkGetDeviceProcAddr(device, "vkGetImageMemoryRequirements2KHR"));
                functionTable.vkGetImageSparseMemoryRequirements2KHR = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements2KHR>(vkGetDeviceProcAddr(device, "vkGetImageSparseMemoryRequirements2KHR"));
            #endif
            #if defined(VK_KHR_maintenance1)
                functionTable.vkTrimCommandPoolKHR = reinterpret_cast<PFN_vkTrimCommandPoolKHR>(vkGetDeviceProcAddr(device, "vkTrimCommandPoolKHR"));
            #endif
            #if defined(VK_KHR_maintenance3)
                functionTable.vkGetDescriptorSetLayoutSupportKHR = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSupportKHR>(vkGetDeviceProcAddr(device, "vkGetDescriptorSetLayoutSupportKHR"));
            #endif
            #if defined(VK_KHR_maintenance4)
                functionTable.vkGetDeviceBufferMemoryRequirementsKHR      = reinterpret_cast<PFN_vkGetDeviceBufferMemoryRequirementsKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceBufferMemoryRequirementsKHR"));
                functionTable.vkGetDeviceImageMemoryRequirementsKHR       = reinterpret_cast<PFN_vkGetDeviceImageMemoryRequirementsKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceImageMemoryRequirementsKHR"));
                functionTable.vkGetDeviceImageSparseMemoryRequirementsKHR = reinterpret_cast<PFN_vkGetDeviceImageSparseMemoryRequirementsKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceImageSparseMemoryRequirementsKHR"));
            #endif
            #if defined(VK_KHR_maintenance5)
                functionTable.vkCmdBindIndexBuffer2KHR             = reinterpret_cast<PFN_vkCmdBindIndexBuffer2KHR>(vkGetDeviceProcAddr(device, "vkCmdBindIndexBuffer2KHR"));
                functionTable.vkGetDeviceImageSubresourceLayoutKHR = reinterpret_cast<PFN_vkGetDeviceImageSubresourceLayoutKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceImageSubresourceLayoutKHR"));
                functionTable.vkGetImageSubresourceLayout2KHR      = reinterpret_cast<PFN_vkGetImageSubresourceLayout2KHR>(vkGetDeviceProcAddr(device, "vkGetImageSubresourceLayout2KHR"));
                functionTable.vkGetRenderingAreaGranularityKHR     = reinterpret_cast<PFN_vkGetRenderingAreaGranularityKHR>(vkGetDeviceProcAddr(device, "vkGetRenderingAreaGranularityKHR"));
            #endif
            #if defined(VK_KHR_map_memory2)
                functionTable.vkMapMemory2KHR   = reinterpret_cast<PFN_vkMapMemory2KHR>(vkGetDeviceProcAddr(device, "vkMapMemory2KHR"));
                functionTable.vkUnmapMemory2KHR = reinterpret_cast<PFN_vkUnmapMemory2KHR>(vkGetDeviceProcAddr(device, "vkUnmapMemory2KHR"));
            #endif
            #if defined(VK_KHR_performance_query)
                functionTable.vkAcquireProfilingLockKHR = reinterpret_cast<PFN_vkAcquireProfilingLockKHR>(vkGetDeviceProcAddr(device, "vkAcquireProfilingLockKHR"));
                functionTable.vkReleaseProfilingLockKHR = reinterpret_cast<PFN_vkReleaseProfilingLockKHR>(vkGetDeviceProcAddr(device, "vkReleaseProfilingLockKHR"));
            #endif
            #if defined(VK_KHR_pipeline_executable_properties)
                functionTable.vkGetPipelineExecutableInternalRepresentationsKHR = reinterpret_cast<PFN_vkGetPipelineExecutableInternalRepresentationsKHR>(vkGetDeviceProcAddr(device, "vkGetPipelineExecutableInternalRepresentationsKHR"));
                functionTable.vkGetPipelineExecutablePropertiesKHR              = reinterpret_cast<PFN_vkGetPipelineExecutablePropertiesKHR>(vkGetDeviceProcAddr(device, "vkGetPipelineExecutablePropertiesKHR"));
                functionTable.vkGetPipelineExecutableStatisticsKHR              = reinterpret_cast<PFN_vkGetPipelineExecutableStatisticsKHR>(vkGetDeviceProcAddr(device, "vkGetPipelineExecutableStatisticsKHR"));
            #endif
            #if defined(VK_KHR_present_wait)
                functionTable.vkWaitForPresentKHR = reinterpret_cast<PFN_vkWaitForPresentKHR>(vkGetDeviceProcAddr(device, "vkWaitForPresentKHR"));
            #endif
            #if defined(VK_KHR_push_descriptor)
                functionTable.vkCmdPushDescriptorSetKHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(vkGetDeviceProcAddr(device, "vkCmdPushDescriptorSetKHR"));
            #endif
            #if defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline)
                functionTable.vkCmdTraceRaysIndirect2KHR = reinterpret_cast<PFN_vkCmdTraceRaysIndirect2KHR>(vkGetDeviceProcAddr(device, "vkCmdTraceRaysIndirect2KHR"));
            #endif
            #if defined(VK_KHR_ray_tracing_pipeline)
                functionTable.vkCmdSetRayTracingPipelineStackSizeKHR            = reinterpret_cast<PFN_vkCmdSetRayTracingPipelineStackSizeKHR>(vkGetDeviceProcAddr(device, "vkCmdSetRayTracingPipelineStackSizeKHR"));
                functionTable.vkCmdTraceRaysIndirectKHR                         = reinterpret_cast<PFN_vkCmdTraceRaysIndirectKHR>(vkGetDeviceProcAddr(device, "vkCmdTraceRaysIndirectKHR"));
                functionTable.vkCmdTraceRaysKHR                                 = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(vkGetDeviceProcAddr(device, "vkCmdTraceRaysKHR"));
                functionTable.vkCreateRayTracingPipelinesKHR                    = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(vkGetDeviceProcAddr(device, "vkCreateRayTracingPipelinesKHR"));
                functionTable.vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR>(vkGetDeviceProcAddr(device, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR"));
                functionTable.vkGetRayTracingShaderGroupHandlesKHR              = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesKHR"));
                functionTable.vkGetRayTracingShaderGroupStackSizeKHR            = reinterpret_cast<PFN_vkGetRayTracingShaderGroupStackSizeKHR>(vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupStackSizeKHR"));
            #endif
            #if defined(VK_KHR_sampler_ycbcr_conversion)
                functionTable.vkCreateSamplerYcbcrConversionKHR  = reinterpret_cast<PFN_vkCreateSamplerYcbcrConversionKHR>(vkGetDeviceProcAddr(device, "vkCreateSamplerYcbcrConversionKHR"));
                functionTable.vkDestroySamplerYcbcrConversionKHR = reinterpret_cast<PFN_vkDestroySamplerYcbcrConversionKHR>(vkGetDeviceProcAddr(device, "vkDestroySamplerYcbcrConversionKHR"));
            #endif
            #if defined(VK_KHR_shared_presentable_image)
                functionTable.vkGetSwapchainStatusKHR = reinterpret_cast<PFN_vkGetSwapchainStatusKHR>(vkGetDeviceProcAddr(device, "vkGetSwapchainStatusKHR"));
            #endif
            #if defined(VK_KHR_swapchain)
                functionTable.vkAcquireNextImageKHR   = reinterpret_cast<PFN_vkAcquireNextImageKHR>(vkGetDeviceProcAddr(device, "vkAcquireNextImageKHR"));
                functionTable.vkCreateSwapchainKHR    = reinterpret_cast<PFN_vkCreateSwapchainKHR>(vkGetDeviceProcAddr(device, "vkCreateSwapchainKHR"));
                functionTable.vkDestroySwapchainKHR   = reinterpret_cast<PFN_vkDestroySwapchainKHR>(vkGetDeviceProcAddr(device, "vkDestroySwapchainKHR"));
                functionTable.vkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(vkGetDeviceProcAddr(device, "vkGetSwapchainImagesKHR"));
                functionTable.vkQueuePresentKHR       = reinterpret_cast<PFN_vkQueuePresentKHR>(vkGetDeviceProcAddr(device, "vkQueuePresentKHR"));
            #endif
            #if defined(VK_KHR_synchronization2)
                functionTable.vkCmdPipelineBarrier2KHR = reinterpret_cast<PFN_vkCmdPipelineBarrier2KHR>(vkGetDeviceProcAddr(device, "vkCmdPipelineBarrier2KHR"));
                functionTable.vkCmdResetEvent2KHR      = reinterpret_cast<PFN_vkCmdResetEvent2KHR>(vkGetDeviceProcAddr(device, "vkCmdResetEvent2KHR"));
                functionTable.vkCmdSetEvent2KHR        = reinterpret_cast<PFN_vkCmdSetEvent2KHR>(vkGetDeviceProcAddr(device, "vkCmdSetEvent2KHR"));
                functionTable.vkCmdWaitEvents2KHR      = reinterpret_cast<PFN_vkCmdWaitEvents2KHR>(vkGetDeviceProcAddr(device, "vkCmdWaitEvents2KHR"));
                functionTable.vkCmdWriteTimestamp2KHR  = reinterpret_cast<PFN_vkCmdWriteTimestamp2KHR>(vkGetDeviceProcAddr(device, "vkCmdWriteTimestamp2KHR"));
                functionTable.vkQueueSubmit2KHR        = reinterpret_cast<PFN_vkQueueSubmit2KHR>(vkGetDeviceProcAddr(device, "vkQueueSubmit2KHR"));
            #endif
            #if defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker)
                functionTable.vkCmdWriteBufferMarker2AMD = reinterpret_cast<PFN_vkCmdWriteBufferMarker2AMD>(vkGetDeviceProcAddr(device, "vkCmdWriteBufferMarker2AMD"));
            #endif
            #if defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints)
                functionTable.vkGetQueueCheckpointData2NV = reinterpret_cast<PFN_vkGetQueueCheckpointData2NV>(vkGetDeviceProcAddr(device, "vkGetQueueCheckpointData2NV"));
            #endif
            #if defined(VK_KHR_timeline_semaphore)
                functionTable.vkGetSemaphoreCounterValueKHR = reinterpret_cast<PFN_vkGetSemaphoreCounterValueKHR>(vkGetDeviceProcAddr(device, "vkGetSemaphoreCounterValueKHR"));
                functionTable.vkSignalSemaphoreKHR          = reinterpret_cast<PFN_vkSignalSemaphoreKHR>(vkGetDeviceProcAddr(device, "vkSignalSemaphoreKHR"));
                functionTable.vkWaitSemaphoresKHR           = reinterpret_cast<PFN_vkWaitSemaphoresKHR>(vkGetDeviceProcAddr(device, "vkWaitSemaphoresKHR"));
            #endif
            #if defined(VK_KHR_video_decode_queue)
                functionTable.vkCmdDecodeVideoKHR = reinterpret_cast<PFN_vkCmdDecodeVideoKHR>(vkGetDeviceProcAddr(device, "vkCmdDecodeVideoKHR"));
            #endif
            #if defined(VK_KHR_video_queue)
                functionTable.vkBindVideoSessionMemoryKHR            = reinterpret_cast<PFN_vkBindVideoSessionMemoryKHR>(vkGetDeviceProcAddr(device, "vkBindVideoSessionMemoryKHR"));
                functionTable.vkCmdBeginVideoCodingKHR               = reinterpret_cast<PFN_vkCmdBeginVideoCodingKHR>(vkGetDeviceProcAddr(device, "vkCmdBeginVideoCodingKHR"));
                functionTable.vkCmdControlVideoCodingKHR             = reinterpret_cast<PFN_vkCmdControlVideoCodingKHR>(vkGetDeviceProcAddr(device, "vkCmdControlVideoCodingKHR"));
                functionTable.vkCmdEndVideoCodingKHR                 = reinterpret_cast<PFN_vkCmdEndVideoCodingKHR>(vkGetDeviceProcAddr(device, "vkCmdEndVideoCodingKHR"));
                functionTable.vkCreateVideoSessionKHR                = reinterpret_cast<PFN_vkCreateVideoSessionKHR>(vkGetDeviceProcAddr(device, "vkCreateVideoSessionKHR"));
                functionTable.vkCreateVideoSessionParametersKHR      = reinterpret_cast<PFN_vkCreateVideoSessionParametersKHR>(vkGetDeviceProcAddr(device, "vkCreateVideoSessionParametersKHR"));
                functionTable.vkDestroyVideoSessionKHR               = reinterpret_cast<PFN_vkDestroyVideoSessionKHR>(vkGetDeviceProcAddr(device, "vkDestroyVideoSessionKHR"));
                functionTable.vkDestroyVideoSessionParametersKHR     = reinterpret_cast<PFN_vkDestroyVideoSessionParametersKHR>(vkGetDeviceProcAddr(device, "vkDestroyVideoSessionParametersKHR"));
                functionTable.vkGetVideoSessionMemoryRequirementsKHR = reinterpret_cast<PFN_vkGetVideoSessionMemoryRequirementsKHR>(vkGetDeviceProcAddr(device, "vkGetVideoSessionMemoryRequirementsKHR"));
                functionTable.vkUpdateVideoSessionParametersKHR      = reinterpret_cast<PFN_vkUpdateVideoSessionParametersKHR>(vkGetDeviceProcAddr(device, "vkUpdateVideoSessionParametersKHR"));
            #endif
            #if defined(VK_NVX_binary_import)
                functionTable.vkCmdCuLaunchKernelNVX = reinterpret_cast<PFN_vkCmdCuLaunchKernelNVX>(vkGetDeviceProcAddr(device, "vkCmdCuLaunchKernelNVX"));
                functionTable.vkCreateCuFunctionNVX  = reinterpret_cast<PFN_vkCreateCuFunctionNVX>(vkGetDeviceProcAddr(device, "vkCreateCuFunctionNVX"));
                functionTable.vkCreateCuModuleNVX    = reinterpret_cast<PFN_vkCreateCuModuleNVX>(vkGetDeviceProcAddr(device, "vkCreateCuModuleNVX"));
                functionTable.vkDestroyCuFunctionNVX = reinterpret_cast<PFN_vkDestroyCuFunctionNVX>(vkGetDeviceProcAddr(device, "vkDestroyCuFunctionNVX"));
                functionTable.vkDestroyCuModuleNVX   = reinterpret_cast<PFN_vkDestroyCuModuleNVX>(vkGetDeviceProcAddr(device, "vkDestroyCuModuleNVX"));
            #endif
            #if defined(VK_NVX_image_view_handle)
                functionTable.vkGetImageViewAddressNVX = reinterpret_cast<PFN_vkGetImageViewAddressNVX>(vkGetDeviceProcAddr(device, "vkGetImageViewAddressNVX"));
                functionTable.vkGetImageViewHandleNVX  = reinterpret_cast<PFN_vkGetImageViewHandleNVX>(vkGetDeviceProcAddr(device, "vkGetImageViewHandleNVX"));
            #endif
            #if defined(VK_NV_clip_space_w_scaling)
                functionTable.vkCmdSetViewportWScalingNV = reinterpret_cast<PFN_vkCmdSetViewportWScalingNV>(vkGetDeviceProcAddr(device, "vkCmdSetViewportWScalingNV"));
            #endif
            #if defined(VK_NV_copy_memory_indirect)
                functionTable.vkCmdCopyMemoryIndirectNV        = reinterpret_cast<PFN_vkCmdCopyMemoryIndirectNV>(vkGetDeviceProcAddr(device, "vkCmdCopyMemoryIndirectNV"));
                functionTable.vkCmdCopyMemoryToImageIndirectNV = reinterpret_cast<PFN_vkCmdCopyMemoryToImageIndirectNV>(vkGetDeviceProcAddr(device, "vkCmdCopyMemoryToImageIndirectNV"));
            #endif
            #if defined(VK_NV_device_diagnostic_checkpoints)
                functionTable.vkCmdSetCheckpointNV       = reinterpret_cast<PFN_vkCmdSetCheckpointNV>(vkGetDeviceProcAddr(device, "vkCmdSetCheckpointNV"));
                functionTable.vkGetQueueCheckpointDataNV = reinterpret_cast<PFN_vkGetQueueCheckpointDataNV>(vkGetDeviceProcAddr(device, "vkGetQueueCheckpointDataNV"));
            #endif
            #if defined(VK_NV_device_generated_commands)
                functionTable.vkCmdBindPipelineShaderGroupNV             = reinterpret_cast<PFN_vkCmdBindPipelineShaderGroupNV>(vkGetDeviceProcAddr(device, "vkCmdBindPipelineShaderGroupNV"));
                functionTable.vkCmdExecuteGeneratedCommandsNV            = reinterpret_cast<PFN_vkCmdExecuteGeneratedCommandsNV>(vkGetDeviceProcAddr(device, "vkCmdExecuteGeneratedCommandsNV"));
                functionTable.vkCmdPreprocessGeneratedCommandsNV         = reinterpret_cast<PFN_vkCmdPreprocessGeneratedCommandsNV>(vkGetDeviceProcAddr(device, "vkCmdPreprocessGeneratedCommandsNV"));
                functionTable.vkCreateIndirectCommandsLayoutNV           = reinterpret_cast<PFN_vkCreateIndirectCommandsLayoutNV>(vkGetDeviceProcAddr(device, "vkCreateIndirectCommandsLayoutNV"));
                functionTable.vkDestroyIndirectCommandsLayoutNV          = reinterpret_cast<PFN_vkDestroyIndirectCommandsLayoutNV>(vkGetDeviceProcAddr(device, "vkDestroyIndirectCommandsLayoutNV"));
                functionTable.vkGetGeneratedCommandsMemoryRequirementsNV = reinterpret_cast<PFN_vkGetGeneratedCommandsMemoryRequirementsNV>(vkGetDeviceProcAddr(device, "vkGetGeneratedCommandsMemoryRequirementsNV"));
            #endif
            #if defined(VK_NV_device_generated_commands_compute)
                functionTable.vkCmdUpdatePipelineIndirectBufferNV       = reinterpret_cast<PFN_vkCmdUpdatePipelineIndirectBufferNV>(vkGetDeviceProcAddr(device, "vkCmdUpdatePipelineIndirectBufferNV"));
                functionTable.vkGetPipelineIndirectDeviceAddressNV      = reinterpret_cast<PFN_vkGetPipelineIndirectDeviceAddressNV>(vkGetDeviceProcAddr(device, "vkGetPipelineIndirectDeviceAddressNV"));
                functionTable.vkGetPipelineIndirectMemoryRequirementsNV = reinterpret_cast<PFN_vkGetPipelineIndirectMemoryRequirementsNV>(vkGetDeviceProcAddr(device, "vkGetPipelineIndirectMemoryRequirementsNV"));
            #endif
            #if defined(VK_NV_external_memory_rdma)
                functionTable.vkGetMemoryRemoteAddressNV = reinterpret_cast<PFN_vkGetMemoryRemoteAddressNV>(vkGetDeviceProcAddr(device, "vkGetMemoryRemoteAddressNV"));
            #endif
            #if defined(VK_NV_external_memory_win32)
                functionTable.vkGetMemoryWin32HandleNV = reinterpret_cast<PFN_vkGetMemoryWin32HandleNV>(vkGetDeviceProcAddr(device, "vkGetMemoryWin32HandleNV"));
            #endif
            #if defined(VK_NV_fragment_shading_rate_enums)
                functionTable.vkCmdSetFragmentShadingRateEnumNV = reinterpret_cast<PFN_vkCmdSetFragmentShadingRateEnumNV>(vkGetDeviceProcAddr(device, "vkCmdSetFragmentShadingRateEnumNV"));
            #endif
            #if defined(VK_NV_memory_decompression)
                functionTable.vkCmdDecompressMemoryIndirectCountNV = reinterpret_cast<PFN_vkCmdDecompressMemoryIndirectCountNV>(vkGetDeviceProcAddr(device, "vkCmdDecompressMemoryIndirectCountNV"));
                functionTable.vkCmdDecompressMemoryNV              = reinterpret_cast<PFN_vkCmdDecompressMemoryNV>(vkGetDeviceProcAddr(device, "vkCmdDecompressMemoryNV"));
            #endif
            #if defined(VK_NV_mesh_shader)
                functionTable.vkCmdDrawMeshTasksIndirectCountNV = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectCountNV>(vkGetDeviceProcAddr(device, "vkCmdDrawMeshTasksIndirectCountNV"));
                functionTable.vkCmdDrawMeshTasksIndirectNV      = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectNV>(vkGetDeviceProcAddr(device, "vkCmdDrawMeshTasksIndirectNV"));
                functionTable.vkCmdDrawMeshTasksNV              = reinterpret_cast<PFN_vkCmdDrawMeshTasksNV>(vkGetDeviceProcAddr(device, "vkCmdDrawMeshTasksNV"));
            #endif
            #if defined(VK_NV_optical_flow)
                functionTable.vkBindOpticalFlowSessionImageNV = reinterpret_cast<PFN_vkBindOpticalFlowSessionImageNV>(vkGetDeviceProcAddr(device, "vkBindOpticalFlowSessionImageNV"));
                functionTable.vkCmdOpticalFlowExecuteNV       = reinterpret_cast<PFN_vkCmdOpticalFlowExecuteNV>(vkGetDeviceProcAddr(device, "vkCmdOpticalFlowExecuteNV"));
                functionTable.vkCreateOpticalFlowSessionNV    = reinterpret_cast<PFN_vkCreateOpticalFlowSessionNV>(vkGetDeviceProcAddr(device, "vkCreateOpticalFlowSessionNV"));
                functionTable.vkDestroyOpticalFlowSessionNV   = reinterpret_cast<PFN_vkDestroyOpticalFlowSessionNV>(vkGetDeviceProcAddr(device, "vkDestroyOpticalFlowSessionNV"));
            #endif
            #if defined(VK_NV_ray_tracing)
                functionTable.vkBindAccelerationStructureMemoryNV            = reinterpret_cast<PFN_vkBindAccelerationStructureMemoryNV>(vkGetDeviceProcAddr(device, "vkBindAccelerationStructureMemoryNV"));
                functionTable.vkCmdBuildAccelerationStructureNV              = reinterpret_cast<PFN_vkCmdBuildAccelerationStructureNV>(vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructureNV"));
                functionTable.vkCmdCopyAccelerationStructureNV               = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureNV>(vkGetDeviceProcAddr(device, "vkCmdCopyAccelerationStructureNV"));
                functionTable.vkCmdTraceRaysNV                               = reinterpret_cast<PFN_vkCmdTraceRaysNV>(vkGetDeviceProcAddr(device, "vkCmdTraceRaysNV"));
                functionTable.vkCmdWriteAccelerationStructuresPropertiesNV   = reinterpret_cast<PFN_vkCmdWriteAccelerationStructuresPropertiesNV>(vkGetDeviceProcAddr(device, "vkCmdWriteAccelerationStructuresPropertiesNV"));
                functionTable.vkCompileDeferredNV                            = reinterpret_cast<PFN_vkCompileDeferredNV>(vkGetDeviceProcAddr(device, "vkCompileDeferredNV"));
                functionTable.vkCreateAccelerationStructureNV                = reinterpret_cast<PFN_vkCreateAccelerationStructureNV>(vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureNV"));
                functionTable.vkCreateRayTracingPipelinesNV                  = reinterpret_cast<PFN_vkCreateRayTracingPipelinesNV>(vkGetDeviceProcAddr(device, "vkCreateRayTracingPipelinesNV"));
                functionTable.vkDestroyAccelerationStructureNV               = reinterpret_cast<PFN_vkDestroyAccelerationStructureNV>(vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureNV"));
                functionTable.vkGetAccelerationStructureHandleNV             = reinterpret_cast<PFN_vkGetAccelerationStructureHandleNV>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureHandleNV"));
                functionTable.vkGetAccelerationStructureMemoryRequirementsNV = reinterpret_cast<PFN_vkGetAccelerationStructureMemoryRequirementsNV>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureMemoryRequirementsNV"));
                functionTable.vkGetRayTracingShaderGroupHandlesNV            = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesNV>(vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesNV"));
            #endif
            #if defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2
                functionTable.vkCmdSetExclusiveScissorEnableNV = reinterpret_cast<PFN_vkCmdSetExclusiveScissorEnableNV>(vkGetDeviceProcAddr(device, "vkCmdSetExclusiveScissorEnableNV"));
            #endif
            #if defined(VK_NV_scissor_exclusive)
                functionTable.vkCmdSetExclusiveScissorNV = reinterpret_cast<PFN_vkCmdSetExclusiveScissorNV>(vkGetDeviceProcAddr(device, "vkCmdSetExclusiveScissorNV"));
            #endif
            #if defined(VK_NV_shading_rate_image)
                functionTable.vkCmdBindShadingRateImageNV          = reinterpret_cast<PFN_vkCmdBindShadingRateImageNV>(vkGetDeviceProcAddr(device, "vkCmdBindShadingRateImageNV"));
                functionTable.vkCmdSetCoarseSampleOrderNV          = reinterpret_cast<PFN_vkCmdSetCoarseSampleOrderNV>(vkGetDeviceProcAddr(device, "vkCmdSetCoarseSampleOrderNV"));
                functionTable.vkCmdSetViewportShadingRatePaletteNV = reinterpret_cast<PFN_vkCmdSetViewportShadingRatePaletteNV>(vkGetDeviceProcAddr(device, "vkCmdSetViewportShadingRatePaletteNV"));
            #endif
            #if defined(VK_QCOM_tile_properties)
                functionTable.vkGetDynamicRenderingTilePropertiesQCOM = reinterpret_cast<PFN_vkGetDynamicRenderingTilePropertiesQCOM>(vkGetDeviceProcAddr(device, "vkGetDynamicRenderingTilePropertiesQCOM"));
                functionTable.vkGetFramebufferTilePropertiesQCOM      = reinterpret_cast<PFN_vkGetFramebufferTilePropertiesQCOM>(vkGetDeviceProcAddr(device, "vkGetFramebufferTilePropertiesQCOM"));
            #endif
            #if defined(VK_QNX_external_memory_screen_buffer)
                functionTable.vkGetScreenBufferPropertiesQNX = reinterpret_cast<PFN_vkGetScreenBufferPropertiesQNX>(vkGetDeviceProcAddr(device, "vkGetScreenBufferPropertiesQNX"));
            #endif
            #if defined(VK_VALVE_descriptor_set_host_mapping)
                functionTable.vkGetDescriptorSetHostMappingVALVE           = reinterpret_cast<PFN_vkGetDescriptorSetHostMappingVALVE>(vkGetDeviceProcAddr(device, "vkGetDescriptorSetHostMappingVALVE"));
                functionTable.vkGetDescriptorSetLayoutHostMappingInfoVALVE = reinterpret_cast<PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE>(vkGetDeviceProcAddr(device, "vkGetDescriptorSetLayoutHostMappingInfoVALVE"));
            #endif
            #if (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object))
                functionTable.vkCmdBindVertexBuffers2EXT       = reinterpret_cast<PFN_vkCmdBindVertexBuffers2EXT>(vkGetDeviceProcAddr(device, "vkCmdBindVertexBuffers2EXT"));
                functionTable.vkCmdSetCullModeEXT              = reinterpret_cast<PFN_vkCmdSetCullModeEXT>(vkGetDeviceProcAddr(device, "vkCmdSetCullModeEXT"));
                functionTable.vkCmdSetDepthBoundsTestEnableEXT = reinterpret_cast<PFN_vkCmdSetDepthBoundsTestEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthBoundsTestEnableEXT"));
                functionTable.vkCmdSetDepthCompareOpEXT        = reinterpret_cast<PFN_vkCmdSetDepthCompareOpEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthCompareOpEXT"));
                functionTable.vkCmdSetDepthTestEnableEXT       = reinterpret_cast<PFN_vkCmdSetDepthTestEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthTestEnableEXT"));
                functionTable.vkCmdSetDepthWriteEnableEXT      = reinterpret_cast<PFN_vkCmdSetDepthWriteEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthWriteEnableEXT"));
                functionTable.vkCmdSetFrontFaceEXT             = reinterpret_cast<PFN_vkCmdSetFrontFaceEXT>(vkGetDeviceProcAddr(device, "vkCmdSetFrontFaceEXT"));
                functionTable.vkCmdSetPrimitiveTopologyEXT     = reinterpret_cast<PFN_vkCmdSetPrimitiveTopologyEXT>(vkGetDeviceProcAddr(device, "vkCmdSetPrimitiveTopologyEXT"));
                functionTable.vkCmdSetScissorWithCountEXT      = reinterpret_cast<PFN_vkCmdSetScissorWithCountEXT>(vkGetDeviceProcAddr(device, "vkCmdSetScissorWithCountEXT"));
                functionTable.vkCmdSetStencilOpEXT             = reinterpret_cast<PFN_vkCmdSetStencilOpEXT>(vkGetDeviceProcAddr(device, "vkCmdSetStencilOpEXT"));
                functionTable.vkCmdSetStencilTestEnableEXT     = reinterpret_cast<PFN_vkCmdSetStencilTestEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetStencilTestEnableEXT"));
                functionTable.vkCmdSetViewportWithCountEXT     = reinterpret_cast<PFN_vkCmdSetViewportWithCountEXT>(vkGetDeviceProcAddr(device, "vkCmdSetViewportWithCountEXT"));
            #endif
            #if (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object))
                functionTable.vkCmdSetDepthBiasEnableEXT         = reinterpret_cast<PFN_vkCmdSetDepthBiasEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthBiasEnableEXT"));
                functionTable.vkCmdSetLogicOpEXT                 = reinterpret_cast<PFN_vkCmdSetLogicOpEXT>(vkGetDeviceProcAddr(device, "vkCmdSetLogicOpEXT"));
                functionTable.vkCmdSetPatchControlPointsEXT      = reinterpret_cast<PFN_vkCmdSetPatchControlPointsEXT>(vkGetDeviceProcAddr(device, "vkCmdSetPatchControlPointsEXT"));
                functionTable.vkCmdSetPrimitiveRestartEnableEXT  = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetPrimitiveRestartEnableEXT"));
                functionTable.vkCmdSetRasterizerDiscardEnableEXT = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetRasterizerDiscardEnableEXT"));
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object))
                functionTable.vkCmdSetAlphaToCoverageEnableEXT            = reinterpret_cast<PFN_vkCmdSetAlphaToCoverageEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetAlphaToCoverageEnableEXT"));
                functionTable.vkCmdSetAlphaToOneEnableEXT                 = reinterpret_cast<PFN_vkCmdSetAlphaToOneEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetAlphaToOneEnableEXT"));
                functionTable.vkCmdSetColorBlendAdvancedEXT               = reinterpret_cast<PFN_vkCmdSetColorBlendAdvancedEXT>(vkGetDeviceProcAddr(device, "vkCmdSetColorBlendAdvancedEXT"));
                functionTable.vkCmdSetColorBlendEnableEXT                 = reinterpret_cast<PFN_vkCmdSetColorBlendEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetColorBlendEnableEXT"));
                functionTable.vkCmdSetColorBlendEquationEXT               = reinterpret_cast<PFN_vkCmdSetColorBlendEquationEXT>(vkGetDeviceProcAddr(device, "vkCmdSetColorBlendEquationEXT"));
                functionTable.vkCmdSetColorWriteMaskEXT                   = reinterpret_cast<PFN_vkCmdSetColorWriteMaskEXT>(vkGetDeviceProcAddr(device, "vkCmdSetColorWriteMaskEXT"));
                functionTable.vkCmdSetConservativeRasterizationModeEXT    = reinterpret_cast<PFN_vkCmdSetConservativeRasterizationModeEXT>(vkGetDeviceProcAddr(device, "vkCmdSetConservativeRasterizationModeEXT"));
                functionTable.vkCmdSetDepthClampEnableEXT                 = reinterpret_cast<PFN_vkCmdSetDepthClampEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthClampEnableEXT"));
                functionTable.vkCmdSetDepthClipEnableEXT                  = reinterpret_cast<PFN_vkCmdSetDepthClipEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthClipEnableEXT"));
                functionTable.vkCmdSetDepthClipNegativeOneToOneEXT        = reinterpret_cast<PFN_vkCmdSetDepthClipNegativeOneToOneEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthClipNegativeOneToOneEXT"));
                functionTable.vkCmdSetExtraPrimitiveOverestimationSizeEXT = reinterpret_cast<PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT>(vkGetDeviceProcAddr(device, "vkCmdSetExtraPrimitiveOverestimationSizeEXT"));
                functionTable.vkCmdSetLineRasterizationModeEXT            = reinterpret_cast<PFN_vkCmdSetLineRasterizationModeEXT>(vkGetDeviceProcAddr(device, "vkCmdSetLineRasterizationModeEXT"));
                functionTable.vkCmdSetLineStippleEnableEXT                = reinterpret_cast<PFN_vkCmdSetLineStippleEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetLineStippleEnableEXT"));
                functionTable.vkCmdSetLogicOpEnableEXT                    = reinterpret_cast<PFN_vkCmdSetLogicOpEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetLogicOpEnableEXT"));
                functionTable.vkCmdSetPolygonModeEXT                      = reinterpret_cast<PFN_vkCmdSetPolygonModeEXT>(vkGetDeviceProcAddr(device, "vkCmdSetPolygonModeEXT"));
                functionTable.vkCmdSetProvokingVertexModeEXT              = reinterpret_cast<PFN_vkCmdSetProvokingVertexModeEXT>(vkGetDeviceProcAddr(device, "vkCmdSetProvokingVertexModeEXT"));
                functionTable.vkCmdSetRasterizationSamplesEXT             = reinterpret_cast<PFN_vkCmdSetRasterizationSamplesEXT>(vkGetDeviceProcAddr(device, "vkCmdSetRasterizationSamplesEXT"));
                functionTable.vkCmdSetRasterizationStreamEXT              = reinterpret_cast<PFN_vkCmdSetRasterizationStreamEXT>(vkGetDeviceProcAddr(device, "vkCmdSetRasterizationStreamEXT"));
                functionTable.vkCmdSetSampleLocationsEnableEXT            = reinterpret_cast<PFN_vkCmdSetSampleLocationsEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetSampleLocationsEnableEXT"));
                functionTable.vkCmdSetSampleMaskEXT                       = reinterpret_cast<PFN_vkCmdSetSampleMaskEXT>(vkGetDeviceProcAddr(device, "vkCmdSetSampleMaskEXT"));
                functionTable.vkCmdSetTessellationDomainOriginEXT         = reinterpret_cast<PFN_vkCmdSetTessellationDomainOriginEXT>(vkGetDeviceProcAddr(device, "vkCmdSetTessellationDomainOriginEXT"));
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling))
                functionTable.vkCmdSetViewportWScalingEnableNV = reinterpret_cast<PFN_vkCmdSetViewportWScalingEnableNV>(vkGetDeviceProcAddr(device, "vkCmdSetViewportWScalingEnableNV"));
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle))
                functionTable.vkCmdSetViewportSwizzleNV = reinterpret_cast<PFN_vkCmdSetViewportSwizzleNV>(vkGetDeviceProcAddr(device, "vkCmdSetViewportSwizzleNV"));
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color))
                functionTable.vkCmdSetCoverageToColorEnableNV   = reinterpret_cast<PFN_vkCmdSetCoverageToColorEnableNV>(vkGetDeviceProcAddr(device, "vkCmdSetCoverageToColorEnableNV"));
                functionTable.vkCmdSetCoverageToColorLocationNV = reinterpret_cast<PFN_vkCmdSetCoverageToColorLocationNV>(vkGetDeviceProcAddr(device, "vkCmdSetCoverageToColorLocationNV"));
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples))
                functionTable.vkCmdSetCoverageModulationModeNV        = reinterpret_cast<PFN_vkCmdSetCoverageModulationModeNV>(vkGetDeviceProcAddr(device, "vkCmdSetCoverageModulationModeNV"));
                functionTable.vkCmdSetCoverageModulationTableEnableNV = reinterpret_cast<PFN_vkCmdSetCoverageModulationTableEnableNV>(vkGetDeviceProcAddr(device, "vkCmdSetCoverageModulationTableEnableNV"));
                functionTable.vkCmdSetCoverageModulationTableNV       = reinterpret_cast<PFN_vkCmdSetCoverageModulationTableNV>(vkGetDeviceProcAddr(device, "vkCmdSetCoverageModulationTableNV"));
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image))
                functionTable.vkCmdSetShadingRateImageEnableNV = reinterpret_cast<PFN_vkCmdSetShadingRateImageEnableNV>(vkGetDeviceProcAddr(device, "vkCmdSetShadingRateImageEnableNV"));
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test))
                functionTable.vkCmdSetRepresentativeFragmentTestEnableNV = reinterpret_cast<PFN_vkCmdSetRepresentativeFragmentTestEnableNV>(vkGetDeviceProcAddr(device, "vkCmdSetRepresentativeFragmentTestEnableNV"));
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode))
                functionTable.vkCmdSetCoverageReductionModeNV = reinterpret_cast<PFN_vkCmdSetCoverageReductionModeNV>(vkGetDeviceProcAddr(device, "vkCmdSetCoverageReductionModeNV"));
            #endif
            #if (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1))
                functionTable.vkGetDeviceGroupSurfacePresentModes2EXT = reinterpret_cast<PFN_vkGetDeviceGroupSurfacePresentModes2EXT>(vkGetDeviceProcAddr(device, "vkGetDeviceGroupSurfacePresentModes2EXT"));
            #endif
            #if (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control))
                functionTable.vkGetImageSubresourceLayout2EXT = reinterpret_cast<PFN_vkGetImageSubresourceLayout2EXT>(vkGetDeviceProcAddr(device, "vkGetImageSubresourceLayout2EXT"));
            #endif
            #if (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state))
                functionTable.vkCmdSetVertexInputEXT = reinterpret_cast<PFN_vkCmdSetVertexInputEXT>(vkGetDeviceProcAddr(device, "vkCmdSetVertexInputEXT"));
            #endif
            #if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template))
                functionTable.vkCmdPushDescriptorSetWithTemplateKHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetWithTemplateKHR>(vkGetDeviceProcAddr(device, "vkCmdPushDescriptorSetWithTemplateKHR"));
            #endif
            #if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
                functionTable.vkGetDeviceGroupPresentCapabilitiesKHR = reinterpret_cast<PFN_vkGetDeviceGroupPresentCapabilitiesKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceGroupPresentCapabilitiesKHR"));
                functionTable.vkGetDeviceGroupSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetDeviceGroupSurfacePresentModesKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceGroupSurfacePresentModesKHR"));
            #endif
            #if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
                functionTable.vkAcquireNextImage2KHR = reinterpret_cast<PFN_vkAcquireNextImage2KHR>(vkGetDeviceProcAddr(device, "vkAcquireNextImage2KHR"));
            #endif
        #pragma endregion

        const VmaVulkanFunctions vulkanFunctions
        {
            .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
            .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
            .vkGetPhysicalDeviceProperties = context.GetFunctionTable().vkGetPhysicalDeviceProperties,
            .vkGetPhysicalDeviceMemoryProperties = context.GetFunctionTable().vkGetPhysicalDeviceMemoryProperties,
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
            // Vulkan 1.1.0 Functions
            .vkGetBufferMemoryRequirements2KHR = functionTable.vkGetBufferMemoryRequirements2,
            .vkGetImageMemoryRequirements2KHR = functionTable.vkGetImageMemoryRequirements2,
            .vkBindBufferMemory2KHR = functionTable.vkBindBufferMemory2,
            .vkBindImageMemory2KHR = functionTable.vkBindImageMemory2,
            .vkGetPhysicalDeviceMemoryProperties2KHR = context.GetFunctionTable().vkGetPhysicalDeviceMemoryProperties2,
            // Vulkan 1.3.0 Functions
            .vkGetDeviceBufferMemoryRequirements = functionTable.vkGetDeviceBufferMemoryRequirements,
            .vkGetDeviceImageMemoryRequirements = functionTable.vkGetDeviceImageMemoryRequirements
        };

        // Set up allocator create info
        const Version version = context.GetBackendVersion();
        const VmaAllocatorCreateInfo vmaCreteInfo
        {
            .flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT,
            .physicalDevice = physicalDevice,
            .device = device,
            .pVulkanFunctions = &vulkanFunctions,
            .instance = context.GetVulkanInstance(),
            .vulkanApiVersion = VK_MAKE_API_VERSION(0, version.GetMajor(), version.GetMinor(), version.GetPatch())
        };

        // Create allocator
        result = vmaCreateAllocator(&vmaCreteInfo, &vmaAllocator);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Cannot create Vulkan device [{0}], as creation of memory allocator failed", name));

        // Create global timeline semaphore
        {
            SR_THROW_IF(!IsExtensionLoaded(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME), UnsupportedFeatureError(SR_FORMAT("Cannot create device Vulkan device [{0}], as it does not support the [{1}] extension", name, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)));

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
            if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Cannot create Vulkan device [{0}], as creation of timeline semaphore failed", name));
            SetResourceName(semaphore, VK_OBJECT_TYPE_SEMAPHORE, SR_FORMAT("Semaphore of device [{0}]", name));
        }

        // Create global (bindless) descriptor set layout
        {
            SR_THROW_IF(!IsExtensionLoaded(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME), UnsupportedFeatureError(SR_FORMAT("Cannot create device Vulkan device [{0}], as it does not support the [{1}] extension", name, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)));

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
            if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Cannot create Vulkan device [{0}], as creation of descriptor set layout failed", name));
            SetResourceName(descriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, SR_FORMAT("Descriptor set layout of device [{0}]", name));
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
                if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Cannot create Vulkan device [{0}], as creation of pipeline layout [{1}] failed", name, i));
                SetResourceName(pipelineLayouts[i], VK_OBJECT_TYPE_PIPELINE_LAYOUT, SR_FORMAT("Pipeline layout [{0}] of device [{1}]", i, name));
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

    std::unique_ptr<Swapchain> VulkanDevice::CreateSwapchain(const SwapchainCreateInfo& createInfo) const
    {
        return std::make_unique<VulkanSwapchain>(context, *this, createInfo);
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
            .maxRenderPassWidth = physicalDeviceProperties.properties.limits.maxFramebufferWidth,
            .maxRenderPassHeight = physicalDeviceProperties.properties.limits.maxFramebufferHeight,
            .maxWorkGroupSize = { physicalDeviceProperties.properties.limits.maxComputeWorkGroupSize[0], physicalDeviceProperties.properties.limits.maxComputeWorkGroupSize[1], physicalDeviceProperties.properties.limits.maxComputeWorkGroupSize[2] },
            .highestImageSampling = GetHighestImageSamplingSupported(),
            .highestSamplerAnisotropy = GetHighestSamplerAnisotropySupported()
        };
    }

    bool VulkanDevice::IsImageFormatSupported(const ImageFormat format, const ImageUsage usage) const noexcept
    {
        // Get format properties
        VkFormatProperties formatProperties = { };
        context.GetFunctionTable().vkGetPhysicalDeviceFormatProperties(physicalDevice, ImageFormatToVkFormat(format), &formatProperties);

        // Check support
        if (usage & ImageUsage::SourceMemory        && !(formatProperties.linearTilingFeatures  & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT               )) return false;
        if (usage & ImageUsage::DestinationMemory   && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_DST_BIT               )) return false;
        if (usage & ImageUsage::Storage             && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT              )) return false;
        if (usage & ImageUsage::Sample              && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT              )) return false;
        if (usage & ImageUsage::Filter              && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT              )) return false;
        if (usage & ImageUsage::ColorAttachment     && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) return false;
        if (usage & ImageUsage::DepthAttachment     && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT   )) return false;
        if (usage & ImageUsage::InputAttachment     && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT              )) return false;
        if (usage & ImageUsage::TransientAttachment && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT           )) return false;
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
        context.GetFunctionTable().vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
        return physicalDeviceProperties;
    }

    VkPhysicalDeviceProperties2 VulkanDevice::GetPhysicalDeviceProperties2(void* pNext) const noexcept
    {
        VkPhysicalDeviceProperties2 physicalDeviceProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR, .pNext = pNext };
        context.GetFunctionTable().vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties);
        return physicalDeviceProperties;
    }

    VkPhysicalDeviceFeatures VulkanDevice::GetPhysicalDeviceFeatures() const noexcept
    {
        VkPhysicalDeviceFeatures physicalDeviceFeatures = { };
        context.GetFunctionTable().vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
        return physicalDeviceFeatures;
    }

    VkPhysicalDeviceFeatures2 VulkanDevice::GetPhysicalDeviceFeatures2(void* const pNext) const noexcept
    {
        VkPhysicalDeviceFeatures2 physicalDeviceFeatures = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR, .pNext = pNext };
        context.GetFunctionTable().vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures);
        return physicalDeviceFeatures;
    }

    /* --- SETTER METHODS --- */

    void VulkanDevice::SetResourceName(VkHandle64 object, const VkObjectType type, const std::string_view resourceName) const noexcept
    {
        SetResourceName(reinterpret_cast<VkHandle32>(object), type, resourceName);
    }

    void VulkanDevice::SetResourceName(const VkHandle32 object, const VkObjectType type, const std::string_view resourceName) const noexcept
    {
        if (!context.IsExtensionLoaded(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) return;

        // Set up object name info
        const VkDebugUtilsObjectNameInfoEXT objectNameInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .objectType = type,
            .objectHandle = reinterpret_cast<uint64>(object),
            .pObjectName = resourceName.data(),
        };

        // Assign resource name
        context.GetFunctionTable().vkSetDebugUtilsObjectNameEXT(device, &objectNameInfo);
    }

    /* --- OPERATORS --- */

    bool VulkanDevice::operator==(const VulkanDevice& other) noexcept
    {
        VkPhysicalDeviceProperties thisPhysicalDeviceProperties = { };
        context.GetFunctionTable().vkGetPhysicalDeviceProperties(physicalDevice, &thisPhysicalDeviceProperties);

        VkPhysicalDeviceProperties otherPhysicalDeviceProperties = { };
        other.context.GetFunctionTable().vkGetPhysicalDeviceProperties(other.physicalDevice, &otherPhysicalDeviceProperties);

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
