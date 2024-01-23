//
// Created by Nikolay Kanchevski on 10.09.23.
//

#pragma once

#include "../../Device.h"
#include "VulkanResource.h"

#include <vk_mem_alloc.h>
#include "VulkanInstance.h"

namespace Sierra
{

    class SIERRA_API VulkanDevice final : public Device, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanDevice(const VulkanInstance &instance, const DeviceCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void SubmitCommandBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, const std::initializer_list<std::reference_wrapper<std::unique_ptr<CommandBuffer>>> &commandBuffersToWait = { }) const override;
        void WaitForCommandBuffer(const std::unique_ptr<CommandBuffer> &commandBuffer) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const std::string& GetDeviceName() const override { return deviceName; }

        [[nodiscard]] bool IsImageConfigurationSupported(ImageFormat format, ImageUsage usage) const override;
        [[nodiscard]] bool IsImageSamplingSupported(ImageSampling sampling) const override;
        [[nodiscard]] ImageSampling GetHighestImageSamplingSupported() const override;

        [[nodiscard]] inline VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
        [[nodiscard]] inline VkDevice GetLogicalDevice() const { return logicalDevice; }
        [[nodiscard]] inline VmaAllocator GetMemoryAllocator() const { return vmaAllocator; }

        [[nodiscard]] inline uint32 GetGeneralQueueFamily() const { return generalQueueFamily; }
        [[nodiscard]] inline VkQueue GetGeneralQueue() const { return generalQueue; }

        [[nodiscard]] inline VkSemaphore GetSharedTimelineSemaphore() const { return sharedTimelineSemaphore; }
        [[nodiscard]] inline uint64 GetNewSignalValue() const { lastReservedSignalValue++; return lastReservedSignalValue; }

        [[nodiscard]] bool IsExtensionLoaded(const std::string &extensionName) const;
        [[nodiscard]] inline auto& GetFunctionTable() const { return functionTable; }

        /* --- SETTER METHODS --- */
        void SetObjectName(VkHandle object, VkObjectType objectType, const std::string &name) const;

        /* --- DESTRUCTOR --- */
        ~VulkanDevice() override;

    private:
        const VulkanInstance &instance;

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        std::string deviceName;

        struct
        {
            #if defined(VK_VERSION_1_0)
                PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
                PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
                PFN_vkAllocateMemory vkAllocateMemory;
                PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
                PFN_vkBindBufferMemory vkBindBufferMemory;
                PFN_vkBindImageMemory vkBindImageMemory;
                PFN_vkCmdBeginQuery vkCmdBeginQuery;
                PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
                PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;
                PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer;
                PFN_vkCmdBindPipeline vkCmdBindPipeline;
                PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;
                PFN_vkCmdBlitImage vkCmdBlitImage;
                PFN_vkCmdClearAttachments vkCmdClearAttachments;
                PFN_vkCmdClearColorImage vkCmdClearColorImage;
                PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage;
                PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
                PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
                PFN_vkCmdCopyImage vkCmdCopyImage;
                PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer;
                PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults;
                PFN_vkCmdDispatch vkCmdDispatch;
                PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect;
                PFN_vkCmdDraw vkCmdDraw;
                PFN_vkCmdDrawIndexed vkCmdDrawIndexed;
                PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect;
                PFN_vkCmdDrawIndirect vkCmdDrawIndirect;
                PFN_vkCmdEndQuery vkCmdEndQuery;
                PFN_vkCmdEndRenderPass vkCmdEndRenderPass;
                PFN_vkCmdExecuteCommands vkCmdExecuteCommands;
                PFN_vkCmdFillBuffer vkCmdFillBuffer;
                PFN_vkCmdNextSubpass vkCmdNextSubpass;
                PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
                PFN_vkCmdPushConstants vkCmdPushConstants;
                PFN_vkCmdResetEvent vkCmdResetEvent;
                PFN_vkCmdResetQueryPool vkCmdResetQueryPool;
                PFN_vkCmdResolveImage vkCmdResolveImage;
                PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants;
                PFN_vkCmdSetDepthBias vkCmdSetDepthBias;
                PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds;
                PFN_vkCmdSetEvent vkCmdSetEvent;
                PFN_vkCmdSetLineWidth vkCmdSetLineWidth;
                PFN_vkCmdSetScissor vkCmdSetScissor;
                PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask;
                PFN_vkCmdSetStencilReference vkCmdSetStencilReference;
                PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask;
                PFN_vkCmdSetViewport vkCmdSetViewport;
                PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer;
                PFN_vkCmdWaitEvents vkCmdWaitEvents;
                PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp;
                PFN_vkCreateBuffer vkCreateBuffer;
                PFN_vkCreateBufferView vkCreateBufferView;
                PFN_vkCreateCommandPool vkCreateCommandPool;
                PFN_vkCreateComputePipelines vkCreateComputePipelines;
                PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
                PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
                PFN_vkCreateEvent vkCreateEvent;
                PFN_vkCreateFence vkCreateFence;
                PFN_vkCreateFramebuffer vkCreateFramebuffer;
                PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
                PFN_vkCreateImage vkCreateImage;
                PFN_vkCreateImageView vkCreateImageView;
                PFN_vkCreatePipelineCache vkCreatePipelineCache;
                PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
                PFN_vkCreateQueryPool vkCreateQueryPool;
                PFN_vkCreateRenderPass vkCreateRenderPass;
                PFN_vkCreateSampler vkCreateSampler;
                PFN_vkCreateSemaphore vkCreateSemaphore;
                PFN_vkCreateShaderModule vkCreateShaderModule;
                PFN_vkDestroyBuffer vkDestroyBuffer;
                PFN_vkDestroyBufferView vkDestroyBufferView;
                PFN_vkDestroyCommandPool vkDestroyCommandPool;
                PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
                PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;
                PFN_vkDestroyDevice vkDestroyDevice;
                PFN_vkDestroyEvent vkDestroyEvent;
                PFN_vkDestroyFence vkDestroyFence;
                PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
                PFN_vkDestroyImage vkDestroyImage;
                PFN_vkDestroyImageView vkDestroyImageView;
                PFN_vkDestroyPipeline vkDestroyPipeline;
                PFN_vkDestroyPipelineCache vkDestroyPipelineCache;
                PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
                PFN_vkDestroyQueryPool vkDestroyQueryPool;
                PFN_vkDestroyRenderPass vkDestroyRenderPass;
                PFN_vkDestroySampler vkDestroySampler;
                PFN_vkDestroySemaphore vkDestroySemaphore;
                PFN_vkDestroyShaderModule vkDestroyShaderModule;
                PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
                PFN_vkEndCommandBuffer vkEndCommandBuffer;
                PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges;
                PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
                PFN_vkFreeDescriptorSets vkFreeDescriptorSets;
                PFN_vkFreeMemory vkFreeMemory;
                PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
                PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment;
                PFN_vkGetDeviceQueue vkGetDeviceQueue;
                PFN_vkGetEventStatus vkGetEventStatus;
                PFN_vkGetFenceStatus vkGetFenceStatus;
                PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
                PFN_vkGetImageSparseMemoryRequirements vkGetImageSparseMemoryRequirements;
                PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout;
                PFN_vkGetPipelineCacheData vkGetPipelineCacheData;
                PFN_vkGetQueryPoolResults vkGetQueryPoolResults;
                PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity;
                PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges;
                PFN_vkMapMemory vkMapMemory;
                PFN_vkMergePipelineCaches vkMergePipelineCaches;
                PFN_vkQueueBindSparse vkQueueBindSparse;
                PFN_vkQueueSubmit vkQueueSubmit;
                PFN_vkQueueWaitIdle vkQueueWaitIdle;
                PFN_vkResetCommandBuffer vkResetCommandBuffer;
                PFN_vkResetCommandPool vkResetCommandPool;
                PFN_vkResetDescriptorPool vkResetDescriptorPool;
                PFN_vkResetEvent vkResetEvent;
                PFN_vkResetFences vkResetFences;
                PFN_vkSetEvent vkSetEvent;
                PFN_vkUnmapMemory vkUnmapMemory;
                PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
                PFN_vkWaitForFences vkWaitForFences;
            #endif
            #if defined(VK_VERSION_1_1)
                PFN_vkBindBufferMemory2 vkBindBufferMemory2;
                PFN_vkBindImageMemory2 vkBindImageMemory2;
                PFN_vkCmdDispatchBase vkCmdDispatchBase;
                PFN_vkCmdSetDeviceMask vkCmdSetDeviceMask;
                PFN_vkCreateDescriptorUpdateTemplate vkCreateDescriptorUpdateTemplate;
                PFN_vkCreateSamplerYcbcrConversion vkCreateSamplerYcbcrConversion;
                PFN_vkDestroyDescriptorUpdateTemplate vkDestroyDescriptorUpdateTemplate;
                PFN_vkDestroySamplerYcbcrConversion vkDestroySamplerYcbcrConversion;
                PFN_vkGetBufferMemoryRequirements2 vkGetBufferMemoryRequirements2;
                PFN_vkGetDescriptorSetLayoutSupport vkGetDescriptorSetLayoutSupport;
                PFN_vkGetDeviceGroupPeerMemoryFeatures vkGetDeviceGroupPeerMemoryFeatures;
                PFN_vkGetDeviceQueue2 vkGetDeviceQueue2;
                PFN_vkGetImageMemoryRequirements2 vkGetImageMemoryRequirements2;
                PFN_vkGetImageSparseMemoryRequirements2 vkGetImageSparseMemoryRequirements2;
                PFN_vkTrimCommandPool vkTrimCommandPool;
                PFN_vkUpdateDescriptorSetWithTemplate vkUpdateDescriptorSetWithTemplate;
            #endif
            #if defined(VK_VERSION_1_2)
                PFN_vkCmdBeginRenderPass2 vkCmdBeginRenderPass2;
                PFN_vkCmdDrawIndexedIndirectCount vkCmdDrawIndexedIndirectCount;
                PFN_vkCmdDrawIndirectCount vkCmdDrawIndirectCount;
                PFN_vkCmdEndRenderPass2 vkCmdEndRenderPass2;
                PFN_vkCmdNextSubpass2 vkCmdNextSubpass2;
                PFN_vkCreateRenderPass2 vkCreateRenderPass2;
                PFN_vkGetBufferDeviceAddress vkGetBufferDeviceAddress;
                PFN_vkGetBufferOpaqueCaptureAddress vkGetBufferOpaqueCaptureAddress;
                PFN_vkGetDeviceMemoryOpaqueCaptureAddress vkGetDeviceMemoryOpaqueCaptureAddress;
                PFN_vkGetSemaphoreCounterValue vkGetSemaphoreCounterValue;
                PFN_vkResetQueryPool vkResetQueryPool;
                PFN_vkSignalSemaphore vkSignalSemaphore;
                PFN_vkWaitSemaphores vkWaitSemaphores;
            #endif
            #if defined(VK_VERSION_1_3)
                PFN_vkCmdBeginRendering vkCmdBeginRendering;
                PFN_vkCmdBindVertexBuffers2 vkCmdBindVertexBuffers2;
                PFN_vkCmdBlitImage2 vkCmdBlitImage2;
                PFN_vkCmdCopyBuffer2 vkCmdCopyBuffer2;
                PFN_vkCmdCopyBufferToImage2 vkCmdCopyBufferToImage2;
                PFN_vkCmdCopyImage2 vkCmdCopyImage2;
                PFN_vkCmdCopyImageToBuffer2 vkCmdCopyImageToBuffer2;
                PFN_vkCmdEndRendering vkCmdEndRendering;
                PFN_vkCmdPipelineBarrier2 vkCmdPipelineBarrier2;
                PFN_vkCmdResetEvent2 vkCmdResetEvent2;
                PFN_vkCmdResolveImage2 vkCmdResolveImage2;
                PFN_vkCmdSetCullMode vkCmdSetCullMode;
                PFN_vkCmdSetDepthBiasEnable vkCmdSetDepthBiasEnable;
                PFN_vkCmdSetDepthBoundsTestEnable vkCmdSetDepthBoundsTestEnable;
                PFN_vkCmdSetDepthCompareOp vkCmdSetDepthCompareOp;
                PFN_vkCmdSetDepthTestEnable vkCmdSetDepthTestEnable;
                PFN_vkCmdSetDepthWriteEnable vkCmdSetDepthWriteEnable;
                PFN_vkCmdSetEvent2 vkCmdSetEvent2;
                PFN_vkCmdSetFrontFace vkCmdSetFrontFace;
                PFN_vkCmdSetPrimitiveRestartEnable vkCmdSetPrimitiveRestartEnable;
                PFN_vkCmdSetPrimitiveTopology vkCmdSetPrimitiveTopology;
                PFN_vkCmdSetRasterizerDiscardEnable vkCmdSetRasterizerDiscardEnable;
                PFN_vkCmdSetScissorWithCount vkCmdSetScissorWithCount;
                PFN_vkCmdSetStencilOp vkCmdSetStencilOp;
                PFN_vkCmdSetStencilTestEnable vkCmdSetStencilTestEnable;
                PFN_vkCmdSetViewportWithCount vkCmdSetViewportWithCount;
                PFN_vkCmdWaitEvents2 vkCmdWaitEvents2;
                PFN_vkCmdWriteTimestamp2 vkCmdWriteTimestamp2;
                PFN_vkCreatePrivateDataSlot vkCreatePrivateDataSlot;
                PFN_vkDestroyPrivateDataSlot vkDestroyPrivateDataSlot;
                PFN_vkGetDeviceBufferMemoryRequirements vkGetDeviceBufferMemoryRequirements;
                PFN_vkGetDeviceImageMemoryRequirements vkGetDeviceImageMemoryRequirements;
                PFN_vkGetDeviceImageSparseMemoryRequirements vkGetDeviceImageSparseMemoryRequirements;
                PFN_vkGetPrivateData vkGetPrivateData;
                PFN_vkQueueSubmit2 vkQueueSubmit2;
                PFN_vkSetPrivateData vkSetPrivateData;
            #endif
            #if defined(VK_AMDX_shader_enqueue)
                PFN_vkCmdDispatchGraphAMDX vkCmdDispatchGraphAMDX;
                PFN_vkCmdDispatchGraphIndirectAMDX vkCmdDispatchGraphIndirectAMDX;
                PFN_vkCmdDispatchGraphIndirectCountAMDX vkCmdDispatchGraphIndirectCountAMDX;
                PFN_vkCmdInitializeGraphScratchMemoryAMDX vkCmdInitializeGraphScratchMemoryAMDX;
                PFN_vkCreateExecutionGraphPipelinesAMDX vkCreateExecutionGraphPipelinesAMDX;
                PFN_vkGetExecutionGraphPipelineNodeIndexAMDX vkGetExecutionGraphPipelineNodeIndexAMDX;
                PFN_vkGetExecutionGraphPipelineScratchSizeAMDX vkGetExecutionGraphPipelineScratchSizeAMDX;
            #endif
            #if defined(VK_AMD_buffer_marker)
                PFN_vkCmdWriteBufferMarkerAMD vkCmdWriteBufferMarkerAMD;
            #endif
            #if defined(VK_AMD_display_native_hdr)
                PFN_vkSetLocalDimmingAMD vkSetLocalDimmingAMD;
            #endif
            #if defined(VK_AMD_draw_indirect_count)
                PFN_vkCmdDrawIndexedIndirectCountAMD vkCmdDrawIndexedIndirectCountAMD;
                PFN_vkCmdDrawIndirectCountAMD vkCmdDrawIndirectCountAMD;
            #endif
            #if defined(VK_AMD_shader_info)
                PFN_vkGetShaderInfoAMD vkGetShaderInfoAMD;
            #endif
            #if defined(VK_ANDROID_external_memory_android_hardware_buffer)
                PFN_vkGetAndroidHardwareBufferPropertiesANDROID vkGetAndroidHardwareBufferPropertiesANDROID;
                PFN_vkGetMemoryAndroidHardwareBufferANDROID vkGetMemoryAndroidHardwareBufferANDROID;
            #endif
            #if defined(VK_EXT_attachment_feedback_loop_dynamic_state)
                PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT vkCmdSetAttachmentFeedbackLoopEnableEXT;
            #endif
            #if defined(VK_EXT_buffer_device_address)
                PFN_vkGetBufferDeviceAddressEXT vkGetBufferDeviceAddressEXT;
            #endif
            #if defined(VK_EXT_calibrated_timestamps)
                PFN_vkGetCalibratedTimestampsEXT vkGetCalibratedTimestampsEXT;
            #endif
            #if defined(VK_EXT_color_write_enable)
                PFN_vkCmdSetColorWriteEnableEXT vkCmdSetColorWriteEnableEXT;
            #endif
            #if defined(VK_EXT_conditional_rendering)
                PFN_vkCmdBeginConditionalRenderingEXT vkCmdBeginConditionalRenderingEXT;
                PFN_vkCmdEndConditionalRenderingEXT vkCmdEndConditionalRenderingEXT;
            #endif
            #if defined(VK_EXT_debug_marker)
                PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT;
                PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT;
                PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT;
                PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT;
                PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTagEXT;
            #endif
            #if defined(VK_EXT_depth_bias_control)
                PFN_vkCmdSetDepthBias2EXT vkCmdSetDepthBias2EXT;
            #endif
            #if defined(VK_EXT_descriptor_buffer)
                PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT vkCmdBindDescriptorBufferEmbeddedSamplersEXT;
                PFN_vkCmdBindDescriptorBuffersEXT vkCmdBindDescriptorBuffersEXT;
                PFN_vkCmdSetDescriptorBufferOffsetsEXT vkCmdSetDescriptorBufferOffsetsEXT;
                PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT vkGetBufferOpaqueCaptureDescriptorDataEXT;
                PFN_vkGetDescriptorEXT vkGetDescriptorEXT;
                PFN_vkGetDescriptorSetLayoutBindingOffsetEXT vkGetDescriptorSetLayoutBindingOffsetEXT;
                PFN_vkGetDescriptorSetLayoutSizeEXT vkGetDescriptorSetLayoutSizeEXT;
                PFN_vkGetImageOpaqueCaptureDescriptorDataEXT vkGetImageOpaqueCaptureDescriptorDataEXT;
                PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT vkGetImageViewOpaqueCaptureDescriptorDataEXT;
                PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT vkGetSamplerOpaqueCaptureDescriptorDataEXT;
            #endif
            #if defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing))
                PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT;
            #endif
            #if defined(VK_EXT_device_fault)
                PFN_vkGetDeviceFaultInfoEXT vkGetDeviceFaultInfoEXT;
            #endif
            #if defined(VK_EXT_discard_rectangles)
                PFN_vkCmdSetDiscardRectangleEXT vkCmdSetDiscardRectangleEXT;
            #endif
            #if defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2
                PFN_vkCmdSetDiscardRectangleEnableEXT vkCmdSetDiscardRectangleEnableEXT;
            PFN_vkCmdSetDiscardRectangleModeEXT vkCmdSetDiscardRectangleModeEXT;
            #endif
            #if defined(VK_EXT_display_control)
                PFN_vkDisplayPowerControlEXT vkDisplayPowerControlEXT;
                PFN_vkGetSwapchainCounterEXT vkGetSwapchainCounterEXT;
                PFN_vkRegisterDeviceEventEXT vkRegisterDeviceEventEXT;
                PFN_vkRegisterDisplayEventEXT vkRegisterDisplayEventEXT;
            #endif
            #if defined(VK_EXT_external_memory_host)
                PFN_vkGetMemoryHostPointerPropertiesEXT vkGetMemoryHostPointerPropertiesEXT;
            #endif
            #if defined(VK_EXT_full_screen_exclusive)
                PFN_vkAcquireFullScreenExclusiveModeEXT vkAcquireFullScreenExclusiveModeEXT;
                PFN_vkReleaseFullScreenExclusiveModeEXT vkReleaseFullScreenExclusiveModeEXT;
            #endif
            #if defined(VK_EXT_hdr_metadata)
                PFN_vkSetHdrMetadataEXT vkSetHdrMetadataEXT;
            #endif
            #if defined(VK_EXT_host_image_copy)
                PFN_vkCopyImageToImageEXT vkCopyImageToImageEXT;
                PFN_vkCopyImageToMemoryEXT vkCopyImageToMemoryEXT;
                PFN_vkCopyMemoryToImageEXT vkCopyMemoryToImageEXT;
                PFN_vkTransitionImageLayoutEXT vkTransitionImageLayoutEXT;
            #endif
            #if defined(VK_EXT_host_query_reset)
                PFN_vkResetQueryPoolEXT vkResetQueryPoolEXT;
            #endif
            #if defined(VK_EXT_image_drm_format_modifier)
                PFN_vkGetImageDrmFormatModifierPropertiesEXT vkGetImageDrmFormatModifierPropertiesEXT;
            #endif
            #if defined(VK_EXT_line_rasterization)
                PFN_vkCmdSetLineStippleEXT vkCmdSetLineStippleEXT;
            #endif
            #if defined(VK_EXT_mesh_shader)
                PFN_vkCmdDrawMeshTasksEXT vkCmdDrawMeshTasksEXT;
                PFN_vkCmdDrawMeshTasksIndirectCountEXT vkCmdDrawMeshTasksIndirectCountEXT;
                PFN_vkCmdDrawMeshTasksIndirectEXT vkCmdDrawMeshTasksIndirectEXT;
            #endif
            #if defined(VK_EXT_metal_objects)
                PFN_vkExportMetalObjectsEXT vkExportMetalObjectsEXT;
            #endif
            #if defined(VK_EXT_multi_draw)
                PFN_vkCmdDrawMultiEXT vkCmdDrawMultiEXT;
                PFN_vkCmdDrawMultiIndexedEXT vkCmdDrawMultiIndexedEXT;
            #endif
            #if defined(VK_EXT_opacity_micromap)
                PFN_vkBuildMicromapsEXT vkBuildMicromapsEXT;
                PFN_vkCmdBuildMicromapsEXT vkCmdBuildMicromapsEXT;
                PFN_vkCmdCopyMemoryToMicromapEXT vkCmdCopyMemoryToMicromapEXT;
                PFN_vkCmdCopyMicromapEXT vkCmdCopyMicromapEXT;
                PFN_vkCmdCopyMicromapToMemoryEXT vkCmdCopyMicromapToMemoryEXT;
                PFN_vkCmdWriteMicromapsPropertiesEXT vkCmdWriteMicromapsPropertiesEXT;
                PFN_vkCopyMemoryToMicromapEXT vkCopyMemoryToMicromapEXT;
                PFN_vkCopyMicromapEXT vkCopyMicromapEXT;
                PFN_vkCopyMicromapToMemoryEXT vkCopyMicromapToMemoryEXT;
                PFN_vkCreateMicromapEXT vkCreateMicromapEXT;
                PFN_vkDestroyMicromapEXT vkDestroyMicromapEXT;
                PFN_vkGetDeviceMicromapCompatibilityEXT vkGetDeviceMicromapCompatibilityEXT;
                PFN_vkGetMicromapBuildSizesEXT vkGetMicromapBuildSizesEXT;
                PFN_vkWriteMicromapsPropertiesEXT vkWriteMicromapsPropertiesEXT;
            #endif
            #if defined(VK_EXT_pageable_device_local_memory)
                PFN_vkSetDeviceMemoryPriorityEXT vkSetDeviceMemoryPriorityEXT;
            #endif
            #if defined(VK_EXT_pipeline_properties)
                PFN_vkGetPipelinePropertiesEXT vkGetPipelinePropertiesEXT;
            #endif
            #if defined(VK_EXT_private_data)
                PFN_vkCreatePrivateDataSlotEXT vkCreatePrivateDataSlotEXT;
                PFN_vkDestroyPrivateDataSlotEXT vkDestroyPrivateDataSlotEXT;
                PFN_vkGetPrivateDataEXT vkGetPrivateDataEXT;
                PFN_vkSetPrivateDataEXT vkSetPrivateDataEXT;
            #endif
            #if defined(VK_EXT_sample_locations)
                PFN_vkCmdSetSampleLocationsEXT vkCmdSetSampleLocationsEXT;
            #endif
            #if defined(VK_EXT_shader_module_identifier)
                PFN_vkGetShaderModuleCreateInfoIdentifierEXT vkGetShaderModuleCreateInfoIdentifierEXT;
                PFN_vkGetShaderModuleIdentifierEXT vkGetShaderModuleIdentifierEXT;
            #endif
            #if defined(VK_EXT_shader_object)
                PFN_vkCmdBindShadersEXT vkCmdBindShadersEXT;
                PFN_vkCreateShadersEXT vkCreateShadersEXT;
                PFN_vkDestroyShaderEXT vkDestroyShaderEXT;
                PFN_vkGetShaderBinaryDataEXT vkGetShaderBinaryDataEXT;
            #endif
            #if defined(VK_EXT_swapchain_maintenance1)
                PFN_vkReleaseSwapchainImagesEXT vkReleaseSwapchainImagesEXT;
            #endif
            #if defined(VK_EXT_transform_feedback)
                PFN_vkCmdBeginQueryIndexedEXT vkCmdBeginQueryIndexedEXT;
                PFN_vkCmdBeginTransformFeedbackEXT vkCmdBeginTransformFeedbackEXT;
                PFN_vkCmdBindTransformFeedbackBuffersEXT vkCmdBindTransformFeedbackBuffersEXT;
                PFN_vkCmdDrawIndirectByteCountEXT vkCmdDrawIndirectByteCountEXT;
                PFN_vkCmdEndQueryIndexedEXT vkCmdEndQueryIndexedEXT;
                PFN_vkCmdEndTransformFeedbackEXT vkCmdEndTransformFeedbackEXT;
            #endif
            #if defined(VK_EXT_validation_cache)
                PFN_vkCreateValidationCacheEXT vkCreateValidationCacheEXT;
                PFN_vkDestroyValidationCacheEXT vkDestroyValidationCacheEXT;
                PFN_vkGetValidationCacheDataEXT vkGetValidationCacheDataEXT;
                PFN_vkMergeValidationCachesEXT vkMergeValidationCachesEXT;
            #endif
            #if defined(VK_FUCHSIA_buffer_collection)
                PFN_vkCreateBufferCollectionFUCHSIA vkCreateBufferCollectionFUCHSIA;
                PFN_vkDestroyBufferCollectionFUCHSIA vkDestroyBufferCollectionFUCHSIA;
                PFN_vkGetBufferCollectionPropertiesFUCHSIA vkGetBufferCollectionPropertiesFUCHSIA;
                PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA vkSetBufferCollectionBufferConstraintsFUCHSIA;
                PFN_vkSetBufferCollectionImageConstraintsFUCHSIA vkSetBufferCollectionImageConstraintsFUCHSIA;
            #endif
            #if defined(VK_FUCHSIA_external_memory)
                PFN_vkGetMemoryZirconHandleFUCHSIA vkGetMemoryZirconHandleFUCHSIA;
                PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA vkGetMemoryZirconHandlePropertiesFUCHSIA;
            #endif
            #if defined(VK_FUCHSIA_external_semaphore)
                PFN_vkGetSemaphoreZirconHandleFUCHSIA vkGetSemaphoreZirconHandleFUCHSIA;
                PFN_vkImportSemaphoreZirconHandleFUCHSIA vkImportSemaphoreZirconHandleFUCHSIA;
            #endif
            #if defined(VK_GOOGLE_display_timing)
                PFN_vkGetPastPresentationTimingGOOGLE vkGetPastPresentationTimingGOOGLE;
                PFN_vkGetRefreshCycleDurationGOOGLE vkGetRefreshCycleDurationGOOGLE;
            #endif
            #if defined(VK_HUAWEI_cluster_culling_shader)
                PFN_vkCmdDrawClusterHUAWEI vkCmdDrawClusterHUAWEI;
                PFN_vkCmdDrawClusterIndirectHUAWEI vkCmdDrawClusterIndirectHUAWEI;
            #endif
            #if defined(VK_HUAWEI_invocation_mask)
                PFN_vkCmdBindInvocationMaskHUAWEI vkCmdBindInvocationMaskHUAWEI;
            #endif
            #if defined(VK_HUAWEI_subpass_shading)
                PFN_vkCmdSubpassShadingHUAWEI vkCmdSubpassShadingHUAWEI;
                PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI;
            #endif
            #if defined(VK_INTEL_performance_query)
                PFN_vkAcquirePerformanceConfigurationINTEL vkAcquirePerformanceConfigurationINTEL;
                PFN_vkCmdSetPerformanceMarkerINTEL vkCmdSetPerformanceMarkerINTEL;
                PFN_vkCmdSetPerformanceOverrideINTEL vkCmdSetPerformanceOverrideINTEL;
                PFN_vkCmdSetPerformanceStreamMarkerINTEL vkCmdSetPerformanceStreamMarkerINTEL;
                PFN_vkGetPerformanceParameterINTEL vkGetPerformanceParameterINTEL;
                PFN_vkInitializePerformanceApiINTEL vkInitializePerformanceApiINTEL;
                PFN_vkQueueSetPerformanceConfigurationINTEL vkQueueSetPerformanceConfigurationINTEL;
                PFN_vkReleasePerformanceConfigurationINTEL vkReleasePerformanceConfigurationINTEL;
                PFN_vkUninitializePerformanceApiINTEL vkUninitializePerformanceApiINTEL;
            #endif
            #if defined(VK_KHR_acceleration_structure)
                PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
                PFN_vkCmdBuildAccelerationStructuresIndirectKHR vkCmdBuildAccelerationStructuresIndirectKHR;
                PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
                PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR;
                PFN_vkCmdCopyAccelerationStructureToMemoryKHR vkCmdCopyAccelerationStructureToMemoryKHR;
                PFN_vkCmdCopyMemoryToAccelerationStructureKHR vkCmdCopyMemoryToAccelerationStructureKHR;
                PFN_vkCmdWriteAccelerationStructuresPropertiesKHR vkCmdWriteAccelerationStructuresPropertiesKHR;
                PFN_vkCopyAccelerationStructureKHR vkCopyAccelerationStructureKHR;
                PFN_vkCopyAccelerationStructureToMemoryKHR vkCopyAccelerationStructureToMemoryKHR;
                PFN_vkCopyMemoryToAccelerationStructureKHR vkCopyMemoryToAccelerationStructureKHR;
                PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
                PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
                PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
                PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
                PFN_vkGetDeviceAccelerationStructureCompatibilityKHR vkGetDeviceAccelerationStructureCompatibilityKHR;
                PFN_vkWriteAccelerationStructuresPropertiesKHR vkWriteAccelerationStructuresPropertiesKHR;
            #endif
            #if defined(VK_KHR_bind_memory2)
                PFN_vkBindBufferMemory2KHR vkBindBufferMemory2KHR;
                PFN_vkBindImageMemory2KHR vkBindImageMemory2KHR;
            #endif
            #if defined(VK_KHR_buffer_device_address)
                PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
                PFN_vkGetBufferOpaqueCaptureAddressKHR vkGetBufferOpaqueCaptureAddressKHR;
                PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR vkGetDeviceMemoryOpaqueCaptureAddressKHR;
            #endif
            #if defined(VK_KHR_copy_commands2)
                PFN_vkCmdBlitImage2KHR vkCmdBlitImage2KHR;
                PFN_vkCmdCopyBuffer2KHR vkCmdCopyBuffer2KHR;
                PFN_vkCmdCopyBufferToImage2KHR vkCmdCopyBufferToImage2KHR;
                PFN_vkCmdCopyImage2KHR vkCmdCopyImage2KHR;
                PFN_vkCmdCopyImageToBuffer2KHR vkCmdCopyImageToBuffer2KHR;
                PFN_vkCmdResolveImage2KHR vkCmdResolveImage2KHR;
            #endif
            #if defined(VK_KHR_create_renderpass2)
                PFN_vkCmdBeginRenderPass2KHR vkCmdBeginRenderPass2KHR;
                PFN_vkCmdEndRenderPass2KHR vkCmdEndRenderPass2KHR;
                PFN_vkCmdNextSubpass2KHR vkCmdNextSubpass2KHR;
                PFN_vkCreateRenderPass2KHR vkCreateRenderPass2KHR;
            #endif
            #if defined(VK_KHR_deferred_host_operations)
                PFN_vkCreateDeferredOperationKHR vkCreateDeferredOperationKHR;
                PFN_vkDeferredOperationJoinKHR vkDeferredOperationJoinKHR;
                PFN_vkDestroyDeferredOperationKHR vkDestroyDeferredOperationKHR;
                PFN_vkGetDeferredOperationMaxConcurrencyKHR vkGetDeferredOperationMaxConcurrencyKHR;
                PFN_vkGetDeferredOperationResultKHR vkGetDeferredOperationResultKHR;
            #endif
            #if defined(VK_KHR_descriptor_update_template)
                PFN_vkCreateDescriptorUpdateTemplateKHR vkCreateDescriptorUpdateTemplateKHR;
                PFN_vkDestroyDescriptorUpdateTemplateKHR vkDestroyDescriptorUpdateTemplateKHR;
                PFN_vkUpdateDescriptorSetWithTemplateKHR vkUpdateDescriptorSetWithTemplateKHR;
            #endif
            #if defined(VK_KHR_device_group)
                PFN_vkCmdDispatchBaseKHR vkCmdDispatchBaseKHR;
                PFN_vkCmdSetDeviceMaskKHR vkCmdSetDeviceMaskKHR;
                PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR vkGetDeviceGroupPeerMemoryFeaturesKHR;
            #endif
            #if defined(VK_KHR_display_swapchain)
                PFN_vkCreateSharedSwapchainsKHR vkCreateSharedSwapchainsKHR;
            #endif
            #if defined(VK_KHR_draw_indirect_count)
                PFN_vkCmdDrawIndexedIndirectCountKHR vkCmdDrawIndexedIndirectCountKHR;
                PFN_vkCmdDrawIndirectCountKHR vkCmdDrawIndirectCountKHR;
            #endif
            #if defined(VK_KHR_dynamic_rendering)
                PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR;
                PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR;
            #endif
            #if defined(VK_KHR_external_fence_fd)
                PFN_vkGetFenceFdKHR vkGetFenceFdKHR;
                PFN_vkImportFenceFdKHR vkImportFenceFdKHR;
            #endif
            #if defined(VK_KHR_external_fence_win32)
                PFN_vkGetFenceWin32HandleKHR vkGetFenceWin32HandleKHR;
                PFN_vkImportFenceWin32HandleKHR vkImportFenceWin32HandleKHR;
            #endif
            #if defined(VK_KHR_external_memory_fd)
                PFN_vkGetMemoryFdKHR vkGetMemoryFdKHR;
                PFN_vkGetMemoryFdPropertiesKHR vkGetMemoryFdPropertiesKHR;
            #endif
            #if defined(VK_KHR_external_memory_win32)
                PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR;
                PFN_vkGetMemoryWin32HandlePropertiesKHR vkGetMemoryWin32HandlePropertiesKHR;
            #endif
            #if defined(VK_KHR_external_semaphore_fd)
                PFN_vkGetSemaphoreFdKHR vkGetSemaphoreFdKHR;
                PFN_vkImportSemaphoreFdKHR vkImportSemaphoreFdKHR;
            #endif
            #if defined(VK_KHR_external_semaphore_win32)
                PFN_vkGetSemaphoreWin32HandleKHR vkGetSemaphoreWin32HandleKHR;
                PFN_vkImportSemaphoreWin32HandleKHR vkImportSemaphoreWin32HandleKHR;
            #endif
            #if defined(VK_KHR_fragment_shading_rate)
                PFN_vkCmdSetFragmentShadingRateKHR vkCmdSetFragmentShadingRateKHR;
            #endif
            #if defined(VK_KHR_get_memory_requirements2)
                PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR;
                PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR;
                PFN_vkGetImageSparseMemoryRequirements2KHR vkGetImageSparseMemoryRequirements2KHR;
            #endif
            #if defined(VK_KHR_maintenance1)
                PFN_vkTrimCommandPoolKHR vkTrimCommandPoolKHR;
            #endif
            #if defined(VK_KHR_maintenance3)
                PFN_vkGetDescriptorSetLayoutSupportKHR vkGetDescriptorSetLayoutSupportKHR;
            #endif
            #if defined(VK_KHR_maintenance4)
                PFN_vkGetDeviceBufferMemoryRequirementsKHR vkGetDeviceBufferMemoryRequirementsKHR;
                PFN_vkGetDeviceImageMemoryRequirementsKHR vkGetDeviceImageMemoryRequirementsKHR;
                PFN_vkGetDeviceImageSparseMemoryRequirementsKHR vkGetDeviceImageSparseMemoryRequirementsKHR;
            #endif
            #if defined(VK_KHR_maintenance5)
                PFN_vkCmdBindIndexBuffer2KHR vkCmdBindIndexBuffer2KHR;
                PFN_vkGetDeviceImageSubresourceLayoutKHR vkGetDeviceImageSubresourceLayoutKHR;
                PFN_vkGetImageSubresourceLayout2KHR vkGetImageSubresourceLayout2KHR;
                PFN_vkGetRenderingAreaGranularityKHR vkGetRenderingAreaGranularityKHR;
            #endif
            #if defined(VK_KHR_map_memory2)
                PFN_vkMapMemory2KHR vkMapMemory2KHR;
            PFN_vkUnmapMemory2KHR vkUnmapMemory2KHR;
            #endif
            #if defined(VK_KHR_performance_query)
                PFN_vkAcquireProfilingLockKHR vkAcquireProfilingLockKHR;
                PFN_vkReleaseProfilingLockKHR vkReleaseProfilingLockKHR;
            #endif
            #if defined(VK_KHR_pipeline_executable_properties)
                PFN_vkGetPipelineExecutableInternalRepresentationsKHR vkGetPipelineExecutableInternalRepresentationsKHR;
                PFN_vkGetPipelineExecutablePropertiesKHR vkGetPipelineExecutablePropertiesKHR;
                PFN_vkGetPipelineExecutableStatisticsKHR vkGetPipelineExecutableStatisticsKHR;
            #endif
            #if defined(VK_KHR_present_wait)
                PFN_vkWaitForPresentKHR vkWaitForPresentKHR;
            #endif
            #if defined(VK_KHR_push_descriptor)
                PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR;
            #endif
            #if defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline)
                PFN_vkCmdTraceRaysIndirect2KHR vkCmdTraceRaysIndirect2KHR;
            #endif
            #if defined(VK_KHR_ray_tracing_pipeline)
                PFN_vkCmdSetRayTracingPipelineStackSizeKHR vkCmdSetRayTracingPipelineStackSizeKHR;
                PFN_vkCmdTraceRaysIndirectKHR vkCmdTraceRaysIndirectKHR;
                PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
                PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;
                PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR vkGetRayTracingCaptureReplayShaderGroupHandlesKHR;
                PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
                PFN_vkGetRayTracingShaderGroupStackSizeKHR vkGetRayTracingShaderGroupStackSizeKHR;
            #endif
            #if defined(VK_KHR_sampler_ycbcr_conversion)
                PFN_vkCreateSamplerYcbcrConversionKHR vkCreateSamplerYcbcrConversionKHR;
                PFN_vkDestroySamplerYcbcrConversionKHR vkDestroySamplerYcbcrConversionKHR;
            #endif
            #if defined(VK_KHR_shared_presentable_image)
                PFN_vkGetSwapchainStatusKHR vkGetSwapchainStatusKHR;
            #endif
            #if defined(VK_KHR_swapchain)
                PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
                PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
                PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
                PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
                PFN_vkQueuePresentKHR vkQueuePresentKHR;
            #endif
            #if defined(VK_KHR_synchronization2)
                PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR;
                PFN_vkCmdResetEvent2KHR vkCmdResetEvent2KHR;
                PFN_vkCmdSetEvent2KHR vkCmdSetEvent2KHR;
                PFN_vkCmdWaitEvents2KHR vkCmdWaitEvents2KHR;
                PFN_vkCmdWriteTimestamp2KHR vkCmdWriteTimestamp2KHR;
                PFN_vkQueueSubmit2KHR vkQueueSubmit2KHR;
            #endif
            #if defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker)
                PFN_vkCmdWriteBufferMarker2AMD vkCmdWriteBufferMarker2AMD;
            #endif
            #if defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints)
                PFN_vkGetQueueCheckpointData2NV vkGetQueueCheckpointData2NV;
            #endif
            #if defined(VK_KHR_timeline_semaphore)
                PFN_vkGetSemaphoreCounterValueKHR vkGetSemaphoreCounterValueKHR;
                PFN_vkSignalSemaphoreKHR vkSignalSemaphoreKHR;
                PFN_vkWaitSemaphoresKHR vkWaitSemaphoresKHR;
            #endif
            #if defined(VK_KHR_video_decode_queue)
                PFN_vkCmdDecodeVideoKHR vkCmdDecodeVideoKHR;
            #endif
            #if defined(VK_KHR_video_encode_queue)
                PFN_vkCmdEncodeVideoKHR vkCmdEncodeVideoKHR;
                PFN_vkGetEncodedVideoSessionParametersKHR vkGetEncodedVideoSessionParametersKHR;
            #endif
            #if defined(VK_KHR_video_queue)
                PFN_vkBindVideoSessionMemoryKHR vkBindVideoSessionMemoryKHR;
                PFN_vkCmdBeginVideoCodingKHR vkCmdBeginVideoCodingKHR;
                PFN_vkCmdControlVideoCodingKHR vkCmdControlVideoCodingKHR;
                PFN_vkCmdEndVideoCodingKHR vkCmdEndVideoCodingKHR;
                PFN_vkCreateVideoSessionKHR vkCreateVideoSessionKHR;
                PFN_vkCreateVideoSessionParametersKHR vkCreateVideoSessionParametersKHR;
                PFN_vkDestroyVideoSessionKHR vkDestroyVideoSessionKHR;
                PFN_vkDestroyVideoSessionParametersKHR vkDestroyVideoSessionParametersKHR;
                PFN_vkGetVideoSessionMemoryRequirementsKHR vkGetVideoSessionMemoryRequirementsKHR;
                PFN_vkUpdateVideoSessionParametersKHR vkUpdateVideoSessionParametersKHR;
            #endif
            #if defined(VK_NVX_binary_import)
                PFN_vkCmdCuLaunchKernelNVX vkCmdCuLaunchKernelNVX;
                PFN_vkCreateCuFunctionNVX vkCreateCuFunctionNVX;
                PFN_vkCreateCuModuleNVX vkCreateCuModuleNVX;
                PFN_vkDestroyCuFunctionNVX vkDestroyCuFunctionNVX;
                PFN_vkDestroyCuModuleNVX vkDestroyCuModuleNVX;
            #endif
            #if defined(VK_NVX_image_view_handle)
                PFN_vkGetImageViewAddressNVX vkGetImageViewAddressNVX;
                PFN_vkGetImageViewHandleNVX vkGetImageViewHandleNVX;
            #endif
            #if defined(VK_NV_clip_space_w_scaling)
                PFN_vkCmdSetViewportWScalingNV vkCmdSetViewportWScalingNV;
            #endif
            #if defined(VK_NV_copy_memory_indirect)
                PFN_vkCmdCopyMemoryIndirectNV vkCmdCopyMemoryIndirectNV;
                PFN_vkCmdCopyMemoryToImageIndirectNV vkCmdCopyMemoryToImageIndirectNV;
            #endif
            #if defined(VK_NV_device_diagnostic_checkpoints)
                PFN_vkCmdSetCheckpointNV vkCmdSetCheckpointNV;
                PFN_vkGetQueueCheckpointDataNV vkGetQueueCheckpointDataNV;
            #endif
            #if defined(VK_NV_device_generated_commands)
                PFN_vkCmdBindPipelineShaderGroupNV vkCmdBindPipelineShaderGroupNV;
                PFN_vkCmdExecuteGeneratedCommandsNV vkCmdExecuteGeneratedCommandsNV;
                PFN_vkCmdPreprocessGeneratedCommandsNV vkCmdPreprocessGeneratedCommandsNV;
                PFN_vkCreateIndirectCommandsLayoutNV vkCreateIndirectCommandsLayoutNV;
                PFN_vkDestroyIndirectCommandsLayoutNV vkDestroyIndirectCommandsLayoutNV;
                PFN_vkGetGeneratedCommandsMemoryRequirementsNV vkGetGeneratedCommandsMemoryRequirementsNV;
            #endif
            #if defined(VK_NV_device_generated_commands_compute)
                PFN_vkCmdUpdatePipelineIndirectBufferNV vkCmdUpdatePipelineIndirectBufferNV;
                PFN_vkGetPipelineIndirectDeviceAddressNV vkGetPipelineIndirectDeviceAddressNV;
                PFN_vkGetPipelineIndirectMemoryRequirementsNV vkGetPipelineIndirectMemoryRequirementsNV;
            #endif
            #if defined(VK_NV_external_memory_rdma)
                PFN_vkGetMemoryRemoteAddressNV vkGetMemoryRemoteAddressNV;
            #endif
            #if defined(VK_NV_external_memory_win32)
                PFN_vkGetMemoryWin32HandleNV vkGetMemoryWin32HandleNV;
            #endif
            #if defined(VK_NV_fragment_shading_rate_enums)
                PFN_vkCmdSetFragmentShadingRateEnumNV vkCmdSetFragmentShadingRateEnumNV;
            #endif
            #if defined(VK_NV_memory_decompression)
                PFN_vkCmdDecompressMemoryIndirectCountNV vkCmdDecompressMemoryIndirectCountNV;
                PFN_vkCmdDecompressMemoryNV vkCmdDecompressMemoryNV;
            #endif
            #if defined(VK_NV_mesh_shader)
                PFN_vkCmdDrawMeshTasksIndirectCountNV vkCmdDrawMeshTasksIndirectCountNV;
                PFN_vkCmdDrawMeshTasksIndirectNV vkCmdDrawMeshTasksIndirectNV;
                PFN_vkCmdDrawMeshTasksNV vkCmdDrawMeshTasksNV;
            #endif
            #if defined(VK_NV_optical_flow)
                PFN_vkBindOpticalFlowSessionImageNV vkBindOpticalFlowSessionImageNV;
                PFN_vkCmdOpticalFlowExecuteNV vkCmdOpticalFlowExecuteNV;
                PFN_vkCreateOpticalFlowSessionNV vkCreateOpticalFlowSessionNV;
                PFN_vkDestroyOpticalFlowSessionNV vkDestroyOpticalFlowSessionNV;
            #endif
            #if defined(VK_NV_ray_tracing)
                PFN_vkBindAccelerationStructureMemoryNV vkBindAccelerationStructureMemoryNV;
                PFN_vkCmdBuildAccelerationStructureNV vkCmdBuildAccelerationStructureNV;
                PFN_vkCmdCopyAccelerationStructureNV vkCmdCopyAccelerationStructureNV;
                PFN_vkCmdTraceRaysNV vkCmdTraceRaysNV;
                PFN_vkCmdWriteAccelerationStructuresPropertiesNV vkCmdWriteAccelerationStructuresPropertiesNV;
                PFN_vkCompileDeferredNV vkCompileDeferredNV;
                PFN_vkCreateAccelerationStructureNV vkCreateAccelerationStructureNV;
                PFN_vkCreateRayTracingPipelinesNV vkCreateRayTracingPipelinesNV;
                PFN_vkDestroyAccelerationStructureNV vkDestroyAccelerationStructureNV;
                PFN_vkGetAccelerationStructureHandleNV vkGetAccelerationStructureHandleNV;
                PFN_vkGetAccelerationStructureMemoryRequirementsNV vkGetAccelerationStructureMemoryRequirementsNV;
                PFN_vkGetRayTracingShaderGroupHandlesNV vkGetRayTracingShaderGroupHandlesNV;
            #endif
            #if defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2
                PFN_vkCmdSetExclusiveScissorEnableNV vkCmdSetExclusiveScissorEnableNV;
            #endif
            #if defined(VK_NV_scissor_exclusive)
                PFN_vkCmdSetExclusiveScissorNV vkCmdSetExclusiveScissorNV;
            #endif
            #if defined(VK_NV_shading_rate_image)
                PFN_vkCmdBindShadingRateImageNV vkCmdBindShadingRateImageNV;
                PFN_vkCmdSetCoarseSampleOrderNV vkCmdSetCoarseSampleOrderNV;
                PFN_vkCmdSetViewportShadingRatePaletteNV vkCmdSetViewportShadingRatePaletteNV;
            #endif
            #if defined(VK_QCOM_tile_properties)
                PFN_vkGetDynamicRenderingTilePropertiesQCOM vkGetDynamicRenderingTilePropertiesQCOM;
                PFN_vkGetFramebufferTilePropertiesQCOM vkGetFramebufferTilePropertiesQCOM;
            #endif
            #if defined(VK_QNX_external_memory_screen_buffer)
                PFN_vkGetScreenBufferPropertiesQNX vkGetScreenBufferPropertiesQNX;
            #endif
            #if defined(VK_VALVE_descriptor_set_host_mapping)
                PFN_vkGetDescriptorSetHostMappingVALVE vkGetDescriptorSetHostMappingVALVE;
                PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE vkGetDescriptorSetLayoutHostMappingInfoVALVE;
            #endif
            #if (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object))
                PFN_vkCmdBindVertexBuffers2EXT vkCmdBindVertexBuffers2EXT;
                PFN_vkCmdSetCullModeEXT vkCmdSetCullModeEXT;
                PFN_vkCmdSetDepthBoundsTestEnableEXT vkCmdSetDepthBoundsTestEnableEXT;
                PFN_vkCmdSetDepthCompareOpEXT vkCmdSetDepthCompareOpEXT;
                PFN_vkCmdSetDepthTestEnableEXT vkCmdSetDepthTestEnableEXT;
                PFN_vkCmdSetDepthWriteEnableEXT vkCmdSetDepthWriteEnableEXT;
                PFN_vkCmdSetFrontFaceEXT vkCmdSetFrontFaceEXT;
                PFN_vkCmdSetPrimitiveTopologyEXT vkCmdSetPrimitiveTopologyEXT;
                PFN_vkCmdSetScissorWithCountEXT vkCmdSetScissorWithCountEXT;
                PFN_vkCmdSetStencilOpEXT vkCmdSetStencilOpEXT;
                PFN_vkCmdSetStencilTestEnableEXT vkCmdSetStencilTestEnableEXT;
                PFN_vkCmdSetViewportWithCountEXT vkCmdSetViewportWithCountEXT;
            #endif
            #if (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object))
                PFN_vkCmdSetDepthBiasEnableEXT vkCmdSetDepthBiasEnableEXT;
                PFN_vkCmdSetLogicOpEXT vkCmdSetLogicOpEXT;
                PFN_vkCmdSetPatchControlPointsEXT vkCmdSetPatchControlPointsEXT;
                PFN_vkCmdSetPrimitiveRestartEnableEXT vkCmdSetPrimitiveRestartEnableEXT;
                PFN_vkCmdSetRasterizerDiscardEnableEXT vkCmdSetRasterizerDiscardEnableEXT;
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object))
                PFN_vkCmdSetAlphaToCoverageEnableEXT vkCmdSetAlphaToCoverageEnableEXT;
                PFN_vkCmdSetAlphaToOneEnableEXT vkCmdSetAlphaToOneEnableEXT;
                PFN_vkCmdSetColorBlendAdvancedEXT vkCmdSetColorBlendAdvancedEXT;
                PFN_vkCmdSetColorBlendEnableEXT vkCmdSetColorBlendEnableEXT;
                PFN_vkCmdSetColorBlendEquationEXT vkCmdSetColorBlendEquationEXT;
                PFN_vkCmdSetColorWriteMaskEXT vkCmdSetColorWriteMaskEXT;
                PFN_vkCmdSetConservativeRasterizationModeEXT vkCmdSetConservativeRasterizationModeEXT;
                PFN_vkCmdSetDepthClampEnableEXT vkCmdSetDepthClampEnableEXT;
                PFN_vkCmdSetDepthClipEnableEXT vkCmdSetDepthClipEnableEXT;
                PFN_vkCmdSetDepthClipNegativeOneToOneEXT vkCmdSetDepthClipNegativeOneToOneEXT;
                PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT vkCmdSetExtraPrimitiveOverestimationSizeEXT;
                PFN_vkCmdSetLineRasterizationModeEXT vkCmdSetLineRasterizationModeEXT;
                PFN_vkCmdSetLineStippleEnableEXT vkCmdSetLineStippleEnableEXT;
                PFN_vkCmdSetLogicOpEnableEXT vkCmdSetLogicOpEnableEXT;
                PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT;
                PFN_vkCmdSetProvokingVertexModeEXT vkCmdSetProvokingVertexModeEXT;
                PFN_vkCmdSetRasterizationSamplesEXT vkCmdSetRasterizationSamplesEXT;
                PFN_vkCmdSetRasterizationStreamEXT vkCmdSetRasterizationStreamEXT;
                PFN_vkCmdSetSampleLocationsEnableEXT vkCmdSetSampleLocationsEnableEXT;
                PFN_vkCmdSetSampleMaskEXT vkCmdSetSampleMaskEXT;
                PFN_vkCmdSetTessellationDomainOriginEXT vkCmdSetTessellationDomainOriginEXT;
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling))
                PFN_vkCmdSetViewportWScalingEnableNV vkCmdSetViewportWScalingEnableNV;
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle))
                PFN_vkCmdSetViewportSwizzleNV vkCmdSetViewportSwizzleNV;
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color))
                PFN_vkCmdSetCoverageToColorEnableNV vkCmdSetCoverageToColorEnableNV;
                PFN_vkCmdSetCoverageToColorLocationNV vkCmdSetCoverageToColorLocationNV;
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples))
                PFN_vkCmdSetCoverageModulationModeNV vkCmdSetCoverageModulationModeNV;
                PFN_vkCmdSetCoverageModulationTableEnableNV vkCmdSetCoverageModulationTableEnableNV;
                PFN_vkCmdSetCoverageModulationTableNV vkCmdSetCoverageModulationTableNV;
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image))
                PFN_vkCmdSetShadingRateImageEnableNV vkCmdSetShadingRateImageEnableNV;
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test))
                PFN_vkCmdSetRepresentativeFragmentTestEnableNV vkCmdSetRepresentativeFragmentTestEnableNV;
            #endif
            #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode))
                PFN_vkCmdSetCoverageReductionModeNV vkCmdSetCoverageReductionModeNV;
            #endif
            #if (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1))
                PFN_vkGetDeviceGroupSurfacePresentModes2EXT vkGetDeviceGroupSurfacePresentModes2EXT;
            #endif
            #if (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control))
                PFN_vkGetImageSubresourceLayout2EXT vkGetImageSubresourceLayout2EXT;
            #endif
            #if (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state))
                PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT;
            #endif
            #if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template))
                PFN_vkCmdPushDescriptorSetWithTemplateKHR vkCmdPushDescriptorSetWithTemplateKHR;
            #endif
            #if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
                PFN_vkGetDeviceGroupPresentCapabilitiesKHR vkGetDeviceGroupPresentCapabilitiesKHR;
                PFN_vkGetDeviceGroupSurfacePresentModesKHR vkGetDeviceGroupSurfacePresentModesKHR;
            #endif
            #if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
                PFN_vkAcquireNextImage2KHR vkAcquireNextImage2KHR;
            #endif
        } functionTable = { };

        VkDevice logicalDevice = VK_NULL_HANDLE;
        VmaAllocator vmaAllocator = VK_NULL_HANDLE;

        uint32 generalQueueFamily = 0;
        VkQueue generalQueue = VK_NULL_HANDLE;

        mutable uint64 lastReservedSignalValue = 0;
        VkSemaphore sharedTimelineSemaphore = VK_NULL_HANDLE;

        struct VulkanDeviceExtension
        {
            std::string name;
            void* data = nullptr;
            std::vector<VulkanDeviceExtension> dependencies = { };
            bool requiredOnlyIfSupported = false;
        };
        const std::vector<VulkanDeviceExtension> DEVICE_EXTENSIONS_TO_QUERY
        {
            #if SR_ENABLE_LOGGING
            {
                .name = VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
                .dependencies = {
                    {
                        .name = VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
                        .requiredOnlyIfSupported = true
                    }
                },
                .requiredOnlyIfSupported = true
            },
            #endif
            #if SR_PLATFORM_APPLE
            {
                .name = VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
                .data = new VkPhysicalDevicePortabilitySubsetFeaturesKHR {
                    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR
                },
                .requiredOnlyIfSupported = true
            },
            #endif
            {
                // Core in Vulkan 1.1
                .name = VK_KHR_MAINTENANCE_1_EXTENSION_NAME
            },
            {
                .name = VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME,
                .data = new VkPhysicalDeviceImagelessFramebufferFeaturesKHR {
                    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR,
                    .imagelessFramebuffer = VK_TRUE
                },
                .dependencies = {
                    {
                        .name = VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME
                    }
                }
            },
            {
                .name = VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME
            },
            {
                // Core in Vulkan 1.2
                .name = VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
                .data = new VkPhysicalDeviceTimelineSemaphoreFeaturesKHR {
                    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR,
                    .timelineSemaphore = VK_TRUE
                }
            },
            {
                .name = VK_KHR_SWAPCHAIN_EXTENSION_NAME
            },
        };
        std::vector<Hash> loadedExtensions;

        static bool IsExtensionSupported(const char* extensionName, const std::vector<VkExtensionProperties> &supportedExtensions);
        bool AddExtensionIfSupported(const VulkanDeviceExtension &extension, const std::vector<VkExtensionProperties> &supportedExtensions, void* pNextChain, std::vector<const char*> &extensionList, std::vector<void*> &extensionDataToFree);

    };

}
