//
// Created by Nikolay Kantchevski on 9/28/26.
//

#pragma once

#include "vulkan.h"

namespace Sierra
{

    struct VulkanDeviceFunctionTable
    {
        #if defined(VK_VERSION_1_0)
            PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers = nullptr;
            PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets = nullptr;
            PFN_vkAllocateMemory vkAllocateMemory = nullptr;
            PFN_vkBeginCommandBuffer vkBeginCommandBuffer = nullptr;
            PFN_vkBindBufferMemory vkBindBufferMemory = nullptr;
            PFN_vkBindImageMemory vkBindImageMemory = nullptr;
            PFN_vkCmdBeginQuery vkCmdBeginQuery = nullptr;
            PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass = nullptr;
            PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets = nullptr;
            PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer = nullptr;
            PFN_vkCmdBindPipeline vkCmdBindPipeline = nullptr;
            PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers = nullptr;
            PFN_vkCmdBlitImage vkCmdBlitImage = nullptr;
            PFN_vkCmdClearAttachments vkCmdClearAttachments = nullptr;
            PFN_vkCmdClearColorImage vkCmdClearColorImage = nullptr;
            PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage = nullptr;
            PFN_vkCmdCopyBuffer vkCmdCopyBuffer = nullptr;
            PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage = nullptr;
            PFN_vkCmdCopyImage vkCmdCopyImage = nullptr;
            PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer = nullptr;
            PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults = nullptr;
            PFN_vkCmdDispatch vkCmdDispatch = nullptr;
            PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect = nullptr;
            PFN_vkCmdDraw vkCmdDraw = nullptr;
            PFN_vkCmdDrawIndexed vkCmdDrawIndexed = nullptr;
            PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect = nullptr;
            PFN_vkCmdDrawIndirect vkCmdDrawIndirect = nullptr;
            PFN_vkCmdEndQuery vkCmdEndQuery = nullptr;
            PFN_vkCmdEndRenderPass vkCmdEndRenderPass = nullptr;
            PFN_vkCmdExecuteCommands vkCmdExecuteCommands = nullptr;
            PFN_vkCmdFillBuffer vkCmdFillBuffer = nullptr;
            PFN_vkCmdNextSubpass vkCmdNextSubpass = nullptr;
            PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier = nullptr;
            PFN_vkCmdPushConstants vkCmdPushConstants = nullptr;
            PFN_vkCmdResetEvent vkCmdResetEvent = nullptr;
            PFN_vkCmdResetQueryPool vkCmdResetQueryPool = nullptr;
            PFN_vkCmdResolveImage vkCmdResolveImage = nullptr;
            PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants = nullptr;
            PFN_vkCmdSetDepthBias vkCmdSetDepthBias = nullptr;
            PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds = nullptr;
            PFN_vkCmdSetEvent vkCmdSetEvent = nullptr;
            PFN_vkCmdSetLineWidth vkCmdSetLineWidth = nullptr;
            PFN_vkCmdSetScissor vkCmdSetScissor = nullptr;
            PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask = nullptr;
            PFN_vkCmdSetStencilReference vkCmdSetStencilReference = nullptr;
            PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask = nullptr;
            PFN_vkCmdSetViewport vkCmdSetViewport = nullptr;
            PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer = nullptr;
            PFN_vkCmdWaitEvents vkCmdWaitEvents = nullptr;
            PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp = nullptr;
            PFN_vkCreateBuffer vkCreateBuffer = nullptr;
            PFN_vkCreateBufferView vkCreateBufferView = nullptr;
            PFN_vkCreateCommandPool vkCreateCommandPool = nullptr;
            PFN_vkCreateComputePipelines vkCreateComputePipelines = nullptr;
            PFN_vkCreateDescriptorPool vkCreateDescriptorPool = nullptr;
            PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout = nullptr;
            PFN_vkCreateEvent vkCreateEvent = nullptr;
            PFN_vkCreateFence vkCreateFence = nullptr;
            PFN_vkCreateFramebuffer vkCreateFramebuffer = nullptr;
            PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines = nullptr;
            PFN_vkCreateImage vkCreateImage = nullptr;
            PFN_vkCreateImageView vkCreateImageView = nullptr;
            PFN_vkCreatePipelineCache vkCreatePipelineCache = nullptr;
            PFN_vkCreatePipelineLayout vkCreatePipelineLayout = nullptr;
            PFN_vkCreateQueryPool vkCreateQueryPool = nullptr;
            PFN_vkCreateRenderPass vkCreateRenderPass = nullptr;
            PFN_vkCreateSampler vkCreateSampler = nullptr;
            PFN_vkCreateSemaphore vkCreateSemaphore = nullptr;
            PFN_vkCreateShaderModule vkCreateShaderModule = nullptr;
            PFN_vkDestroyBuffer vkDestroyBuffer = nullptr;
            PFN_vkDestroyBufferView vkDestroyBufferView = nullptr;
            PFN_vkDestroyCommandPool vkDestroyCommandPool = nullptr;
            PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool = nullptr;
            PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout = nullptr;
            PFN_vkDestroyDevice vkDestroyDevice = nullptr;
            PFN_vkDestroyEvent vkDestroyEvent = nullptr;
            PFN_vkDestroyFence vkDestroyFence = nullptr;
            PFN_vkDestroyFramebuffer vkDestroyFramebuffer = nullptr;
            PFN_vkDestroyImage vkDestroyImage = nullptr;
            PFN_vkDestroyImageView vkDestroyImageView = nullptr;
            PFN_vkDestroyPipeline vkDestroyPipeline = nullptr;
            PFN_vkDestroyPipelineCache vkDestroyPipelineCache = nullptr;
            PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout = nullptr;
            PFN_vkDestroyQueryPool vkDestroyQueryPool = nullptr;
            PFN_vkDestroyRenderPass vkDestroyRenderPass = nullptr;
            PFN_vkDestroySampler vkDestroySampler = nullptr;
            PFN_vkDestroySemaphore vkDestroySemaphore = nullptr;
            PFN_vkDestroyShaderModule vkDestroyShaderModule = nullptr;
            PFN_vkDeviceWaitIdle vkDeviceWaitIdle = nullptr;
            PFN_vkEndCommandBuffer vkEndCommandBuffer = nullptr;
            PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges = nullptr;
            PFN_vkFreeCommandBuffers vkFreeCommandBuffers = nullptr;
            PFN_vkFreeDescriptorSets vkFreeDescriptorSets = nullptr;
            PFN_vkFreeMemory vkFreeMemory = nullptr;
            PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements = nullptr;
            PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment = nullptr;
            PFN_vkGetDeviceQueue vkGetDeviceQueue = nullptr;
            PFN_vkGetEventStatus vkGetEventStatus = nullptr;
            PFN_vkGetFenceStatus vkGetFenceStatus = nullptr;
            PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements = nullptr;
            PFN_vkGetImageSparseMemoryRequirements vkGetImageSparseMemoryRequirements = nullptr;
            PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout = nullptr;
            PFN_vkGetPipelineCacheData vkGetPipelineCacheData = nullptr;
            PFN_vkGetQueryPoolResults vkGetQueryPoolResults = nullptr;
            PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity = nullptr;
            PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges = nullptr;
            PFN_vkMapMemory vkMapMemory = nullptr;
            PFN_vkMergePipelineCaches vkMergePipelineCaches = nullptr;
            PFN_vkQueueBindSparse vkQueueBindSparse = nullptr;
            PFN_vkQueueSubmit vkQueueSubmit = nullptr;
            PFN_vkQueueWaitIdle vkQueueWaitIdle = nullptr;
            PFN_vkResetCommandBuffer vkResetCommandBuffer = nullptr;
            PFN_vkResetCommandPool vkResetCommandPool = nullptr;
            PFN_vkResetDescriptorPool vkResetDescriptorPool = nullptr;
            PFN_vkResetEvent vkResetEvent = nullptr;
            PFN_vkResetFences vkResetFences = nullptr;
            PFN_vkSetEvent vkSetEvent = nullptr;
            PFN_vkUnmapMemory vkUnmapMemory = nullptr;
            PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets = nullptr;
            PFN_vkWaitForFences vkWaitForFences = nullptr;
        #endif
        #if defined(VK_VERSION_1_1)
            PFN_vkBindBufferMemory2 vkBindBufferMemory2 = nullptr;
            PFN_vkBindImageMemory2 vkBindImageMemory2 = nullptr;
            PFN_vkCmdDispatchBase vkCmdDispatchBase = nullptr;
            PFN_vkCmdSetDeviceMask vkCmdSetDeviceMask = nullptr;
            PFN_vkCreateDescriptorUpdateTemplate vkCreateDescriptorUpdateTemplate = nullptr;
            PFN_vkCreateSamplerYcbcrConversion vkCreateSamplerYcbcrConversion = nullptr;
            PFN_vkDestroyDescriptorUpdateTemplate vkDestroyDescriptorUpdateTemplate = nullptr;
            PFN_vkDestroySamplerYcbcrConversion vkDestroySamplerYcbcrConversion = nullptr;
            PFN_vkGetBufferMemoryRequirements2 vkGetBufferMemoryRequirements2 = nullptr;
            PFN_vkGetDescriptorSetLayoutSupport vkGetDescriptorSetLayoutSupport = nullptr;
            PFN_vkGetDeviceGroupPeerMemoryFeatures vkGetDeviceGroupPeerMemoryFeatures = nullptr;
            PFN_vkGetDeviceQueue2 vkGetDeviceQueue2 = nullptr;
            PFN_vkGetImageMemoryRequirements2 vkGetImageMemoryRequirements2 = nullptr;
            PFN_vkGetImageSparseMemoryRequirements2 vkGetImageSparseMemoryRequirements2 = nullptr;
            PFN_vkTrimCommandPool vkTrimCommandPool = nullptr;
            PFN_vkUpdateDescriptorSetWithTemplate vkUpdateDescriptorSetWithTemplate = nullptr;
        #endif
        #if defined(VK_VERSION_1_2)
            PFN_vkCmdBeginRenderPass2 vkCmdBeginRenderPass2 = nullptr;
            PFN_vkCmdDrawIndexedIndirectCount vkCmdDrawIndexedIndirectCount = nullptr;
            PFN_vkCmdDrawIndirectCount vkCmdDrawIndirectCount = nullptr;
            PFN_vkCmdEndRenderPass2 vkCmdEndRenderPass2 = nullptr;
            PFN_vkCmdNextSubpass2 vkCmdNextSubpass2 = nullptr;
            PFN_vkCreateRenderPass2 vkCreateRenderPass2 = nullptr;
            PFN_vkGetBufferDeviceAddress vkGetBufferDeviceAddress = nullptr;
            PFN_vkGetBufferOpaqueCaptureAddress vkGetBufferOpaqueCaptureAddress = nullptr;
            PFN_vkGetDeviceMemoryOpaqueCaptureAddress vkGetDeviceMemoryOpaqueCaptureAddress = nullptr;
            PFN_vkGetSemaphoreCounterValue vkGetSemaphoreCounterValue = nullptr;
            PFN_vkResetQueryPool vkResetQueryPool = nullptr;
            PFN_vkSignalSemaphore vkSignalSemaphore = nullptr;
            PFN_vkWaitSemaphores vkWaitSemaphores = nullptr;
        #endif
        #if defined(VK_VERSION_1_3)
            PFN_vkCmdBeginRendering vkCmdBeginRendering = nullptr;
            PFN_vkCmdBindVertexBuffers2 vkCmdBindVertexBuffers2 = nullptr;
            PFN_vkCmdBlitImage2 vkCmdBlitImage2 = nullptr;
            PFN_vkCmdCopyBuffer2 vkCmdCopyBuffer2 = nullptr;
            PFN_vkCmdCopyBufferToImage2 vkCmdCopyBufferToImage2 = nullptr;
            PFN_vkCmdCopyImage2 vkCmdCopyImage2 = nullptr;
            PFN_vkCmdCopyImageToBuffer2 vkCmdCopyImageToBuffer2 = nullptr;
            PFN_vkCmdEndRendering vkCmdEndRendering = nullptr;
            PFN_vkCmdPipelineBarrier2 vkCmdPipelineBarrier2 = nullptr;
            PFN_vkCmdResetEvent2 vkCmdResetEvent2 = nullptr;
            PFN_vkCmdResolveImage2 vkCmdResolveImage2 = nullptr;
            PFN_vkCmdSetCullMode vkCmdSetCullMode = nullptr;
            PFN_vkCmdSetDepthBiasEnable vkCmdSetDepthBiasEnable = nullptr;
            PFN_vkCmdSetDepthBoundsTestEnable vkCmdSetDepthBoundsTestEnable = nullptr;
            PFN_vkCmdSetDepthCompareOp vkCmdSetDepthCompareOp = nullptr;
            PFN_vkCmdSetDepthTestEnable vkCmdSetDepthTestEnable = nullptr;
            PFN_vkCmdSetDepthWriteEnable vkCmdSetDepthWriteEnable = nullptr;
            PFN_vkCmdSetEvent2 vkCmdSetEvent2 = nullptr;
            PFN_vkCmdSetFrontFace vkCmdSetFrontFace = nullptr;
            PFN_vkCmdSetPrimitiveRestartEnable vkCmdSetPrimitiveRestartEnable = nullptr;
            PFN_vkCmdSetPrimitiveTopology vkCmdSetPrimitiveTopology = nullptr;
            PFN_vkCmdSetRasterizerDiscardEnable vkCmdSetRasterizerDiscardEnable = nullptr;
            PFN_vkCmdSetScissorWithCount vkCmdSetScissorWithCount = nullptr;
            PFN_vkCmdSetStencilOp vkCmdSetStencilOp = nullptr;
            PFN_vkCmdSetStencilTestEnable vkCmdSetStencilTestEnable = nullptr;
            PFN_vkCmdSetViewportWithCount vkCmdSetViewportWithCount = nullptr;
            PFN_vkCmdWaitEvents2 vkCmdWaitEvents2 = nullptr;
            PFN_vkCmdWriteTimestamp2 vkCmdWriteTimestamp2 = nullptr;
            PFN_vkCreatePrivateDataSlot vkCreatePrivateDataSlot = nullptr;
            PFN_vkDestroyPrivateDataSlot vkDestroyPrivateDataSlot = nullptr;
            PFN_vkGetDeviceBufferMemoryRequirements vkGetDeviceBufferMemoryRequirements = nullptr;
            PFN_vkGetDeviceImageMemoryRequirements vkGetDeviceImageMemoryRequirements = nullptr;
            PFN_vkGetDeviceImageSparseMemoryRequirements vkGetDeviceImageSparseMemoryRequirements = nullptr;
            PFN_vkGetPrivateData vkGetPrivateData = nullptr;
            PFN_vkQueueSubmit2 vkQueueSubmit2 = nullptr;
            PFN_vkSetPrivateData vkSetPrivateData = nullptr;
        #endif
        #if defined(VK_AMDX_shader_enqueue)
            PFN_vkCmdDispatchGraphAMDX vkCmdDispatchGraphAMDX = nullptr;
                PFN_vkCmdDispatchGraphIndirectAMDX vkCmdDispatchGraphIndirectAMDX = nullptr;
                PFN_vkCmdDispatchGraphIndirectCountAMDX vkCmdDispatchGraphIndirectCountAMDX = nullptr;
                PFN_vkCmdInitializeGraphScratchMemoryAMDX vkCmdInitializeGraphScratchMemoryAMDX = nullptr;
                PFN_vkCreateExecutionGraphPipelinesAMDX vkCreateExecutionGraphPipelinesAMDX = nullptr;
                PFN_vkGetExecutionGraphPipelineNodeIndexAMDX vkGetExecutionGraphPipelineNodeIndexAMDX = nullptr;
                PFN_vkGetExecutionGraphPipelineScratchSizeAMDX vkGetExecutionGraphPipelineScratchSizeAMDX = nullptr;
        #endif
        #if defined(VK_AMD_buffer_marker)
            PFN_vkCmdWriteBufferMarkerAMD vkCmdWriteBufferMarkerAMD = nullptr;
        #endif
        #if defined(VK_AMD_display_native_hdr)
            PFN_vkSetLocalDimmingAMD vkSetLocalDimmingAMD = nullptr;
        #endif
        #if defined(VK_AMD_draw_indirect_count)
            PFN_vkCmdDrawIndexedIndirectCountAMD vkCmdDrawIndexedIndirectCountAMD = nullptr;
            PFN_vkCmdDrawIndirectCountAMD vkCmdDrawIndirectCountAMD = nullptr;
        #endif
        #if defined(VK_AMD_shader_info)
            PFN_vkGetShaderInfoAMD vkGetShaderInfoAMD = nullptr;
        #endif
        #if defined(VK_ANDROID_external_memory_android_hardware_buffer)
            PFN_vkGetAndroidHardwareBufferPropertiesANDROID vkGetAndroidHardwareBufferPropertiesANDROID = nullptr;
                PFN_vkGetMemoryAndroidHardwareBufferANDROID vkGetMemoryAndroidHardwareBufferANDROID = nullptr;
        #endif
        #if defined(VK_EXT_attachment_feedback_loop_dynamic_state)
            PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT vkCmdSetAttachmentFeedbackLoopEnableEXT = nullptr;
        #endif
        #if defined(VK_EXT_buffer_device_address)
            PFN_vkGetBufferDeviceAddressEXT vkGetBufferDeviceAddressEXT = nullptr;
        #endif
        #if defined(VK_EXT_calibrated_timestamps)
            PFN_vkGetCalibratedTimestampsEXT vkGetCalibratedTimestampsEXT = nullptr;
        #endif
        #if defined(VK_EXT_color_write_enable)
            PFN_vkCmdSetColorWriteEnableEXT vkCmdSetColorWriteEnableEXT = nullptr;
        #endif
        #if defined(VK_EXT_conditional_rendering)
            PFN_vkCmdBeginConditionalRenderingEXT vkCmdBeginConditionalRenderingEXT = nullptr;
            PFN_vkCmdEndConditionalRenderingEXT vkCmdEndConditionalRenderingEXT = nullptr;
        #endif
        #if defined(VK_EXT_debug_marker)
            PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT = nullptr;
            PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT = nullptr;
            PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT = nullptr;
            PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT = nullptr;
            PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTagEXT = nullptr;
        #endif
        #if defined(VK_EXT_depth_bias_control)
            PFN_vkCmdSetDepthBias2EXT vkCmdSetDepthBias2EXT = nullptr;
        #endif
        #if defined(VK_EXT_descriptor_buffer)
            PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT vkCmdBindDescriptorBufferEmbeddedSamplersEXT = nullptr;
            PFN_vkCmdBindDescriptorBuffersEXT vkCmdBindDescriptorBuffersEXT = nullptr;
            PFN_vkCmdSetDescriptorBufferOffsetsEXT vkCmdSetDescriptorBufferOffsetsEXT = nullptr;
            PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT vkGetBufferOpaqueCaptureDescriptorDataEXT = nullptr;
            PFN_vkGetDescriptorEXT vkGetDescriptorEXT = nullptr;
            PFN_vkGetDescriptorSetLayoutBindingOffsetEXT vkGetDescriptorSetLayoutBindingOffsetEXT = nullptr;
            PFN_vkGetDescriptorSetLayoutSizeEXT vkGetDescriptorSetLayoutSizeEXT = nullptr;
            PFN_vkGetImageOpaqueCaptureDescriptorDataEXT vkGetImageOpaqueCaptureDescriptorDataEXT = nullptr;
            PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT vkGetImageViewOpaqueCaptureDescriptorDataEXT = nullptr;
            PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT vkGetSamplerOpaqueCaptureDescriptorDataEXT = nullptr;
        #endif
        #if defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing))
            PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT = nullptr;
        #endif
        #if defined(VK_EXT_device_fault)
            PFN_vkGetDeviceFaultInfoEXT vkGetDeviceFaultInfoEXT = nullptr;
        #endif
        #if defined(VK_EXT_discard_rectangles)
            PFN_vkCmdSetDiscardRectangleEXT vkCmdSetDiscardRectangleEXT = nullptr;
        #endif
        #if defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2
            PFN_vkCmdSetDiscardRectangleEnableEXT vkCmdSetDiscardRectangleEnableEXT = nullptr;
            PFN_vkCmdSetDiscardRectangleModeEXT vkCmdSetDiscardRectangleModeEXT = nullptr;
        #endif
        #if defined(VK_EXT_display_control)
            PFN_vkDisplayPowerControlEXT vkDisplayPowerControlEXT = nullptr;
            PFN_vkGetSwapchainCounterEXT vkGetSwapchainCounterEXT = nullptr;
            PFN_vkRegisterDeviceEventEXT vkRegisterDeviceEventEXT = nullptr;
            PFN_vkRegisterDisplayEventEXT vkRegisterDisplayEventEXT = nullptr;
        #endif
        #if defined(VK_EXT_external_memory_host)
            PFN_vkGetMemoryHostPointerPropertiesEXT vkGetMemoryHostPointerPropertiesEXT = nullptr;
        #endif
        #if defined(VK_EXT_full_screen_exclusive)
            PFN_vkAcquireFullScreenExclusiveModeEXT vkAcquireFullScreenExclusiveModeEXT = nullptr;
                PFN_vkReleaseFullScreenExclusiveModeEXT vkReleaseFullScreenExclusiveModeEXT = nullptr;
        #endif
        #if defined(VK_EXT_hdr_metadata)
            PFN_vkSetHdrMetadataEXT vkSetHdrMetadataEXT = nullptr;
        #endif
        #if defined(VK_EXT_host_image_copy)
            PFN_vkCopyImageToImageEXT vkCopyImageToImageEXT = nullptr;
                PFN_vkCopyImageToMemoryEXT vkCopyImageToMemoryEXT = nullptr;
                PFN_vkCopyMemoryToImageEXT vkCopyMemoryToImageEXT = nullptr;
                PFN_vkTransitionImageLayoutEXT vkTransitionImageLayoutEXT = nullptr;
        #endif
        #if defined(VK_EXT_host_query_reset)
            PFN_vkResetQueryPoolEXT vkResetQueryPoolEXT = nullptr;
        #endif
        #if defined(VK_EXT_image_drm_format_modifier)
            PFN_vkGetImageDrmFormatModifierPropertiesEXT vkGetImageDrmFormatModifierPropertiesEXT = nullptr;
        #endif
        #if defined(VK_EXT_line_rasterization)
            PFN_vkCmdSetLineStippleEXT vkCmdSetLineStippleEXT = nullptr;
        #endif
        #if defined(VK_EXT_mesh_shader)
            PFN_vkCmdDrawMeshTasksEXT vkCmdDrawMeshTasksEXT = nullptr;
            PFN_vkCmdDrawMeshTasksIndirectCountEXT vkCmdDrawMeshTasksIndirectCountEXT = nullptr;
            PFN_vkCmdDrawMeshTasksIndirectEXT vkCmdDrawMeshTasksIndirectEXT = nullptr;
        #endif
        #if defined(VK_EXT_vulkan_objects)
            PFN_vkExportVulkanObjectsEXT vkExportVulkanObjectsEXT = nullptr;
        #endif
        #if defined(VK_EXT_multi_draw)
            PFN_vkCmdDrawMultiEXT vkCmdDrawMultiEXT = nullptr;
            PFN_vkCmdDrawMultiIndexedEXT vkCmdDrawMultiIndexedEXT = nullptr;
        #endif
        #if defined(VK_EXT_opacity_micromap)
            PFN_vkBuildMicromapsEXT vkBuildMicromapsEXT = nullptr;
            PFN_vkCmdBuildMicromapsEXT vkCmdBuildMicromapsEXT = nullptr;
            PFN_vkCmdCopyMemoryToMicromapEXT vkCmdCopyMemoryToMicromapEXT = nullptr;
            PFN_vkCmdCopyMicromapEXT vkCmdCopyMicromapEXT = nullptr;
            PFN_vkCmdCopyMicromapToMemoryEXT vkCmdCopyMicromapToMemoryEXT = nullptr;
            PFN_vkCmdWriteMicromapsPropertiesEXT vkCmdWriteMicromapsPropertiesEXT = nullptr;
            PFN_vkCopyMemoryToMicromapEXT vkCopyMemoryToMicromapEXT = nullptr;
            PFN_vkCopyMicromapEXT vkCopyMicromapEXT = nullptr;
            PFN_vkCopyMicromapToMemoryEXT vkCopyMicromapToMemoryEXT = nullptr;
            PFN_vkCreateMicromapEXT vkCreateMicromapEXT = nullptr;
            PFN_vkDestroyMicromapEXT vkDestroyMicromapEXT = nullptr;
            PFN_vkGetDeviceMicromapCompatibilityEXT vkGetDeviceMicromapCompatibilityEXT = nullptr;
            PFN_vkGetMicromapBuildSizesEXT vkGetMicromapBuildSizesEXT = nullptr;
            PFN_vkWriteMicromapsPropertiesEXT vkWriteMicromapsPropertiesEXT = nullptr;
        #endif
        #if defined(VK_EXT_pageable_device_local_memory)
            PFN_vkSetDeviceMemoryPriorityEXT vkSetDeviceMemoryPriorityEXT = nullptr;
        #endif
        #if defined(VK_EXT_pipeline_properties)
            PFN_vkGetPipelinePropertiesEXT vkGetPipelinePropertiesEXT = nullptr;
        #endif
        #if defined(VK_EXT_private_data)
            PFN_vkCreatePrivateDataSlotEXT vkCreatePrivateDataSlotEXT = nullptr;
            PFN_vkDestroyPrivateDataSlotEXT vkDestroyPrivateDataSlotEXT = nullptr;
            PFN_vkGetPrivateDataEXT vkGetPrivateDataEXT = nullptr;
            PFN_vkSetPrivateDataEXT vkSetPrivateDataEXT = nullptr;
        #endif
        #if defined(VK_EXT_sample_locations)
            PFN_vkCmdSetSampleLocationsEXT vkCmdSetSampleLocationsEXT = nullptr;
        #endif
        #if defined(VK_EXT_shader_module_identifier)
            PFN_vkGetShaderModuleCreateInfoIdentifierEXT vkGetShaderModuleCreateInfoIdentifierEXT = nullptr;
            PFN_vkGetShaderModuleIdentifierEXT vkGetShaderModuleIdentifierEXT = nullptr;
        #endif
        #if defined(VK_EXT_shader_object)
            PFN_vkCmdBindShadersEXT vkCmdBindShadersEXT = nullptr;
                PFN_vkCreateShadersEXT vkCreateShadersEXT = nullptr;
                PFN_vkDestroyShaderEXT vkDestroyShaderEXT = nullptr;
                PFN_vkGetShaderBinaryDataEXT vkGetShaderBinaryDataEXT = nullptr;
        #endif
        #if defined(VK_EXT_swapchain_maintenance1)
            PFN_vkReleaseSwapchainImagesEXT vkReleaseSwapchainImagesEXT = nullptr;
        #endif
        #if defined(VK_EXT_transform_feedback)
            PFN_vkCmdBeginQueryIndexedEXT vkCmdBeginQueryIndexedEXT = nullptr;
            PFN_vkCmdBeginTransformFeedbackEXT vkCmdBeginTransformFeedbackEXT = nullptr;
            PFN_vkCmdBindTransformFeedbackBuffersEXT vkCmdBindTransformFeedbackBuffersEXT = nullptr;
            PFN_vkCmdDrawIndirectByteCountEXT vkCmdDrawIndirectByteCountEXT = nullptr;
            PFN_vkCmdEndQueryIndexedEXT vkCmdEndQueryIndexedEXT = nullptr;
            PFN_vkCmdEndTransformFeedbackEXT vkCmdEndTransformFeedbackEXT = nullptr;
        #endif
        #if defined(VK_EXT_validation_cache)
            PFN_vkCreateValidationCacheEXT vkCreateValidationCacheEXT = nullptr;
            PFN_vkDestroyValidationCacheEXT vkDestroyValidationCacheEXT = nullptr;
            PFN_vkGetValidationCacheDataEXT vkGetValidationCacheDataEXT = nullptr;
            PFN_vkMergeValidationCachesEXT vkMergeValidationCachesEXT = nullptr;
        #endif
        #if defined(VK_FUCHSIA_buffer_collection)
            PFN_vkCreateBufferCollectionFUCHSIA vkCreateBufferCollectionFUCHSIA = nullptr;
                PFN_vkDestroyBufferCollectionFUCHSIA vkDestroyBufferCollectionFUCHSIA = nullptr;
                PFN_vkGetBufferCollectionPropertiesFUCHSIA vkGetBufferCollectionPropertiesFUCHSIA = nullptr;
                PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA vkSetBufferCollectionBufferConstraintsFUCHSIA = nullptr;
                PFN_vkSetBufferCollectionImageConstraintsFUCHSIA vkSetBufferCollectionImageConstraintsFUCHSIA = nullptr;
        #endif
        #if defined(VK_FUCHSIA_external_memory)
            PFN_vkGetMemoryZirconHandleFUCHSIA vkGetMemoryZirconHandleFUCHSIA = nullptr;
                PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA vkGetMemoryZirconHandlePropertiesFUCHSIA = nullptr;
        #endif
        #if defined(VK_FUCHSIA_external_semaphore)
            PFN_vkGetSemaphoreZirconHandleFUCHSIA vkGetSemaphoreZirconHandleFUCHSIA = nullptr;
                PFN_vkImportSemaphoreZirconHandleFUCHSIA vkImportSemaphoreZirconHandleFUCHSIA = nullptr;
        #endif
        #if defined(VK_GOOGLE_display_timing)
            PFN_vkGetPastPresentationTimingGOOGLE vkGetPastPresentationTimingGOOGLE = nullptr;
            PFN_vkGetRefreshCycleDurationGOOGLE vkGetRefreshCycleDurationGOOGLE = nullptr;
        #endif
        #if defined(VK_HUAWEI_cluster_culling_shader)
            PFN_vkCmdDrawClusterHUAWEI vkCmdDrawClusterHUAWEI = nullptr;
            PFN_vkCmdDrawClusterIndirectHUAWEI vkCmdDrawClusterIndirectHUAWEI = nullptr;
        #endif
        #if defined(VK_HUAWEI_invocation_mask)
            PFN_vkCmdBindInvocationMaskHUAWEI vkCmdBindInvocationMaskHUAWEI = nullptr;
        #endif
        #if defined(VK_HUAWEI_subpass_shading)
            PFN_vkCmdSubpassShadingHUAWEI vkCmdSubpassShadingHUAWEI = nullptr;
            PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI = nullptr;
        #endif
        #if defined(VK_INTEL_performance_query)
            PFN_vkAcquirePerformanceConfigurationINTEL vkAcquirePerformanceConfigurationINTEL = nullptr;
            PFN_vkCmdSetPerformanceMarkerINTEL vkCmdSetPerformanceMarkerINTEL = nullptr;
            PFN_vkCmdSetPerformanceOverrideINTEL vkCmdSetPerformanceOverrideINTEL = nullptr;
            PFN_vkCmdSetPerformanceStreamMarkerINTEL vkCmdSetPerformanceStreamMarkerINTEL = nullptr;
            PFN_vkGetPerformanceParameterINTEL vkGetPerformanceParameterINTEL = nullptr;
            PFN_vkInitializePerformanceApiINTEL vkInitializePerformanceApiINTEL = nullptr;
            PFN_vkQueueSetPerformanceConfigurationINTEL vkQueueSetPerformanceConfigurationINTEL = nullptr;
            PFN_vkReleasePerformanceConfigurationINTEL vkReleasePerformanceConfigurationINTEL = nullptr;
            PFN_vkUninitializePerformanceApiINTEL vkUninitializePerformanceApiINTEL = nullptr;
        #endif
        #if defined(VK_KHR_acceleration_structure)
            PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR = nullptr;
            PFN_vkCmdBuildAccelerationStructuresIndirectKHR vkCmdBuildAccelerationStructuresIndirectKHR = nullptr;
            PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR = nullptr;
            PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR = nullptr;
            PFN_vkCmdCopyAccelerationStructureToMemoryKHR vkCmdCopyAccelerationStructureToMemoryKHR = nullptr;
            PFN_vkCmdCopyMemoryToAccelerationStructureKHR vkCmdCopyMemoryToAccelerationStructureKHR = nullptr;
            PFN_vkCmdWriteAccelerationStructuresPropertiesKHR vkCmdWriteAccelerationStructuresPropertiesKHR = nullptr;
            PFN_vkCopyAccelerationStructureKHR vkCopyAccelerationStructureKHR = nullptr;
            PFN_vkCopyAccelerationStructureToMemoryKHR vkCopyAccelerationStructureToMemoryKHR = nullptr;
            PFN_vkCopyMemoryToAccelerationStructureKHR vkCopyMemoryToAccelerationStructureKHR = nullptr;
            PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR = nullptr;
            PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR = nullptr;
            PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR = nullptr;
            PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR = nullptr;
            PFN_vkGetDeviceAccelerationStructureCompatibilityKHR vkGetDeviceAccelerationStructureCompatibilityKHR = nullptr;
            PFN_vkWriteAccelerationStructuresPropertiesKHR vkWriteAccelerationStructuresPropertiesKHR = nullptr;
        #endif
        #if defined(VK_KHR_bind_memory2)
            PFN_vkBindBufferMemory2KHR vkBindBufferMemory2KHR = nullptr;
            PFN_vkBindImageMemory2KHR vkBindImageMemory2KHR = nullptr;
        #endif
        #if defined(VK_KHR_buffer_device_address)
            PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR = nullptr;
            PFN_vkGetBufferOpaqueCaptureAddressKHR vkGetBufferOpaqueCaptureAddressKHR = nullptr;
            PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR vkGetDeviceMemoryOpaqueCaptureAddressKHR = nullptr;
        #endif
        #if defined(VK_KHR_copy_commands2)
            PFN_vkCmdBlitImage2KHR vkCmdBlitImage2KHR = nullptr;
            PFN_vkCmdCopyBuffer2KHR vkCmdCopyBuffer2KHR = nullptr;
            PFN_vkCmdCopyBufferToImage2KHR vkCmdCopyBufferToImage2KHR = nullptr;
            PFN_vkCmdCopyImage2KHR vkCmdCopyImage2KHR = nullptr;
            PFN_vkCmdCopyImageToBuffer2KHR vkCmdCopyImageToBuffer2KHR = nullptr;
            PFN_vkCmdResolveImage2KHR vkCmdResolveImage2KHR = nullptr;
        #endif
        #if defined(VK_KHR_create_renderpass2)
            PFN_vkCmdBeginRenderPass2KHR vkCmdBeginRenderPass2KHR = nullptr;
            PFN_vkCmdEndRenderPass2KHR vkCmdEndRenderPass2KHR = nullptr;
            PFN_vkCmdNextSubpass2KHR vkCmdNextSubpass2KHR = nullptr;
            PFN_vkCreateRenderPass2KHR vkCreateRenderPass2KHR = nullptr;
        #endif
        #if defined(VK_KHR_deferred_host_operations)
            PFN_vkCreateDeferredOperationKHR vkCreateDeferredOperationKHR = nullptr;
            PFN_vkDeferredOperationJoinKHR vkDeferredOperationJoinKHR = nullptr;
            PFN_vkDestroyDeferredOperationKHR vkDestroyDeferredOperationKHR = nullptr;
            PFN_vkGetDeferredOperationMaxConcurrencyKHR vkGetDeferredOperationMaxConcurrencyKHR = nullptr;
            PFN_vkGetDeferredOperationResultKHR vkGetDeferredOperationResultKHR = nullptr;
        #endif
        #if defined(VK_KHR_descriptor_update_template)
            PFN_vkCreateDescriptorUpdateTemplateKHR vkCreateDescriptorUpdateTemplateKHR = nullptr;
            PFN_vkDestroyDescriptorUpdateTemplateKHR vkDestroyDescriptorUpdateTemplateKHR = nullptr;
            PFN_vkUpdateDescriptorSetWithTemplateKHR vkUpdateDescriptorSetWithTemplateKHR = nullptr;
        #endif
        #if defined(VK_KHR_device_group)
            PFN_vkCmdDispatchBaseKHR vkCmdDispatchBaseKHR = nullptr;
            PFN_vkCmdSetDeviceMaskKHR vkCmdSetDeviceMaskKHR = nullptr;
            PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR vkGetDeviceGroupPeerMemoryFeaturesKHR = nullptr;
        #endif
        #if defined(VK_KHR_display_swapchain)
            PFN_vkCreateSharedSwapchainsKHR vkCreateSharedSwapchainsKHR = nullptr;
        #endif
        #if defined(VK_KHR_draw_indirect_count)
            PFN_vkCmdDrawIndexedIndirectCountKHR vkCmdDrawIndexedIndirectCountKHR = nullptr;
            PFN_vkCmdDrawIndirectCountKHR vkCmdDrawIndirectCountKHR = nullptr;
        #endif
        #if defined(VK_KHR_dynamic_rendering)
            PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR = nullptr;
            PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR = nullptr;
        #endif
        #if defined(VK_KHR_external_fence_fd)
            PFN_vkGetFenceFdKHR vkGetFenceFdKHR = nullptr;
            PFN_vkImportFenceFdKHR vkImportFenceFdKHR = nullptr;
        #endif
        #if defined(VK_KHR_external_fence_win32)
            PFN_vkGetFenceWin32HandleKHR vkGetFenceWin32HandleKHR = nullptr;
                PFN_vkImportFenceWin32HandleKHR vkImportFenceWin32HandleKHR = nullptr;
        #endif
        #if defined(VK_KHR_external_memory_fd)
            PFN_vkGetMemoryFdKHR vkGetMemoryFdKHR = nullptr;
            PFN_vkGetMemoryFdPropertiesKHR vkGetMemoryFdPropertiesKHR = nullptr;
        #endif
        #if defined(VK_KHR_external_memory_win32)
            PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR = nullptr;
                PFN_vkGetMemoryWin32HandlePropertiesKHR vkGetMemoryWin32HandlePropertiesKHR = nullptr;
        #endif
        #if defined(VK_KHR_external_semaphore_fd)
            PFN_vkGetSemaphoreFdKHR vkGetSemaphoreFdKHR = nullptr;
            PFN_vkImportSemaphoreFdKHR vkImportSemaphoreFdKHR = nullptr;
        #endif
        #if defined(VK_KHR_external_semaphore_win32)
            PFN_vkGetSemaphoreWin32HandleKHR vkGetSemaphoreWin32HandleKHR = nullptr;
                PFN_vkImportSemaphoreWin32HandleKHR vkImportSemaphoreWin32HandleKHR = nullptr;
        #endif
        #if defined(VK_KHR_fragment_shading_rate)
            PFN_vkCmdSetFragmentShadingRateKHR vkCmdSetFragmentShadingRateKHR = nullptr;
        #endif
        #if defined(VK_KHR_get_memory_requirements2)
            PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR = nullptr;
            PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR = nullptr;
            PFN_vkGetImageSparseMemoryRequirements2KHR vkGetImageSparseMemoryRequirements2KHR = nullptr;
        #endif
        #if defined(VK_KHR_maintenance1)
            PFN_vkTrimCommandPoolKHR vkTrimCommandPoolKHR = nullptr;
        #endif
        #if defined(VK_KHR_maintenance3)
            PFN_vkGetDescriptorSetLayoutSupportKHR vkGetDescriptorSetLayoutSupportKHR = nullptr;
        #endif
        #if defined(VK_KHR_maintenance4)
            PFN_vkGetDeviceBufferMemoryRequirementsKHR vkGetDeviceBufferMemoryRequirementsKHR = nullptr;
            PFN_vkGetDeviceImageMemoryRequirementsKHR vkGetDeviceImageMemoryRequirementsKHR = nullptr;
            PFN_vkGetDeviceImageSparseMemoryRequirementsKHR vkGetDeviceImageSparseMemoryRequirementsKHR = nullptr;
        #endif
        #if defined(VK_KHR_maintenance5)
            PFN_vkCmdBindIndexBuffer2KHR vkCmdBindIndexBuffer2KHR = nullptr;
                PFN_vkGetDeviceImageSubresourceLayoutKHR vkGetDeviceImageSubresourceLayoutKHR = nullptr;
                PFN_vkGetImageSubresourceLayout2KHR vkGetImageSubresourceLayout2KHR = nullptr;
                PFN_vkGetRenderingAreaGranularityKHR vkGetRenderingAreaGranularityKHR = nullptr;
        #endif
        #if defined(VK_KHR_map_memory2)
            PFN_vkMapMemory2KHR vkMapMemory2KHR = nullptr;
            PFN_vkUnmapMemory2KHR vkUnmapMemory2KHR = nullptr;
        #endif
        #if defined(VK_KHR_performance_query)
            PFN_vkAcquireProfilingLockKHR vkAcquireProfilingLockKHR = nullptr;
            PFN_vkReleaseProfilingLockKHR vkReleaseProfilingLockKHR = nullptr;
        #endif
        #if defined(VK_KHR_pipeline_executable_properties)
            PFN_vkGetPipelineExecutableInternalRepresentationsKHR vkGetPipelineExecutableInternalRepresentationsKHR = nullptr;
            PFN_vkGetPipelineExecutablePropertiesKHR vkGetPipelineExecutablePropertiesKHR = nullptr;
            PFN_vkGetPipelineExecutableStatisticsKHR vkGetPipelineExecutableStatisticsKHR = nullptr;
        #endif
        #if defined(VK_KHR_present_wait)
            PFN_vkWaitForPresentKHR vkWaitForPresentKHR = nullptr;
        #endif
        #if defined(VK_KHR_push_descriptor)
            PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR = nullptr;
        #endif
        #if defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline)
            PFN_vkCmdTraceRaysIndirect2KHR vkCmdTraceRaysIndirect2KHR = nullptr;
        #endif
        #if defined(VK_KHR_ray_tracing_pipeline)
            PFN_vkCmdSetRayTracingPipelineStackSizeKHR vkCmdSetRayTracingPipelineStackSizeKHR = nullptr;
            PFN_vkCmdTraceRaysIndirectKHR vkCmdTraceRaysIndirectKHR = nullptr;
            PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR = nullptr;
            PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR = nullptr;
            PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = nullptr;
            PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR = nullptr;
            PFN_vkGetRayTracingShaderGroupStackSizeKHR vkGetRayTracingShaderGroupStackSizeKHR = nullptr;
        #endif
        #if defined(VK_KHR_sampler_ycbcr_conversion)
            PFN_vkCreateSamplerYcbcrConversionKHR vkCreateSamplerYcbcrConversionKHR = nullptr;
            PFN_vkDestroySamplerYcbcrConversionKHR vkDestroySamplerYcbcrConversionKHR = nullptr;
        #endif
        #if defined(VK_KHR_shared_presentable_image)
            PFN_vkGetSwapchainStatusKHR vkGetSwapchainStatusKHR = nullptr;
        #endif
        #if defined(VK_KHR_swapchain)
            PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR = nullptr;
            PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR = nullptr;
            PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR = nullptr;
            PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR = nullptr;
            PFN_vkQueuePresentKHR vkQueuePresentKHR = nullptr;
        #endif
        #if defined(VK_KHR_synchronization2)
            PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR = nullptr;
            PFN_vkCmdResetEvent2KHR vkCmdResetEvent2KHR = nullptr;
            PFN_vkCmdSetEvent2KHR vkCmdSetEvent2KHR = nullptr;
            PFN_vkCmdWaitEvents2KHR vkCmdWaitEvents2KHR = nullptr;
            PFN_vkCmdWriteTimestamp2KHR vkCmdWriteTimestamp2KHR = nullptr;
            PFN_vkQueueSubmit2KHR vkQueueSubmit2KHR = nullptr;
        #endif
        #if defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker)
            PFN_vkCmdWriteBufferMarker2AMD vkCmdWriteBufferMarker2AMD = nullptr;
        #endif
        #if defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints)
            PFN_vkGetQueueCheckpointData2NV vkGetQueueCheckpointData2NV = nullptr;
        #endif
        #if defined(VK_KHR_timeline_semaphore)
            PFN_vkGetSemaphoreCounterValueKHR vkGetSemaphoreCounterValueKHR = nullptr;
            PFN_vkSignalSemaphoreKHR vkSignalSemaphoreKHR = nullptr;
            PFN_vkWaitSemaphoresKHR vkWaitSemaphoresKHR = nullptr;
        #endif
        #if defined(VK_KHR_video_decode_queue)
            PFN_vkCmdDecodeVideoKHR vkCmdDecodeVideoKHR = nullptr;
        #endif
        #if defined(VK_KHR_video_encode_queue)
            PFN_vkCmdEncodeVideoKHR vkCmdEncodeVideoKHR = nullptr;
                PFN_vkGetEncodedVideoSessionParametersKHR vkGetEncodedVideoSessionParametersKHR = nullptr;
        #endif
        #if defined(VK_KHR_video_queue)
            PFN_vkBindVideoSessionMemoryKHR vkBindVideoSessionMemoryKHR = nullptr;
            PFN_vkCmdBeginVideoCodingKHR vkCmdBeginVideoCodingKHR = nullptr;
            PFN_vkCmdControlVideoCodingKHR vkCmdControlVideoCodingKHR = nullptr;
            PFN_vkCmdEndVideoCodingKHR vkCmdEndVideoCodingKHR = nullptr;
            PFN_vkCreateVideoSessionKHR vkCreateVideoSessionKHR = nullptr;
            PFN_vkCreateVideoSessionParametersKHR vkCreateVideoSessionParametersKHR = nullptr;
            PFN_vkDestroyVideoSessionKHR vkDestroyVideoSessionKHR = nullptr;
            PFN_vkDestroyVideoSessionParametersKHR vkDestroyVideoSessionParametersKHR = nullptr;
            PFN_vkGetVideoSessionMemoryRequirementsKHR vkGetVideoSessionMemoryRequirementsKHR = nullptr;
            PFN_vkUpdateVideoSessionParametersKHR vkUpdateVideoSessionParametersKHR = nullptr;
        #endif
        #if defined(VK_NVX_binary_import)
            PFN_vkCmdCuLaunchKernelNVX vkCmdCuLaunchKernelNVX = nullptr;
            PFN_vkCreateCuFunctionNVX vkCreateCuFunctionNVX = nullptr;
            PFN_vkCreateCuModuleNVX vkCreateCuModuleNVX = nullptr;
            PFN_vkDestroyCuFunctionNVX vkDestroyCuFunctionNVX = nullptr;
            PFN_vkDestroyCuModuleNVX vkDestroyCuModuleNVX = nullptr;
        #endif
        #if defined(VK_NVX_image_view_handle)
            PFN_vkGetImageViewAddressNVX vkGetImageViewAddressNVX = nullptr;
            PFN_vkGetImageViewHandleNVX vkGetImageViewHandleNVX = nullptr;
        #endif
        #if defined(VK_NV_clip_space_w_scaling)
            PFN_vkCmdSetViewportWScalingNV vkCmdSetViewportWScalingNV = nullptr;
        #endif
        #if defined(VK_NV_copy_memory_indirect)
            PFN_vkCmdCopyMemoryIndirectNV vkCmdCopyMemoryIndirectNV = nullptr;
            PFN_vkCmdCopyMemoryToImageIndirectNV vkCmdCopyMemoryToImageIndirectNV = nullptr;
        #endif
        #if defined(VK_NV_device_diagnostic_checkpoints)
            PFN_vkCmdSetCheckpointNV vkCmdSetCheckpointNV = nullptr;
            PFN_vkGetQueueCheckpointDataNV vkGetQueueCheckpointDataNV = nullptr;
        #endif
        #if defined(VK_NV_device_generated_commands)
            PFN_vkCmdBindPipelineShaderGroupNV vkCmdBindPipelineShaderGroupNV = nullptr;
            PFN_vkCmdExecuteGeneratedCommandsNV vkCmdExecuteGeneratedCommandsNV = nullptr;
            PFN_vkCmdPreprocessGeneratedCommandsNV vkCmdPreprocessGeneratedCommandsNV = nullptr;
            PFN_vkCreateIndirectCommandsLayoutNV vkCreateIndirectCommandsLayoutNV = nullptr;
            PFN_vkDestroyIndirectCommandsLayoutNV vkDestroyIndirectCommandsLayoutNV = nullptr;
            PFN_vkGetGeneratedCommandsMemoryRequirementsNV vkGetGeneratedCommandsMemoryRequirementsNV = nullptr;
        #endif
        #if defined(VK_NV_device_generated_commands_compute)
            PFN_vkCmdUpdatePipelineIndirectBufferNV vkCmdUpdatePipelineIndirectBufferNV = nullptr;
                PFN_vkGetPipelineIndirectDeviceAddressNV vkGetPipelineIndirectDeviceAddressNV = nullptr;
                PFN_vkGetPipelineIndirectMemoryRequirementsNV vkGetPipelineIndirectMemoryRequirementsNV = nullptr;
        #endif
        #if defined(VK_NV_external_memory_rdma)
            PFN_vkGetMemoryRemoteAddressNV vkGetMemoryRemoteAddressNV = nullptr;
        #endif
        #if defined(VK_NV_external_memory_win32)
            PFN_vkGetMemoryWin32HandleNV vkGetMemoryWin32HandleNV = nullptr;
        #endif
        #if defined(VK_NV_fragment_shading_rate_enums)
            PFN_vkCmdSetFragmentShadingRateEnumNV vkCmdSetFragmentShadingRateEnumNV = nullptr;
        #endif
        #if defined(VK_NV_memory_decompression)
            PFN_vkCmdDecompressMemoryIndirectCountNV vkCmdDecompressMemoryIndirectCountNV = nullptr;
            PFN_vkCmdDecompressMemoryNV vkCmdDecompressMemoryNV = nullptr;
        #endif
        #if defined(VK_NV_mesh_shader)
            PFN_vkCmdDrawMeshTasksIndirectCountNV vkCmdDrawMeshTasksIndirectCountNV = nullptr;
            PFN_vkCmdDrawMeshTasksIndirectNV vkCmdDrawMeshTasksIndirectNV = nullptr;
            PFN_vkCmdDrawMeshTasksNV vkCmdDrawMeshTasksNV = nullptr;
        #endif
        #if defined(VK_NV_optical_flow)
            PFN_vkBindOpticalFlowSessionImageNV vkBindOpticalFlowSessionImageNV = nullptr;
            PFN_vkCmdOpticalFlowExecuteNV vkCmdOpticalFlowExecuteNV = nullptr;
            PFN_vkCreateOpticalFlowSessionNV vkCreateOpticalFlowSessionNV = nullptr;
            PFN_vkDestroyOpticalFlowSessionNV vkDestroyOpticalFlowSessionNV = nullptr;
        #endif
        #if defined(VK_NV_ray_tracing)
            PFN_vkBindAccelerationStructureMemoryNV vkBindAccelerationStructureMemoryNV = nullptr;
            PFN_vkCmdBuildAccelerationStructureNV vkCmdBuildAccelerationStructureNV = nullptr;
            PFN_vkCmdCopyAccelerationStructureNV vkCmdCopyAccelerationStructureNV = nullptr;
            PFN_vkCmdTraceRaysNV vkCmdTraceRaysNV = nullptr;
            PFN_vkCmdWriteAccelerationStructuresPropertiesNV vkCmdWriteAccelerationStructuresPropertiesNV = nullptr;
            PFN_vkCompileDeferredNV vkCompileDeferredNV = nullptr;
            PFN_vkCreateAccelerationStructureNV vkCreateAccelerationStructureNV = nullptr;
            PFN_vkCreateRayTracingPipelinesNV vkCreateRayTracingPipelinesNV = nullptr;
            PFN_vkDestroyAccelerationStructureNV vkDestroyAccelerationStructureNV = nullptr;
            PFN_vkGetAccelerationStructureHandleNV vkGetAccelerationStructureHandleNV = nullptr;
            PFN_vkGetAccelerationStructureMemoryRequirementsNV vkGetAccelerationStructureMemoryRequirementsNV = nullptr;
            PFN_vkGetRayTracingShaderGroupHandlesNV vkGetRayTracingShaderGroupHandlesNV = nullptr;
        #endif
        #if defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2
            PFN_vkCmdSetExclusiveScissorEnableNV vkCmdSetExclusiveScissorEnableNV = nullptr;
        #endif
        #if defined(VK_NV_scissor_exclusive)
            PFN_vkCmdSetExclusiveScissorNV vkCmdSetExclusiveScissorNV = nullptr;
        #endif
        #if defined(VK_NV_shading_rate_image)
            PFN_vkCmdBindShadingRateImageNV vkCmdBindShadingRateImageNV = nullptr;
            PFN_vkCmdSetCoarseSampleOrderNV vkCmdSetCoarseSampleOrderNV = nullptr;
            PFN_vkCmdSetViewportShadingRatePaletteNV vkCmdSetViewportShadingRatePaletteNV = nullptr;
        #endif
        #if defined(VK_QCOM_tile_properties)
            PFN_vkGetDynamicRenderingTilePropertiesQCOM vkGetDynamicRenderingTilePropertiesQCOM = nullptr;
            PFN_vkGetFramebufferTilePropertiesQCOM vkGetFramebufferTilePropertiesQCOM = nullptr;
        #endif
        #if defined(VK_QNX_external_memory_screen_buffer)
            PFN_vkGetScreenBufferPropertiesQNX vkGetScreenBufferPropertiesQNX = nullptr;
        #endif
        #if defined(VK_VALVE_descriptor_set_host_mapping)
            PFN_vkGetDescriptorSetHostMappingVALVE vkGetDescriptorSetHostMappingVALVE = nullptr;
            PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE vkGetDescriptorSetLayoutHostMappingInfoVALVE = nullptr;
        #endif
        #if (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object))
            PFN_vkCmdBindVertexBuffers2EXT vkCmdBindVertexBuffers2EXT = nullptr;
            PFN_vkCmdSetCullModeEXT vkCmdSetCullModeEXT = nullptr;
            PFN_vkCmdSetDepthBoundsTestEnableEXT vkCmdSetDepthBoundsTestEnableEXT = nullptr;
            PFN_vkCmdSetDepthCompareOpEXT vkCmdSetDepthCompareOpEXT = nullptr;
            PFN_vkCmdSetDepthTestEnableEXT vkCmdSetDepthTestEnableEXT = nullptr;
            PFN_vkCmdSetDepthWriteEnableEXT vkCmdSetDepthWriteEnableEXT = nullptr;
            PFN_vkCmdSetFrontFaceEXT vkCmdSetFrontFaceEXT = nullptr;
            PFN_vkCmdSetPrimitiveTopologyEXT vkCmdSetPrimitiveTopologyEXT = nullptr;
            PFN_vkCmdSetScissorWithCountEXT vkCmdSetScissorWithCountEXT = nullptr;
            PFN_vkCmdSetStencilOpEXT vkCmdSetStencilOpEXT = nullptr;
            PFN_vkCmdSetStencilTestEnableEXT vkCmdSetStencilTestEnableEXT = nullptr;
            PFN_vkCmdSetViewportWithCountEXT vkCmdSetViewportWithCountEXT = nullptr;
        #endif
        #if (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object))
            PFN_vkCmdSetDepthBiasEnableEXT vkCmdSetDepthBiasEnableEXT = nullptr;
            PFN_vkCmdSetLogicOpEXT vkCmdSetLogicOpEXT = nullptr;
            PFN_vkCmdSetPatchControlPointsEXT vkCmdSetPatchControlPointsEXT = nullptr;
            PFN_vkCmdSetPrimitiveRestartEnableEXT vkCmdSetPrimitiveRestartEnableEXT = nullptr;
            PFN_vkCmdSetRasterizerDiscardEnableEXT vkCmdSetRasterizerDiscardEnableEXT = nullptr;
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object))
            PFN_vkCmdSetAlphaToCoverageEnableEXT vkCmdSetAlphaToCoverageEnableEXT = nullptr;
            PFN_vkCmdSetAlphaToOneEnableEXT vkCmdSetAlphaToOneEnableEXT = nullptr;
            PFN_vkCmdSetColorBlendAdvancedEXT vkCmdSetColorBlendAdvancedEXT = nullptr;
            PFN_vkCmdSetColorBlendEnableEXT vkCmdSetColorBlendEnableEXT = nullptr;
            PFN_vkCmdSetColorBlendEquationEXT vkCmdSetColorBlendEquationEXT = nullptr;
            PFN_vkCmdSetColorWriteMaskEXT vkCmdSetColorWriteMaskEXT = nullptr;
            PFN_vkCmdSetConservativeRasterizationModeEXT vkCmdSetConservativeRasterizationModeEXT = nullptr;
            PFN_vkCmdSetDepthClampEnableEXT vkCmdSetDepthClampEnableEXT = nullptr;
            PFN_vkCmdSetDepthClipEnableEXT vkCmdSetDepthClipEnableEXT = nullptr;
            PFN_vkCmdSetDepthClipNegativeOneToOneEXT vkCmdSetDepthClipNegativeOneToOneEXT = nullptr;
            PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT vkCmdSetExtraPrimitiveOverestimationSizeEXT = nullptr;
            PFN_vkCmdSetLineRasterizationModeEXT vkCmdSetLineRasterizationModeEXT = nullptr;
            PFN_vkCmdSetLineStippleEnableEXT vkCmdSetLineStippleEnableEXT = nullptr;
            PFN_vkCmdSetLogicOpEnableEXT vkCmdSetLogicOpEnableEXT = nullptr;
            PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT = nullptr;
            PFN_vkCmdSetProvokingVertexModeEXT vkCmdSetProvokingVertexModeEXT = nullptr;
            PFN_vkCmdSetRasterizationSamplesEXT vkCmdSetRasterizationSamplesEXT = nullptr;
            PFN_vkCmdSetRasterizationStreamEXT vkCmdSetRasterizationStreamEXT = nullptr;
            PFN_vkCmdSetSampleLocationsEnableEXT vkCmdSetSampleLocationsEnableEXT = nullptr;
            PFN_vkCmdSetSampleMaskEXT vkCmdSetSampleMaskEXT = nullptr;
            PFN_vkCmdSetTessellationDomainOriginEXT vkCmdSetTessellationDomainOriginEXT = nullptr;
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling))
            PFN_vkCmdSetViewportWScalingEnableNV vkCmdSetViewportWScalingEnableNV = nullptr;
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle))
            PFN_vkCmdSetViewportSwizzleNV vkCmdSetViewportSwizzleNV = nullptr;
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color))
            PFN_vkCmdSetCoverageToColorEnableNV vkCmdSetCoverageToColorEnableNV = nullptr;
            PFN_vkCmdSetCoverageToColorLocationNV vkCmdSetCoverageToColorLocationNV = nullptr;
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples))
            PFN_vkCmdSetCoverageModulationModeNV vkCmdSetCoverageModulationModeNV = nullptr;
            PFN_vkCmdSetCoverageModulationTableEnableNV vkCmdSetCoverageModulationTableEnableNV = nullptr;
            PFN_vkCmdSetCoverageModulationTableNV vkCmdSetCoverageModulationTableNV = nullptr;
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image))
            PFN_vkCmdSetShadingRateImageEnableNV vkCmdSetShadingRateImageEnableNV = nullptr;
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test))
            PFN_vkCmdSetRepresentativeFragmentTestEnableNV vkCmdSetRepresentativeFragmentTestEnableNV = nullptr;
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode))
            PFN_vkCmdSetCoverageReductionModeNV vkCmdSetCoverageReductionModeNV = nullptr;
        #endif
        #if (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1))
            PFN_vkGetDeviceGroupSurfacePresentModes2EXT vkGetDeviceGroupSurfacePresentModes2EXT = nullptr;
        #endif
        #if (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control))
            PFN_vkGetImageSubresourceLayout2EXT vkGetImageSubresourceLayout2EXT = nullptr;
        #endif
        #if (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state))
            PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT = nullptr;
        #endif
        #if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template))
            PFN_vkCmdPushDescriptorSetWithTemplateKHR vkCmdPushDescriptorSetWithTemplateKHR = nullptr;
        #endif
        #if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
            PFN_vkGetDeviceGroupPresentCapabilitiesKHR vkGetDeviceGroupPresentCapabilitiesKHR = nullptr;
            PFN_vkGetDeviceGroupSurfacePresentModesKHR vkGetDeviceGroupSurfacePresentModesKHR = nullptr;
        #endif
        #if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
            PFN_vkAcquireNextImage2KHR vkAcquireNextImage2KHR = nullptr;
        #endif
    };

    [[nodiscard]] SIERRA_API VulkanDeviceFunctionTable LoadVulkanDeviceFunctionTable(VkDevice device) noexcept;

}