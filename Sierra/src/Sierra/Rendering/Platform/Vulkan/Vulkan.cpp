//
// Created by Nikolay Kanchevski on 18.11.23.
//

#include "Vulkan.h"

//PFN_vkAllocateCommandBuffers AllocateCommandBuffers = nullptr;
//PFN_vkAllocateDescriptorSets AllocateDescriptorSets = nullptr;
//PFN_vkAllocateMemory AllocateMemory = nullptr;

namespace Sierra
{

    void VulkanLoader::Initialize()
    {
        #if defined(VK_VERSION_1_0)
            vkAllocateCommandBuffers = vkAllocateCommandBuffers;
            vkAllocateDescriptorSets = vkAllocateDescriptorSets;
            vkAllocateMemory = vkAllocateMemory;
            vkBeginCommandBuffer = vkBeginCommandBuffer;
            vkBindBufferMemory = vkBindBufferMemory;
            vkBindImageMemory = vkBindImageMemory;
            vkCmdBeginQuery = vkCmdBeginQuery;
            vkCmdBeginRenderPass = vkCmdBeginRenderPass;
            vkCmdBindDescriptorSets = vkCmdBindDescriptorSets;
            vkCmdBindIndexBuffer = vkCmdBindIndexBuffer;
            vkCmdBindPipeline = vkCmdBindPipeline;
            vkCmdBindVertexBuffers = vkCmdBindVertexBuffers;
            vkCmdBlitImage = vkCmdBlitImage;
            vkCmdClearAttachments = vkCmdClearAttachments;
            vkCmdClearColorImage = vkCmdClearColorImage;
            vkCmdClearDepthStencilImage = vkCmdClearDepthStencilImage;
            vkCmdCopyBuffer = vkCmdCopyBuffer;
            vkCmdCopyBufferToImage = vkCmdCopyBufferToImage;
            vkCmdCopyImage = vkCmdCopyImage;
            vkCmdCopyImageToBuffer = vkCmdCopyImageToBuffer;
            vkCmdCopyQueryPoolResults = vkCmdCopyQueryPoolResults;
            vkCmdDispatch = vkCmdDispatch;
            vkCmdDispatchIndirect = vkCmdDispatchIndirect;
            vkCmdDraw = vkCmdDraw;
            vkCmdDrawIndexed = vkCmdDrawIndexed;
            vkCmdDrawIndexedIndirect = vkCmdDrawIndexedIndirect;
            vkCmdDrawIndirect = vkCmdDrawIndirect;
            vkCmdEndQuery = vkCmdEndQuery;
            vkCmdEndRenderPass = vkCmdEndRenderPass;
            vkCmdExecuteCommands = vkCmdExecuteCommands;
            vkCmdFillBuffer = vkCmdFillBuffer;
            vkCmdNextSubpass = vkCmdNextSubpass;
            vkCmdPipelineBarrier = vkCmdPipelineBarrier;
            vkCmdPushConstants = vkCmdPushConstants;
            vkCmdResetEvent = vkCmdResetEvent;
            vkCmdResetQueryPool = vkCmdResetQueryPool;
            vkCmdResolveImage = vkCmdResolveImage;
            vkCmdSetBlendConstants = vkCmdSetBlendConstants;
            vkCmdSetDepthBias = vkCmdSetDepthBias;
            vkCmdSetDepthBounds = vkCmdSetDepthBounds;
            vkCmdSetEvent = vkCmdSetEvent;
            vkCmdSetLineWidth = vkCmdSetLineWidth;
            vkCmdSetScissor = vkCmdSetScissor;
            vkCmdSetStencilCompareMask = vkCmdSetStencilCompareMask;
            vkCmdSetStencilReference = vkCmdSetStencilReference;
            vkCmdSetStencilWriteMask = vkCmdSetStencilWriteMask;
            vkCmdSetViewport = vkCmdSetViewport;
            vkCmdUpdateBuffer = vkCmdUpdateBuffer;
            vkCmdWaitEvents = vkCmdWaitEvents;
            vkCmdWriteTimestamp = vkCmdWriteTimestamp;
            vkCreateBuffer = vkCreateBuffer;
            vkCreateBufferView = vkCreateBufferView;
            vkCreateCommandPool = vkCreateCommandPool;
            vkCreateComputePipelines = vkCreateComputePipelines;
            vkCreateDescriptorPool = vkCreateDescriptorPool;
            vkCreateDescriptorSetLayout = vkCreateDescriptorSetLayout;
            vkCreateDevice = vkCreateDevice;
            vkCreateEvent = vkCreateEvent;
            vkCreateFence = vkCreateFence;
            vkCreateFramebuffer = vkCreateFramebuffer;
            vkCreateGraphicsPipelines = vkCreateGraphicsPipelines;
            vkCreateImage = vkCreateImage;
            vkCreateImageView = vkCreateImageView;
            vkCreateInstance = vkCreateInstance;
            vkCreatePipelineCache = vkCreatePipelineCache;
            vkCreatePipelineLayout = vkCreatePipelineLayout;
            vkCreateQueryPool = vkCreateQueryPool;
            vkCreateRenderPass = vkCreateRenderPass;
            vkCreateSampler = vkCreateSampler;
            vkCreateSemaphore = vkCreateSemaphore;
            vkCreateShaderModule = vkCreateShaderModule;
            vkDestroyBuffer = vkDestroyBuffer;
            vkDestroyBufferView = vkDestroyBufferView;
            vkDestroyCommandPool = vkDestroyCommandPool;
            vkDestroyDescriptorPool = vkDestroyDescriptorPool;
            vkDestroyDescriptorSetLayout = vkDestroyDescriptorSetLayout;
            vkDestroyDevice = vkDestroyDevice;
            vkDestroyEvent = vkDestroyEvent;
            vkDestroyFence = vkDestroyFence;
            vkDestroyFramebuffer = vkDestroyFramebuffer;
            vkDestroyImage = vkDestroyImage;
            vkDestroyImageView = vkDestroyImageView;
            vkDestroyInstance = vkDestroyInstance;
            vkDestroyPipeline = vkDestroyPipeline;
            vkDestroyPipelineCache = vkDestroyPipelineCache;
            vkDestroyPipelineLayout = vkDestroyPipelineLayout;
            vkDestroyQueryPool = vkDestroyQueryPool;
            vkDestroyRenderPass = vkDestroyRenderPass;
            vkDestroySampler = vkDestroySampler;
            vkDestroySemaphore = vkDestroySemaphore;
            vkDestroyShaderModule = vkDestroyShaderModule;
            vkDeviceWaitIdle = vkDeviceWaitIdle;
            vkEndCommandBuffer = vkEndCommandBuffer;
            vkEnumerateDeviceExtensionProperties = vkEnumerateDeviceExtensionProperties;
            vkEnumerateDeviceLayerProperties = vkEnumerateDeviceLayerProperties;
            vkEnumerateInstanceExtensionProperties = vkEnumerateInstanceExtensionProperties;
            vkEnumerateInstanceLayerProperties = vkEnumerateInstanceLayerProperties;
            vkEnumeratePhysicalDevices = vkEnumeratePhysicalDevices;
            vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
            vkFreeCommandBuffers = vkFreeCommandBuffers;
            vkFreeDescriptorSets = vkFreeDescriptorSets;
            vkFreeMemory = vkFreeMemory;
            vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
            vkGetDeviceMemoryCommitment = vkGetDeviceMemoryCommitment;
            vkGetDeviceProcAddr = vkGetDeviceProcAddr;
            vkGetDeviceQueue = vkGetDeviceQueue;
            vkGetEventStatus = vkGetEventStatus;
            vkGetFenceStatus = vkGetFenceStatus;
            vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
            vkGetImageSparseMemoryRequirements = vkGetImageSparseMemoryRequirements;
            vkGetImageSubresourceLayout = vkGetImageSubresourceLayout;
            vkGetInstanceProcAddr = vkGetInstanceProcAddr;
            vkGetPhysicalDeviceFeatures = vkGetPhysicalDeviceFeatures;
            vkGetPhysicalDeviceFormatProperties = vkGetPhysicalDeviceFormatProperties;
            vkGetPhysicalDeviceImageFormatProperties = vkGetPhysicalDeviceImageFormatProperties;
            vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
            vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
            vkGetPhysicalDeviceQueueFamilyProperties = vkGetPhysicalDeviceQueueFamilyProperties;
            vkGetPhysicalDeviceSparseImageFormatProperties = vkGetPhysicalDeviceSparseImageFormatProperties;
            vkGetPipelineCacheData = vkGetPipelineCacheData;
            vkGetQueryPoolResults = vkGetQueryPoolResults;
            vkGetRenderAreaGranularity = vkGetRenderAreaGranularity;
            vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
            vkMapMemory = vkMapMemory;
            vkMergePipelineCaches = vkMergePipelineCaches;
            vkQueueBindSparse = vkQueueBindSparse;
            vkQueueSubmit = vkQueueSubmit;
            vkQueueWaitIdle = vkQueueWaitIdle;
            vkResetCommandBuffer = vkResetCommandBuffer;
            vkResetCommandPool = vkResetCommandPool;
            vkResetDescriptorPool = vkResetDescriptorPool;
            vkResetEvent = vkResetEvent;
            vkResetFences = vkResetFences;
            vkSetEvent = vkSetEvent;
            vkUnmapMemory = vkUnmapMemory;
            vkUpdateDescriptorSets = vkUpdateDescriptorSets;
            vkWaitForFences = vkWaitForFences;
        #endif
        #if defined(VK_VERSION_1_1)
            vkBindBufferMemory2 = vkBindBufferMemory2;
            vkBindImageMemory2 = vkBindImageMemory2;
            vkCmdDispatchBase = vkCmdDispatchBase;
            vkCmdSetDeviceMask = vkCmdSetDeviceMask;
            vkCreateDescriptorUpdateTemplate = vkCreateDescriptorUpdateTemplate;
            vkCreateSamplerYcbcrConversion = vkCreateSamplerYcbcrConversion;
            vkDestroyDescriptorUpdateTemplate = vkDestroyDescriptorUpdateTemplate;
            vkDestroySamplerYcbcrConversion = vkDestroySamplerYcbcrConversion;
            vkEnumerateInstanceVersion = vkEnumerateInstanceVersion;
            vkEnumeratePhysicalDeviceGroups = vkEnumeratePhysicalDeviceGroups;
            vkGetBufferMemoryRequirements2 = vkGetBufferMemoryRequirements2;
            vkGetDescriptorSetLayoutSupport = vkGetDescriptorSetLayoutSupport;
            vkGetDeviceGroupPeerMemoryFeatures = vkGetDeviceGroupPeerMemoryFeatures;
            vkGetDeviceQueue2 = vkGetDeviceQueue2;
            vkGetImageMemoryRequirements2 = vkGetImageMemoryRequirements2;
            vkGetImageSparseMemoryRequirements2 = vkGetImageSparseMemoryRequirements2;
            vkGetPhysicalDeviceExternalBufferProperties = vkGetPhysicalDeviceExternalBufferProperties;
            vkGetPhysicalDeviceExternalFenceProperties = vkGetPhysicalDeviceExternalFenceProperties;
            vkGetPhysicalDeviceExternalSemaphoreProperties = vkGetPhysicalDeviceExternalSemaphoreProperties;
            vkGetPhysicalDeviceFeatures2 = vkGetPhysicalDeviceFeatures2;
            vkGetPhysicalDeviceFormatProperties2 = vkGetPhysicalDeviceFormatProperties2;
            vkGetPhysicalDeviceImageFormatProperties2 = vkGetPhysicalDeviceImageFormatProperties2;
            vkGetPhysicalDeviceMemoryProperties2 = vkGetPhysicalDeviceMemoryProperties2;
            vkGetPhysicalDeviceProperties2 = vkGetPhysicalDeviceProperties2;
            vkGetPhysicalDeviceQueueFamilyProperties2 = vkGetPhysicalDeviceQueueFamilyProperties2;
            vkGetPhysicalDeviceSparseImageFormatProperties2 = vkGetPhysicalDeviceSparseImageFormatProperties2;
            vkTrimCommandPool = vkTrimCommandPool;
            vkUpdateDescriptorSetWithTemplate = vkUpdateDescriptorSetWithTemplate;
        #endif
        #if defined(VK_VERSION_1_2)
            vkCmdBeginRenderPass2 = vkCmdBeginRenderPass2;
            vkCmdDrawIndexedIndirectCount = vkCmdDrawIndexedIndirectCount;
            vkCmdDrawIndirectCount = vkCmdDrawIndirectCount;
            vkCmdEndRenderPass2 = vkCmdEndRenderPass2;
            vkCmdNextSubpass2 = vkCmdNextSubpass2;
            vkCreateRenderPass2 = vkCreateRenderPass2;
            vkGetBufferDeviceAddress = vkGetBufferDeviceAddress;
            vkGetBufferOpaqueCaptureAddress = vkGetBufferOpaqueCaptureAddress;
            vkGetDeviceMemoryOpaqueCaptureAddress = vkGetDeviceMemoryOpaqueCaptureAddress;
            vkGetSemaphoreCounterValue = vkGetSemaphoreCounterValue;
            vkResetQueryPool = vkResetQueryPool;
            vkSignalSemaphore = vkSignalSemaphore;
            vkWaitSemaphores = vkWaitSemaphores;
        #endif
        #if defined(VK_VERSION_1_3)
            vkCmdBeginRendering = vkCmdBeginRendering;
            vkCmdBindVertexBuffers2 = vkCmdBindVertexBuffers2;
            vkCmdBlitImage2 = vkCmdBlitImage2;
            vkCmdCopyBuffer2 = vkCmdCopyBuffer2;
            vkCmdCopyBufferToImage2 = vkCmdCopyBufferToImage2;
            vkCmdCopyImage2 = vkCmdCopyImage2;
            vkCmdCopyImageToBuffer2 = vkCmdCopyImageToBuffer2;
            vkCmdEndRendering = vkCmdEndRendering;
            vkCmdPipelineBarrier2 = vkCmdPipelineBarrier2;
            vkCmdResetEvent2 = vkCmdResetEvent2;
            vkCmdResolveImage2 = vkCmdResolveImage2;
            vkCmdSetCullMode = vkCmdSetCullMode;
            vkCmdSetDepthBiasEnable = vkCmdSetDepthBiasEnable;
            vkCmdSetDepthBoundsTestEnable = vkCmdSetDepthBoundsTestEnable;
            vkCmdSetDepthCompareOp = vkCmdSetDepthCompareOp;
            vkCmdSetDepthTestEnable = vkCmdSetDepthTestEnable;
            vkCmdSetDepthWriteEnable = vkCmdSetDepthWriteEnable;
            vkCmdSetEvent2 = vkCmdSetEvent2;
            vkCmdSetFrontFace = vkCmdSetFrontFace;
            vkCmdSetPrimitiveRestartEnable = vkCmdSetPrimitiveRestartEnable;
            vkCmdSetPrimitiveTopology = vkCmdSetPrimitiveTopology;
            vkCmdSetRasterizerDiscardEnable = vkCmdSetRasterizerDiscardEnable;
            vkCmdSetScissorWithCount = vkCmdSetScissorWithCount;
            vkCmdSetStencilOp = vkCmdSetStencilOp;
            vkCmdSetStencilTestEnable = vkCmdSetStencilTestEnable;
            vkCmdSetViewportWithCount = vkCmdSetViewportWithCount;
            vkCmdWaitEvents2 = vkCmdWaitEvents2;
            vkCmdWriteTimestamp2 = vkCmdWriteTimestamp2;
            vkCreatePrivateDataSlot = vkCreatePrivateDataSlot;
            vkDestroyPrivateDataSlot = vkDestroyPrivateDataSlot;
            vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements;
            vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements;
            vkGetDeviceImageSparseMemoryRequirements = vkGetDeviceImageSparseMemoryRequirements;
            vkGetPhysicalDeviceToolProperties = vkGetPhysicalDeviceToolProperties;
            vkGetPrivateData = vkGetPrivateData;
            vkQueueSubmit2 = vkQueueSubmit2;
            vkSetPrivateData = vkSetPrivateData;
        #endif
    }

    void VulkanLoader::LoadInstance(const VkInstance vkInstance)
    {
        #if defined(VK_VERSION_1_0)
            vkCreateDevice = vkCreateDevice;
//            vkDestroyInstance = vkDestroyInstance;
//            vkEnumerateDeviceExtensionProperties = vkEnumerateDeviceExtensionProperties;
            vkEnumerateDeviceLayerProperties = vkEnumerateDeviceLayerProperties;
            vkEnumeratePhysicalDevices = vkEnumeratePhysicalDevices;
            vkGetDeviceProcAddr = vkGetDeviceProcAddr;
//            vkGetPhysicalDeviceFeatures = vkGetPhysicalDeviceFeatures;
            vkGetPhysicalDeviceFormatProperties = vkGetPhysicalDeviceFormatProperties;
            vkGetPhysicalDeviceImageFormatProperties = vkGetPhysicalDeviceImageFormatProperties;
//            vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
//            vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
//            vkGetPhysicalDeviceQueueFamilyProperties = vkGetPhysicalDeviceQueueFamilyProperties;
            vkGetPhysicalDeviceSparseImageFormatProperties = vkGetPhysicalDeviceSparseImageFormatProperties;
        #endif
        #if defined(VK_VERSION_1_1)
            vkEnumeratePhysicalDeviceGroups = vkEnumeratePhysicalDeviceGroups;
            vkGetPhysicalDeviceExternalBufferProperties = vkGetPhysicalDeviceExternalBufferProperties;
            vkGetPhysicalDeviceExternalFenceProperties = vkGetPhysicalDeviceExternalFenceProperties;
            vkGetPhysicalDeviceExternalSemaphoreProperties = vkGetPhysicalDeviceExternalSemaphoreProperties;
            vkGetPhysicalDeviceFeatures2 = vkGetPhysicalDeviceFeatures2;
            vkGetPhysicalDeviceFormatProperties2 = vkGetPhysicalDeviceFormatProperties2;
            vkGetPhysicalDeviceImageFormatProperties2 = vkGetPhysicalDeviceImageFormatProperties2;
            vkGetPhysicalDeviceMemoryProperties2 = vkGetPhysicalDeviceMemoryProperties2;
            vkGetPhysicalDeviceProperties2 = vkGetPhysicalDeviceProperties2;
            vkGetPhysicalDeviceQueueFamilyProperties2 = vkGetPhysicalDeviceQueueFamilyProperties2;
            vkGetPhysicalDeviceSparseImageFormatProperties2 = vkGetPhysicalDeviceSparseImageFormatProperties2;
        #endif
        #if defined(VK_VERSION_1_3)
            vkGetPhysicalDeviceToolProperties = vkGetPhysicalDeviceToolProperties;
        #endif
        #if defined(VK_EXT_acquire_drm_display)
            vkAcquireDrmDisplayEXT = vkAcquireDrmDisplayEXT;
            vkGetDrmDisplayEXT = vkGetDrmDisplayEXT;
        #endif
        #if defined(VK_EXT_acquire_xlib_display)
            vkAcquireXlibDisplayEXT = vkAcquireXlibDisplayEXT;
            vkGetRandROutputDisplayEXT = vkGetRandROutputDisplayEXT;
        #endif
        #if defined(VK_EXT_calibrated_timestamps)
            vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = vkGetPhysicalDeviceCalibrateableTimeDomainsEXT;
        #endif
        #if defined(VK_EXT_debug_report)
            vkCreateDebugReportCallbackEXT = vkCreateDebugReportCallbackEXT;
            vkDebugReportMessageEXT = vkDebugReportMessageEXT;
            vkDestroyDebugReportCallbackEXT = vkDestroyDebugReportCallbackEXT;
        #endif
        #if defined(VK_EXT_debug_utils)
            vkCmdBeginDebugUtilsLabelEXT = vkCmdBeginDebugUtilsLabelEXT;
            vkCmdEndDebugUtilsLabelEXT = vkCmdEndDebugUtilsLabelEXT;
            vkCmdInsertDebugUtilsLabelEXT = vkCmdInsertDebugUtilsLabelEXT;
            vkCreateDebugUtilsMessengerEXT = vkCreateDebugUtilsMessengerEXT;
            vkDestroyDebugUtilsMessengerEXT = vkDestroyDebugUtilsMessengerEXT;
            vkQueueBeginDebugUtilsLabelEXT = vkQueueBeginDebugUtilsLabelEXT;
            vkQueueEndDebugUtilsLabelEXT = vkQueueEndDebugUtilsLabelEXT;
            vkQueueInsertDebugUtilsLabelEXT = vkQueueInsertDebugUtilsLabelEXT;
            vkSetDebugUtilsObjectNameEXT = vkSetDebugUtilsObjectNameEXT;
            vkSetDebugUtilsObjectTagEXT = vkSetDebugUtilsObjectTagEXT;
            vkSubmitDebugUtilsMessageEXT = vkSubmitDebugUtilsMessageEXT;
        #endif
        #if defined(VK_EXT_direct_mode_display)
            vkReleaseDisplayEXT = vkReleaseDisplayEXT;
        #endif
        #if defined(VK_EXT_directfb_surface)
            vkCreateDirectFBSurfaceEXT = vkCreateDirectFBSurfaceEXT;
            vkGetPhysicalDeviceDirectFBPresentationSupportEXT = vkGetPhysicalDeviceDirectFBPresentationSupportEXT;
        #endif
        #if defined(VK_EXT_display_surface_counter)
            vkGetPhysicalDeviceSurfaceCapabilities2EXT = vkGetPhysicalDeviceSurfaceCapabilities2EXT;
        #endif
        #if defined(VK_EXT_full_screen_exclusive)
            vkGetPhysicalDeviceSurfacePresentModes2EXT = vkGetPhysicalDeviceSurfacePresentModes2EXT;
        #endif
        #if defined(VK_EXT_headless_surface)
            vkCreateHeadlessSurfaceEXT = vkCreateHeadlessSurfaceEXT;
        #endif
        #if defined(VK_EXT_metal_surface)
            vkCreateMetalSurfaceEXT = vkCreateMetalSurfaceEXT;
        #endif
        #if defined(VK_EXT_sample_locations)
            vkGetPhysicalDeviceMultisamplePropertiesEXT = vkGetPhysicalDeviceMultisamplePropertiesEXT;
        #endif
        #if defined(VK_EXT_tooling_info)
            vkGetPhysicalDeviceToolPropertiesEXT = vkGetPhysicalDeviceToolPropertiesEXT;
        #endif
        #if defined(VK_FUCHSIA_imagepipe_surface)
            vkCreateImagePipeSurfaceFUCHSIA = vkCreateImagePipeSurfaceFUCHSIA;
        #endif
        #if defined(VK_GGP_stream_descriptor_surface)
            vkCreateStreamDescriptorSurfaceGGP = vkCreateStreamDescriptorSurfaceGGP;
        #endif
        #if defined(VK_KHR_android_surface)
            vkCreateAndroidSurfaceKHR = vkCreateAndroidSurfaceKHR;
        #endif
        #if defined(VK_KHR_cooperative_matrix)
            vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR = vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR;
        #endif
        #if defined(VK_KHR_device_group_creation)
            vkEnumeratePhysicalDeviceGroupsKHR = vkEnumeratePhysicalDeviceGroupsKHR;
        #endif
        #if defined(VK_KHR_display)
            vkCreateDisplayModeKHR = vkCreateDisplayModeKHR;
            vkCreateDisplayPlaneSurfaceKHR = vkCreateDisplayPlaneSurfaceKHR;
            vkGetDisplayModePropertiesKHR = vkGetDisplayModePropertiesKHR;
            vkGetDisplayPlaneCapabilitiesKHR = vkGetDisplayPlaneCapabilitiesKHR;
            vkGetDisplayPlaneSupportedDisplaysKHR = vkGetDisplayPlaneSupportedDisplaysKHR;
            vkGetPhysicalDeviceDisplayPlanePropertiesKHR = vkGetPhysicalDeviceDisplayPlanePropertiesKHR;
            vkGetPhysicalDeviceDisplayPropertiesKHR = vkGetPhysicalDeviceDisplayPropertiesKHR;
        #endif
        #if defined(VK_KHR_external_fence_capabilities)
            vkGetPhysicalDeviceExternalFencePropertiesKHR = vkGetPhysicalDeviceExternalFencePropertiesKHR;
        #endif
        #if defined(VK_KHR_external_memory_capabilities)
            vkGetPhysicalDeviceExternalBufferPropertiesKHR = vkGetPhysicalDeviceExternalBufferPropertiesKHR;
        #endif
        #if defined(VK_KHR_external_semaphore_capabilities)
            vkGetPhysicalDeviceExternalSemaphorePropertiesKHR = vkGetPhysicalDeviceExternalSemaphorePropertiesKHR;
        #endif
        #if defined(VK_KHR_fragment_shading_rate)
            vkGetPhysicalDeviceFragmentShadingRatesKHR = vkGetPhysicalDeviceFragmentShadingRatesKHR;
        #endif
        #if defined(VK_KHR_get_display_properties2)
            vkGetDisplayModeProperties2KHR = vkGetDisplayModeProperties2KHR;
            vkGetDisplayPlaneCapabilities2KHR = vkGetDisplayPlaneCapabilities2KHR;
            vkGetPhysicalDeviceDisplayPlaneProperties2KHR = vkGetPhysicalDeviceDisplayPlaneProperties2KHR;
            vkGetPhysicalDeviceDisplayProperties2KHR = vkGetPhysicalDeviceDisplayProperties2KHR;
        #endif
        #if defined(VK_KHR_get_physical_device_properties2)
            vkGetPhysicalDeviceFeatures2KHR = vkGetPhysicalDeviceFeatures2KHR;
            vkGetPhysicalDeviceFormatProperties2KHR = vkGetPhysicalDeviceFormatProperties2KHR;
            vkGetPhysicalDeviceImageFormatProperties2KHR = vkGetPhysicalDeviceImageFormatProperties2KHR;
            vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;
            vkGetPhysicalDeviceProperties2KHR = vkGetPhysicalDeviceProperties2KHR;
            vkGetPhysicalDeviceQueueFamilyProperties2KHR = vkGetPhysicalDeviceQueueFamilyProperties2KHR;
            vkGetPhysicalDeviceSparseImageFormatProperties2KHR = vkGetPhysicalDeviceSparseImageFormatProperties2KHR;
        #endif
        #if defined(VK_KHR_get_surface_capabilities2)
            vkGetPhysicalDeviceSurfaceCapabilities2KHR = vkGetPhysicalDeviceSurfaceCapabilities2KHR;
            vkGetPhysicalDeviceSurfaceFormats2KHR = vkGetPhysicalDeviceSurfaceFormats2KHR;
        #endif
        #if defined(VK_KHR_performance_query)
            vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR = vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR;
            vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR = vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR;
        #endif
        #if defined(VK_KHR_surface)
            vkDestroySurfaceKHR = vkDestroySurfaceKHR;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR = vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
            vkGetPhysicalDeviceSurfaceFormatsKHR = vkGetPhysicalDeviceSurfaceFormatsKHR;
            vkGetPhysicalDeviceSurfacePresentModesKHR = vkGetPhysicalDeviceSurfacePresentModesKHR;
            vkGetPhysicalDeviceSurfaceSupportKHR = vkGetPhysicalDeviceSurfaceSupportKHR;
        #endif
        #if defined(VK_KHR_video_encode_queue)
            vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR = vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR;
        #endif
        #if defined(VK_KHR_video_queue)
            vkGetPhysicalDeviceVideoCapabilitiesKHR = vkGetPhysicalDeviceVideoCapabilitiesKHR;
            vkGetPhysicalDeviceVideoFormatPropertiesKHR = vkGetPhysicalDeviceVideoFormatPropertiesKHR;
        #endif
        #if defined(VK_KHR_wayland_surface)
            vkCreateWaylandSurfaceKHR = vkCreateWaylandSurfaceKHR;
            vkGetPhysicalDeviceWaylandPresentationSupportKHR = vkGetPhysicalDeviceWaylandPresentationSupportKHR;
        #endif
        #if defined(VK_KHR_win32_surface)
            vkCreateWin32SurfaceKHR = vkCreateWin32SurfaceKHR;
            vkGetPhysicalDeviceWin32PresentationSupportKHR = vkGetPhysicalDeviceWin32PresentationSupportKHR;
        #endif
        #if defined(VK_KHR_xcb_surface)
            vkCreateXcbSurfaceKHR = vkCreateXcbSurfaceKHR;
            vkGetPhysicalDeviceXcbPresentationSupportKHR = vkGetPhysicalDeviceXcbPresentationSupportKHR;
        #endif
        #if defined(VK_KHR_xlib_surface)
            vkCreateXlibSurfaceKHR = vkCreateXlibSurfaceKHR;
            vkGetPhysicalDeviceXlibPresentationSupportKHR = vkGetPhysicalDeviceXlibPresentationSupportKHR;
        #endif
        #if defined(VK_MVK_ios_surface)
            vkCreateIOSSurfaceMVK = vkCreateIOSSurfaceMVK;
        #endif
        #if defined(VK_MVK_macos_surface)
            vkCreateMacOSSurfaceMVK = vkCreateMacOSSurfaceMVK;
        #endif
        #if defined(VK_NN_vi_surface)
            vkCreateViSurfaceNN = vkCreateViSurfaceNN;
        #endif
        #if defined(VK_NV_acquire_winrt_display)
            vkAcquireWinrtDisplayNV = vkAcquireWinrtDisplayNV;
            vkGetWinrtDisplayNV = vkGetWinrtDisplayNV;
        #endif
        #if defined(VK_NV_cooperative_matrix)
            vkGetPhysicalDeviceCooperativeMatrixPropertiesNV = vkGetPhysicalDeviceCooperativeMatrixPropertiesNV;
        #endif
        #if defined(VK_NV_coverage_reduction_mode)
            vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV = vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV;
        #endif
        #if defined(VK_NV_external_memory_capabilities)
            vkGetPhysicalDeviceExternalImageFormatPropertiesNV = vkGetPhysicalDeviceExternalImageFormatPropertiesNV;
        #endif
        #if defined(VK_NV_optical_flow)
            vkGetPhysicalDeviceOpticalFlowImageFormatsNV = vkGetPhysicalDeviceOpticalFlowImageFormatsNV;
        #endif
        #if defined(VK_QNX_screen_surface)
            vkCreateScreenSurfaceQNX = vkCreateScreenSurfaceQNX;
            vkGetPhysicalDeviceScreenPresentationSupportQNX = vkGetPhysicalDeviceScreenPresentationSupportQNX;
        #endif
        #if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
            vkGetPhysicalDevicePresentRectanglesKHR = vkGetPhysicalDevicePresentRectanglesKHR;
        #endif
    }

    void VulkanLoader::LoadDevice(VkDevice vkDevice)
    {
        #if defined(VK_VERSION_1_0)
            vkAllocateCommandBuffers = vkAllocateCommandBuffers;
            vkAllocateDescriptorSets = vkAllocateDescriptorSets;
            vkAllocateMemory = vkAllocateMemory;
            vkBeginCommandBuffer = vkBeginCommandBuffer;
//            vkBindBufferMemory = vkBindBufferMemory;
//            vkBindImageMemory = vkBindImageMemory;
            vkCmdBeginQuery = vkCmdBeginQuery;
            vkCmdBeginRenderPass = vkCmdBeginRenderPass;
            vkCmdBindDescriptorSets = vkCmdBindDescriptorSets;
            vkCmdBindIndexBuffer = vkCmdBindIndexBuffer;
            vkCmdBindPipeline = vkCmdBindPipeline;
            vkCmdBindVertexBuffers = vkCmdBindVertexBuffers;
            vkCmdBlitImage = vkCmdBlitImage;
            vkCmdClearAttachments = vkCmdClearAttachments;
            vkCmdClearColorImage = vkCmdClearColorImage;
            vkCmdClearDepthStencilImage = vkCmdClearDepthStencilImage;
            vkCmdCopyBuffer = vkCmdCopyBuffer;
            vkCmdCopyBufferToImage = vkCmdCopyBufferToImage;
            vkCmdCopyImage = vkCmdCopyImage;
            vkCmdCopyImageToBuffer = vkCmdCopyImageToBuffer;
            vkCmdCopyQueryPoolResults = vkCmdCopyQueryPoolResults;
            vkCmdDispatch = vkCmdDispatch;
            vkCmdDispatchIndirect = vkCmdDispatchIndirect;
            vkCmdDraw = vkCmdDraw;
            vkCmdDrawIndexed = vkCmdDrawIndexed;
            vkCmdDrawIndexedIndirect = vkCmdDrawIndexedIndirect;
            vkCmdDrawIndirect = vkCmdDrawIndirect;
            vkCmdEndQuery = vkCmdEndQuery;
            vkCmdEndRenderPass = vkCmdEndRenderPass;
            vkCmdExecuteCommands = vkCmdExecuteCommands;
            vkCmdFillBuffer = vkCmdFillBuffer;
            vkCmdNextSubpass = vkCmdNextSubpass;
            vkCmdPipelineBarrier = vkCmdPipelineBarrier;
            vkCmdPushConstants = vkCmdPushConstants;
            vkCmdResetEvent = vkCmdResetEvent;
            vkCmdResetQueryPool = vkCmdResetQueryPool;
            vkCmdResolveImage = vkCmdResolveImage;
            vkCmdSetBlendConstants = vkCmdSetBlendConstants;
            vkCmdSetDepthBias = vkCmdSetDepthBias;
            vkCmdSetDepthBounds = vkCmdSetDepthBounds;
            vkCmdSetEvent = vkCmdSetEvent;
            vkCmdSetLineWidth = vkCmdSetLineWidth;
            vkCmdSetScissor = vkCmdSetScissor;
            vkCmdSetStencilCompareMask = vkCmdSetStencilCompareMask;
            vkCmdSetStencilReference = vkCmdSetStencilReference;
            vkCmdSetStencilWriteMask = vkCmdSetStencilWriteMask;
            vkCmdSetViewport = vkCmdSetViewport;
            vkCmdUpdateBuffer = vkCmdUpdateBuffer;
            vkCmdWaitEvents = vkCmdWaitEvents;
            vkCmdWriteTimestamp = vkCmdWriteTimestamp;
            vkCreateBuffer = vkCreateBuffer;
            vkCreateBufferView = vkCreateBufferView;
            vkCreateCommandPool = vkCreateCommandPool;
            vkCreateComputePipelines = vkCreateComputePipelines;
            vkCreateDescriptorPool = vkCreateDescriptorPool;
            vkCreateDescriptorSetLayout = vkCreateDescriptorSetLayout;
            vkCreateEvent = vkCreateEvent;
            vkCreateFence = vkCreateFence;
            vkCreateFramebuffer = vkCreateFramebuffer;
            vkCreateGraphicsPipelines = vkCreateGraphicsPipelines;
            vkCreateImage = vkCreateImage;
            vkCreateImageView = vkCreateImageView;
            vkCreatePipelineCache = vkCreatePipelineCache;
            vkCreatePipelineLayout = vkCreatePipelineLayout;
            vkCreateQueryPool = vkCreateQueryPool;
            vkCreateRenderPass = vkCreateRenderPass;
            vkCreateSampler = vkCreateSampler;
            vkCreateSemaphore = vkCreateSemaphore;
            vkCreateShaderModule = vkCreateShaderModule;
            vkDestroyBuffer = vkDestroyBuffer;
            vkDestroyBufferView = vkDestroyBufferView;
            vkDestroyCommandPool = vkDestroyCommandPool;
            vkDestroyDescriptorPool = vkDestroyDescriptorPool;
            vkDestroyDescriptorSetLayout = vkDestroyDescriptorSetLayout;
            vkDestroyDevice = vkDestroyDevice;
            vkDestroyEvent = vkDestroyEvent;
            vkDestroyFence = vkDestroyFence;
            vkDestroyFramebuffer = vkDestroyFramebuffer;
            vkDestroyImage = vkDestroyImage;
            vkDestroyImageView = vkDestroyImageView;
            vkDestroyPipeline = vkDestroyPipeline;
            vkDestroyPipelineCache = vkDestroyPipelineCache;
            vkDestroyPipelineLayout = vkDestroyPipelineLayout;
            vkDestroyQueryPool = vkDestroyQueryPool;
            vkDestroyRenderPass = vkDestroyRenderPass;
            vkDestroySampler = vkDestroySampler;
            vkDestroySemaphore = vkDestroySemaphore;
//            vkDestroyShaderModule = vkDestroyShaderModule;
//            vkDeviceWaitIdle = vkDeviceWaitIdle;
//            vkEndCommandBuffer = vkEndCommandBuffer;
//            vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
            vkFreeCommandBuffers = vkFreeCommandBuffers;
            vkFreeDescriptorSets = vkFreeDescriptorSets;
            vkFreeMemory = vkFreeMemory;
//            vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
            vkGetDeviceMemoryCommitment = vkGetDeviceMemoryCommitment;
            vkGetDeviceQueue = vkGetDeviceQueue;
            vkGetEventStatus = vkGetEventStatus;
            vkGetFenceStatus = vkGetFenceStatus;
//            vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
            vkGetImageSparseMemoryRequirements = vkGetImageSparseMemoryRequirements;
            vkGetImageSubresourceLayout = vkGetImageSubresourceLayout;
            vkGetPipelineCacheData = vkGetPipelineCacheData;
            vkGetQueryPoolResults = vkGetQueryPoolResults;
            vkGetRenderAreaGranularity = vkGetRenderAreaGranularity;
//            vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
            vkMapMemory = vkMapMemory;
            vkMergePipelineCaches = vkMergePipelineCaches;
            vkQueueBindSparse = vkQueueBindSparse;
            vkQueueSubmit = vkQueueSubmit;
            vkQueueWaitIdle = vkQueueWaitIdle;
            vkResetCommandBuffer = vkResetCommandBuffer;
            vkResetCommandPool = vkResetCommandPool;
            vkResetDescriptorPool = vkResetDescriptorPool;
            vkResetEvent = vkResetEvent;
            vkResetFences = vkResetFences;
            vkSetEvent = vkSetEvent;
            vkUnmapMemory = vkUnmapMemory;
            vkUpdateDescriptorSets = vkUpdateDescriptorSets;
            vkWaitForFences = vkWaitForFences;
        #endif 
        #if defined(VK_VERSION_1_1)
            vkBindBufferMemory2 = vkBindBufferMemory2;
            vkBindImageMemory2 = vkBindImageMemory2;
            vkCmdDispatchBase = vkCmdDispatchBase;
            vkCmdSetDeviceMask = vkCmdSetDeviceMask;
            vkCreateDescriptorUpdateTemplate = vkCreateDescriptorUpdateTemplate;
            vkCreateSamplerYcbcrConversion = vkCreateSamplerYcbcrConversion;
            vkDestroyDescriptorUpdateTemplate = vkDestroyDescriptorUpdateTemplate;
            vkDestroySamplerYcbcrConversion = vkDestroySamplerYcbcrConversion;
            vkGetBufferMemoryRequirements2 = vkGetBufferMemoryRequirements2;
            vkGetDescriptorSetLayoutSupport = vkGetDescriptorSetLayoutSupport;
            vkGetDeviceGroupPeerMemoryFeatures = vkGetDeviceGroupPeerMemoryFeatures;
            vkGetDeviceQueue2 = vkGetDeviceQueue2;
            vkGetImageMemoryRequirements2 = vkGetImageMemoryRequirements2;
            vkGetImageSparseMemoryRequirements2 = vkGetImageSparseMemoryRequirements2;
            vkTrimCommandPool = vkTrimCommandPool;
            vkUpdateDescriptorSetWithTemplate = vkUpdateDescriptorSetWithTemplate;
        #endif 
        #if defined(VK_VERSION_1_2)
            vkCmdBeginRenderPass2 = vkCmdBeginRenderPass2;
            vkCmdDrawIndexedIndirectCount = vkCmdDrawIndexedIndirectCount;
            vkCmdDrawIndirectCount = vkCmdDrawIndirectCount;
            vkCmdEndRenderPass2 = vkCmdEndRenderPass2;
            vkCmdNextSubpass2 = vkCmdNextSubpass2;
            vkCreateRenderPass2 = vkCreateRenderPass2;
            vkGetBufferDeviceAddress = vkGetBufferDeviceAddress;
            vkGetBufferOpaqueCaptureAddress = vkGetBufferOpaqueCaptureAddress;
            vkGetDeviceMemoryOpaqueCaptureAddress = vkGetDeviceMemoryOpaqueCaptureAddress;
            vkGetSemaphoreCounterValue = vkGetSemaphoreCounterValue;
            vkResetQueryPool = vkResetQueryPool;
            vkSignalSemaphore = vkSignalSemaphore;
            vkWaitSemaphores = vkWaitSemaphores;
        #endif 
        #if defined(VK_VERSION_1_3)
            vkCmdBeginRendering = vkCmdBeginRendering;
            vkCmdBindVertexBuffers2 = vkCmdBindVertexBuffers2;
            vkCmdBlitImage2 = vkCmdBlitImage2;
            vkCmdCopyBuffer2 = vkCmdCopyBuffer2;
            vkCmdCopyBufferToImage2 = vkCmdCopyBufferToImage2;
            vkCmdCopyImage2 = vkCmdCopyImage2;
            vkCmdCopyImageToBuffer2 = vkCmdCopyImageToBuffer2;
            vkCmdEndRendering = vkCmdEndRendering;
            vkCmdPipelineBarrier2 = vkCmdPipelineBarrier2;
            vkCmdResetEvent2 = vkCmdResetEvent2;
            vkCmdResolveImage2 = vkCmdResolveImage2;
            vkCmdSetCullMode = vkCmdSetCullMode;
            vkCmdSetDepthBiasEnable = vkCmdSetDepthBiasEnable;
            vkCmdSetDepthBoundsTestEnable = vkCmdSetDepthBoundsTestEnable;
            vkCmdSetDepthCompareOp = vkCmdSetDepthCompareOp;
            vkCmdSetDepthTestEnable = vkCmdSetDepthTestEnable;
            vkCmdSetDepthWriteEnable = vkCmdSetDepthWriteEnable;
            vkCmdSetEvent2 = vkCmdSetEvent2;
            vkCmdSetFrontFace = vkCmdSetFrontFace;
            vkCmdSetPrimitiveRestartEnable = vkCmdSetPrimitiveRestartEnable;
            vkCmdSetPrimitiveTopology = vkCmdSetPrimitiveTopology;
            vkCmdSetRasterizerDiscardEnable = vkCmdSetRasterizerDiscardEnable;
            vkCmdSetScissorWithCount = vkCmdSetScissorWithCount;
            vkCmdSetStencilOp = vkCmdSetStencilOp;
            vkCmdSetStencilTestEnable = vkCmdSetStencilTestEnable;
            vkCmdSetViewportWithCount = vkCmdSetViewportWithCount;
            vkCmdWaitEvents2 = vkCmdWaitEvents2;
            vkCmdWriteTimestamp2 = vkCmdWriteTimestamp2;
            vkCreatePrivateDataSlot = vkCreatePrivateDataSlot;
            vkDestroyPrivateDataSlot = vkDestroyPrivateDataSlot;
//            vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements;
//            vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements;
            vkGetDeviceImageSparseMemoryRequirements = vkGetDeviceImageSparseMemoryRequirements;
            vkGetPrivateData = vkGetPrivateData;
            vkQueueSubmit2 = vkQueueSubmit2;
            vkSetPrivateData = vkSetPrivateData;
        #endif 
        #if defined(VK_AMDX_shader_enqueue)
            vkCmdDispatchGraphAMDX = vkCmdDispatchGraphAMDX;
            vkCmdDispatchGraphIndirectAMDX = vkCmdDispatchGraphIndirectAMDX;
            vkCmdDispatchGraphIndirectCountAMDX = vkCmdDispatchGraphIndirectCountAMDX;
            vkCmdInitializeGraphScratchMemoryAMDX = vkCmdInitializeGraphScratchMemoryAMDX;
            vkCreateExecutionGraphPipelinesAMDX = vkCreateExecutionGraphPipelinesAMDX;
            vkGetExecutionGraphPipelineNodeIndexAMDX = vkGetExecutionGraphPipelineNodeIndexAMDX;
            vkGetExecutionGraphPipelineScratchSizeAMDX = vkGetExecutionGraphPipelineScratchSizeAMDX;
        #endif 
        #if defined(VK_AMD_buffer_marker)
            vkCmdWriteBufferMarkerAMD = vkCmdWriteBufferMarkerAMD;
        #endif 
        #if defined(VK_AMD_display_native_hdr)
            vkSetLocalDimmingAMD = vkSetLocalDimmingAMD;
        #endif 
        #if defined(VK_AMD_draw_indirect_count)
            vkCmdDrawIndexedIndirectCountAMD = vkCmdDrawIndexedIndirectCountAMD;
            vkCmdDrawIndirectCountAMD = vkCmdDrawIndirectCountAMD;
        #endif 
        #if defined(VK_AMD_shader_info)
            vkGetShaderInfoAMD = vkGetShaderInfoAMD;
        #endif 
        #if defined(VK_ANDROID_external_memory_android_hardware_buffer)
            vkGetAndroidHardwareBufferPropertiesANDROID = vkGetAndroidHardwareBufferPropertiesANDROID;
            vkGetMemoryAndroidHardwareBufferANDROID = vkGetMemoryAndroidHardwareBufferANDROID;
        #endif 
        #if defined(VK_EXT_attachment_feedback_loop_dynamic_state)
            vkCmdSetAttachmentFeedbackLoopEnableEXT = vkCmdSetAttachmentFeedbackLoopEnableEXT;
        #endif 
        #if defined(VK_EXT_buffer_device_address)
            vkGetBufferDeviceAddressEXT = vkGetBufferDeviceAddressEXT;
        #endif 
        #if defined(VK_EXT_calibrated_timestamps)
            vkGetCalibratedTimestampsEXT = vkGetCalibratedTimestampsEXT;
        #endif 
        #if defined(VK_EXT_color_write_enable)
            vkCmdSetColorWriteEnableEXT = vkCmdSetColorWriteEnableEXT;
        #endif 
        #if defined(VK_EXT_conditional_rendering)
            vkCmdBeginConditionalRenderingEXT = vkCmdBeginConditionalRenderingEXT;
            vkCmdEndConditionalRenderingEXT = vkCmdEndConditionalRenderingEXT;
        #endif 
        #if defined(VK_EXT_debug_marker)
            vkCmdDebugMarkerBeginEXT = vkCmdDebugMarkerBeginEXT;
            vkCmdDebugMarkerEndEXT = vkCmdDebugMarkerEndEXT;
            vkCmdDebugMarkerInsertEXT = vkCmdDebugMarkerInsertEXT;
            vkDebugMarkerSetObjectNameEXT = vkDebugMarkerSetObjectNameEXT;
            vkDebugMarkerSetObjectTagEXT = vkDebugMarkerSetObjectTagEXT;
        #endif 
        #if defined(VK_EXT_depth_bias_control)
            vkCmdSetDepthBias2EXT = vkCmdSetDepthBias2EXT;
        #endif 
        #if defined(VK_EXT_descriptor_buffer)
            vkCmdBindDescriptorBufferEmbeddedSamplersEXT = vkCmdBindDescriptorBufferEmbeddedSamplersEXT;
            vkCmdBindDescriptorBuffersEXT = vkCmdBindDescriptorBuffersEXT;
            vkCmdSetDescriptorBufferOffsetsEXT = vkCmdSetDescriptorBufferOffsetsEXT;
            vkGetBufferOpaqueCaptureDescriptorDataEXT = vkGetBufferOpaqueCaptureDescriptorDataEXT;
            vkGetDescriptorEXT = vkGetDescriptorEXT;
            vkGetDescriptorSetLayoutBindingOffsetEXT = vkGetDescriptorSetLayoutBindingOffsetEXT;
            vkGetDescriptorSetLayoutSizeEXT = vkGetDescriptorSetLayoutSizeEXT;
            vkGetImageOpaqueCaptureDescriptorDataEXT = vkGetImageOpaqueCaptureDescriptorDataEXT;
            vkGetImageViewOpaqueCaptureDescriptorDataEXT = vkGetImageViewOpaqueCaptureDescriptorDataEXT;
            vkGetSamplerOpaqueCaptureDescriptorDataEXT = vkGetSamplerOpaqueCaptureDescriptorDataEXT;
        #endif 
        #if defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing))
            vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT = vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT;
        #endif 
        #if defined(VK_EXT_device_fault)
            vkGetDeviceFaultInfoEXT = vkGetDeviceFaultInfoEXT;
        #endif 
        #if defined(VK_EXT_discard_rectangles)
            vkCmdSetDiscardRectangleEXT = vkCmdSetDiscardRectangleEXT;
        #endif 
        #if defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2
            vkCmdSetDiscardRectangleEnableEXT = vkCmdSetDiscardRectangleEnableEXT;
            vkCmdSetDiscardRectangleModeEXT = vkCmdSetDiscardRectangleModeEXT;
        #endif 
        #if defined(VK_EXT_display_control)
            vkDisplayPowerControlEXT = vkDisplayPowerControlEXT;
            vkGetSwapchainCounterEXT = vkGetSwapchainCounterEXT;
            vkRegisterDeviceEventEXT = vkRegisterDeviceEventEXT;
            vkRegisterDisplayEventEXT = vkRegisterDisplayEventEXT;
        #endif 
        #if defined(VK_EXT_external_memory_host)
            vkGetMemoryHostPointerPropertiesEXT = vkGetMemoryHostPointerPropertiesEXT;
        #endif 
        #if defined(VK_EXT_full_screen_exclusive)
            vkAcquireFullScreenExclusiveModeEXT = vkAcquireFullScreenExclusiveModeEXT;
            vkReleaseFullScreenExclusiveModeEXT = vkReleaseFullScreenExclusiveModeEXT;
        #endif 
        #if defined(VK_EXT_hdr_metadata)
            vkSetHdrMetadataEXT = vkSetHdrMetadataEXT;
        #endif 
        #if defined(VK_EXT_host_image_copy)
            vkCopyImageToImageEXT = vkCopyImageToImageEXT;
            vkCopyImageToMemoryEXT = vkCopyImageToMemoryEXT;
            vkCopyMemoryToImageEXT = vkCopyMemoryToImageEXT;
            vkTransitionImageLayoutEXT = vkTransitionImageLayoutEXT;
        #endif 
        #if defined(VK_EXT_host_query_reset)
            vkResetQueryPoolEXT = vkResetQueryPoolEXT;
        #endif 
        #if defined(VK_EXT_image_drm_format_modifier)
            vkGetImageDrmFormatModifierPropertiesEXT = vkGetImageDrmFormatModifierPropertiesEXT;
        #endif 
        #if defined(VK_EXT_line_rasterization)
            vkCmdSetLineStippleEXT = vkCmdSetLineStippleEXT;
        #endif 
        #if defined(VK_EXT_mesh_shader)
            vkCmdDrawMeshTasksEXT = vkCmdDrawMeshTasksEXT;
            vkCmdDrawMeshTasksIndirectCountEXT = vkCmdDrawMeshTasksIndirectCountEXT;
            vkCmdDrawMeshTasksIndirectEXT = vkCmdDrawMeshTasksIndirectEXT;
        #endif 
        #if defined(VK_EXT_metal_objects)
            vkExportMetalObjectsEXT = vkExportMetalObjectsEXT;
        #endif 
        #if defined(VK_EXT_multi_draw)
            vkCmdDrawMultiEXT = vkCmdDrawMultiEXT;
            vkCmdDrawMultiIndexedEXT = vkCmdDrawMultiIndexedEXT;
        #endif 
        #if defined(VK_EXT_opacity_micromap)
            vkBuildMicromapsEXT = vkBuildMicromapsEXT;
            vkCmdBuildMicromapsEXT = vkCmdBuildMicromapsEXT;
            vkCmdCopyMemoryToMicromapEXT = vkCmdCopyMemoryToMicromapEXT;
            vkCmdCopyMicromapEXT = vkCmdCopyMicromapEXT;
            vkCmdCopyMicromapToMemoryEXT = vkCmdCopyMicromapToMemoryEXT;
            vkCmdWriteMicromapsPropertiesEXT = vkCmdWriteMicromapsPropertiesEXT;
            vkCopyMemoryToMicromapEXT = vkCopyMemoryToMicromapEXT;
            vkCopyMicromapEXT = vkCopyMicromapEXT;
            vkCopyMicromapToMemoryEXT = vkCopyMicromapToMemoryEXT;
            vkCreateMicromapEXT = vkCreateMicromapEXT;
            vkDestroyMicromapEXT = vkDestroyMicromapEXT;
            vkGetDeviceMicromapCompatibilityEXT = vkGetDeviceMicromapCompatibilityEXT;
            vkGetMicromapBuildSizesEXT = vkGetMicromapBuildSizesEXT;
            vkWriteMicromapsPropertiesEXT = vkWriteMicromapsPropertiesEXT;
        #endif 
        #if defined(VK_EXT_pageable_device_local_memory)
            vkSetDeviceMemoryPriorityEXT = vkSetDeviceMemoryPriorityEXT;
        #endif 
        #if defined(VK_EXT_pipeline_properties)
            vkGetPipelinePropertiesEXT = vkGetPipelinePropertiesEXT;
        #endif 
        #if defined(VK_EXT_private_data)
            vkCreatePrivateDataSlotEXT = vkCreatePrivateDataSlotEXT;
            vkDestroyPrivateDataSlotEXT = vkDestroyPrivateDataSlotEXT;
            vkGetPrivateDataEXT = vkGetPrivateDataEXT;
            vkSetPrivateDataEXT = vkSetPrivateDataEXT;
        #endif 
        #if defined(VK_EXT_sample_locations)
            vkCmdSetSampleLocationsEXT = vkCmdSetSampleLocationsEXT;
        #endif 
        #if defined(VK_EXT_shader_module_identifier)
            vkGetShaderModuleCreateInfoIdentifierEXT = vkGetShaderModuleCreateInfoIdentifierEXT;
            vkGetShaderModuleIdentifierEXT = vkGetShaderModuleIdentifierEXT;
        #endif 
        #if defined(VK_EXT_shader_object)
            vkCmdBindShadersEXT = vkCmdBindShadersEXT;
            vkCreateShadersEXT = vkCreateShadersEXT;
            vkDestroyShaderEXT = vkDestroyShaderEXT;
            vkGetShaderBinaryDataEXT = vkGetShaderBinaryDataEXT;
        #endif 
        #if defined(VK_EXT_swapchain_maintenance1)
            vkReleaseSwapchainImagesEXT = vkReleaseSwapchainImagesEXT;
        #endif 
        #if defined(VK_EXT_transform_feedback)
            vkCmdBeginQueryIndexedEXT = vkCmdBeginQueryIndexedEXT;
            vkCmdBeginTransformFeedbackEXT = vkCmdBeginTransformFeedbackEXT;
            vkCmdBindTransformFeedbackBuffersEXT = vkCmdBindTransformFeedbackBuffersEXT;
            vkCmdDrawIndirectByteCountEXT = vkCmdDrawIndirectByteCountEXT;
            vkCmdEndQueryIndexedEXT = vkCmdEndQueryIndexedEXT;
            vkCmdEndTransformFeedbackEXT = vkCmdEndTransformFeedbackEXT;
        #endif 
        #if defined(VK_EXT_validation_cache)
            vkCreateValidationCacheEXT = vkCreateValidationCacheEXT;
            vkDestroyValidationCacheEXT = vkDestroyValidationCacheEXT;
            vkGetValidationCacheDataEXT = vkGetValidationCacheDataEXT;
            vkMergeValidationCachesEXT = vkMergeValidationCachesEXT;
        #endif 
        #if defined(VK_FUCHSIA_buffer_collection)
            vkCreateBufferCollectionFUCHSIA = vkCreateBufferCollectionFUCHSIA;
            vkDestroyBufferCollectionFUCHSIA = vkDestroyBufferCollectionFUCHSIA;
            vkGetBufferCollectionPropertiesFUCHSIA = vkGetBufferCollectionPropertiesFUCHSIA;
            vkSetBufferCollectionBufferConstraintsFUCHSIA = vkSetBufferCollectionBufferConstraintsFUCHSIA;
            vkSetBufferCollectionImageConstraintsFUCHSIA = vkSetBufferCollectionImageConstraintsFUCHSIA;
        #endif 
        #if defined(VK_FUCHSIA_external_memory)
            vkGetMemoryZirconHandleFUCHSIA = vkGetMemoryZirconHandleFUCHSIA;
            vkGetMemoryZirconHandlePropertiesFUCHSIA = vkGetMemoryZirconHandlePropertiesFUCHSIA;
        #endif 
        #if defined(VK_FUCHSIA_external_semaphore)
            vkGetSemaphoreZirconHandleFUCHSIA = vkGetSemaphoreZirconHandleFUCHSIA;
            vkImportSemaphoreZirconHandleFUCHSIA = vkImportSemaphoreZirconHandleFUCHSIA;
        #endif 
        #if defined(VK_GOOGLE_display_timing)
            vkGetPastPresentationTimingGOOGLE = vkGetPastPresentationTimingGOOGLE;
            vkGetRefreshCycleDurationGOOGLE = vkGetRefreshCycleDurationGOOGLE;
        #endif 
        #if defined(VK_HUAWEI_cluster_culling_shader)
            vkCmdDrawClusterHUAWEI = vkCmdDrawClusterHUAWEI;
            vkCmdDrawClusterIndirectHUAWEI = vkCmdDrawClusterIndirectHUAWEI;
        #endif 
        #if defined(VK_HUAWEI_invocation_mask)
            vkCmdBindInvocationMaskHUAWEI = vkCmdBindInvocationMaskHUAWEI;
        #endif 
        #if defined(VK_HUAWEI_subpass_shading)
            vkCmdSubpassShadingHUAWEI = vkCmdSubpassShadingHUAWEI;
            vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI = vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI;
        #endif 
        #if defined(VK_INTEL_performance_query)
            vkAcquirePerformanceConfigurationINTEL = vkAcquirePerformanceConfigurationINTEL;
            vkCmdSetPerformanceMarkerINTEL = vkCmdSetPerformanceMarkerINTEL;
            vkCmdSetPerformanceOverrideINTEL = vkCmdSetPerformanceOverrideINTEL;
            vkCmdSetPerformanceStreamMarkerINTEL = vkCmdSetPerformanceStreamMarkerINTEL;
            vkGetPerformanceParameterINTEL = vkGetPerformanceParameterINTEL;
            vkInitializePerformanceApiINTEL = vkInitializePerformanceApiINTEL;
            vkQueueSetPerformanceConfigurationINTEL = vkQueueSetPerformanceConfigurationINTEL;
            vkReleasePerformanceConfigurationINTEL = vkReleasePerformanceConfigurationINTEL;
            vkUninitializePerformanceApiINTEL = vkUninitializePerformanceApiINTEL;
        #endif 
        #if defined(VK_KHR_acceleration_structure)
            vkBuildAccelerationStructuresKHR = vkBuildAccelerationStructuresKHR;
            vkCmdBuildAccelerationStructuresIndirectKHR = vkCmdBuildAccelerationStructuresIndirectKHR;
            vkCmdBuildAccelerationStructuresKHR = vkCmdBuildAccelerationStructuresKHR;
            vkCmdCopyAccelerationStructureKHR = vkCmdCopyAccelerationStructureKHR;
            vkCmdCopyAccelerationStructureToMemoryKHR = vkCmdCopyAccelerationStructureToMemoryKHR;
            vkCmdCopyMemoryToAccelerationStructureKHR = vkCmdCopyMemoryToAccelerationStructureKHR;
            vkCmdWriteAccelerationStructuresPropertiesKHR = vkCmdWriteAccelerationStructuresPropertiesKHR;
            vkCopyAccelerationStructureKHR = vkCopyAccelerationStructureKHR;
            vkCopyAccelerationStructureToMemoryKHR = vkCopyAccelerationStructureToMemoryKHR;
            vkCopyMemoryToAccelerationStructureKHR = vkCopyMemoryToAccelerationStructureKHR;
            vkCreateAccelerationStructureKHR = vkCreateAccelerationStructureKHR;
            vkDestroyAccelerationStructureKHR = vkDestroyAccelerationStructureKHR;
            vkGetAccelerationStructureBuildSizesKHR = vkGetAccelerationStructureBuildSizesKHR;
            vkGetAccelerationStructureDeviceAddressKHR = vkGetAccelerationStructureDeviceAddressKHR;
            vkGetDeviceAccelerationStructureCompatibilityKHR = vkGetDeviceAccelerationStructureCompatibilityKHR;
            vkWriteAccelerationStructuresPropertiesKHR = vkWriteAccelerationStructuresPropertiesKHR;
        #endif 
        #if defined(VK_KHR_bind_memory2)
            vkBindBufferMemory2KHR = vkBindBufferMemory2KHR;
            vkBindImageMemory2KHR = vkBindImageMemory2KHR;
        #endif 
        #if defined(VK_KHR_buffer_device_address)
            vkGetBufferDeviceAddressKHR = vkGetBufferDeviceAddressKHR;
            vkGetBufferOpaqueCaptureAddressKHR = vkGetBufferOpaqueCaptureAddressKHR;
            vkGetDeviceMemoryOpaqueCaptureAddressKHR = vkGetDeviceMemoryOpaqueCaptureAddressKHR;
        #endif 
        #if defined(VK_KHR_copy_commands2)
            vkCmdBlitImage2KHR = vkCmdBlitImage2KHR;
            vkCmdCopyBuffer2KHR = vkCmdCopyBuffer2KHR;
            vkCmdCopyBufferToImage2KHR = vkCmdCopyBufferToImage2KHR;
            vkCmdCopyImage2KHR = vkCmdCopyImage2KHR;
            vkCmdCopyImageToBuffer2KHR = vkCmdCopyImageToBuffer2KHR;
            vkCmdResolveImage2KHR = vkCmdResolveImage2KHR;
        #endif 
        #if defined(VK_KHR_create_renderpass2)
            vkCmdBeginRenderPass2KHR = vkCmdBeginRenderPass2KHR;
            vkCmdEndRenderPass2KHR = vkCmdEndRenderPass2KHR;
            vkCmdNextSubpass2KHR = vkCmdNextSubpass2KHR;
            vkCreateRenderPass2KHR = vkCreateRenderPass2KHR;
        #endif 
        #if defined(VK_KHR_deferred_host_operations)
            vkCreateDeferredOperationKHR = vkCreateDeferredOperationKHR;
            vkDeferredOperationJoinKHR = vkDeferredOperationJoinKHR;
            vkDestroyDeferredOperationKHR = vkDestroyDeferredOperationKHR;
            vkGetDeferredOperationMaxConcurrencyKHR = vkGetDeferredOperationMaxConcurrencyKHR;
            vkGetDeferredOperationResultKHR = vkGetDeferredOperationResultKHR;
        #endif 
        #if defined(VK_KHR_descriptor_update_template)
            vkCreateDescriptorUpdateTemplateKHR = vkCreateDescriptorUpdateTemplateKHR;
            vkDestroyDescriptorUpdateTemplateKHR = vkDestroyDescriptorUpdateTemplateKHR;
            vkUpdateDescriptorSetWithTemplateKHR = vkUpdateDescriptorSetWithTemplateKHR;
        #endif 
        #if defined(VK_KHR_device_group)
            vkCmdDispatchBaseKHR = vkCmdDispatchBaseKHR;
            vkCmdSetDeviceMaskKHR = vkCmdSetDeviceMaskKHR;
            vkGetDeviceGroupPeerMemoryFeaturesKHR = vkGetDeviceGroupPeerMemoryFeaturesKHR;
        #endif 
        #if defined(VK_KHR_display_swapchain)
            vkCreateSharedSwapchainsKHR = vkCreateSharedSwapchainsKHR;
        #endif 
        #if defined(VK_KHR_draw_indirect_count)
            vkCmdDrawIndexedIndirectCountKHR = vkCmdDrawIndexedIndirectCountKHR;
            vkCmdDrawIndirectCountKHR = vkCmdDrawIndirectCountKHR;
        #endif 
        #if defined(VK_KHR_dynamic_rendering)
            vkCmdBeginRenderingKHR = vkCmdBeginRenderingKHR;
            vkCmdEndRenderingKHR = vkCmdEndRenderingKHR;
        #endif 
        #if defined(VK_KHR_external_fence_fd)
            vkGetFenceFdKHR = vkGetFenceFdKHR;
            vkImportFenceFdKHR = vkImportFenceFdKHR;
        #endif 
        #if defined(VK_KHR_external_fence_win32)
            vkGetFenceWin32HandleKHR = vkGetFenceWin32HandleKHR;
            vkImportFenceWin32HandleKHR = vkImportFenceWin32HandleKHR;
        #endif 
        #if defined(VK_KHR_external_memory_fd)
            vkGetMemoryFdKHR = vkGetMemoryFdKHR;
            vkGetMemoryFdPropertiesKHR = vkGetMemoryFdPropertiesKHR;
        #endif 
        #if defined(VK_KHR_external_memory_win32)
            vkGetMemoryWin32HandleKHR = vkGetMemoryWin32HandleKHR;
            vkGetMemoryWin32HandlePropertiesKHR = vkGetMemoryWin32HandlePropertiesKHR;
        #endif 
        #if defined(VK_KHR_external_semaphore_fd)
            vkGetSemaphoreFdKHR = vkGetSemaphoreFdKHR;
            vkImportSemaphoreFdKHR = vkImportSemaphoreFdKHR;
        #endif 
        #if defined(VK_KHR_external_semaphore_win32)
            vkGetSemaphoreWin32HandleKHR = vkGetSemaphoreWin32HandleKHR;
            vkImportSemaphoreWin32HandleKHR = vkImportSemaphoreWin32HandleKHR;
        #endif 
        #if defined(VK_KHR_fragment_shading_rate)
            vkCmdSetFragmentShadingRateKHR = vkCmdSetFragmentShadingRateKHR;
        #endif 
        #if defined(VK_KHR_get_memory_requirements2)
            vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
            vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
            vkGetImageSparseMemoryRequirements2KHR = vkGetImageSparseMemoryRequirements2KHR;
        #endif 
        #if defined(VK_KHR_maintenance1)
            vkTrimCommandPoolKHR = vkTrimCommandPoolKHR;
        #endif 
        #if defined(VK_KHR_maintenance3)
            vkGetDescriptorSetLayoutSupportKHR = vkGetDescriptorSetLayoutSupportKHR;
        #endif 
        #if defined(VK_KHR_maintenance4)
            vkGetDeviceBufferMemoryRequirementsKHR = vkGetDeviceBufferMemoryRequirementsKHR;
            vkGetDeviceImageMemoryRequirementsKHR = vkGetDeviceImageMemoryRequirementsKHR;
            vkGetDeviceImageSparseMemoryRequirementsKHR = vkGetDeviceImageSparseMemoryRequirementsKHR;
        #endif 
        #if defined(VK_KHR_maintenance5)
            vkCmdBindIndexBuffer2KHR = vkCmdBindIndexBuffer2KHR;
            vkGetDeviceImageSubresourceLayoutKHR = vkGetDeviceImageSubresourceLayoutKHR;
            vkGetImageSubresourceLayout2KHR = vkGetImageSubresourceLayout2KHR;
            vkGetRenderingAreaGranularityKHR = vkGetRenderingAreaGranularityKHR;
        #endif 
        #if defined(VK_KHR_map_memory2)
            vkMapMemory2KHR = vkMapMemory2KHR;
            vkUnmapMemory2KHR = vkUnmapMemory2KHR;
        #endif 
        #if defined(VK_KHR_performance_query)
            vkAcquireProfilingLockKHR = vkAcquireProfilingLockKHR;
            vkReleaseProfilingLockKHR = vkReleaseProfilingLockKHR;
        #endif 
        #if defined(VK_KHR_pipeline_executable_properties)
            vkGetPipelineExecutableInternalRepresentationsKHR = vkGetPipelineExecutableInternalRepresentationsKHR;
            vkGetPipelineExecutablePropertiesKHR = vkGetPipelineExecutablePropertiesKHR;
            vkGetPipelineExecutableStatisticsKHR = vkGetPipelineExecutableStatisticsKHR;
        #endif 
        #if defined(VK_KHR_present_wait)
            vkWaitForPresentKHR = vkWaitForPresentKHR;
        #endif 
        #if defined(VK_KHR_push_descriptor)
            vkCmdPushDescriptorSetKHR = vkCmdPushDescriptorSetKHR;
        #endif 
        #if defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline)
            vkCmdTraceRaysIndirect2KHR = vkCmdTraceRaysIndirect2KHR;
        #endif 
        #if defined(VK_KHR_ray_tracing_pipeline)
            vkCmdSetRayTracingPipelineStackSizeKHR = vkCmdSetRayTracingPipelineStackSizeKHR;
            vkCmdTraceRaysIndirectKHR = vkCmdTraceRaysIndirectKHR;
            vkCmdTraceRaysKHR = vkCmdTraceRaysKHR;
            vkCreateRayTracingPipelinesKHR = vkCreateRayTracingPipelinesKHR;
            vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = vkGetRayTracingCaptureReplayShaderGroupHandlesKHR;
            vkGetRayTracingShaderGroupHandlesKHR = vkGetRayTracingShaderGroupHandlesKHR;
            vkGetRayTracingShaderGroupStackSizeKHR = vkGetRayTracingShaderGroupStackSizeKHR;
        #endif 
        #if defined(VK_KHR_sampler_ycbcr_conversion)
            vkCreateSamplerYcbcrConversionKHR = vkCreateSamplerYcbcrConversionKHR;
            vkDestroySamplerYcbcrConversionKHR = vkDestroySamplerYcbcrConversionKHR;
        #endif 
        #if defined(VK_KHR_shared_presentable_image)
            vkGetSwapchainStatusKHR = vkGetSwapchainStatusKHR;
        #endif 
        #if defined(VK_KHR_swapchain)
            vkAcquireNextImageKHR = vkAcquireNextImageKHR;
            vkCreateSwapchainKHR = vkCreateSwapchainKHR;
            vkDestroySwapchainKHR = vkDestroySwapchainKHR;
            vkGetSwapchainImagesKHR = vkGetSwapchainImagesKHR;
            vkQueuePresentKHR = vkQueuePresentKHR;
        #endif 
        #if defined(VK_KHR_synchronization2)
            vkCmdPipelineBarrier2KHR = vkCmdPipelineBarrier2KHR;
            vkCmdResetEvent2KHR = vkCmdResetEvent2KHR;
            vkCmdSetEvent2KHR = vkCmdSetEvent2KHR;
            vkCmdWaitEvents2KHR = vkCmdWaitEvents2KHR;
            vkCmdWriteTimestamp2KHR = vkCmdWriteTimestamp2KHR;
            vkQueueSubmit2KHR = vkQueueSubmit2KHR;
        #endif 
        #if defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker)
            vkCmdWriteBufferMarker2AMD = vkCmdWriteBufferMarker2AMD;
        #endif 
        #if defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints)
            vkGetQueueCheckpointData2NV = vkGetQueueCheckpointData2NV;
        #endif 
        #if defined(VK_KHR_timeline_semaphore)
            vkGetSemaphoreCounterValueKHR = vkGetSemaphoreCounterValueKHR;
            vkSignalSemaphoreKHR = vkSignalSemaphoreKHR;
            vkWaitSemaphoresKHR = vkWaitSemaphoresKHR;
        #endif 
        #if defined(VK_KHR_video_decode_queue)
            vkCmdDecodeVideoKHR = vkCmdDecodeVideoKHR;
        #endif 
        #if defined(VK_KHR_video_encode_queue)
            vkCmdEncodeVideoKHR = vkCmdEncodeVideoKHR;
            vkGetEncodedVideoSessionParametersKHR = vkGetEncodedVideoSessionParametersKHR;
        #endif 
        #if defined(VK_KHR_video_queue)
            vkBindVideoSessionMemoryKHR = vkBindVideoSessionMemoryKHR;
            vkCmdBeginVideoCodingKHR = vkCmdBeginVideoCodingKHR;
            vkCmdControlVideoCodingKHR = vkCmdControlVideoCodingKHR;
            vkCmdEndVideoCodingKHR = vkCmdEndVideoCodingKHR;
            vkCreateVideoSessionKHR = vkCreateVideoSessionKHR;
            vkCreateVideoSessionParametersKHR = vkCreateVideoSessionParametersKHR;
            vkDestroyVideoSessionKHR = vkDestroyVideoSessionKHR;
            vkDestroyVideoSessionParametersKHR = vkDestroyVideoSessionParametersKHR;
            vkGetVideoSessionMemoryRequirementsKHR = vkGetVideoSessionMemoryRequirementsKHR;
            vkUpdateVideoSessionParametersKHR = vkUpdateVideoSessionParametersKHR;
        #endif 
        #if defined(VK_NVX_binary_import)
            vkCmdCuLaunchKernelNVX = vkCmdCuLaunchKernelNVX;
            vkCreateCuFunctionNVX = vkCreateCuFunctionNVX;
            vkCreateCuModuleNVX = vkCreateCuModuleNVX;
            vkDestroyCuFunctionNVX = vkDestroyCuFunctionNVX;
            vkDestroyCuModuleNVX = vkDestroyCuModuleNVX;
        #endif 
        #if defined(VK_NVX_image_view_handle)
            vkGetImageViewAddressNVX = vkGetImageViewAddressNVX;
            vkGetImageViewHandleNVX = vkGetImageViewHandleNVX;
        #endif 
        #if defined(VK_NV_clip_space_w_scaling)
            vkCmdSetViewportWScalingNV = vkCmdSetViewportWScalingNV;
        #endif 
        #if defined(VK_NV_copy_memory_indirect)
            vkCmdCopyMemoryIndirectNV = vkCmdCopyMemoryIndirectNV;
            vkCmdCopyMemoryToImageIndirectNV = vkCmdCopyMemoryToImageIndirectNV;
        #endif 
        #if defined(VK_NV_device_diagnostic_checkpoints)
            vkCmdSetCheckpointNV = vkCmdSetCheckpointNV;
            vkGetQueueCheckpointDataNV = vkGetQueueCheckpointDataNV;
        #endif 
        #if defined(VK_NV_device_generated_commands)
            vkCmdBindPipelineShaderGroupNV = vkCmdBindPipelineShaderGroupNV;
            vkCmdExecuteGeneratedCommandsNV = vkCmdExecuteGeneratedCommandsNV;
            vkCmdPreprocessGeneratedCommandsNV = vkCmdPreprocessGeneratedCommandsNV;
            vkCreateIndirectCommandsLayoutNV = vkCreateIndirectCommandsLayoutNV;
            vkDestroyIndirectCommandsLayoutNV = vkDestroyIndirectCommandsLayoutNV;
            vkGetGeneratedCommandsMemoryRequirementsNV = vkGetGeneratedCommandsMemoryRequirementsNV;
        #endif 
        #if defined(VK_NV_device_generated_commands_compute)
            vkCmdUpdatePipelineIndirectBufferNV = vkCmdUpdatePipelineIndirectBufferNV;
            vkGetPipelineIndirectDeviceAddressNV = vkGetPipelineIndirectDeviceAddressNV;
            vkGetPipelineIndirectMemoryRequirementsNV = vkGetPipelineIndirectMemoryRequirementsNV;
        #endif 
        #if defined(VK_NV_external_memory_rdma)
            vkGetMemoryRemoteAddressNV = vkGetMemoryRemoteAddressNV;
        #endif 
        #if defined(VK_NV_external_memory_win32)
            vkGetMemoryWin32HandleNV = vkGetMemoryWin32HandleNV;
        #endif 
        #if defined(VK_NV_fragment_shading_rate_enums)
            vkCmdSetFragmentShadingRateEnumNV = vkCmdSetFragmentShadingRateEnumNV;
        #endif 
        #if defined(VK_NV_memory_decompression)
            vkCmdDecompressMemoryIndirectCountNV = vkCmdDecompressMemoryIndirectCountNV;
            vkCmdDecompressMemoryNV = vkCmdDecompressMemoryNV;
        #endif 
        #if defined(VK_NV_mesh_shader)
            vkCmdDrawMeshTasksIndirectCountNV = vkCmdDrawMeshTasksIndirectCountNV;
            vkCmdDrawMeshTasksIndirectNV = vkCmdDrawMeshTasksIndirectNV;
            vkCmdDrawMeshTasksNV = vkCmdDrawMeshTasksNV;
        #endif 
        #if defined(VK_NV_optical_flow)
            vkBindOpticalFlowSessionImageNV = vkBindOpticalFlowSessionImageNV;
            vkCmdOpticalFlowExecuteNV = vkCmdOpticalFlowExecuteNV;
            vkCreateOpticalFlowSessionNV = vkCreateOpticalFlowSessionNV;
            vkDestroyOpticalFlowSessionNV = vkDestroyOpticalFlowSessionNV;
        #endif 
        #if defined(VK_NV_ray_tracing)
            vkBindAccelerationStructureMemoryNV = vkBindAccelerationStructureMemoryNV;
            vkCmdBuildAccelerationStructureNV = vkCmdBuildAccelerationStructureNV;
            vkCmdCopyAccelerationStructureNV = vkCmdCopyAccelerationStructureNV;
            vkCmdTraceRaysNV = vkCmdTraceRaysNV;
            vkCmdWriteAccelerationStructuresPropertiesNV = vkCmdWriteAccelerationStructuresPropertiesNV;
            vkCompileDeferredNV = vkCompileDeferredNV;
            vkCreateAccelerationStructureNV = vkCreateAccelerationStructureNV;
            vkCreateRayTracingPipelinesNV = vkCreateRayTracingPipelinesNV;
            vkDestroyAccelerationStructureNV = vkDestroyAccelerationStructureNV;
            vkGetAccelerationStructureHandleNV = vkGetAccelerationStructureHandleNV;
            vkGetAccelerationStructureMemoryRequirementsNV = vkGetAccelerationStructureMemoryRequirementsNV;
            vkGetRayTracingShaderGroupHandlesNV = vkGetRayTracingShaderGroupHandlesNV;
        #endif 
        #if defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2
            vkCmdSetExclusiveScissorEnableNV = vkCmdSetExclusiveScissorEnableNV;
        #endif 
        #if defined(VK_NV_scissor_exclusive)
            vkCmdSetExclusiveScissorNV = vkCmdSetExclusiveScissorNV;
        #endif 
        #if defined(VK_NV_shading_rate_image)
            vkCmdBindShadingRateImageNV = vkCmdBindShadingRateImageNV;
            vkCmdSetCoarseSampleOrderNV = vkCmdSetCoarseSampleOrderNV;
            vkCmdSetViewportShadingRatePaletteNV = vkCmdSetViewportShadingRatePaletteNV;
        #endif 
        #if defined(VK_QCOM_tile_properties)
            vkGetDynamicRenderingTilePropertiesQCOM = vkGetDynamicRenderingTilePropertiesQCOM;
            vkGetFramebufferTilePropertiesQCOM = vkGetFramebufferTilePropertiesQCOM;
        #endif 
        #if defined(VK_QNX_external_memory_screen_buffer)
            vkGetScreenBufferPropertiesQNX = vkGetScreenBufferPropertiesQNX;
        #endif 
        #if defined(VK_VALVE_descriptor_set_host_mapping)
            vkGetDescriptorSetHostMappingVALVE = vkGetDescriptorSetHostMappingVALVE;
            vkGetDescriptorSetLayoutHostMappingInfoVALVE = vkGetDescriptorSetLayoutHostMappingInfoVALVE;
        #endif 
        #if (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object))
            vkCmdBindVertexBuffers2EXT = vkCmdBindVertexBuffers2EXT;
            vkCmdSetCullModeEXT = vkCmdSetCullModeEXT;
            vkCmdSetDepthBoundsTestEnableEXT = vkCmdSetDepthBoundsTestEnableEXT;
            vkCmdSetDepthCompareOpEXT = vkCmdSetDepthCompareOpEXT;
            vkCmdSetDepthTestEnableEXT = vkCmdSetDepthTestEnableEXT;
            vkCmdSetDepthWriteEnableEXT = vkCmdSetDepthWriteEnableEXT;
            vkCmdSetFrontFaceEXT = vkCmdSetFrontFaceEXT;
            vkCmdSetPrimitiveTopologyEXT = vkCmdSetPrimitiveTopologyEXT;
            vkCmdSetScissorWithCountEXT = vkCmdSetScissorWithCountEXT;
            vkCmdSetStencilOpEXT = vkCmdSetStencilOpEXT;
            vkCmdSetStencilTestEnableEXT = vkCmdSetStencilTestEnableEXT;
            vkCmdSetViewportWithCountEXT = vkCmdSetViewportWithCountEXT;
        #endif 
        #if (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object))
            vkCmdSetDepthBiasEnableEXT = vkCmdSetDepthBiasEnableEXT;
            vkCmdSetLogicOpEXT = vkCmdSetLogicOpEXT;
            vkCmdSetPatchControlPointsEXT = vkCmdSetPatchControlPointsEXT;
            vkCmdSetPrimitiveRestartEnableEXT = vkCmdSetPrimitiveRestartEnableEXT;
            vkCmdSetRasterizerDiscardEnableEXT = vkCmdSetRasterizerDiscardEnableEXT;
        #endif 
        #if (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object))
            vkCmdSetAlphaToCoverageEnableEXT = vkCmdSetAlphaToCoverageEnableEXT;
            vkCmdSetAlphaToOneEnableEXT = vkCmdSetAlphaToOneEnableEXT;
            vkCmdSetColorBlendAdvancedEXT = vkCmdSetColorBlendAdvancedEXT;
            vkCmdSetColorBlendEnableEXT = vkCmdSetColorBlendEnableEXT;
            vkCmdSetColorBlendEquationEXT = vkCmdSetColorBlendEquationEXT;
            vkCmdSetColorWriteMaskEXT = vkCmdSetColorWriteMaskEXT;
            vkCmdSetConservativeRasterizationModeEXT = vkCmdSetConservativeRasterizationModeEXT;
            vkCmdSetDepthClampEnableEXT = vkCmdSetDepthClampEnableEXT;
            vkCmdSetDepthClipEnableEXT = vkCmdSetDepthClipEnableEXT;
            vkCmdSetDepthClipNegativeOneToOneEXT = vkCmdSetDepthClipNegativeOneToOneEXT;
            vkCmdSetExtraPrimitiveOverestimationSizeEXT = vkCmdSetExtraPrimitiveOverestimationSizeEXT;
            vkCmdSetLineRasterizationModeEXT = vkCmdSetLineRasterizationModeEXT;
            vkCmdSetLineStippleEnableEXT = vkCmdSetLineStippleEnableEXT;
            vkCmdSetLogicOpEnableEXT = vkCmdSetLogicOpEnableEXT;
            vkCmdSetPolygonModeEXT = vkCmdSetPolygonModeEXT;
            vkCmdSetProvokingVertexModeEXT = vkCmdSetProvokingVertexModeEXT;
            vkCmdSetRasterizationSamplesEXT = vkCmdSetRasterizationSamplesEXT;
            vkCmdSetRasterizationStreamEXT = vkCmdSetRasterizationStreamEXT;
            vkCmdSetSampleLocationsEnableEXT = vkCmdSetSampleLocationsEnableEXT;
            vkCmdSetSampleMaskEXT = vkCmdSetSampleMaskEXT;
            vkCmdSetTessellationDomainOriginEXT = vkCmdSetTessellationDomainOriginEXT;
        #endif 
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling))
            vkCmdSetViewportWScalingEnableNV = vkCmdSetViewportWScalingEnableNV;
        #endif 
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle))
            vkCmdSetViewportSwizzleNV = vkCmdSetViewportSwizzleNV;
        #endif 
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color))
            vkCmdSetCoverageToColorEnableNV = vkCmdSetCoverageToColorEnableNV;
            vkCmdSetCoverageToColorLocationNV = vkCmdSetCoverageToColorLocationNV;
        #endif 
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples))
            vkCmdSetCoverageModulationModeNV = vkCmdSetCoverageModulationModeNV;
            vkCmdSetCoverageModulationTableEnableNV = vkCmdSetCoverageModulationTableEnableNV;
            vkCmdSetCoverageModulationTableNV = vkCmdSetCoverageModulationTableNV;
        #endif 
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image))
            vkCmdSetShadingRateImageEnableNV = vkCmdSetShadingRateImageEnableNV;
        #endif 
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test))
            vkCmdSetRepresentativeFragmentTestEnableNV = vkCmdSetRepresentativeFragmentTestEnableNV;
        #endif 
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode))
            vkCmdSetCoverageReductionModeNV = vkCmdSetCoverageReductionModeNV;
        #endif 
        #if (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1))
            vkGetDeviceGroupSurfacePresentModes2EXT = vkGetDeviceGroupSurfacePresentModes2EXT;
        #endif 
        #if (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control))
            vkGetImageSubresourceLayout2EXT = vkGetImageSubresourceLayout2EXT;
        #endif 
        #if (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state))
            vkCmdSetVertexInputEXT = vkCmdSetVertexInputEXT;
        #endif 
        #if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template))
            vkCmdPushDescriptorSetWithTemplateKHR = vkCmdPushDescriptorSetWithTemplateKHR;
        #endif 
        #if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
            vkGetDeviceGroupPresentCapabilitiesKHR = vkGetDeviceGroupPresentCapabilitiesKHR;
            vkGetDeviceGroupSurfacePresentModesKHR = vkGetDeviceGroupSurfacePresentModesKHR;
        #endif 
        #if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
            vkAcquireNextImage2KHR = vkAcquireNextImage2KHR;
        #endif
    }

}