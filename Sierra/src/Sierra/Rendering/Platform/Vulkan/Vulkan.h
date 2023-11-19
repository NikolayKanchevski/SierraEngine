//
// Created by Nikolay Kanchevski on 18.11.23.
//

#pragma once

//#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#if SR_ENABLE_LOGGING
    #define VK_VALIDATE(FUNCTION, MESSAGE) if (VkResult result = FUNCTION; result != VK_SUCCESS) SR_ERROR("[Vulkan]: {0}() failed: {1} Error code: {2}.", std::string_view(#FUNCTION).substr(0, std::string_view(#FUNCTION).rfind('(')), MESSAGE, static_cast<int32>(result))
#else
    #define VK_VALIDATE(FUNCTION, MESSAGE) static_cast<void>(FUNCTION)
#endif

namespace Sierra
{

    class SIERRA_API VulkanLoader
    {
    private:
        static void Initialize();
        static void LoadInstance(VkInstance Instance);
        static void LoadDevice(VkDevice Device);

        friend class VulkanInstance;
        friend class VulkanDevice;

    };

}

namespace VK
{
    #if defined(VK_VERSION_1_0)
        extern PFN_vkAllocateCommandBuffers AllocateCommandBuffers;
        extern PFN_vkAllocateDescriptorSets AllocateDescriptorSets;
        extern PFN_vkAllocateMemory AllocateMemory;
        extern PFN_vkBeginCommandBuffer BeginCommandBuffer;
        extern PFN_vkBindBufferMemory BindBufferMemory;
        extern PFN_vkBindImageMemory BindImageMemory;
        extern PFN_vkCmdBeginQuery CmdBeginQuery;
        extern PFN_vkCmdBeginRenderPass CmdBeginRenderPass;
        extern PFN_vkCmdBindDescriptorSets CmdBindDescriptorSets;
        extern PFN_vkCmdBindIndexBuffer CmdBindIndexBuffer;
        extern PFN_vkCmdBindPipeline CmdBindPipeline;
        extern PFN_vkCmdBindVertexBuffers CmdBindVertexBuffers;
        extern PFN_vkCmdBlitImage CmdBlitImage;
        extern PFN_vkCmdClearAttachments CmdClearAttachments;
        extern PFN_vkCmdClearColorImage CmdClearColorImage;
        extern PFN_vkCmdClearDepthStencilImage CmdClearDepthStencilImage;
        extern PFN_vkCmdCopyBuffer CmdCopyBuffer;
        extern PFN_vkCmdCopyBufferToImage CmdCopyBufferToImage;
        extern PFN_vkCmdCopyImage CmdCopyImage;
        extern PFN_vkCmdCopyImageToBuffer CmdCopyImageToBuffer;
        extern PFN_vkCmdCopyQueryPoolResults CmdCopyQueryPoolResults;
        extern PFN_vkCmdDispatch CmdDispatch;
        extern PFN_vkCmdDispatchIndirect CmdDispatchIndirect;
        extern PFN_vkCmdDraw CmdDraw;
        extern PFN_vkCmdDrawIndexed CmdDrawIndexed;
        extern PFN_vkCmdDrawIndexedIndirect CmdDrawIndexedIndirect;
        extern PFN_vkCmdDrawIndirect CmdDrawIndirect;
        extern PFN_vkCmdEndQuery CmdEndQuery;
        extern PFN_vkCmdEndRenderPass CmdEndRenderPass;
        extern PFN_vkCmdExecuteCommands CmdExecuteCommands;
        extern PFN_vkCmdFillBuffer CmdFillBuffer;
        extern PFN_vkCmdNextSubpass CmdNextSubpass;
        extern PFN_vkCmdPipelineBarrier CmdPipelineBarrier;
        extern PFN_vkCmdPushConstants CmdPushConstants;
        extern PFN_vkCmdResetEvent CmdResetEvent;
        extern PFN_vkCmdResetQueryPool CmdResetQueryPool;
        extern PFN_vkCmdResolveImage CmdResolveImage;
        extern PFN_vkCmdSetBlendConstants CmdSetBlendConstants;
        extern PFN_vkCmdSetDepthBias CmdSetDepthBias;
        extern PFN_vkCmdSetDepthBounds CmdSetDepthBounds;
        extern PFN_vkCmdSetEvent CmdSetEvent;
        extern PFN_vkCmdSetLineWidth CmdSetLineWidth;
        extern PFN_vkCmdSetScissor CmdSetScissor;
        extern PFN_vkCmdSetStencilCompareMask CmdSetStencilCompareMask;
        extern PFN_vkCmdSetStencilReference CmdSetStencilReference;
        extern PFN_vkCmdSetStencilWriteMask CmdSetStencilWriteMask;
        extern PFN_vkCmdSetViewport CmdSetViewport;
        extern PFN_vkCmdUpdateBuffer CmdUpdateBuffer;
        extern PFN_vkCmdWaitEvents CmdWaitEvents;
        extern PFN_vkCmdWriteTimestamp CmdWriteTimestamp;
        extern PFN_vkCreateBuffer CreateBuffer;
        extern PFN_vkCreateBufferView CreateBufferView;
        extern PFN_vkCreateCommandPool CreateCommandPool;
        extern PFN_vkCreateComputePipelines CreateComputePipelines;
        extern PFN_vkCreateDescriptorPool CreateDescriptorPool;
        extern PFN_vkCreateDescriptorSetLayout CreateDescriptorSetLayout;
        extern PFN_vkCreateDevice CreateDevice;
        extern PFN_vkCreateEvent CreateEvent;
        extern PFN_vkCreateFence CreateFence;
        extern PFN_vkCreateFramebuffer CreateFramebuffer;
        extern PFN_vkCreateGraphicsPipelines CreateGraphicsPipelines;
        extern PFN_vkCreateImage CreateImage;
        extern PFN_vkCreateImageView CreateImageView;
        extern PFN_vkCreateInstance CreateInstance;
        extern PFN_vkCreatePipelineCache CreatePipelineCache;
        extern PFN_vkCreatePipelineLayout CreatePipelineLayout;
        extern PFN_vkCreateQueryPool CreateQueryPool;
        extern PFN_vkCreateRenderPass CreateRenderPass;
        extern PFN_vkCreateSampler CreateSampler;
        extern PFN_vkCreateSemaphore CreateSemaphore;
        extern PFN_vkCreateShaderModule CreateShaderModule;
        extern PFN_vkDestroyBuffer DestroyBuffer;
        extern PFN_vkDestroyBufferView DestroyBufferView;
        extern PFN_vkDestroyCommandPool DestroyCommandPool;
        extern PFN_vkDestroyDescriptorPool DestroyDescriptorPool;
        extern PFN_vkDestroyDescriptorSetLayout DestroyDescriptorSetLayout;
        extern PFN_vkDestroyDevice DestroyDevice;
        extern PFN_vkDestroyEvent DestroyEvent;
        extern PFN_vkDestroyFence DestroyFence;
        extern PFN_vkDestroyFramebuffer DestroyFramebuffer;
        extern PFN_vkDestroyImage DestroyImage;
        extern PFN_vkDestroyImageView DestroyImageView;
        extern PFN_vkDestroyInstance DestroyInstance;
        extern PFN_vkDestroyPipeline DestroyPipeline;
        extern PFN_vkDestroyPipelineCache DestroyPipelineCache;
        extern PFN_vkDestroyPipelineLayout DestroyPipelineLayout;
        extern PFN_vkDestroyQueryPool DestroyQueryPool;
        extern PFN_vkDestroyRenderPass DestroyRenderPass;
        extern PFN_vkDestroySampler DestroySampler;
        extern PFN_vkDestroySemaphore DestroySemaphore;
        extern PFN_vkDestroyShaderModule DestroyShaderModule;
        extern PFN_vkDeviceWaitIdle DeviceWaitIdle;
        extern PFN_vkEndCommandBuffer EndCommandBuffer;
        extern PFN_vkEnumerateDeviceExtensionProperties EnumerateDeviceExtensionProperties;
        extern PFN_vkEnumerateDeviceLayerProperties EnumerateDeviceLayerProperties;
        extern PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties;
        extern PFN_vkEnumerateInstanceLayerProperties EnumerateInstanceLayerProperties;
        extern PFN_vkEnumeratePhysicalDevices EnumeratePhysicalDevices;
        extern PFN_vkFlushMappedMemoryRanges FlushMappedMemoryRanges;
        extern PFN_vkFreeCommandBuffers FreeCommandBuffers;
        extern PFN_vkFreeDescriptorSets FreeDescriptorSets;
        extern PFN_vkFreeMemory FreeMemory;
        extern PFN_vkGetBufferMemoryRequirements GetBufferMemoryRequirements;
        extern PFN_vkGetDeviceMemoryCommitment GetDeviceMemoryCommitment;
        extern PFN_vkGetDeviceProcAddr GetDeviceProcAddr;
        extern PFN_vkGetDeviceQueue GetDeviceQueue;
        extern PFN_vkGetEventStatus GetEventStatus;
        extern PFN_vkGetFenceStatus GetFenceStatus;
        extern PFN_vkGetImageMemoryRequirements GetImageMemoryRequirements;
        extern PFN_vkGetImageSparseMemoryRequirements GetImageSparseMemoryRequirements;
        extern PFN_vkGetImageSubresourceLayout GetImageSubresourceLayout;
        extern PFN_vkGetInstanceProcAddr GetInstanceProcAddr;
        extern PFN_vkGetPhysicalDeviceFeatures GetPhysicalDeviceFeatures;
        extern PFN_vkGetPhysicalDeviceFormatProperties GetPhysicalDeviceFormatProperties;
        extern PFN_vkGetPhysicalDeviceImageFormatProperties GetPhysicalDeviceImageFormatProperties;
        extern PFN_vkGetPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties;
        extern PFN_vkGetPhysicalDeviceProperties GetPhysicalDeviceProperties;
        extern PFN_vkGetPhysicalDeviceQueueFamilyProperties GetPhysicalDeviceQueueFamilyProperties;
        extern PFN_vkGetPhysicalDeviceSparseImageFormatProperties GetPhysicalDeviceSparseImageFormatProperties;
        extern PFN_vkGetPipelineCacheData GetPipelineCacheData;
        extern PFN_vkGetQueryPoolResults GetQueryPoolResults;
        extern PFN_vkGetRenderAreaGranularity GetRenderAreaGranularity;
        extern PFN_vkInvalidateMappedMemoryRanges InvalidateMappedMemoryRanges;
        extern PFN_vkMapMemory MapMemory;
        extern PFN_vkMergePipelineCaches MergePipelineCaches;
        extern PFN_vkQueueBindSparse QueueBindSparse;
        extern PFN_vkQueueSubmit QueueSubmit;
        extern PFN_vkQueueWaitIdle QueueWaitIdle;
        extern PFN_vkResetCommandBuffer ResetCommandBuffer;
        extern PFN_vkResetCommandPool ResetCommandPool;
        extern PFN_vkResetDescriptorPool ResetDescriptorPool;
        extern PFN_vkResetEvent ResetEvent;
        extern PFN_vkResetFences ResetFences;
        extern PFN_vkSetEvent SetEvent;
        extern PFN_vkUnmapMemory UnmapMemory;
        extern PFN_vkUpdateDescriptorSets UpdateDescriptorSets;
        extern PFN_vkWaitForFences WaitForFences;
    #endif
    #if defined(VK_VERSION_1_1)
        extern PFN_vkBindBufferMemory2 BindBufferMemory2;
        extern PFN_vkBindImageMemory2 BindImageMemory2;
        extern PFN_vkCmdDispatchBase CmdDispatchBase;
        extern PFN_vkCmdSetDeviceMask CmdSetDeviceMask;
        extern PFN_vkCreateDescriptorUpdateTemplate CreateDescriptorUpdateTemplate;
        extern PFN_vkCreateSamplerYcbcrConversion CreateSamplerYcbcrConversion;
        extern PFN_vkDestroyDescriptorUpdateTemplate DestroyDescriptorUpdateTemplate;
        extern PFN_vkDestroySamplerYcbcrConversion DestroySamplerYcbcrConversion;
        extern PFN_vkEnumerateInstanceVersion EnumerateInstanceVersion;
        extern PFN_vkEnumeratePhysicalDeviceGroups EnumeratePhysicalDeviceGroups;
        extern PFN_vkGetBufferMemoryRequirements2 GetBufferMemoryRequirements2;
        extern PFN_vkGetDescriptorSetLayoutSupport GetDescriptorSetLayoutSupport;
        extern PFN_vkGetDeviceGroupPeerMemoryFeatures GetDeviceGroupPeerMemoryFeatures;
        extern PFN_vkGetDeviceQueue2 GetDeviceQueue2;
        extern PFN_vkGetImageMemoryRequirements2 GetImageMemoryRequirements2;
        extern PFN_vkGetImageSparseMemoryRequirements2 GetImageSparseMemoryRequirements2;
        extern PFN_vkGetPhysicalDeviceExternalBufferProperties GetPhysicalDeviceExternalBufferProperties;
        extern PFN_vkGetPhysicalDeviceExternalFenceProperties GetPhysicalDeviceExternalFenceProperties;
        extern PFN_vkGetPhysicalDeviceExternalSemaphoreProperties GetPhysicalDeviceExternalSemaphoreProperties;
        extern PFN_vkGetPhysicalDeviceFeatures2 GetPhysicalDeviceFeatures2;
        extern PFN_vkGetPhysicalDeviceFormatProperties2 GetPhysicalDeviceFormatProperties2;
        extern PFN_vkGetPhysicalDeviceImageFormatProperties2 GetPhysicalDeviceImageFormatProperties2;
        extern PFN_vkGetPhysicalDeviceMemoryProperties2 GetPhysicalDeviceMemoryProperties2;
        extern PFN_vkGetPhysicalDeviceProperties2 GetPhysicalDeviceProperties2;
        extern PFN_vkGetPhysicalDeviceQueueFamilyProperties2 GetPhysicalDeviceQueueFamilyProperties2;
        extern PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 GetPhysicalDeviceSparseImageFormatProperties2;
        extern PFN_vkTrimCommandPool TrimCommandPool;
        extern PFN_vkUpdateDescriptorSetWithTemplate UpdateDescriptorSetWithTemplate;
    #endif
    #if defined(VK_VERSION_1_2)
        extern PFN_vkCmdBeginRenderPass2 CmdBeginRenderPass2;
        extern PFN_vkCmdDrawIndexedIndirectCount CmdDrawIndexedIndirectCount;
        extern PFN_vkCmdDrawIndirectCount CmdDrawIndirectCount;
        extern PFN_vkCmdEndRenderPass2 CmdEndRenderPass2;
        extern PFN_vkCmdNextSubpass2 CmdNextSubpass2;
        extern PFN_vkCreateRenderPass2 CreateRenderPass2;
        extern PFN_vkGetBufferDeviceAddress GetBufferDeviceAddress;
        extern PFN_vkGetBufferOpaqueCaptureAddress GetBufferOpaqueCaptureAddress;
        extern PFN_vkGetDeviceMemoryOpaqueCaptureAddress GetDeviceMemoryOpaqueCaptureAddress;
        extern PFN_vkGetSemaphoreCounterValue GetSemaphoreCounterValue;
        extern PFN_vkResetQueryPool ResetQueryPool;
        extern PFN_vkSignalSemaphore SignalSemaphore;
        extern PFN_vkWaitSemaphores WaitSemaphores;
    #endif
    #if defined(VK_VERSION_1_3)
        extern PFN_vkCmdBeginRendering CmdBeginRendering;
        extern PFN_vkCmdBindVertexBuffers2 CmdBindVertexBuffers2;
        extern PFN_vkCmdBlitImage2 CmdBlitImage2;
        extern PFN_vkCmdCopyBuffer2 CmdCopyBuffer2;
        extern PFN_vkCmdCopyBufferToImage2 CmdCopyBufferToImage2;
        extern PFN_vkCmdCopyImage2 CmdCopyImage2;
        extern PFN_vkCmdCopyImageToBuffer2 CmdCopyImageToBuffer2;
        extern PFN_vkCmdEndRendering CmdEndRendering;
        extern PFN_vkCmdPipelineBarrier2 CmdPipelineBarrier2;
        extern PFN_vkCmdResetEvent2 CmdResetEvent2;
        extern PFN_vkCmdResolveImage2 CmdResolveImage2;
        extern PFN_vkCmdSetCullMode CmdSetCullMode;
        extern PFN_vkCmdSetDepthBiasEnable CmdSetDepthBiasEnable;
        extern PFN_vkCmdSetDepthBoundsTestEnable CmdSetDepthBoundsTestEnable;
        extern PFN_vkCmdSetDepthCompareOp CmdSetDepthCompareOp;
        extern PFN_vkCmdSetDepthTestEnable CmdSetDepthTestEnable;
        extern PFN_vkCmdSetDepthWriteEnable CmdSetDepthWriteEnable;
        extern PFN_vkCmdSetEvent2 CmdSetEvent2;
        extern PFN_vkCmdSetFrontFace CmdSetFrontFace;
        extern PFN_vkCmdSetPrimitiveRestartEnable CmdSetPrimitiveRestartEnable;
        extern PFN_vkCmdSetPrimitiveTopology CmdSetPrimitiveTopology;
        extern PFN_vkCmdSetRasterizerDiscardEnable CmdSetRasterizerDiscardEnable;
        extern PFN_vkCmdSetScissorWithCount CmdSetScissorWithCount;
        extern PFN_vkCmdSetStencilOp CmdSetStencilOp;
        extern PFN_vkCmdSetStencilTestEnable CmdSetStencilTestEnable;
        extern PFN_vkCmdSetViewportWithCount CmdSetViewportWithCount;
        extern PFN_vkCmdWaitEvents2 CmdWaitEvents2;
        extern PFN_vkCmdWriteTimestamp2 CmdWriteTimestamp2;
        extern PFN_vkCreatePrivateDataSlot CreatePrivateDataSlot;
        extern PFN_vkDestroyPrivateDataSlot DestroyPrivateDataSlot;
        extern PFN_vkGetDeviceBufferMemoryRequirements GetDeviceBufferMemoryRequirements;
        extern PFN_vkGetDeviceImageMemoryRequirements GetDeviceImageMemoryRequirements;
        extern PFN_vkGetDeviceImageSparseMemoryRequirements GetDeviceImageSparseMemoryRequirements;
        extern PFN_vkGetPhysicalDeviceToolProperties GetPhysicalDeviceToolProperties;
        extern PFN_vkGetPrivateData GetPrivateData;
        extern PFN_vkQueueSubmit2 QueueSubmit2;
        extern PFN_vkSetPrivateData SetPrivateData;
    #endif
    #if defined(VK_AMDX_shader_enqueue)
        extern PFN_vkCmdDispatchGraphAMDX CmdDispatchGraphAMDX;
        extern PFN_vkCmdDispatchGraphIndirectAMDX CmdDispatchGraphIndirectAMDX;
        extern PFN_vkCmdDispatchGraphIndirectCountAMDX CmdDispatchGraphIndirectCountAMDX;
        extern PFN_vkCmdInitializeGraphScratchMemoryAMDX CmdInitializeGraphScratchMemoryAMDX;
        extern PFN_vkCreateExecutionGraphPipelinesAMDX CreateExecutionGraphPipelinesAMDX;
        extern PFN_vkGetExecutionGraphPipelineNodeIndexAMDX GetExecutionGraphPipelineNodeIndexAMDX;
        extern PFN_vkGetExecutionGraphPipelineScratchSizeAMDX GetExecutionGraphPipelineScratchSizeAMDX;
    #endif
    #if defined(VK_AMD_buffer_marker)
        extern PFN_vkCmdWriteBufferMarkerAMD CmdWriteBufferMarkerAMD;
    #endif
    #if defined(VK_AMD_display_native_hdr)
        extern PFN_vkSetLocalDimmingAMD SetLocalDimmingAMD;
    #endif
    #if defined(VK_AMD_draw_indirect_count)
        extern PFN_vkCmdDrawIndexedIndirectCountAMD CmdDrawIndexedIndirectCountAMD;
        extern PFN_vkCmdDrawIndirectCountAMD CmdDrawIndirectCountAMD;
    #endif
    #if defined(VK_AMD_shader_info)
        extern PFN_vkGetShaderInfoAMD GetShaderInfoAMD;
    #endif
    #if defined(VK_ANDROID_external_memory_android_hardware_buffer)
        extern PFN_vkGetAndroidHardwareBufferPropertiesANDROID GetAndroidHardwareBufferPropertiesANDROID;
        extern PFN_vkGetMemoryAndroidHardwareBufferANDROID GetMemoryAndroidHardwareBufferANDROID;
    #endif
    #if defined(VK_EXT_acquire_drm_display)
        extern PFN_vkAcquireDrmDisplayEXT AcquireDrmDisplayEXT;
        extern PFN_vkGetDrmDisplayEXT GetDrmDisplayEXT;
    #endif
    #if defined(VK_EXT_acquire_xlib_display)
        extern PFN_vkAcquireXlibDisplayEXT AcquireXlibDisplayEXT;
        extern PFN_vkGetRandROutputDisplayEXT GetRandROutputDisplayEXT;
    #endif
    #if defined(VK_EXT_attachment_feedback_loop_dynamic_state)
        extern PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT CmdSetAttachmentFeedbackLoopEnableEXT;
    #endif
    #if defined(VK_EXT_buffer_device_address)
        extern PFN_vkGetBufferDeviceAddressEXT GetBufferDeviceAddressEXT;
    #endif
    #if defined(VK_EXT_calibrated_timestamps)
        extern PFN_vkGetCalibratedTimestampsEXT GetCalibratedTimestampsEXT;
        extern PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT GetPhysicalDeviceCalibrateableTimeDomainsEXT;
    #endif
    #if defined(VK_EXT_color_write_enable)
        extern PFN_vkCmdSetColorWriteEnableEXT CmdSetColorWriteEnableEXT;
    #endif
    #if defined(VK_EXT_conditional_rendering)
        extern PFN_vkCmdBeginConditionalRenderingEXT CmdBeginConditionalRenderingEXT;
        extern PFN_vkCmdEndConditionalRenderingEXT CmdEndConditionalRenderingEXT;
    #endif
    #if defined(VK_EXT_debug_marker)
        extern PFN_vkCmdDebugMarkerBeginEXT CmdDebugMarkerBeginEXT;
        extern PFN_vkCmdDebugMarkerEndEXT CmdDebugMarkerEndEXT;
        extern PFN_vkCmdDebugMarkerInsertEXT CmdDebugMarkerInsertEXT;
        extern PFN_vkDebugMarkerSetObjectNameEXT DebugMarkerSetObjectNameEXT;
        extern PFN_vkDebugMarkerSetObjectTagEXT DebugMarkerSetObjectTagEXT;
    #endif
    #if defined(VK_EXT_debug_report)
        extern PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallbackEXT;
        extern PFN_vkDebugReportMessageEXT DebugReportMessageEXT;
        extern PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallbackEXT;
    #endif
    #if defined(VK_EXT_debug_utils)
        extern PFN_vkCmdBeginDebugUtilsLabelEXT CmdBeginDebugUtilsLabelEXT;
        extern PFN_vkCmdEndDebugUtilsLabelEXT CmdEndDebugUtilsLabelEXT;
        extern PFN_vkCmdInsertDebugUtilsLabelEXT CmdInsertDebugUtilsLabelEXT;
        extern PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT;
        extern PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT;
        extern PFN_vkQueueBeginDebugUtilsLabelEXT QueueBeginDebugUtilsLabelEXT;
        extern PFN_vkQueueEndDebugUtilsLabelEXT QueueEndDebugUtilsLabelEXT;
        extern PFN_vkQueueInsertDebugUtilsLabelEXT QueueInsertDebugUtilsLabelEXT;
        extern PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectNameEXT;
        extern PFN_vkSetDebugUtilsObjectTagEXT SetDebugUtilsObjectTagEXT;
        extern PFN_vkSubmitDebugUtilsMessageEXT SubmitDebugUtilsMessageEXT;
    #endif
    #if defined(VK_EXT_depth_bias_control)
        extern PFN_vkCmdSetDepthBias2EXT CmdSetDepthBias2EXT;
    #endif
    #if defined(VK_EXT_descriptor_buffer)
        extern PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT CmdBindDescriptorBufferEmbeddedSamplersEXT;
        extern PFN_vkCmdBindDescriptorBuffersEXT CmdBindDescriptorBuffersEXT;
        extern PFN_vkCmdSetDescriptorBufferOffsetsEXT CmdSetDescriptorBufferOffsetsEXT;
        extern PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT GetBufferOpaqueCaptureDescriptorDataEXT;
        extern PFN_vkGetDescriptorEXT GetDescriptorEXT;
        extern PFN_vkGetDescriptorSetLayoutBindingOffsetEXT GetDescriptorSetLayoutBindingOffsetEXT;
        extern PFN_vkGetDescriptorSetLayoutSizeEXT GetDescriptorSetLayoutSizeEXT;
        extern PFN_vkGetImageOpaqueCaptureDescriptorDataEXT GetImageOpaqueCaptureDescriptorDataEXT;
        extern PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT GetImageViewOpaqueCaptureDescriptorDataEXT;
        extern PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT GetSamplerOpaqueCaptureDescriptorDataEXT;
    #endif
    #if defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing))
        extern PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT GetAccelerationStructureOpaqueCaptureDescriptorDataEXT;
    #endif
    #if defined(VK_EXT_device_fault)
        extern PFN_vkGetDeviceFaultInfoEXT GetDeviceFaultInfoEXT;
    #endif
    #if defined(VK_EXT_direct_mode_display)
        extern PFN_vkReleaseDisplayEXT ReleaseDisplayEXT;
    #endif
    #if defined(VK_EXT_directfb_surface)
        extern PFN_vkCreateDirectFBSurfaceEXT CreateDirectFBSurfaceEXT;
        extern PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT GetPhysicalDeviceDirectFBPresentationSupportEXT;
    #endif
    #if defined(VK_EXT_discard_rectangles)
        extern PFN_vkCmdSetDiscardRectangleEXT CmdSetDiscardRectangleEXT;
    #endif
    #if defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2
        extern PFN_vkCmdSetDiscardRectangleEnableEXT CmdSetDiscardRectangleEnableEXT;
        extern PFN_vkCmdSetDiscardRectangleModeEXT CmdSetDiscardRectangleModeEXT;
    #endif
    #if defined(VK_EXT_display_control)
        extern PFN_vkDisplayPowerControlEXT DisplayPowerControlEXT;
        extern PFN_vkGetSwapchainCounterEXT GetSwapchainCounterEXT;
        extern PFN_vkRegisterDeviceEventEXT RegisterDeviceEventEXT;
        extern PFN_vkRegisterDisplayEventEXT RegisterDisplayEventEXT;
    #endif
    #if defined(VK_EXT_display_surface_counter)
        extern PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT GetPhysicalDeviceSurfaceCapabilities2EXT;
    #endif
    #if defined(VK_EXT_external_memory_host)
        extern PFN_vkGetMemoryHostPointerPropertiesEXT GetMemoryHostPointerPropertiesEXT;
    #endif
    #if defined(VK_EXT_full_screen_exclusive)
        extern PFN_vkAcquireFullScreenExclusiveModeEXT AcquireFullScreenExclusiveModeEXT;
        extern PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT GetPhysicalDeviceSurfacePresentModes2EXT;
        extern PFN_vkReleaseFullScreenExclusiveModeEXT ReleaseFullScreenExclusiveModeEXT;
    #endif
    #if defined(VK_EXT_hdr_metadata)
        extern PFN_vkSetHdrMetadataEXT SetHdrMetadataEXT;
    #endif
    #if defined(VK_EXT_headless_surface)
        extern PFN_vkCreateHeadlessSurfaceEXT CreateHeadlessSurfaceEXT;
    #endif
    #if defined(VK_EXT_host_image_copy)
        extern PFN_vkCopyImageToImageEXT CopyImageToImageEXT;
        extern PFN_vkCopyImageToMemoryEXT CopyImageToMemoryEXT;
        extern PFN_vkCopyMemoryToImageEXT CopyMemoryToImageEXT;
        extern PFN_vkTransitionImageLayoutEXT TransitionImageLayoutEXT;
    #endif
    #if defined(VK_EXT_host_query_reset)
        extern PFN_vkResetQueryPoolEXT ResetQueryPoolEXT;
    #endif
    #if defined(VK_EXT_image_drm_format_modifier)
        extern PFN_vkGetImageDrmFormatModifierPropertiesEXT GetImageDrmFormatModifierPropertiesEXT;
    #endif
    #if defined(VK_EXT_line_rasterization)
        extern PFN_vkCmdSetLineStippleEXT CmdSetLineStippleEXT;
    #endif
    #if defined(VK_EXT_mesh_shader)
        extern PFN_vkCmdDrawMeshTasksEXT CmdDrawMeshTasksEXT;
        extern PFN_vkCmdDrawMeshTasksIndirectCountEXT CmdDrawMeshTasksIndirectCountEXT;
        extern PFN_vkCmdDrawMeshTasksIndirectEXT CmdDrawMeshTasksIndirectEXT;
    #endif
    #if defined(VK_EXT_metal_objects)
        extern PFN_vkExportMetalObjectsEXT ExportMetalObjectsEXT;
    #endif
    #if defined(VK_EXT_metal_surface)
        extern PFN_vkCreateMetalSurfaceEXT CreateMetalSurfaceEXT;
    #endif
    #if defined(VK_EXT_multi_draw)
        extern PFN_vkCmdDrawMultiEXT CmdDrawMultiEXT;
        extern PFN_vkCmdDrawMultiIndexedEXT CmdDrawMultiIndexedEXT;
    #endif
    #if defined(VK_EXT_opacity_micromap)
        extern PFN_vkBuildMicromapsEXT BuildMicromapsEXT;
        extern PFN_vkCmdBuildMicromapsEXT CmdBuildMicromapsEXT;
        extern PFN_vkCmdCopyMemoryToMicromapEXT CmdCopyMemoryToMicromapEXT;
        extern PFN_vkCmdCopyMicromapEXT CmdCopyMicromapEXT;
        extern PFN_vkCmdCopyMicromapToMemoryEXT CmdCopyMicromapToMemoryEXT;
        extern PFN_vkCmdWriteMicromapsPropertiesEXT CmdWriteMicromapsPropertiesEXT;
        extern PFN_vkCopyMemoryToMicromapEXT CopyMemoryToMicromapEXT;
        extern PFN_vkCopyMicromapEXT CopyMicromapEXT;
        extern PFN_vkCopyMicromapToMemoryEXT CopyMicromapToMemoryEXT;
        extern PFN_vkCreateMicromapEXT CreateMicromapEXT;
        extern PFN_vkDestroyMicromapEXT DestroyMicromapEXT;
        extern PFN_vkGetDeviceMicromapCompatibilityEXT GetDeviceMicromapCompatibilityEXT;
        extern PFN_vkGetMicromapBuildSizesEXT GetMicromapBuildSizesEXT;
        extern PFN_vkWriteMicromapsPropertiesEXT WriteMicromapsPropertiesEXT;
    #endif
    #if defined(VK_EXT_pageable_device_local_memory)
        extern PFN_vkSetDeviceMemoryPriorityEXT SetDeviceMemoryPriorityEXT;
    #endif
    #if defined(VK_EXT_pipeline_properties)
        extern PFN_vkGetPipelinePropertiesEXT GetPipelinePropertiesEXT;
    #endif
    #if defined(VK_EXT_private_data)
        extern PFN_vkCreatePrivateDataSlotEXT CreatePrivateDataSlotEXT;
        extern PFN_vkDestroyPrivateDataSlotEXT DestroyPrivateDataSlotEXT;
        extern PFN_vkGetPrivateDataEXT GetPrivateDataEXT;
        extern PFN_vkSetPrivateDataEXT SetPrivateDataEXT;
    #endif
    #if defined(VK_EXT_sample_locations)
        extern PFN_vkCmdSetSampleLocationsEXT CmdSetSampleLocationsEXT;
        extern PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT GetPhysicalDeviceMultisamplePropertiesEXT;
    #endif
    #if defined(VK_EXT_shader_module_identifier)
        extern PFN_vkGetShaderModuleCreateInfoIdentifierEXT GetShaderModuleCreateInfoIdentifierEXT;
        extern PFN_vkGetShaderModuleIdentifierEXT GetShaderModuleIdentifierEXT;
    #endif
    #if defined(VK_EXT_shader_object)
        extern PFN_vkCmdBindShadersEXT CmdBindShadersEXT;
        extern PFN_vkCreateShadersEXT CreateShadersEXT;
        extern PFN_vkDestroyShaderEXT DestroyShaderEXT;
        extern PFN_vkGetShaderBinaryDataEXT GetShaderBinaryDataEXT;
    #endif
    #if defined(VK_EXT_swapchain_maintenance1)
        extern PFN_vkReleaseSwapchainImagesEXT ReleaseSwapchainImagesEXT;
    #endif
    #if defined(VK_EXT_tooling_info)
        extern PFN_vkGetPhysicalDeviceToolPropertiesEXT GetPhysicalDeviceToolPropertiesEXT;
    #endif
    #if defined(VK_EXT_transform_feedback)
        extern PFN_vkCmdBeginQueryIndexedEXT CmdBeginQueryIndexedEXT;
        extern PFN_vkCmdBeginTransformFeedbackEXT CmdBeginTransformFeedbackEXT;
        extern PFN_vkCmdBindTransformFeedbackBuffersEXT CmdBindTransformFeedbackBuffersEXT;
        extern PFN_vkCmdDrawIndirectByteCountEXT CmdDrawIndirectByteCountEXT;
        extern PFN_vkCmdEndQueryIndexedEXT CmdEndQueryIndexedEXT;
        extern PFN_vkCmdEndTransformFeedbackEXT CmdEndTransformFeedbackEXT;
    #endif
    #if defined(VK_EXT_validation_cache)
        extern PFN_vkCreateValidationCacheEXT CreateValidationCacheEXT;
        extern PFN_vkDestroyValidationCacheEXT DestroyValidationCacheEXT;
        extern PFN_vkGetValidationCacheDataEXT GetValidationCacheDataEXT;
        extern PFN_vkMergeValidationCachesEXT MergeValidationCachesEXT;
    #endif
    #if defined(VK_FUCHSIA_buffer_collection)
        extern PFN_vkCreateBufferCollectionFUCHSIA CreateBufferCollectionFUCHSIA;
        extern PFN_vkDestroyBufferCollectionFUCHSIA DestroyBufferCollectionFUCHSIA;
        extern PFN_vkGetBufferCollectionPropertiesFUCHSIA GetBufferCollectionPropertiesFUCHSIA;
        extern PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA SetBufferCollectionBufferConstraintsFUCHSIA;
        extern PFN_vkSetBufferCollectionImageConstraintsFUCHSIA SetBufferCollectionImageConstraintsFUCHSIA;
    #endif
    #if defined(VK_FUCHSIA_external_memory)
        extern PFN_vkGetMemoryZirconHandleFUCHSIA GetMemoryZirconHandleFUCHSIA;
        extern PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA GetMemoryZirconHandlePropertiesFUCHSIA;
    #endif
    #if defined(VK_FUCHSIA_external_semaphore)
        extern PFN_vkGetSemaphoreZirconHandleFUCHSIA GetSemaphoreZirconHandleFUCHSIA;
        extern PFN_vkImportSemaphoreZirconHandleFUCHSIA ImportSemaphoreZirconHandleFUCHSIA;
    #endif
    #if defined(VK_FUCHSIA_imagepipe_surface)
        extern PFN_vkCreateImagePipeSurfaceFUCHSIA CreateImagePipeSurfaceFUCHSIA;
    #endif
    #if defined(VK_GGP_stream_descriptor_surface)
        extern PFN_vkCreateStreamDescriptorSurfaceGGP CreateStreamDescriptorSurfaceGGP;
    #endif
    #if defined(VK_GOOGLE_display_timing)
        extern PFN_vkGetPastPresentationTimingGOOGLE GetPastPresentationTimingGOOGLE;
        extern PFN_vkGetRefreshCycleDurationGOOGLE GetRefreshCycleDurationGOOGLE;
    #endif
    #if defined(VK_HUAWEI_cluster_culling_shader)
        extern PFN_vkCmdDrawClusterHUAWEI CmdDrawClusterHUAWEI;
        extern PFN_vkCmdDrawClusterIndirectHUAWEI CmdDrawClusterIndirectHUAWEI;
    #endif
    #if defined(VK_HUAWEI_invocation_mask)
        extern PFN_vkCmdBindInvocationMaskHUAWEI CmdBindInvocationMaskHUAWEI;
    #endif
    #if defined(VK_HUAWEI_subpass_shading)
        extern PFN_vkCmdSubpassShadingHUAWEI CmdSubpassShadingHUAWEI;
        extern PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI;
    #endif
    #if defined(VK_INTEL_performance_query)
        extern PFN_vkAcquirePerformanceConfigurationINTEL AcquirePerformanceConfigurationINTEL;
        extern PFN_vkCmdSetPerformanceMarkerINTEL CmdSetPerformanceMarkerINTEL;
        extern PFN_vkCmdSetPerformanceOverrideINTEL CmdSetPerformanceOverrideINTEL;
        extern PFN_vkCmdSetPerformanceStreamMarkerINTEL CmdSetPerformanceStreamMarkerINTEL;
        extern PFN_vkGetPerformanceParameterINTEL GetPerformanceParameterINTEL;
        extern PFN_vkInitializePerformanceApiINTEL InitializePerformanceApiINTEL;
        extern PFN_vkQueueSetPerformanceConfigurationINTEL QueueSetPerformanceConfigurationINTEL;
        extern PFN_vkReleasePerformanceConfigurationINTEL ReleasePerformanceConfigurationINTEL;
        extern PFN_vkUninitializePerformanceApiINTEL UninitializePerformanceApiINTEL;
    #endif
    #if defined(VK_KHR_acceleration_structure)
        extern PFN_vkBuildAccelerationStructuresKHR BuildAccelerationStructuresKHR;
        extern PFN_vkCmdBuildAccelerationStructuresIndirectKHR CmdBuildAccelerationStructuresIndirectKHR;
        extern PFN_vkCmdBuildAccelerationStructuresKHR CmdBuildAccelerationStructuresKHR;
        extern PFN_vkCmdCopyAccelerationStructureKHR CmdCopyAccelerationStructureKHR;
        extern PFN_vkCmdCopyAccelerationStructureToMemoryKHR CmdCopyAccelerationStructureToMemoryKHR;
        extern PFN_vkCmdCopyMemoryToAccelerationStructureKHR CmdCopyMemoryToAccelerationStructureKHR;
        extern PFN_vkCmdWriteAccelerationStructuresPropertiesKHR CmdWriteAccelerationStructuresPropertiesKHR;
        extern PFN_vkCopyAccelerationStructureKHR CopyAccelerationStructureKHR;
        extern PFN_vkCopyAccelerationStructureToMemoryKHR CopyAccelerationStructureToMemoryKHR;
        extern PFN_vkCopyMemoryToAccelerationStructureKHR CopyMemoryToAccelerationStructureKHR;
        extern PFN_vkCreateAccelerationStructureKHR CreateAccelerationStructureKHR;
        extern PFN_vkDestroyAccelerationStructureKHR DestroyAccelerationStructureKHR;
        extern PFN_vkGetAccelerationStructureBuildSizesKHR GetAccelerationStructureBuildSizesKHR;
        extern PFN_vkGetAccelerationStructureDeviceAddressKHR GetAccelerationStructureDeviceAddressKHR;
        extern PFN_vkGetDeviceAccelerationStructureCompatibilityKHR GetDeviceAccelerationStructureCompatibilityKHR;
        extern PFN_vkWriteAccelerationStructuresPropertiesKHR WriteAccelerationStructuresPropertiesKHR;
    #endif
    #if defined(VK_KHR_android_surface)
        extern PFN_vkCreateAndroidSurfaceKHR CreateAndroidSurfaceKHR;
    #endif
    #if defined(VK_KHR_bind_memory2)
        extern PFN_vkBindBufferMemory2KHR BindBufferMemory2KHR;
        extern PFN_vkBindImageMemory2KHR BindImageMemory2KHR;
    #endif
    #if defined(VK_KHR_buffer_device_address)
        extern PFN_vkGetBufferDeviceAddressKHR GetBufferDeviceAddressKHR;
        extern PFN_vkGetBufferOpaqueCaptureAddressKHR GetBufferOpaqueCaptureAddressKHR;
        extern PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR GetDeviceMemoryOpaqueCaptureAddressKHR;
    #endif
    #if defined(VK_KHR_cooperative_matrix)
        extern PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR GetPhysicalDeviceCooperativeMatrixPropertiesKHR;
    #endif
    #if defined(VK_KHR_copy_commands2)
        extern PFN_vkCmdBlitImage2KHR CmdBlitImage2KHR;
        extern PFN_vkCmdCopyBuffer2KHR CmdCopyBuffer2KHR;
        extern PFN_vkCmdCopyBufferToImage2KHR CmdCopyBufferToImage2KHR;
        extern PFN_vkCmdCopyImage2KHR CmdCopyImage2KHR;
        extern PFN_vkCmdCopyImageToBuffer2KHR CmdCopyImageToBuffer2KHR;
        extern PFN_vkCmdResolveImage2KHR CmdResolveImage2KHR;
    #endif
    #if defined(VK_KHR_create_renderpass2)
        extern PFN_vkCmdBeginRenderPass2KHR CmdBeginRenderPass2KHR;
        extern PFN_vkCmdEndRenderPass2KHR CmdEndRenderPass2KHR;
        extern PFN_vkCmdNextSubpass2KHR CmdNextSubpass2KHR;
        extern PFN_vkCreateRenderPass2KHR CreateRenderPass2KHR;
    #endif
    #if defined(VK_KHR_deferred_host_operations)
        extern PFN_vkCreateDeferredOperationKHR CreateDeferredOperationKHR;
        extern PFN_vkDeferredOperationJoinKHR DeferredOperationJoinKHR;
        extern PFN_vkDestroyDeferredOperationKHR DestroyDeferredOperationKHR;
        extern PFN_vkGetDeferredOperationMaxConcurrencyKHR GetDeferredOperationMaxConcurrencyKHR;
        extern PFN_vkGetDeferredOperationResultKHR GetDeferredOperationResultKHR;
    #endif
    #if defined(VK_KHR_descriptor_update_template)
        extern PFN_vkCreateDescriptorUpdateTemplateKHR CreateDescriptorUpdateTemplateKHR;
        extern PFN_vkDestroyDescriptorUpdateTemplateKHR DestroyDescriptorUpdateTemplateKHR;
        extern PFN_vkUpdateDescriptorSetWithTemplateKHR UpdateDescriptorSetWithTemplateKHR;
    #endif
    #if defined(VK_KHR_device_group)
        extern PFN_vkCmdDispatchBaseKHR CmdDispatchBaseKHR;
        extern PFN_vkCmdSetDeviceMaskKHR CmdSetDeviceMaskKHR;
        extern PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR GetDeviceGroupPeerMemoryFeaturesKHR;
    #endif
    #if defined(VK_KHR_device_group_creation)
        extern PFN_vkEnumeratePhysicalDeviceGroupsKHR EnumeratePhysicalDeviceGroupsKHR;
    #endif
    #if defined(VK_KHR_display)
        extern PFN_vkCreateDisplayModeKHR CreateDisplayModeKHR;
        extern PFN_vkCreateDisplayPlaneSurfaceKHR CreateDisplayPlaneSurfaceKHR;
        extern PFN_vkGetDisplayModePropertiesKHR GetDisplayModePropertiesKHR;
        extern PFN_vkGetDisplayPlaneCapabilitiesKHR GetDisplayPlaneCapabilitiesKHR;
        extern PFN_vkGetDisplayPlaneSupportedDisplaysKHR GetDisplayPlaneSupportedDisplaysKHR;
        extern PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR GetPhysicalDeviceDisplayPlanePropertiesKHR;
        extern PFN_vkGetPhysicalDeviceDisplayPropertiesKHR GetPhysicalDeviceDisplayPropertiesKHR;
    #endif
    #if defined(VK_KHR_display_swapchain)
        extern PFN_vkCreateSharedSwapchainsKHR CreateSharedSwapchainsKHR;
    #endif
    #if defined(VK_KHR_draw_indirect_count)
        extern PFN_vkCmdDrawIndexedIndirectCountKHR CmdDrawIndexedIndirectCountKHR;
        extern PFN_vkCmdDrawIndirectCountKHR CmdDrawIndirectCountKHR;
    #endif
    #if defined(VK_KHR_dynamic_rendering)
        extern PFN_vkCmdBeginRenderingKHR CmdBeginRenderingKHR;
        extern PFN_vkCmdEndRenderingKHR CmdEndRenderingKHR;
    #endif
    #if defined(VK_KHR_external_fence_capabilities)
        extern PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR GetPhysicalDeviceExternalFencePropertiesKHR;
    #endif
    #if defined(VK_KHR_external_fence_fd)
        extern PFN_vkGetFenceFdKHR GetFenceFdKHR;
        extern PFN_vkImportFenceFdKHR ImportFenceFdKHR;
    #endif
    #if defined(VK_KHR_external_fence_win32)
        extern PFN_vkGetFenceWin32HandleKHR GetFenceWin32HandleKHR;
        extern PFN_vkImportFenceWin32HandleKHR ImportFenceWin32HandleKHR;
    #endif
    #if defined(VK_KHR_external_memory_capabilities)
        extern PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR GetPhysicalDeviceExternalBufferPropertiesKHR;
    #endif
    #if defined(VK_KHR_external_memory_fd)
        extern PFN_vkGetMemoryFdKHR GetMemoryFdKHR;
        extern PFN_vkGetMemoryFdPropertiesKHR GetMemoryFdPropertiesKHR;
    #endif
    #if defined(VK_KHR_external_memory_win32)
        extern PFN_vkGetMemoryWin32HandleKHR GetMemoryWin32HandleKHR;
        extern PFN_vkGetMemoryWin32HandlePropertiesKHR GetMemoryWin32HandlePropertiesKHR;
    #endif
    #if defined(VK_KHR_external_semaphore_capabilities)
        extern PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR GetPhysicalDeviceExternalSemaphorePropertiesKHR;
    #endif
    #if defined(VK_KHR_external_semaphore_fd)
        extern PFN_vkGetSemaphoreFdKHR GetSemaphoreFdKHR;
        extern PFN_vkImportSemaphoreFdKHR ImportSemaphoreFdKHR;
    #endif
    #if defined(VK_KHR_external_semaphore_win32)
        extern PFN_vkGetSemaphoreWin32HandleKHR GetSemaphoreWin32HandleKHR;
        extern PFN_vkImportSemaphoreWin32HandleKHR ImportSemaphoreWin32HandleKHR;
    #endif
    #if defined(VK_KHR_fragment_shading_rate)
        extern PFN_vkCmdSetFragmentShadingRateKHR CmdSetFragmentShadingRateKHR;
        extern PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR GetPhysicalDeviceFragmentShadingRatesKHR;
    #endif
    #if defined(VK_KHR_get_display_properties2)
        extern PFN_vkGetDisplayModeProperties2KHR GetDisplayModeProperties2KHR;
        extern PFN_vkGetDisplayPlaneCapabilities2KHR GetDisplayPlaneCapabilities2KHR;
        extern PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR GetPhysicalDeviceDisplayPlaneProperties2KHR;
        extern PFN_vkGetPhysicalDeviceDisplayProperties2KHR GetPhysicalDeviceDisplayProperties2KHR;
    #endif
    #if defined(VK_KHR_get_memory_requirements2)
        extern PFN_vkGetBufferMemoryRequirements2KHR GetBufferMemoryRequirements2KHR;
        extern PFN_vkGetImageMemoryRequirements2KHR GetImageMemoryRequirements2KHR;
        extern PFN_vkGetImageSparseMemoryRequirements2KHR GetImageSparseMemoryRequirements2KHR;
    #endif
    #if defined(VK_KHR_get_physical_device_properties2)
        extern PFN_vkGetPhysicalDeviceFeatures2KHR GetPhysicalDeviceFeatures2KHR;
        extern PFN_vkGetPhysicalDeviceFormatProperties2KHR GetPhysicalDeviceFormatProperties2KHR;
        extern PFN_vkGetPhysicalDeviceImageFormatProperties2KHR GetPhysicalDeviceImageFormatProperties2KHR;
        extern PFN_vkGetPhysicalDeviceMemoryProperties2KHR GetPhysicalDeviceMemoryProperties2KHR;
        extern PFN_vkGetPhysicalDeviceProperties2KHR GetPhysicalDeviceProperties2KHR;
        extern PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR GetPhysicalDeviceQueueFamilyProperties2KHR;
        extern PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR GetPhysicalDeviceSparseImageFormatProperties2KHR;
    #endif
    #if defined(VK_KHR_get_surface_capabilities2)
        extern PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR GetPhysicalDeviceSurfaceCapabilities2KHR;
        extern PFN_vkGetPhysicalDeviceSurfaceFormats2KHR GetPhysicalDeviceSurfaceFormats2KHR;
    #endif
    #if defined(VK_KHR_maintenance1)
        extern PFN_vkTrimCommandPoolKHR TrimCommandPoolKHR;
    #endif
    #if defined(VK_KHR_maintenance3)
        extern PFN_vkGetDescriptorSetLayoutSupportKHR GetDescriptorSetLayoutSupportKHR;
    #endif
    #if defined(VK_KHR_maintenance4)
        extern PFN_vkGetDeviceBufferMemoryRequirementsKHR GetDeviceBufferMemoryRequirementsKHR;
        extern PFN_vkGetDeviceImageMemoryRequirementsKHR GetDeviceImageMemoryRequirementsKHR;
        extern PFN_vkGetDeviceImageSparseMemoryRequirementsKHR GetDeviceImageSparseMemoryRequirementsKHR;
    #endif
    #if defined(VK_KHR_maintenance5)
        extern PFN_vkCmdBindIndexBuffer2KHR CmdBindIndexBuffer2KHR;
        extern PFN_vkGetDeviceImageSubresourceLayoutKHR GetDeviceImageSubresourceLayoutKHR;
        extern PFN_vkGetImageSubresourceLayout2KHR GetImageSubresourceLayout2KHR;
        extern PFN_vkGetRenderingAreaGranularityKHR GetRenderingAreaGranularityKHR;
    #endif
    #if defined(VK_KHR_map_memory2)
        extern PFN_vkMapMemory2KHR MapMemory2KHR;
        extern PFN_vkUnmapMemory2KHR UnmapMemory2KHR;
    #endif
    #if defined(VK_KHR_performance_query)
        extern PFN_vkAcquireProfilingLockKHR AcquireProfilingLockKHR;
        extern PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR;
        extern PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR;
        extern PFN_vkReleaseProfilingLockKHR ReleaseProfilingLockKHR;
    #endif
    #if defined(VK_KHR_pipeline_executable_properties)
        extern PFN_vkGetPipelineExecutableInternalRepresentationsKHR GetPipelineExecutableInternalRepresentationsKHR;
        extern PFN_vkGetPipelineExecutablePropertiesKHR GetPipelineExecutablePropertiesKHR;
        extern PFN_vkGetPipelineExecutableStatisticsKHR GetPipelineExecutableStatisticsKHR;
    #endif
    #if defined(VK_KHR_present_wait)
        extern PFN_vkWaitForPresentKHR WaitForPresentKHR;
    #endif
    #if defined(VK_KHR_push_descriptor)
        extern PFN_vkCmdPushDescriptorSetKHR CmdPushDescriptorSetKHR;
    #endif
    #if defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline)
        extern PFN_vkCmdTraceRaysIndirect2KHR CmdTraceRaysIndirect2KHR;
    #endif
    #if defined(VK_KHR_ray_tracing_pipeline)
        extern PFN_vkCmdSetRayTracingPipelineStackSizeKHR CmdSetRayTracingPipelineStackSizeKHR;
        extern PFN_vkCmdTraceRaysIndirectKHR CmdTraceRaysIndirectKHR;
        extern PFN_vkCmdTraceRaysKHR CmdTraceRaysKHR;
        extern PFN_vkCreateRayTracingPipelinesKHR CreateRayTracingPipelinesKHR;
        extern PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR GetRayTracingCaptureReplayShaderGroupHandlesKHR;
        extern PFN_vkGetRayTracingShaderGroupHandlesKHR GetRayTracingShaderGroupHandlesKHR;
        extern PFN_vkGetRayTracingShaderGroupStackSizeKHR GetRayTracingShaderGroupStackSizeKHR;
    #endif
    #if defined(VK_KHR_sampler_ycbcr_conversion)
        extern PFN_vkCreateSamplerYcbcrConversionKHR CreateSamplerYcbcrConversionKHR;
        extern PFN_vkDestroySamplerYcbcrConversionKHR DestroySamplerYcbcrConversionKHR;
    #endif
    #if defined(VK_KHR_shared_presentable_image)
        extern PFN_vkGetSwapchainStatusKHR GetSwapchainStatusKHR;
    #endif
    #if defined(VK_KHR_surface)
        extern PFN_vkDestroySurfaceKHR DestroySurfaceKHR;
        extern PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR GetPhysicalDeviceSurfaceCapabilitiesKHR;
        extern PFN_vkGetPhysicalDeviceSurfaceFormatsKHR GetPhysicalDeviceSurfaceFormatsKHR;
        extern PFN_vkGetPhysicalDeviceSurfacePresentModesKHR GetPhysicalDeviceSurfacePresentModesKHR;
        extern PFN_vkGetPhysicalDeviceSurfaceSupportKHR GetPhysicalDeviceSurfaceSupportKHR;
    #endif
    #if defined(VK_KHR_swapchain)
        extern PFN_vkAcquireNextImageKHR AcquireNextImageKHR;
        extern PFN_vkCreateSwapchainKHR CreateSwapchainKHR;
        extern PFN_vkDestroySwapchainKHR DestroySwapchainKHR;
        extern PFN_vkGetSwapchainImagesKHR GetSwapchainImagesKHR;
        extern PFN_vkQueuePresentKHR QueuePresentKHR;
    #endif
    #if defined(VK_KHR_synchronization2)
        extern PFN_vkCmdPipelineBarrier2KHR CmdPipelineBarrier2KHR;
        extern PFN_vkCmdResetEvent2KHR CmdResetEvent2KHR;
        extern PFN_vkCmdSetEvent2KHR CmdSetEvent2KHR;
        extern PFN_vkCmdWaitEvents2KHR CmdWaitEvents2KHR;
        extern PFN_vkCmdWriteTimestamp2KHR CmdWriteTimestamp2KHR;
        extern PFN_vkQueueSubmit2KHR QueueSubmit2KHR;
    #endif
    #if defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker)
        extern PFN_vkCmdWriteBufferMarker2AMD CmdWriteBufferMarker2AMD;
    #endif
    #if defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints)
        extern PFN_vkGetQueueCheckpointData2NV GetQueueCheckpointData2NV;
    #endif
    #if defined(VK_KHR_timeline_semaphore)
        extern PFN_vkGetSemaphoreCounterValueKHR GetSemaphoreCounterValueKHR;
        extern PFN_vkSignalSemaphoreKHR SignalSemaphoreKHR;
        extern PFN_vkWaitSemaphoresKHR WaitSemaphoresKHR;
    #endif
    #if defined(VK_KHR_video_decode_queue)
        extern PFN_vkCmdDecodeVideoKHR CmdDecodeVideoKHR;
    #endif
    #if defined(VK_KHR_video_encode_queue)
        extern PFN_vkCmdEncodeVideoKHR CmdEncodeVideoKHR;
        extern PFN_vkGetEncodedVideoSessionParametersKHR GetEncodedVideoSessionParametersKHR;
        extern PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR GetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR;
    #endif
    #if defined(VK_KHR_video_queue)
        extern PFN_vkBindVideoSessionMemoryKHR BindVideoSessionMemoryKHR;
        extern PFN_vkCmdBeginVideoCodingKHR CmdBeginVideoCodingKHR;
        extern PFN_vkCmdControlVideoCodingKHR CmdControlVideoCodingKHR;
        extern PFN_vkCmdEndVideoCodingKHR CmdEndVideoCodingKHR;
        extern PFN_vkCreateVideoSessionKHR CreateVideoSessionKHR;
        extern PFN_vkCreateVideoSessionParametersKHR CreateVideoSessionParametersKHR;
        extern PFN_vkDestroyVideoSessionKHR DestroyVideoSessionKHR;
        extern PFN_vkDestroyVideoSessionParametersKHR DestroyVideoSessionParametersKHR;
        extern PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR GetPhysicalDeviceVideoCapabilitiesKHR;
        extern PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR GetPhysicalDeviceVideoFormatPropertiesKHR;
        extern PFN_vkGetVideoSessionMemoryRequirementsKHR GetVideoSessionMemoryRequirementsKHR;
        extern PFN_vkUpdateVideoSessionParametersKHR UpdateVideoSessionParametersKHR;
    #endif
    #if defined(VK_KHR_wayland_surface)
        extern PFN_vkCreateWaylandSurfaceKHR CreateWaylandSurfaceKHR;
        extern PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR GetPhysicalDeviceWaylandPresentationSupportKHR;
    #endif
    #if defined(VK_KHR_win32_surface)
        extern PFN_vkCreateWin32SurfaceKHR CreateWin32SurfaceKHR;
        extern PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR GetPhysicalDeviceWin32PresentationSupportKHR;
    #endif
    #if defined(VK_KHR_xcb_surface)
        extern PFN_vkCreateXcbSurfaceKHR CreateXcbSurfaceKHR;
        extern PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR GetPhysicalDeviceXcbPresentationSupportKHR;
    #endif
    #if defined(VK_KHR_xlib_surface)
        extern PFN_vkCreateXlibSurfaceKHR CreateXlibSurfaceKHR;
        extern PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR GetPhysicalDeviceXlibPresentationSupportKHR;
    #endif
    #if defined(VK_MVK_ios_surface)
        extern PFN_vkCreateIOSSurfaceMVK CreateIOSSurfaceMVK;
    #endif
    #if defined(VK_MVK_macos_surface)
        extern PFN_vkCreateMacOSSurfaceMVK CreateMacOSSurfaceMVK;
    #endif
    #if defined(VK_NN_vi_surface)
        extern PFN_vkCreateViSurfaceNN CreateViSurfaceNN;
    #endif
    #if defined(VK_NVX_binary_import)
        extern PFN_vkCmdCuLaunchKernelNVX CmdCuLaunchKernelNVX;
        extern PFN_vkCreateCuFunctionNVX CreateCuFunctionNVX;
        extern PFN_vkCreateCuModuleNVX CreateCuModuleNVX;
        extern PFN_vkDestroyCuFunctionNVX DestroyCuFunctionNVX;
        extern PFN_vkDestroyCuModuleNVX DestroyCuModuleNVX;
    #endif
    #if defined(VK_NVX_image_view_handle)
        extern PFN_vkGetImageViewAddressNVX GetImageViewAddressNVX;
        extern PFN_vkGetImageViewHandleNVX GetImageViewHandleNVX;
    #endif
    #if defined(VK_NV_acquire_winrt_display)
        extern PFN_vkAcquireWinrtDisplayNV AcquireWinrtDisplayNV;
        extern PFN_vkGetWinrtDisplayNV GetWinrtDisplayNV;
    #endif
    #if defined(VK_NV_clip_space_w_scaling)
        extern PFN_vkCmdSetViewportWScalingNV CmdSetViewportWScalingNV;
    #endif
    #if defined(VK_NV_cooperative_matrix)
        extern PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV GetPhysicalDeviceCooperativeMatrixPropertiesNV;
    #endif
    #if defined(VK_NV_copy_memory_indirect)
        extern PFN_vkCmdCopyMemoryIndirectNV CmdCopyMemoryIndirectNV;
        extern PFN_vkCmdCopyMemoryToImageIndirectNV CmdCopyMemoryToImageIndirectNV;
    #endif
    #if defined(VK_NV_coverage_reduction_mode)
        extern PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV;
    #endif
    #if defined(VK_NV_device_diagnostic_checkpoints)
        extern PFN_vkCmdSetCheckpointNV CmdSetCheckpointNV;
        extern PFN_vkGetQueueCheckpointDataNV GetQueueCheckpointDataNV;
    #endif
    #if defined(VK_NV_device_generated_commands)
        extern PFN_vkCmdBindPipelineShaderGroupNV CmdBindPipelineShaderGroupNV;
        extern PFN_vkCmdExecuteGeneratedCommandsNV CmdExecuteGeneratedCommandsNV;
        extern PFN_vkCmdPreprocessGeneratedCommandsNV CmdPreprocessGeneratedCommandsNV;
        extern PFN_vkCreateIndirectCommandsLayoutNV CreateIndirectCommandsLayoutNV;
        extern PFN_vkDestroyIndirectCommandsLayoutNV DestroyIndirectCommandsLayoutNV;
        extern PFN_vkGetGeneratedCommandsMemoryRequirementsNV GetGeneratedCommandsMemoryRequirementsNV;
    #endif
    #if defined(VK_NV_device_generated_commands_compute)
        extern PFN_vkCmdUpdatePipelineIndirectBufferNV CmdUpdatePipelineIndirectBufferNV;
        extern PFN_vkGetPipelineIndirectDeviceAddressNV GetPipelineIndirectDeviceAddressNV;
        extern PFN_vkGetPipelineIndirectMemoryRequirementsNV GetPipelineIndirectMemoryRequirementsNV;
    #endif
    #if defined(VK_NV_external_memory_capabilities)
        extern PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV GetPhysicalDeviceExternalImageFormatPropertiesNV;
    #endif
    #if defined(VK_NV_external_memory_rdma)
        extern PFN_vkGetMemoryRemoteAddressNV GetMemoryRemoteAddressNV;
    #endif
    #if defined(VK_NV_external_memory_win32)
        extern PFN_vkGetMemoryWin32HandleNV GetMemoryWin32HandleNV;
    #endif
    #if defined(VK_NV_fragment_shading_rate_enums)
        extern PFN_vkCmdSetFragmentShadingRateEnumNV CmdSetFragmentShadingRateEnumNV;
    #endif
    #if defined(VK_NV_memory_decompression)
        extern PFN_vkCmdDecompressMemoryIndirectCountNV CmdDecompressMemoryIndirectCountNV;
        extern PFN_vkCmdDecompressMemoryNV CmdDecompressMemoryNV;
    #endif
    #if defined(VK_NV_mesh_shader)
        extern PFN_vkCmdDrawMeshTasksIndirectCountNV CmdDrawMeshTasksIndirectCountNV;
        extern PFN_vkCmdDrawMeshTasksIndirectNV CmdDrawMeshTasksIndirectNV;
        extern PFN_vkCmdDrawMeshTasksNV CmdDrawMeshTasksNV;
    #endif
    #if defined(VK_NV_optical_flow)
        extern PFN_vkBindOpticalFlowSessionImageNV BindOpticalFlowSessionImageNV;
        extern PFN_vkCmdOpticalFlowExecuteNV CmdOpticalFlowExecuteNV;
        extern PFN_vkCreateOpticalFlowSessionNV CreateOpticalFlowSessionNV;
        extern PFN_vkDestroyOpticalFlowSessionNV DestroyOpticalFlowSessionNV;
        extern PFN_vkGetPhysicalDeviceOpticalFlowImageFormatsNV GetPhysicalDeviceOpticalFlowImageFormatsNV;
    #endif
    #if defined(VK_NV_ray_tracing)
        extern PFN_vkBindAccelerationStructureMemoryNV BindAccelerationStructureMemoryNV;
        extern PFN_vkCmdBuildAccelerationStructureNV CmdBuildAccelerationStructureNV;
        extern PFN_vkCmdCopyAccelerationStructureNV CmdCopyAccelerationStructureNV;
        extern PFN_vkCmdTraceRaysNV CmdTraceRaysNV;
        extern PFN_vkCmdWriteAccelerationStructuresPropertiesNV CmdWriteAccelerationStructuresPropertiesNV;
        extern PFN_vkCompileDeferredNV CompileDeferredNV;
        extern PFN_vkCreateAccelerationStructureNV CreateAccelerationStructureNV;
        extern PFN_vkCreateRayTracingPipelinesNV CreateRayTracingPipelinesNV;
        extern PFN_vkDestroyAccelerationStructureNV DestroyAccelerationStructureNV;
        extern PFN_vkGetAccelerationStructureHandleNV GetAccelerationStructureHandleNV;
        extern PFN_vkGetAccelerationStructureMemoryRequirementsNV GetAccelerationStructureMemoryRequirementsNV;
        extern PFN_vkGetRayTracingShaderGroupHandlesNV GetRayTracingShaderGroupHandlesNV;
    #endif
    #if defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2
        extern PFN_vkCmdSetExclusiveScissorEnableNV CmdSetExclusiveScissorEnableNV;
    #endif
    #if defined(VK_NV_scissor_exclusive)
        extern PFN_vkCmdSetExclusiveScissorNV CmdSetExclusiveScissorNV;
    #endif
    #if defined(VK_NV_shading_rate_image)
        extern PFN_vkCmdBindShadingRateImageNV CmdBindShadingRateImageNV;
        extern PFN_vkCmdSetCoarseSampleOrderNV CmdSetCoarseSampleOrderNV;
        extern PFN_vkCmdSetViewportShadingRatePaletteNV CmdSetViewportShadingRatePaletteNV;
    #endif
    #if defined(VK_QCOM_tile_properties)
        extern PFN_vkGetDynamicRenderingTilePropertiesQCOM GetDynamicRenderingTilePropertiesQCOM;
        extern PFN_vkGetFramebufferTilePropertiesQCOM GetFramebufferTilePropertiesQCOM;
    #endif
    #if defined(VK_QNX_external_memory_screen_buffer)
        extern PFN_vkGetScreenBufferPropertiesQNX GetScreenBufferPropertiesQNX;
    #endif
    #if defined(VK_QNX_screen_surface)
        extern PFN_vkCreateScreenSurfaceQNX CreateScreenSurfaceQNX;
        extern PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX GetPhysicalDeviceScreenPresentationSupportQNX;
    #endif
    #if defined(VK_VALVE_descriptor_set_host_mapping)
        extern PFN_vkGetDescriptorSetHostMappingVALVE GetDescriptorSetHostMappingVALVE;
        extern PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE GetDescriptorSetLayoutHostMappingInfoVALVE;
    #endif
    #if (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object))
        extern PFN_vkCmdBindVertexBuffers2EXT CmdBindVertexBuffers2EXT;
        extern PFN_vkCmdSetCullModeEXT CmdSetCullModeEXT;
        extern PFN_vkCmdSetDepthBoundsTestEnableEXT CmdSetDepthBoundsTestEnableEXT;
        extern PFN_vkCmdSetDepthCompareOpEXT CmdSetDepthCompareOpEXT;
        extern PFN_vkCmdSetDepthTestEnableEXT CmdSetDepthTestEnableEXT;
        extern PFN_vkCmdSetDepthWriteEnableEXT CmdSetDepthWriteEnableEXT;
        extern PFN_vkCmdSetFrontFaceEXT CmdSetFrontFaceEXT;
        extern PFN_vkCmdSetPrimitiveTopologyEXT CmdSetPrimitiveTopologyEXT;
        extern PFN_vkCmdSetScissorWithCountEXT CmdSetScissorWithCountEXT;
        extern PFN_vkCmdSetStencilOpEXT CmdSetStencilOpEXT;
        extern PFN_vkCmdSetStencilTestEnableEXT CmdSetStencilTestEnableEXT;
        extern PFN_vkCmdSetViewportWithCountEXT CmdSetViewportWithCountEXT;
    #endif
    #if (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object))
        extern PFN_vkCmdSetDepthBiasEnableEXT CmdSetDepthBiasEnableEXT;
        extern PFN_vkCmdSetLogicOpEXT CmdSetLogicOpEXT;
        extern PFN_vkCmdSetPatchControlPointsEXT CmdSetPatchControlPointsEXT;
        extern PFN_vkCmdSetPrimitiveRestartEnableEXT CmdSetPrimitiveRestartEnableEXT;
        extern PFN_vkCmdSetRasterizerDiscardEnableEXT CmdSetRasterizerDiscardEnableEXT;
    #endif
    #if (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object))
        extern PFN_vkCmdSetAlphaToCoverageEnableEXT CmdSetAlphaToCoverageEnableEXT;
        extern PFN_vkCmdSetAlphaToOneEnableEXT CmdSetAlphaToOneEnableEXT;
        extern PFN_vkCmdSetColorBlendAdvancedEXT CmdSetColorBlendAdvancedEXT;
        extern PFN_vkCmdSetColorBlendEnableEXT CmdSetColorBlendEnableEXT;
        extern PFN_vkCmdSetColorBlendEquationEXT CmdSetColorBlendEquationEXT;
        extern PFN_vkCmdSetColorWriteMaskEXT CmdSetColorWriteMaskEXT;
        extern PFN_vkCmdSetConservativeRasterizationModeEXT CmdSetConservativeRasterizationModeEXT;
        extern PFN_vkCmdSetDepthClampEnableEXT CmdSetDepthClampEnableEXT;
        extern PFN_vkCmdSetDepthClipEnableEXT CmdSetDepthClipEnableEXT;
        extern PFN_vkCmdSetDepthClipNegativeOneToOneEXT CmdSetDepthClipNegativeOneToOneEXT;
        extern PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT CmdSetExtraPrimitiveOverestimationSizeEXT;
        extern PFN_vkCmdSetLineRasterizationModeEXT CmdSetLineRasterizationModeEXT;
        extern PFN_vkCmdSetLineStippleEnableEXT CmdSetLineStippleEnableEXT;
        extern PFN_vkCmdSetLogicOpEnableEXT CmdSetLogicOpEnableEXT;
        extern PFN_vkCmdSetPolygonModeEXT CmdSetPolygonModeEXT;
        extern PFN_vkCmdSetProvokingVertexModeEXT CmdSetProvokingVertexModeEXT;
        extern PFN_vkCmdSetRasterizationSamplesEXT CmdSetRasterizationSamplesEXT;
        extern PFN_vkCmdSetRasterizationStreamEXT CmdSetRasterizationStreamEXT;
        extern PFN_vkCmdSetSampleLocationsEnableEXT CmdSetSampleLocationsEnableEXT;
        extern PFN_vkCmdSetSampleMaskEXT CmdSetSampleMaskEXT;
        extern PFN_vkCmdSetTessellationDomainOriginEXT CmdSetTessellationDomainOriginEXT;
    #endif
    #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling))
        extern PFN_vkCmdSetViewportWScalingEnableNV CmdSetViewportWScalingEnableNV;
    #endif
    #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle))
        extern PFN_vkCmdSetViewportSwizzleNV CmdSetViewportSwizzleNV;
    #endif
    #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color))
        extern PFN_vkCmdSetCoverageToColorEnableNV CmdSetCoverageToColorEnableNV;
        extern PFN_vkCmdSetCoverageToColorLocationNV CmdSetCoverageToColorLocationNV;
    #endif
    #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples))
        extern PFN_vkCmdSetCoverageModulationModeNV CmdSetCoverageModulationModeNV;
        extern PFN_vkCmdSetCoverageModulationTableEnableNV CmdSetCoverageModulationTableEnableNV;
        extern PFN_vkCmdSetCoverageModulationTableNV CmdSetCoverageModulationTableNV;
    #endif
    #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image))
        extern PFN_vkCmdSetShadingRateImageEnableNV CmdSetShadingRateImageEnableNV;
    #endif
    #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test))
        extern PFN_vkCmdSetRepresentativeFragmentTestEnableNV CmdSetRepresentativeFragmentTestEnableNV;
    #endif
    #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode))
        extern PFN_vkCmdSetCoverageReductionModeNV CmdSetCoverageReductionModeNV;
    #endif
    #if (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1))
        extern PFN_vkGetDeviceGroupSurfacePresentModes2EXT GetDeviceGroupSurfacePresentModes2EXT;
    #endif
    #if (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control))
        extern PFN_vkGetImageSubresourceLayout2EXT GetImageSubresourceLayout2EXT;
    #endif
    #if (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state))
        extern PFN_vkCmdSetVertexInputEXT CmdSetVertexInputEXT;
    #endif
    #if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template))
        extern PFN_vkCmdPushDescriptorSetWithTemplateKHR CmdPushDescriptorSetWithTemplateKHR;
    #endif
    #if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
        extern PFN_vkGetDeviceGroupPresentCapabilitiesKHR GetDeviceGroupPresentCapabilitiesKHR;
        extern PFN_vkGetDeviceGroupSurfacePresentModesKHR GetDeviceGroupSurfacePresentModesKHR;
        extern PFN_vkGetPhysicalDevicePresentRectanglesKHR GetPhysicalDevicePresentRectanglesKHR;
    #endif
    #if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
        extern PFN_vkAcquireNextImage2KHR AcquireNextImage2KHR;
    #endif
}