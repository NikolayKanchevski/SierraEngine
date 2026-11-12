//
// Created by Nikolay Kantchevski on 9/28/26.
//

#include "VulkanDevice.h"

namespace Sierra
{
    VulkanDeviceFunctionTable LoadVulkanDeviceFunctionTable(const VkDevice device) noexcept
    {
        return
        {
        #if defined(VK_VERSION_1_0)
            .vkAllocateCommandBuffers           = reinterpret_cast<PFN_vkAllocateCommandBuffers>(vkGetDeviceProcAddr(device, "vkAllocateCommandBuffers")),
            .vkAllocateDescriptorSets           = reinterpret_cast<PFN_vkAllocateDescriptorSets>(vkGetDeviceProcAddr(device, "vkAllocateDescriptorSets")),
            .vkAllocateMemory                   = reinterpret_cast<PFN_vkAllocateMemory>(vkGetDeviceProcAddr(device, "vkAllocateMemory")),
            .vkBeginCommandBuffer               = reinterpret_cast<PFN_vkBeginCommandBuffer>(vkGetDeviceProcAddr(device, "vkBeginCommandBuffer")),
            .vkBindBufferMemory                 = reinterpret_cast<PFN_vkBindBufferMemory>(vkGetDeviceProcAddr(device, "vkBindBufferMemory")),
            .vkBindImageMemory                  = reinterpret_cast<PFN_vkBindImageMemory>(vkGetDeviceProcAddr(device, "vkBindImageMemory")),
            .vkCmdBeginQuery                    = reinterpret_cast<PFN_vkCmdBeginQuery>(vkGetDeviceProcAddr(device, "vkCmdBeginQuery")),
            .vkCmdBeginRenderPass               = reinterpret_cast<PFN_vkCmdBeginRenderPass>(vkGetDeviceProcAddr(device, "vkCmdBeginRenderPass")),
            .vkCmdBindDescriptorSets            = reinterpret_cast<PFN_vkCmdBindDescriptorSets>(vkGetDeviceProcAddr(device, "vkCmdBindDescriptorSets")),
            .vkCmdBindIndexBuffer               = reinterpret_cast<PFN_vkCmdBindIndexBuffer>(vkGetDeviceProcAddr(device, "vkCmdBindIndexBuffer")),
            .vkCmdBindPipeline                  = reinterpret_cast<PFN_vkCmdBindPipeline>(vkGetDeviceProcAddr(device, "vkCmdBindPipeline")),
            .vkCmdBindVertexBuffers             = reinterpret_cast<PFN_vkCmdBindVertexBuffers>(vkGetDeviceProcAddr(device, "vkCmdBindVertexBuffers")),
            .vkCmdBlitImage                     = reinterpret_cast<PFN_vkCmdBlitImage>(vkGetDeviceProcAddr(device, "vkCmdBlitImage")),
            .vkCmdClearAttachments              = reinterpret_cast<PFN_vkCmdClearAttachments>(vkGetDeviceProcAddr(device, "vkCmdClearAttachments")),
            .vkCmdClearColorImage               = reinterpret_cast<PFN_vkCmdClearColorImage>(vkGetDeviceProcAddr(device, "vkCmdClearColorImage")),
            .vkCmdClearDepthStencilImage        = reinterpret_cast<PFN_vkCmdClearDepthStencilImage>(vkGetDeviceProcAddr(device, "vkCmdClearDepthStencilImage")),
            .vkCmdCopyBuffer                    = reinterpret_cast<PFN_vkCmdCopyBuffer>(vkGetDeviceProcAddr(device, "vkCmdCopyBuffer")),
            .vkCmdCopyBufferToImage             = reinterpret_cast<PFN_vkCmdCopyBufferToImage>(vkGetDeviceProcAddr(device, "vkCmdCopyBufferToImage")),
            .vkCmdCopyImage                     = reinterpret_cast<PFN_vkCmdCopyImage>(vkGetDeviceProcAddr(device, "vkCmdCopyImage")),
            .vkCmdCopyImageToBuffer             = reinterpret_cast<PFN_vkCmdCopyImageToBuffer>(vkGetDeviceProcAddr(device, "vkCmdCopyImageToBuffer")),
            .vkCmdCopyQueryPoolResults          = reinterpret_cast<PFN_vkCmdCopyQueryPoolResults>(vkGetDeviceProcAddr(device, "vkCmdCopyQueryPoolResults")),
            .vkCmdDispatch                      = reinterpret_cast<PFN_vkCmdDispatch>(vkGetDeviceProcAddr(device, "vkCmdDispatch")),
            .vkCmdDispatchIndirect              = reinterpret_cast<PFN_vkCmdDispatchIndirect>(vkGetDeviceProcAddr(device, "vkCmdDispatchIndirect")),
            .vkCmdDraw                          = reinterpret_cast<PFN_vkCmdDraw>(vkGetDeviceProcAddr(device, "vkCmdDraw")),
            .vkCmdDrawIndexed                   = reinterpret_cast<PFN_vkCmdDrawIndexed>(vkGetDeviceProcAddr(device, "vkCmdDrawIndexed")),
            .vkCmdDrawIndexedIndirect           = reinterpret_cast<PFN_vkCmdDrawIndexedIndirect>(vkGetDeviceProcAddr(device, "vkCmdDrawIndexedIndirect")),
            .vkCmdDrawIndirect                  = reinterpret_cast<PFN_vkCmdDrawIndirect>(vkGetDeviceProcAddr(device, "vkCmdDrawIndirect")),
            .vkCmdEndQuery                      = reinterpret_cast<PFN_vkCmdEndQuery>(vkGetDeviceProcAddr(device, "vkCmdEndQuery")),
            .vkCmdEndRenderPass                 = reinterpret_cast<PFN_vkCmdEndRenderPass>(vkGetDeviceProcAddr(device, "vkCmdEndRenderPass")),
            .vkCmdExecuteCommands               = reinterpret_cast<PFN_vkCmdExecuteCommands>(vkGetDeviceProcAddr(device, "vkCmdExecuteCommands")),
            .vkCmdFillBuffer                    = reinterpret_cast<PFN_vkCmdFillBuffer>(vkGetDeviceProcAddr(device, "vkCmdFillBuffer")),
            .vkCmdNextSubpass                   = reinterpret_cast<PFN_vkCmdNextSubpass>(vkGetDeviceProcAddr(device, "vkCmdNextSubpass")),
            .vkCmdPipelineBarrier               = reinterpret_cast<PFN_vkCmdPipelineBarrier>(vkGetDeviceProcAddr(device, "vkCmdPipelineBarrier")),
            .vkCmdPushConstants                 = reinterpret_cast<PFN_vkCmdPushConstants>(vkGetDeviceProcAddr(device, "vkCmdPushConstants")),
            .vkCmdResetEvent                    = reinterpret_cast<PFN_vkCmdResetEvent>(vkGetDeviceProcAddr(device, "vkCmdResetEvent")),
            .vkCmdResetQueryPool                = reinterpret_cast<PFN_vkCmdResetQueryPool>(vkGetDeviceProcAddr(device, "vkCmdResetQueryPool")),
            .vkCmdResolveImage                  = reinterpret_cast<PFN_vkCmdResolveImage>(vkGetDeviceProcAddr(device, "vkCmdResolveImage")),
            .vkCmdSetBlendConstants             = reinterpret_cast<PFN_vkCmdSetBlendConstants>(vkGetDeviceProcAddr(device, "vkCmdSetBlendConstants")),
            .vkCmdSetDepthBias                  = reinterpret_cast<PFN_vkCmdSetDepthBias>(vkGetDeviceProcAddr(device, "vkCmdSetDepthBias")),
            .vkCmdSetDepthBounds                = reinterpret_cast<PFN_vkCmdSetDepthBounds>(vkGetDeviceProcAddr(device, "vkCmdSetDepthBounds")),
            .vkCmdSetEvent                      = reinterpret_cast<PFN_vkCmdSetEvent>(vkGetDeviceProcAddr(device, "vkCmdSetEvent")),
            .vkCmdSetLineWidth                  = reinterpret_cast<PFN_vkCmdSetLineWidth>(vkGetDeviceProcAddr(device, "vkCmdSetLineWidth")),
            .vkCmdSetScissor                    = reinterpret_cast<PFN_vkCmdSetScissor>(vkGetDeviceProcAddr(device, "vkCmdSetScissor")),
            .vkCmdSetStencilCompareMask         = reinterpret_cast<PFN_vkCmdSetStencilCompareMask>(vkGetDeviceProcAddr(device, "vkCmdSetStencilCompareMask")),
            .vkCmdSetStencilReference           = reinterpret_cast<PFN_vkCmdSetStencilReference>(vkGetDeviceProcAddr(device, "vkCmdSetStencilReference")),
            .vkCmdSetStencilWriteMask           = reinterpret_cast<PFN_vkCmdSetStencilWriteMask>(vkGetDeviceProcAddr(device, "vkCmdSetStencilWriteMask")),
            .vkCmdSetViewport                   = reinterpret_cast<PFN_vkCmdSetViewport>(vkGetDeviceProcAddr(device, "vkCmdSetViewport")),
            .vkCmdUpdateBuffer                  = reinterpret_cast<PFN_vkCmdUpdateBuffer>(vkGetDeviceProcAddr(device, "vkCmdUpdateBuffer")),
            .vkCmdWaitEvents                    = reinterpret_cast<PFN_vkCmdWaitEvents>(vkGetDeviceProcAddr(device, "vkCmdWaitEvents")),
            .vkCmdWriteTimestamp                = reinterpret_cast<PFN_vkCmdWriteTimestamp>(vkGetDeviceProcAddr(device, "vkCmdWriteTimestamp")),
            .vkCreateBuffer                     = reinterpret_cast<PFN_vkCreateBuffer>(vkGetDeviceProcAddr(device, "vkCreateBuffer")),
            .vkCreateBufferView                 = reinterpret_cast<PFN_vkCreateBufferView>(vkGetDeviceProcAddr(device, "vkCreateBufferView")),
            .vkCreateCommandPool                = reinterpret_cast<PFN_vkCreateCommandPool>(vkGetDeviceProcAddr(device, "vkCreateCommandPool")),
            .vkCreateComputePipelines           = reinterpret_cast<PFN_vkCreateComputePipelines>(vkGetDeviceProcAddr(device, "vkCreateComputePipelines")),
            .vkCreateDescriptorPool             = reinterpret_cast<PFN_vkCreateDescriptorPool>(vkGetDeviceProcAddr(device, "vkCreateDescriptorPool")),
            .vkCreateDescriptorSetLayout        = reinterpret_cast<PFN_vkCreateDescriptorSetLayout>(vkGetDeviceProcAddr(device, "vkCreateDescriptorSetLayout")),
            .vkCreateEvent                      = reinterpret_cast<PFN_vkCreateEvent>(vkGetDeviceProcAddr(device, "vkCreateEvent")),
            .vkCreateFence                      = reinterpret_cast<PFN_vkCreateFence>(vkGetDeviceProcAddr(device, "vkCreateFence")),
            .vkCreateFramebuffer                = reinterpret_cast<PFN_vkCreateFramebuffer>(vkGetDeviceProcAddr(device, "vkCreateFramebuffer")),
            .vkCreateGraphicsPipelines          = reinterpret_cast<PFN_vkCreateGraphicsPipelines>(vkGetDeviceProcAddr(device, "vkCreateGraphicsPipelines")),
            .vkCreateImage                      = reinterpret_cast<PFN_vkCreateImage>(vkGetDeviceProcAddr(device, "vkCreateImage")),
            .vkCreateImageView                  = reinterpret_cast<PFN_vkCreateImageView>(vkGetDeviceProcAddr(device, "vkCreateImageView")),
            .vkCreatePipelineCache              = reinterpret_cast<PFN_vkCreatePipelineCache>(vkGetDeviceProcAddr(device, "vkCreatePipelineCache")),
            .vkCreatePipelineLayout             = reinterpret_cast<PFN_vkCreatePipelineLayout>(vkGetDeviceProcAddr(device, "vkCreatePipelineLayout")),
            .vkCreateQueryPool                  = reinterpret_cast<PFN_vkCreateQueryPool>(vkGetDeviceProcAddr(device, "vkCreateQueryPool")),
            .vkCreateRenderPass                 = reinterpret_cast<PFN_vkCreateRenderPass>(vkGetDeviceProcAddr(device, "vkCreateRenderPass")),
            .vkCreateSampler                    = reinterpret_cast<PFN_vkCreateSampler>(vkGetDeviceProcAddr(device, "vkCreateSampler")),
            .vkCreateSemaphore                  = reinterpret_cast<PFN_vkCreateSemaphore>(vkGetDeviceProcAddr(device, "vkCreateSemaphore")),
            .vkCreateShaderModule               = reinterpret_cast<PFN_vkCreateShaderModule>(vkGetDeviceProcAddr(device, "vkCreateShaderModule")),
            .vkDestroyBuffer                    = reinterpret_cast<PFN_vkDestroyBuffer>(vkGetDeviceProcAddr(device, "vkDestroyBuffer")),
            .vkDestroyBufferView                = reinterpret_cast<PFN_vkDestroyBufferView>(vkGetDeviceProcAddr(device, "vkDestroyBufferView")),
            .vkDestroyCommandPool               = reinterpret_cast<PFN_vkDestroyCommandPool>(vkGetDeviceProcAddr(device, "vkDestroyCommandPool")),
            .vkDestroyDescriptorPool            = reinterpret_cast<PFN_vkDestroyDescriptorPool>(vkGetDeviceProcAddr(device, "vkDestroyDescriptorPool")),
            .vkDestroyDescriptorSetLayout       = reinterpret_cast<PFN_vkDestroyDescriptorSetLayout>(vkGetDeviceProcAddr(device, "vkDestroyDescriptorSetLayout")),
            .vkDestroyDevice                    = reinterpret_cast<PFN_vkDestroyDevice>(vkGetDeviceProcAddr(device, "vkDestroyDevice")),
            .vkDestroyEvent                     = reinterpret_cast<PFN_vkDestroyEvent>(vkGetDeviceProcAddr(device, "vkDestroyEvent")),
            .vkDestroyFence                     = reinterpret_cast<PFN_vkDestroyFence>(vkGetDeviceProcAddr(device, "vkDestroyFence")),
            .vkDestroyFramebuffer               = reinterpret_cast<PFN_vkDestroyFramebuffer>(vkGetDeviceProcAddr(device, "vkDestroyFramebuffer")),
            .vkDestroyImage                     = reinterpret_cast<PFN_vkDestroyImage>(vkGetDeviceProcAddr(device, "vkDestroyImage")),
            .vkDestroyImageView                 = reinterpret_cast<PFN_vkDestroyImageView>(vkGetDeviceProcAddr(device, "vkDestroyImageView")),
            .vkDestroyPipeline                  = reinterpret_cast<PFN_vkDestroyPipeline>(vkGetDeviceProcAddr(device, "vkDestroyPipeline")),
            .vkDestroyPipelineCache             = reinterpret_cast<PFN_vkDestroyPipelineCache>(vkGetDeviceProcAddr(device, "vkDestroyPipelineCache")),
            .vkDestroyPipelineLayout            = reinterpret_cast<PFN_vkDestroyPipelineLayout>(vkGetDeviceProcAddr(device, "vkDestroyPipelineLayout")),
            .vkDestroyQueryPool                 = reinterpret_cast<PFN_vkDestroyQueryPool>(vkGetDeviceProcAddr(device, "vkDestroyQueryPool")),
            .vkDestroyRenderPass                = reinterpret_cast<PFN_vkDestroyRenderPass>(vkGetDeviceProcAddr(device, "vkDestroyRenderPass")),
            .vkDestroySampler                   = reinterpret_cast<PFN_vkDestroySampler>(vkGetDeviceProcAddr(device, "vkDestroySampler")),
            .vkDestroySemaphore                 = reinterpret_cast<PFN_vkDestroySemaphore>(vkGetDeviceProcAddr(device, "vkDestroySemaphore")),
            .vkDestroyShaderModule              = reinterpret_cast<PFN_vkDestroyShaderModule>(vkGetDeviceProcAddr(device, "vkDestroyShaderModule")),
            .vkDeviceWaitIdle                   = reinterpret_cast<PFN_vkDeviceWaitIdle>(vkGetDeviceProcAddr(device, "vkDeviceWaitIdle")),
            .vkEndCommandBuffer                 = reinterpret_cast<PFN_vkEndCommandBuffer>(vkGetDeviceProcAddr(device, "vkEndCommandBuffer")),
            .vkFlushMappedMemoryRanges          = reinterpret_cast<PFN_vkFlushMappedMemoryRanges>(vkGetDeviceProcAddr(device, "vkFlushMappedMemoryRanges")),
            .vkFreeCommandBuffers               = reinterpret_cast<PFN_vkFreeCommandBuffers>(vkGetDeviceProcAddr(device, "vkFreeCommandBuffers")),
            .vkFreeDescriptorSets               = reinterpret_cast<PFN_vkFreeDescriptorSets>(vkGetDeviceProcAddr(device, "vkFreeDescriptorSets")),
            .vkFreeMemory                       = reinterpret_cast<PFN_vkFreeMemory>(vkGetDeviceProcAddr(device, "vkFreeMemory")),
            .vkGetBufferMemoryRequirements      = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(vkGetDeviceProcAddr(device, "vkGetBufferMemoryRequirements")),
            .vkGetDeviceMemoryCommitment        = reinterpret_cast<PFN_vkGetDeviceMemoryCommitment>(vkGetDeviceProcAddr(device, "vkGetDeviceMemoryCommitment")),
            .vkGetDeviceQueue                   = reinterpret_cast<PFN_vkGetDeviceQueue>(vkGetDeviceProcAddr(device, "vkGetDeviceQueue")),
            .vkGetEventStatus                   = reinterpret_cast<PFN_vkGetEventStatus>(vkGetDeviceProcAddr(device, "vkGetEventStatus")),
            .vkGetFenceStatus                   = reinterpret_cast<PFN_vkGetFenceStatus>(vkGetDeviceProcAddr(device, "vkGetFenceStatus")),
            .vkGetImageMemoryRequirements       = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(vkGetDeviceProcAddr(device, "vkGetImageMemoryRequirements")),
            .vkGetImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements>(vkGetDeviceProcAddr(device, "vkGetImageSparseMemoryRequirements")),
            .vkGetImageSubresourceLayout        = reinterpret_cast<PFN_vkGetImageSubresourceLayout>(vkGetDeviceProcAddr(device, "vkGetImageSubresourceLayout")),
            .vkGetPipelineCacheData             = reinterpret_cast<PFN_vkGetPipelineCacheData>(vkGetDeviceProcAddr(device, "vkGetPipelineCacheData")),
            .vkGetQueryPoolResults              = reinterpret_cast<PFN_vkGetQueryPoolResults>(vkGetDeviceProcAddr(device, "vkGetQueryPoolResults")),
            .vkGetRenderAreaGranularity         = reinterpret_cast<PFN_vkGetRenderAreaGranularity>(vkGetDeviceProcAddr(device, "vkGetRenderAreaGranularity")),
            .vkInvalidateMappedMemoryRanges     = reinterpret_cast<PFN_vkInvalidateMappedMemoryRanges>(vkGetDeviceProcAddr(device, "vkInvalidateMappedMemoryRanges")),
            .vkMapMemory                        = reinterpret_cast<PFN_vkMapMemory>(vkGetDeviceProcAddr(device, "vkMapMemory")),
            .vkMergePipelineCaches              = reinterpret_cast<PFN_vkMergePipelineCaches>(vkGetDeviceProcAddr(device, "vkMergePipelineCaches")),
            .vkQueueBindSparse                  = reinterpret_cast<PFN_vkQueueBindSparse>(vkGetDeviceProcAddr(device, "vkQueueBindSparse")),
            .vkQueueSubmit                      = reinterpret_cast<PFN_vkQueueSubmit>(vkGetDeviceProcAddr(device, "vkQueueSubmit")),
            .vkQueueWaitIdle                    = reinterpret_cast<PFN_vkQueueWaitIdle>(vkGetDeviceProcAddr(device, "vkQueueWaitIdle")),
            .vkResetCommandBuffer               = reinterpret_cast<PFN_vkResetCommandBuffer>(vkGetDeviceProcAddr(device, "vkResetCommandBuffer")),
            .vkResetCommandPool                 = reinterpret_cast<PFN_vkResetCommandPool>(vkGetDeviceProcAddr(device, "vkResetCommandPool")),
            .vkResetDescriptorPool              = reinterpret_cast<PFN_vkResetDescriptorPool>(vkGetDeviceProcAddr(device, "vkResetDescriptorPool")),
            .vkResetEvent                       = reinterpret_cast<PFN_vkResetEvent>(vkGetDeviceProcAddr(device, "vkResetEvent")),
            .vkResetFences                      = reinterpret_cast<PFN_vkResetFences>(vkGetDeviceProcAddr(device, "vkResetFences")),
            .vkSetEvent                         = reinterpret_cast<PFN_vkSetEvent>(vkGetDeviceProcAddr(device, "vkSetEvent")),
            .vkUnmapMemory                      = reinterpret_cast<PFN_vkUnmapMemory>(vkGetDeviceProcAddr(device, "vkUnmapMemory")),
            .vkUpdateDescriptorSets             = reinterpret_cast<PFN_vkUpdateDescriptorSets>(vkGetDeviceProcAddr(device, "vkUpdateDescriptorSets")),
            .vkWaitForFences                    = reinterpret_cast<PFN_vkWaitForFences>(vkGetDeviceProcAddr(device, "vkWaitForFences")),
        #endif
        #if defined(VK_VERSION_1_1)
            .vkBindBufferMemory2                 = reinterpret_cast<PFN_vkBindBufferMemory2>(vkGetDeviceProcAddr(device, "vkBindBufferMemory2")),
            .vkBindImageMemory2                  = reinterpret_cast<PFN_vkBindImageMemory2>(vkGetDeviceProcAddr(device, "vkBindImageMemory2")),
            .vkCmdDispatchBase                   = reinterpret_cast<PFN_vkCmdDispatchBase>(vkGetDeviceProcAddr(device, "vkCmdDispatchBase")),
            .vkCmdSetDeviceMask                  = reinterpret_cast<PFN_vkCmdSetDeviceMask>(vkGetDeviceProcAddr(device, "vkCmdSetDeviceMask")),
            .vkCreateDescriptorUpdateTemplate    = reinterpret_cast<PFN_vkCreateDescriptorUpdateTemplate>(vkGetDeviceProcAddr(device, "vkCreateDescriptorUpdateTemplate")),
            .vkCreateSamplerYcbcrConversion      = reinterpret_cast<PFN_vkCreateSamplerYcbcrConversion>(vkGetDeviceProcAddr(device, "vkCreateSamplerYcbcrConversion")),
            .vkDestroyDescriptorUpdateTemplate   = reinterpret_cast<PFN_vkDestroyDescriptorUpdateTemplate>(vkGetDeviceProcAddr(device, "vkDestroyDescriptorUpdateTemplate")),
            .vkDestroySamplerYcbcrConversion     = reinterpret_cast<PFN_vkDestroySamplerYcbcrConversion>(vkGetDeviceProcAddr(device, "vkDestroySamplerYcbcrConversion")),
            .vkGetBufferMemoryRequirements2      = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2>(vkGetDeviceProcAddr(device, "vkGetBufferMemoryRequirements2")),
            .vkGetDescriptorSetLayoutSupport     = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSupport>(vkGetDeviceProcAddr(device, "vkGetDescriptorSetLayoutSupport")),
            .vkGetDeviceGroupPeerMemoryFeatures  = reinterpret_cast<PFN_vkGetDeviceGroupPeerMemoryFeatures>(vkGetDeviceProcAddr(device, "vkGetDeviceGroupPeerMemoryFeatures")),
            .vkGetDeviceQueue2                   = reinterpret_cast<PFN_vkGetDeviceQueue2>(vkGetDeviceProcAddr(device, "vkGetDeviceQueue2")),
            .vkGetImageMemoryRequirements2       = reinterpret_cast<PFN_vkGetImageMemoryRequirements2>(vkGetDeviceProcAddr(device, "vkGetImageMemoryRequirements2")),
            .vkGetImageSparseMemoryRequirements2 = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements2>(vkGetDeviceProcAddr(device, "vkGetImageSparseMemoryRequirements2")),
            .vkTrimCommandPool                   = reinterpret_cast<PFN_vkTrimCommandPool>(vkGetDeviceProcAddr(device, "vkTrimCommandPool")),
            .vkUpdateDescriptorSetWithTemplate   = reinterpret_cast<PFN_vkUpdateDescriptorSetWithTemplate>(vkGetDeviceProcAddr(device, "vkUpdateDescriptorSetWithTemplate")),
        #endif
        #if defined(VK_VERSION_1_2)
            .vkCmdBeginRenderPass2                 = reinterpret_cast<PFN_vkCmdBeginRenderPass2>(vkGetDeviceProcAddr(device, "vkCmdBeginRenderPass2")),
            .vkCmdDrawIndexedIndirectCount         = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCount>(vkGetDeviceProcAddr(device, "vkCmdDrawIndexedIndirectCount")),
            .vkCmdDrawIndirectCount                = reinterpret_cast<PFN_vkCmdDrawIndirectCount>(vkGetDeviceProcAddr(device, "vkCmdDrawIndirectCount")),
            .vkCmdEndRenderPass2                   = reinterpret_cast<PFN_vkCmdEndRenderPass2>(vkGetDeviceProcAddr(device, "vkCmdEndRenderPass2")),
            .vkCmdNextSubpass2                     = reinterpret_cast<PFN_vkCmdNextSubpass2>(vkGetDeviceProcAddr(device, "vkCmdNextSubpass2")),
            .vkCreateRenderPass2                   = reinterpret_cast<PFN_vkCreateRenderPass2>(vkGetDeviceProcAddr(device, "vkCreateRenderPass2")),
            .vkGetBufferDeviceAddress              = reinterpret_cast<PFN_vkGetBufferDeviceAddress>(vkGetDeviceProcAddr(device, "vkGetBufferDeviceAddress")),
            .vkGetBufferOpaqueCaptureAddress       = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureAddress>(vkGetDeviceProcAddr(device, "vkGetBufferOpaqueCaptureAddress")),
            .vkGetDeviceMemoryOpaqueCaptureAddress = reinterpret_cast<PFN_vkGetDeviceMemoryOpaqueCaptureAddress>(vkGetDeviceProcAddr(device, "vkGetDeviceMemoryOpaqueCaptureAddress")),
            .vkGetSemaphoreCounterValue            = reinterpret_cast<PFN_vkGetSemaphoreCounterValue>(vkGetDeviceProcAddr(device, "vkGetSemaphoreCounterValue")),
            .vkResetQueryPool                      = reinterpret_cast<PFN_vkResetQueryPool>(vkGetDeviceProcAddr(device, "vkResetQueryPool")),
            .vkSignalSemaphore                     = reinterpret_cast<PFN_vkSignalSemaphore>(vkGetDeviceProcAddr(device, "vkSignalSemaphore")),
            .vkWaitSemaphores                      = reinterpret_cast<PFN_vkWaitSemaphores>(vkGetDeviceProcAddr(device, "vkWaitSemaphores")),
        #endif
        #if defined(VK_VERSION_1_3)
            .vkCmdBeginRendering                      = reinterpret_cast<PFN_vkCmdBeginRendering>(vkGetDeviceProcAddr(device, "vkCmdBeginRendering")),
            .vkCmdBindVertexBuffers2                  = reinterpret_cast<PFN_vkCmdBindVertexBuffers2>(vkGetDeviceProcAddr(device, "vkCmdBindVertexBuffers2")),
            .vkCmdBlitImage2                          = reinterpret_cast<PFN_vkCmdBlitImage2>(vkGetDeviceProcAddr(device, "vkCmdBlitImage2")),
            .vkCmdCopyBuffer2                         = reinterpret_cast<PFN_vkCmdCopyBuffer2>(vkGetDeviceProcAddr(device, "vkCmdCopyBuffer2")),
            .vkCmdCopyBufferToImage2                  = reinterpret_cast<PFN_vkCmdCopyBufferToImage2>(vkGetDeviceProcAddr(device, "vkCmdCopyBufferToImage2")),
            .vkCmdCopyImage2                          = reinterpret_cast<PFN_vkCmdCopyImage2>(vkGetDeviceProcAddr(device, "vkCmdCopyImage2")),
            .vkCmdCopyImageToBuffer2                  = reinterpret_cast<PFN_vkCmdCopyImageToBuffer2>(vkGetDeviceProcAddr(device, "vkCmdCopyImageToBuffer2")),
            .vkCmdEndRendering                        = reinterpret_cast<PFN_vkCmdEndRendering>(vkGetDeviceProcAddr(device, "vkCmdEndRendering")),
            .vkCmdPipelineBarrier2                    = reinterpret_cast<PFN_vkCmdPipelineBarrier2>(vkGetDeviceProcAddr(device, "vkCmdPipelineBarrier2")),
            .vkCmdResetEvent2                         = reinterpret_cast<PFN_vkCmdResetEvent2>(vkGetDeviceProcAddr(device, "vkCmdResetEvent2")),
            .vkCmdResolveImage2                       = reinterpret_cast<PFN_vkCmdResolveImage2>(vkGetDeviceProcAddr(device, "vkCmdResolveImage2")),
            .vkCmdSetCullMode                         = reinterpret_cast<PFN_vkCmdSetCullMode>(vkGetDeviceProcAddr(device, "vkCmdSetCullMode")),
            .vkCmdSetDepthBiasEnable                  = reinterpret_cast<PFN_vkCmdSetDepthBiasEnable>(vkGetDeviceProcAddr(device, "vkCmdSetDepthBiasEnable")),
            .vkCmdSetDepthBoundsTestEnable            = reinterpret_cast<PFN_vkCmdSetDepthBoundsTestEnable>(vkGetDeviceProcAddr(device, "vkCmdSetDepthBoundsTestEnable")),
            .vkCmdSetDepthCompareOp                   = reinterpret_cast<PFN_vkCmdSetDepthCompareOp>(vkGetDeviceProcAddr(device, "vkCmdSetDepthCompareOp")),
            .vkCmdSetDepthTestEnable                  = reinterpret_cast<PFN_vkCmdSetDepthTestEnable>(vkGetDeviceProcAddr(device, "vkCmdSetDepthTestEnable")),
            .vkCmdSetDepthWriteEnable                 = reinterpret_cast<PFN_vkCmdSetDepthWriteEnable>(vkGetDeviceProcAddr(device, "vkCmdSetDepthWriteEnable")),
            .vkCmdSetEvent2                           = reinterpret_cast<PFN_vkCmdSetEvent2>(vkGetDeviceProcAddr(device, "vkCmdSetEvent2")),
            .vkCmdSetFrontFace                        = reinterpret_cast<PFN_vkCmdSetFrontFace>(vkGetDeviceProcAddr(device, "vkCmdSetFrontFace")),
            .vkCmdSetPrimitiveRestartEnable           = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnable>(vkGetDeviceProcAddr(device, "vkCmdSetPrimitiveRestartEnable")),
            .vkCmdSetPrimitiveTopology                = reinterpret_cast<PFN_vkCmdSetPrimitiveTopology>(vkGetDeviceProcAddr(device, "vkCmdSetPrimitiveTopology")),
            .vkCmdSetRasterizerDiscardEnable          = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnable>(vkGetDeviceProcAddr(device, "vkCmdSetRasterizerDiscardEnable")),
            .vkCmdSetScissorWithCount                 = reinterpret_cast<PFN_vkCmdSetScissorWithCount>(vkGetDeviceProcAddr(device, "vkCmdSetScissorWithCount")),
            .vkCmdSetStencilOp                        = reinterpret_cast<PFN_vkCmdSetStencilOp>(vkGetDeviceProcAddr(device, "vkCmdSetStencilOp")),
            .vkCmdSetStencilTestEnable                = reinterpret_cast<PFN_vkCmdSetStencilTestEnable>(vkGetDeviceProcAddr(device, "vkCmdSetStencilTestEnable")),
            .vkCmdSetViewportWithCount                = reinterpret_cast<PFN_vkCmdSetViewportWithCount>(vkGetDeviceProcAddr(device, "vkCmdSetViewportWithCount")),
            .vkCmdWaitEvents2                         = reinterpret_cast<PFN_vkCmdWaitEvents2>(vkGetDeviceProcAddr(device, "vkCmdWaitEvents2")),
            .vkCmdWriteTimestamp2                     = reinterpret_cast<PFN_vkCmdWriteTimestamp2>(vkGetDeviceProcAddr(device, "vkCmdWriteTimestamp2")),
            .vkCreatePrivateDataSlot                  = reinterpret_cast<PFN_vkCreatePrivateDataSlot>(vkGetDeviceProcAddr(device, "vkCreatePrivateDataSlot")),
            .vkDestroyPrivateDataSlot                 = reinterpret_cast<PFN_vkDestroyPrivateDataSlot>(vkGetDeviceProcAddr(device, "vkDestroyPrivateDataSlot")),
            .vkGetDeviceBufferMemoryRequirements      = reinterpret_cast<PFN_vkGetDeviceBufferMemoryRequirements>(vkGetDeviceProcAddr(device, "vkGetDeviceBufferMemoryRequirements")),
            .vkGetDeviceImageMemoryRequirements       = reinterpret_cast<PFN_vkGetDeviceImageMemoryRequirements>(vkGetDeviceProcAddr(device, "vkGetDeviceImageMemoryRequirements")),
            .vkGetDeviceImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageSparseMemoryRequirements>(vkGetDeviceProcAddr(device, "vkGetDeviceImageSparseMemoryRequirements")),
            .vkGetPrivateData                         = reinterpret_cast<PFN_vkGetPrivateData>(vkGetDeviceProcAddr(device, "vkGetPrivateData")),
            .vkQueueSubmit2                           = reinterpret_cast<PFN_vkQueueSubmit2>(vkGetDeviceProcAddr(device, "vkQueueSubmit2")),
            .vkSetPrivateData                         = reinterpret_cast<PFN_vkSetPrivateData>(vkGetDeviceProcAddr(device, "vkSetPrivateData")),
        #endif
        #if defined(VK_AMD_buffer_marker)
            .vkCmdWriteBufferMarkerAMD = reinterpret_cast<PFN_vkCmdWriteBufferMarkerAMD>(vkGetDeviceProcAddr(device, "vkCmdWriteBufferMarkerAMD")),
        #endif
        #if defined(VK_AMD_display_native_hdr)
            .vkSetLocalDimmingAMD = reinterpret_cast<PFN_vkSetLocalDimmingAMD>(vkGetDeviceProcAddr(device, "vkSetLocalDimmingAMD")),
        #endif
        #if defined(VK_AMD_draw_indirect_count)
            .vkCmdDrawIndexedIndirectCountAMD = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCountAMD>(vkGetDeviceProcAddr(device, "vkCmdDrawIndexedIndirectCountAMD")),
            .vkCmdDrawIndirectCountAMD        = reinterpret_cast<PFN_vkCmdDrawIndirectCountAMD>(vkGetDeviceProcAddr(device, "vkCmdDrawIndirectCountAMD")),
        #endif
        #if defined(VK_AMD_shader_info)
            .vkGetShaderInfoAMD = reinterpret_cast<PFN_vkGetShaderInfoAMD>(vkGetDeviceProcAddr(device, "vkGetShaderInfoAMD")),
        #endif
        #if defined(VK_ANDROID_external_memory_android_hardware_buffer)
            .vkGetAndroidHardwareBufferPropertiesANDROID = reinterpret_cast<PFN_vkGetAndroidHardwareBufferPropertiesANDROID>(vkGetDeviceProcAddr(device, "vkGetAndroidHardwareBufferPropertiesANDROID")),
            .vkGetMemoryAndroidHardwareBufferANDROID     = reinterpret_cast<PFN_vkGetMemoryAndroidHardwareBufferANDROID>(vkGetDeviceProcAddr(device, "vkGetMemoryAndroidHardwareBufferANDROID")),
        #endif
        #if defined(VK_EXT_attachment_feedback_loop_dynamic_state)
            .vkCmdSetAttachmentFeedbackLoopEnableEXT = reinterpret_cast<PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetAttachmentFeedbackLoopEnableEXT")),
        #endif
        #if defined(VK_EXT_buffer_device_address)
            .vkGetBufferDeviceAddressEXT = reinterpret_cast<PFN_vkGetBufferDeviceAddressEXT>(vkGetDeviceProcAddr(device, "vkGetBufferDeviceAddressEXT")),
        #endif
        #if defined(VK_EXT_calibrated_timestamps)
            .vkGetCalibratedTimestampsEXT = reinterpret_cast<PFN_vkGetCalibratedTimestampsEXT>(vkGetDeviceProcAddr(device, "vkGetCalibratedTimestampsEXT")),
        #endif
        #if defined(VK_EXT_color_write_enable)
            .vkCmdSetColorWriteEnableEXT = reinterpret_cast<PFN_vkCmdSetColorWriteEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetColorWriteEnableEXT")),
        #endif
        #if defined(VK_EXT_conditional_rendering)
            .vkCmdBeginConditionalRenderingEXT = reinterpret_cast<PFN_vkCmdBeginConditionalRenderingEXT>(vkGetDeviceProcAddr(device, "vkCmdBeginConditionalRenderingEXT")),
            .vkCmdEndConditionalRenderingEXT   = reinterpret_cast<PFN_vkCmdEndConditionalRenderingEXT>(vkGetDeviceProcAddr(device, "vkCmdEndConditionalRenderingEXT")),
        #endif
        #if defined(VK_EXT_debug_marker)
            .vkCmdDebugMarkerBeginEXT      = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(vkGetDeviceProcAddr(device, "vkCmdDebugMarkerBeginEXT")),
            .vkCmdDebugMarkerEndEXT        = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>(vkGetDeviceProcAddr(device, "vkCmdDebugMarkerEndEXT")),
            .vkCmdDebugMarkerInsertEXT     = reinterpret_cast<PFN_vkCmdDebugMarkerInsertEXT>(vkGetDeviceProcAddr(device, "vkCmdDebugMarkerInsertEXT")),
            .vkDebugMarkerSetObjectNameEXT = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>(vkGetDeviceProcAddr(device, "vkDebugMarkerSetObjectNameEXT")),
            .vkDebugMarkerSetObjectTagEXT  = reinterpret_cast<PFN_vkDebugMarkerSetObjectTagEXT>(vkGetDeviceProcAddr(device, "vkDebugMarkerSetObjectTagEXT")),
        #endif
        #if defined(VK_EXT_depth_bias_control)
            .vkCmdSetDepthBias2EXT = reinterpret_cast<PFN_vkCmdSetDepthBias2EXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthBias2EXT")),
        #endif
        #if defined(VK_EXT_descriptor_buffer)
            .vkCmdBindDescriptorBufferEmbeddedSamplersEXT = reinterpret_cast<PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT>(vkGetDeviceProcAddr(device, "vkCmdBindDescriptorBufferEmbeddedSamplersEXT")),
            .vkCmdBindDescriptorBuffersEXT                = reinterpret_cast<PFN_vkCmdBindDescriptorBuffersEXT>(vkGetDeviceProcAddr(device, "vkCmdBindDescriptorBuffersEXT")),
            .vkCmdSetDescriptorBufferOffsetsEXT           = reinterpret_cast<PFN_vkCmdSetDescriptorBufferOffsetsEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDescriptorBufferOffsetsEXT")),
            .vkGetBufferOpaqueCaptureDescriptorDataEXT    = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT>(vkGetDeviceProcAddr(device, "vkGetBufferOpaqueCaptureDescriptorDataEXT")),
            .vkGetDescriptorEXT                           = reinterpret_cast<PFN_vkGetDescriptorEXT>(vkGetDeviceProcAddr(device, "vkGetDescriptorEXT")),
            .vkGetDescriptorSetLayoutBindingOffsetEXT     = reinterpret_cast<PFN_vkGetDescriptorSetLayoutBindingOffsetEXT>(vkGetDeviceProcAddr(device, "vkGetDescriptorSetLayoutBindingOffsetEXT")),
            .vkGetDescriptorSetLayoutSizeEXT              = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSizeEXT>(vkGetDeviceProcAddr(device, "vkGetDescriptorSetLayoutSizeEXT")),
            .vkGetImageOpaqueCaptureDescriptorDataEXT     = reinterpret_cast<PFN_vkGetImageOpaqueCaptureDescriptorDataEXT>(vkGetDeviceProcAddr(device, "vkGetImageOpaqueCaptureDescriptorDataEXT")),
            .vkGetImageViewOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT>(vkGetDeviceProcAddr(device, "vkGetImageViewOpaqueCaptureDescriptorDataEXT")),
            .vkGetSamplerOpaqueCaptureDescriptorDataEXT   = reinterpret_cast<PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT>(vkGetDeviceProcAddr(device, "vkGetSamplerOpaqueCaptureDescriptorDataEXT")),
        #endif
        #if defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing))
            .vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT")),
        #endif
        #if defined(VK_EXT_device_fault)
            .vkGetDeviceFaultInfoEXT = reinterpret_cast<PFN_vkGetDeviceFaultInfoEXT>(vkGetDeviceProcAddr(device, "vkGetDeviceFaultInfoEXT")),
        #endif
        #if defined(VK_EXT_discard_rectangles)
            .vkCmdSetDiscardRectangleEXT = reinterpret_cast<PFN_vkCmdSetDiscardRectangleEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDiscardRectangleEXT")),
        #endif
        #if defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2
            .vkCmdSetDiscardRectangleEnableEXT = reinterpret_cast<PFN_vkCmdSetDiscardRectangleEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDiscardRectangleEnableEXT")),
            .vkCmdSetDiscardRectangleModeEXT   = reinterpret_cast<PFN_vkCmdSetDiscardRectangleModeEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDiscardRectangleModeEXT")),
        #endif
        #if defined(VK_EXT_display_control)
            .vkDisplayPowerControlEXT  = reinterpret_cast<PFN_vkDisplayPowerControlEXT>(vkGetDeviceProcAddr(device, "vkDisplayPowerControlEXT")),
            .vkGetSwapchainCounterEXT  = reinterpret_cast<PFN_vkGetSwapchainCounterEXT>(vkGetDeviceProcAddr(device, "vkGetSwapchainCounterEXT")),
            .vkRegisterDeviceEventEXT  = reinterpret_cast<PFN_vkRegisterDeviceEventEXT>(vkGetDeviceProcAddr(device, "vkRegisterDeviceEventEXT")),
            .vkRegisterDisplayEventEXT = reinterpret_cast<PFN_vkRegisterDisplayEventEXT>(vkGetDeviceProcAddr(device, "vkRegisterDisplayEventEXT")),
        #endif
        #if defined(VK_EXT_external_memory_host)
            .vkGetMemoryHostPointerPropertiesEXT = reinterpret_cast<PFN_vkGetMemoryHostPointerPropertiesEXT>(vkGetDeviceProcAddr(device, "vkGetMemoryHostPointerPropertiesEXT")),
        #endif
        #if defined(VK_EXT_full_screen_exclusive)
            .vkAcquireFullScreenExclusiveModeEXT = reinterpret_cast<PFN_vkAcquireFullScreenExclusiveModeEXT>(vkGetDeviceProcAddr(device, "vkAcquireFullScreenExclusiveModeEXT")),
            .vkReleaseFullScreenExclusiveModeEXT = reinterpret_cast<PFN_vkReleaseFullScreenExclusiveModeEXT>(vkGetDeviceProcAddr(device, "vkReleaseFullScreenExclusiveModeEXT")),
        #endif
        #if defined(VK_EXT_hdr_metadata)
            .vkSetHdrMetadataEXT = reinterpret_cast<PFN_vkSetHdrMetadataEXT>(vkGetDeviceProcAddr(device, "vkSetHdrMetadataEXT")),
        #endif
        #if defined(VK_EXT_host_image_copy)
            .vkCopyImageToImageEXT      = reinterpret_cast<PFN_vkCopyImageToImageEXT>(vkGetDeviceProcAddr(device, "vkCopyImageToImageEXT")),
            .vkCopyImageToMemoryEXT     = reinterpret_cast<PFN_vkCopyImageToMemoryEXT>(vkGetDeviceProcAddr(device, "vkCopyImageToMemoryEXT")),
            .vkCopyMemoryToImageEXT     = reinterpret_cast<PFN_vkCopyMemoryToImageEXT>(vkGetDeviceProcAddr(device, "vkCopyMemoryToImageEXT")),
            .vkTransitionImageLayoutEXT = reinterpret_cast<PFN_vkTransitionImageLayoutEXT>(vkGetDeviceProcAddr(device, "vkTransitionImageLayoutEXT")),
        #endif
        #if defined(VK_EXT_host_query_reset)
            .vkResetQueryPoolEXT = reinterpret_cast<PFN_vkResetQueryPoolEXT>(vkGetDeviceProcAddr(device, "vkResetQueryPoolEXT")),
        #endif
        #if defined(VK_EXT_image_drm_format_modifier)
            .vkGetImageDrmFormatModifierPropertiesEXT = reinterpret_cast<PFN_vkGetImageDrmFormatModifierPropertiesEXT>(vkGetDeviceProcAddr(device, "vkGetImageDrmFormatModifierPropertiesEXT")),
        #endif
        #if defined(VK_EXT_line_rasterization)
            .vkCmdSetLineStippleEXT = reinterpret_cast<PFN_vkCmdSetLineStippleEXT>(vkGetDeviceProcAddr(device, "vkCmdSetLineStippleEXT")),
        #endif
        #if defined(VK_EXT_mesh_shader)
            .vkCmdDrawMeshTasksEXT              = reinterpret_cast<PFN_vkCmdDrawMeshTasksEXT>(vkGetDeviceProcAddr(device, "vkCmdDrawMeshTasksEXT")),
            .vkCmdDrawMeshTasksIndirectCountEXT = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectCountEXT>(vkGetDeviceProcAddr(device, "vkCmdDrawMeshTasksIndirectCountEXT")),
            .vkCmdDrawMeshTasksIndirectEXT      = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectEXT>(vkGetDeviceProcAddr(device, "vkCmdDrawMeshTasksIndirectEXT")),
        #endif
        #if defined(VK_EXT_vulkan_objects)
            .vkExportVulkanObjectsEXT = reinterpret_cast<PFN_vkExportVulkanObjectsEXT>(vkGetDeviceProcAddr(device, "vkExportVulkanObjectsEXT")),
        #endif
        #if defined(VK_EXT_multi_draw)
            .vkCmdDrawMultiEXT        = reinterpret_cast<PFN_vkCmdDrawMultiEXT>(vkGetDeviceProcAddr(device, "vkCmdDrawMultiEXT")),
            .vkCmdDrawMultiIndexedEXT = reinterpret_cast<PFN_vkCmdDrawMultiIndexedEXT>(vkGetDeviceProcAddr(device, "vkCmdDrawMultiIndexedEXT")),
        #endif
        #if defined(VK_EXT_opacity_micromap)
            .vkBuildMicromapsEXT                 = reinterpret_cast<PFN_vkBuildMicromapsEXT>(vkGetDeviceProcAddr(device, "vkBuildMicromapsEXT")),
            .vkCmdBuildMicromapsEXT              = reinterpret_cast<PFN_vkCmdBuildMicromapsEXT>(vkGetDeviceProcAddr(device, "vkCmdBuildMicromapsEXT")),
            .vkCmdCopyMemoryToMicromapEXT        = reinterpret_cast<PFN_vkCmdCopyMemoryToMicromapEXT>(vkGetDeviceProcAddr(device, "vkCmdCopyMemoryToMicromapEXT")),
            .vkCmdCopyMicromapEXT                = reinterpret_cast<PFN_vkCmdCopyMicromapEXT>(vkGetDeviceProcAddr(device, "vkCmdCopyMicromapEXT")),
            .vkCmdCopyMicromapToMemoryEXT        = reinterpret_cast<PFN_vkCmdCopyMicromapToMemoryEXT>(vkGetDeviceProcAddr(device, "vkCmdCopyMicromapToMemoryEXT")),
            .vkCmdWriteMicromapsPropertiesEXT    = reinterpret_cast<PFN_vkCmdWriteMicromapsPropertiesEXT>(vkGetDeviceProcAddr(device, "vkCmdWriteMicromapsPropertiesEXT")),
            .vkCopyMemoryToMicromapEXT           = reinterpret_cast<PFN_vkCopyMemoryToMicromapEXT>(vkGetDeviceProcAddr(device, "vkCopyMemoryToMicromapEXT")),
            .vkCopyMicromapEXT                   = reinterpret_cast<PFN_vkCopyMicromapEXT>(vkGetDeviceProcAddr(device, "vkCopyMicromapEXT")),
            .vkCopyMicromapToMemoryEXT           = reinterpret_cast<PFN_vkCopyMicromapToMemoryEXT>(vkGetDeviceProcAddr(device, "vkCopyMicromapToMemoryEXT")),
            .vkCreateMicromapEXT                 = reinterpret_cast<PFN_vkCreateMicromapEXT>(vkGetDeviceProcAddr(device, "vkCreateMicromapEXT")),
            .vkDestroyMicromapEXT                = reinterpret_cast<PFN_vkDestroyMicromapEXT>(vkGetDeviceProcAddr(device, "vkDestroyMicromapEXT")),
            .vkGetDeviceMicromapCompatibilityEXT = reinterpret_cast<PFN_vkGetDeviceMicromapCompatibilityEXT>(vkGetDeviceProcAddr(device, "vkGetDeviceMicromapCompatibilityEXT")),
            .vkGetMicromapBuildSizesEXT          = reinterpret_cast<PFN_vkGetMicromapBuildSizesEXT>(vkGetDeviceProcAddr(device, "vkGetMicromapBuildSizesEXT")),
            .vkWriteMicromapsPropertiesEXT       = reinterpret_cast<PFN_vkWriteMicromapsPropertiesEXT>(vkGetDeviceProcAddr(device, "vkWriteMicromapsPropertiesEXT")),
        #endif
        #if defined(VK_EXT_pageable_device_local_memory)
            .vkSetDeviceMemoryPriorityEXT = reinterpret_cast<PFN_vkSetDeviceMemoryPriorityEXT>(vkGetDeviceProcAddr(device, "vkSetDeviceMemoryPriorityEXT")),
        #endif
        #if defined(VK_EXT_pipeline_properties)
            .vkGetPipelinePropertiesEXT = reinterpret_cast<PFN_vkGetPipelinePropertiesEXT>(vkGetDeviceProcAddr(device, "vkGetPipelinePropertiesEXT")),
        #endif
        #if defined(VK_EXT_private_data)
            .vkCreatePrivateDataSlotEXT  = reinterpret_cast<PFN_vkCreatePrivateDataSlotEXT>(vkGetDeviceProcAddr(device, "vkCreatePrivateDataSlotEXT")),
            .vkDestroyPrivateDataSlotEXT = reinterpret_cast<PFN_vkDestroyPrivateDataSlotEXT>(vkGetDeviceProcAddr(device, "vkDestroyPrivateDataSlotEXT")),
            .vkGetPrivateDataEXT         = reinterpret_cast<PFN_vkGetPrivateDataEXT>(vkGetDeviceProcAddr(device, "vkGetPrivateDataEXT")),
            .vkSetPrivateDataEXT         = reinterpret_cast<PFN_vkSetPrivateDataEXT>(vkGetDeviceProcAddr(device, "vkSetPrivateDataEXT")),
        #endif
        #if defined(VK_EXT_sample_locations)
            .vkCmdSetSampleLocationsEXT = reinterpret_cast<PFN_vkCmdSetSampleLocationsEXT>(vkGetDeviceProcAddr(device, "vkCmdSetSampleLocationsEXT")),
        #endif
        #if defined(VK_EXT_shader_module_identifier)
            .vkGetShaderModuleCreateInfoIdentifierEXT = reinterpret_cast<PFN_vkGetShaderModuleCreateInfoIdentifierEXT>(vkGetDeviceProcAddr(device, "vkGetShaderModuleCreateInfoIdentifierEXT")),
            .vkGetShaderModuleIdentifierEXT           = reinterpret_cast<PFN_vkGetShaderModuleIdentifierEXT>(vkGetDeviceProcAddr(device, "vkGetShaderModuleIdentifierEXT")),
        #endif
        #if defined(VK_EXT_shader_object)
            .vkCmdBindShadersEXT      = reinterpret_cast<PFN_vkCmdBindShadersEXT>(vkGetDeviceProcAddr(device, "vkCmdBindShadersEXT")),
            .vkCreateShadersEXT       = reinterpret_cast<PFN_vkCreateShadersEXT>(vkGetDeviceProcAddr(device, "vkCreateShadersEXT")),
            .vkDestroyShaderEXT       = reinterpret_cast<PFN_vkDestroyShaderEXT>(vkGetDeviceProcAddr(device, "vkDestroyShaderEXT")),
            .vkGetShaderBinaryDataEXT = reinterpret_cast<PFN_vkGetShaderBinaryDataEXT>(vkGetDeviceProcAddr(device, "vkGetShaderBinaryDataEXT")),
        #endif
        #if defined(VK_EXT_swapchain_maintenance1)
            .vkReleaseSwapchainImagesEXT = reinterpret_cast<PFN_vkReleaseSwapchainImagesEXT>(vkGetDeviceProcAddr(device, "vkReleaseSwapchainImagesEXT")),
        #endif
        #if defined(VK_EXT_transform_feedback)
            .vkCmdBeginQueryIndexedEXT            = reinterpret_cast<PFN_vkCmdBeginQueryIndexedEXT>(vkGetDeviceProcAddr(device, "vkCmdBeginQueryIndexedEXT")),
            .vkCmdBeginTransformFeedbackEXT       = reinterpret_cast<PFN_vkCmdBeginTransformFeedbackEXT>(vkGetDeviceProcAddr(device, "vkCmdBeginTransformFeedbackEXT")),
            .vkCmdBindTransformFeedbackBuffersEXT = reinterpret_cast<PFN_vkCmdBindTransformFeedbackBuffersEXT>(vkGetDeviceProcAddr(device, "vkCmdBindTransformFeedbackBuffersEXT")),
            .vkCmdDrawIndirectByteCountEXT        = reinterpret_cast<PFN_vkCmdDrawIndirectByteCountEXT>(vkGetDeviceProcAddr(device, "vkCmdDrawIndirectByteCountEXT")),
            .vkCmdEndQueryIndexedEXT              = reinterpret_cast<PFN_vkCmdEndQueryIndexedEXT>(vkGetDeviceProcAddr(device, "vkCmdEndQueryIndexedEXT")),
            .vkCmdEndTransformFeedbackEXT         = reinterpret_cast<PFN_vkCmdEndTransformFeedbackEXT>(vkGetDeviceProcAddr(device, "vkCmdEndTransformFeedbackEXT")),
        #endif
        #if defined(VK_EXT_validation_cache)
            .vkCreateValidationCacheEXT  = reinterpret_cast<PFN_vkCreateValidationCacheEXT>(vkGetDeviceProcAddr(device, "vkCreateValidationCacheEXT")),
            .vkDestroyValidationCacheEXT = reinterpret_cast<PFN_vkDestroyValidationCacheEXT>(vkGetDeviceProcAddr(device, "vkDestroyValidationCacheEXT")),
            .vkGetValidationCacheDataEXT = reinterpret_cast<PFN_vkGetValidationCacheDataEXT>(vkGetDeviceProcAddr(device, "vkGetValidationCacheDataEXT")),
            .vkMergeValidationCachesEXT  = reinterpret_cast<PFN_vkMergeValidationCachesEXT>(vkGetDeviceProcAddr(device, "vkMergeValidationCachesEXT")),
        #endif
        #if defined(VK_FUCHSIA_buffer_collection)
            .vkCreateBufferCollectionFUCHSIA               = reinterpret_cast<PFN_vkCreateBufferCollectionFUCHSIA>(vkGetDeviceProcAddr(device, "vkCreateBufferCollectionFUCHSIA")),
            .vkDestroyBufferCollectionFUCHSIA              = reinterpret_cast<PFN_vkDestroyBufferCollectionFUCHSIA>(vkGetDeviceProcAddr(device, "vkDestroyBufferCollectionFUCHSIA")),
            .vkGetBufferCollectionPropertiesFUCHSIA        = reinterpret_cast<PFN_vkGetBufferCollectionPropertiesFUCHSIA>(vkGetDeviceProcAddr(device, "vkGetBufferCollectionPropertiesFUCHSIA")),
            .vkSetBufferCollectionBufferConstraintsFUCHSIA = reinterpret_cast<PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA>(vkGetDeviceProcAddr(device, "vkSetBufferCollectionBufferConstraintsFUCHSIA")),
            .vkSetBufferCollectionImageConstraintsFUCHSIA  = reinterpret_cast<PFN_vkSetBufferCollectionImageConstraintsFUCHSIA>(vkGetDeviceProcAddr(device, "vkSetBufferCollectionImageConstraintsFUCHSIA")),
        #endif
        #if defined(VK_FUCHSIA_external_memory)
            .vkGetMemoryZirconHandleFUCHSIA           = reinterpret_cast<PFN_vkGetMemoryZirconHandleFUCHSIA>(vkGetDeviceProcAddr(device, "vkGetMemoryZirconHandleFUCHSIA")),
            .vkGetMemoryZirconHandlePropertiesFUCHSIA = reinterpret_cast<PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA>(vkGetDeviceProcAddr(device, "vkGetMemoryZirconHandlePropertiesFUCHSIA")),
        #endif
        #if defined(VK_FUCHSIA_external_semaphore)
            .vkGetSemaphoreZirconHandleFUCHSIA    = reinterpret_cast<PFN_vkGetSemaphoreZirconHandleFUCHSIA>(vkGetDeviceProcAddr(device, "vkGetSemaphoreZirconHandleFUCHSIA")),
            .vkImportSemaphoreZirconHandleFUCHSIA = reinterpret_cast<PFN_vkImportSemaphoreZirconHandleFUCHSIA>(vkGetDeviceProcAddr(device, "vkImportSemaphoreZirconHandleFUCHSIA")),
        #endif
        #if defined(VK_GOOGLE_display_timing)
            .vkGetPastPresentationTimingGOOGLE = reinterpret_cast<PFN_vkGetPastPresentationTimingGOOGLE>(vkGetDeviceProcAddr(device, "vkGetPastPresentationTimingGOOGLE")),
            .vkGetRefreshCycleDurationGOOGLE   = reinterpret_cast<PFN_vkGetRefreshCycleDurationGOOGLE>(vkGetDeviceProcAddr(device, "vkGetRefreshCycleDurationGOOGLE")),
        #endif
        #if defined(VK_HUAWEI_cluster_culling_shader)
            .vkCmdDrawClusterHUAWEI         = reinterpret_cast<PFN_vkCmdDrawClusterHUAWEI>(vkGetDeviceProcAddr(device, "vkCmdDrawClusterHUAWEI")),
            .vkCmdDrawClusterIndirectHUAWEI = reinterpret_cast<PFN_vkCmdDrawClusterIndirectHUAWEI>(vkGetDeviceProcAddr(device, "vkCmdDrawClusterIndirectHUAWEI")),
        #endif
        #if defined(VK_HUAWEI_invocation_mask)
            .vkCmdBindInvocationMaskHUAWEI = reinterpret_cast<PFN_vkCmdBindInvocationMaskHUAWEI>(vkGetDeviceProcAddr(device, "vkCmdBindInvocationMaskHUAWEI")),
        #endif
        #if defined(VK_HUAWEI_subpass_shading)
            .vkCmdSubpassShadingHUAWEI                       = reinterpret_cast<PFN_vkCmdSubpassShadingHUAWEI>(vkGetDeviceProcAddr(device, "vkCmdSubpassShadingHUAWEI")),
            .vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI = reinterpret_cast<PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI>(vkGetDeviceProcAddr(device, "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI")),
        #endif
        #if defined(VK_INTEL_performance_query)
            .vkAcquirePerformanceConfigurationINTEL  = reinterpret_cast<PFN_vkAcquirePerformanceConfigurationINTEL>(vkGetDeviceProcAddr(device, "vkAcquirePerformanceConfigurationINTEL")),
            .vkCmdSetPerformanceMarkerINTEL          = reinterpret_cast<PFN_vkCmdSetPerformanceMarkerINTEL>(vkGetDeviceProcAddr(device, "vkCmdSetPerformanceMarkerINTEL")),
            .vkCmdSetPerformanceOverrideINTEL        = reinterpret_cast<PFN_vkCmdSetPerformanceOverrideINTEL>(vkGetDeviceProcAddr(device, "vkCmdSetPerformanceOverrideINTEL")),
            .vkCmdSetPerformanceStreamMarkerINTEL    = reinterpret_cast<PFN_vkCmdSetPerformanceStreamMarkerINTEL>(vkGetDeviceProcAddr(device, "vkCmdSetPerformanceStreamMarkerINTEL")),
            .vkGetPerformanceParameterINTEL          = reinterpret_cast<PFN_vkGetPerformanceParameterINTEL>(vkGetDeviceProcAddr(device, "vkGetPerformanceParameterINTEL")),
            .vkInitializePerformanceApiINTEL         = reinterpret_cast<PFN_vkInitializePerformanceApiINTEL>(vkGetDeviceProcAddr(device, "vkInitializePerformanceApiINTEL")),
            .vkQueueSetPerformanceConfigurationINTEL = reinterpret_cast<PFN_vkQueueSetPerformanceConfigurationINTEL>(vkGetDeviceProcAddr(device, "vkQueueSetPerformanceConfigurationINTEL")),
            .vkReleasePerformanceConfigurationINTEL  = reinterpret_cast<PFN_vkReleasePerformanceConfigurationINTEL>(vkGetDeviceProcAddr(device, "vkReleasePerformanceConfigurationINTEL")),
            .vkUninitializePerformanceApiINTEL       = reinterpret_cast<PFN_vkUninitializePerformanceApiINTEL>(vkGetDeviceProcAddr(device, "vkUninitializePerformanceApiINTEL")),
        #endif
        #if defined(VK_KHR_acceleration_structure)
            .vkBuildAccelerationStructuresKHR                 = reinterpret_cast<PFN_vkBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(device, "vkBuildAccelerationStructuresKHR")),
            .vkCmdBuildAccelerationStructuresIndirectKHR      = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresIndirectKHR>(vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresIndirectKHR")),
            .vkCmdBuildAccelerationStructuresKHR              = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresKHR")),
            .vkCmdCopyAccelerationStructureKHR                = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCmdCopyAccelerationStructureKHR")),
            .vkCmdCopyAccelerationStructureToMemoryKHR        = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureToMemoryKHR>(vkGetDeviceProcAddr(device, "vkCmdCopyAccelerationStructureToMemoryKHR")),
            .vkCmdCopyMemoryToAccelerationStructureKHR        = reinterpret_cast<PFN_vkCmdCopyMemoryToAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCmdCopyMemoryToAccelerationStructureKHR")),
            .vkCmdWriteAccelerationStructuresPropertiesKHR    = reinterpret_cast<PFN_vkCmdWriteAccelerationStructuresPropertiesKHR>(vkGetDeviceProcAddr(device, "vkCmdWriteAccelerationStructuresPropertiesKHR")),
            .vkCopyAccelerationStructureKHR                   = reinterpret_cast<PFN_vkCopyAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCopyAccelerationStructureKHR")),
            .vkCopyAccelerationStructureToMemoryKHR           = reinterpret_cast<PFN_vkCopyAccelerationStructureToMemoryKHR>(vkGetDeviceProcAddr(device, "vkCopyAccelerationStructureToMemoryKHR")),
            .vkCopyMemoryToAccelerationStructureKHR           = reinterpret_cast<PFN_vkCopyMemoryToAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCopyMemoryToAccelerationStructureKHR")),
            .vkCreateAccelerationStructureKHR                 = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureKHR")),
            .vkDestroyAccelerationStructureKHR                = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureKHR")),
            .vkGetAccelerationStructureBuildSizesKHR          = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureBuildSizesKHR")),
            .vkGetAccelerationStructureDeviceAddressKHR       = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureDeviceAddressKHR")),
            .vkGetDeviceAccelerationStructureCompatibilityKHR = reinterpret_cast<PFN_vkGetDeviceAccelerationStructureCompatibilityKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceAccelerationStructureCompatibilityKHR")),
            .vkWriteAccelerationStructuresPropertiesKHR       = reinterpret_cast<PFN_vkWriteAccelerationStructuresPropertiesKHR>(vkGetDeviceProcAddr(device, "vkWriteAccelerationStructuresPropertiesKHR")),
        #endif
        #if defined(VK_KHR_bind_memory2)
            .vkBindBufferMemory2KHR = reinterpret_cast<PFN_vkBindBufferMemory2KHR>(vkGetDeviceProcAddr(device, "vkBindBufferMemory2KHR")),
            .vkBindImageMemory2KHR  = reinterpret_cast<PFN_vkBindImageMemory2KHR>(vkGetDeviceProcAddr(device, "vkBindImageMemory2KHR")),
        #endif
        #if defined(VK_KHR_buffer_device_address)
            .vkGetBufferDeviceAddressKHR              = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(device, "vkGetBufferDeviceAddressKHR")),
            .vkGetBufferOpaqueCaptureAddressKHR       = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureAddressKHR>(vkGetDeviceProcAddr(device, "vkGetBufferOpaqueCaptureAddressKHR")),
            .vkGetDeviceMemoryOpaqueCaptureAddressKHR = reinterpret_cast<PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceMemoryOpaqueCaptureAddressKHR")),
        #endif
        #if defined(VK_KHR_copy_commands2)
            .vkCmdBlitImage2KHR         = reinterpret_cast<PFN_vkCmdBlitImage2KHR>(vkGetDeviceProcAddr(device, "vkCmdBlitImage2KHR")),
            .vkCmdCopyBuffer2KHR        = reinterpret_cast<PFN_vkCmdCopyBuffer2KHR>(vkGetDeviceProcAddr(device, "vkCmdCopyBuffer2KHR")),
            .vkCmdCopyBufferToImage2KHR = reinterpret_cast<PFN_vkCmdCopyBufferToImage2KHR>(vkGetDeviceProcAddr(device, "vkCmdCopyBufferToImage2KHR")),
            .vkCmdCopyImage2KHR         = reinterpret_cast<PFN_vkCmdCopyImage2KHR>(vkGetDeviceProcAddr(device, "vkCmdCopyImage2KHR")),
            .vkCmdCopyImageToBuffer2KHR = reinterpret_cast<PFN_vkCmdCopyImageToBuffer2KHR>(vkGetDeviceProcAddr(device, "vkCmdCopyImageToBuffer2KHR")),
            .vkCmdResolveImage2KHR      = reinterpret_cast<PFN_vkCmdResolveImage2KHR>(vkGetDeviceProcAddr(device, "vkCmdResolveImage2KHR")),
        #endif
        #if defined(VK_KHR_create_renderpass2)
            .vkCmdBeginRenderPass2KHR = reinterpret_cast<PFN_vkCmdBeginRenderPass2KHR>(vkGetDeviceProcAddr(device, "vkCmdBeginRenderPass2KHR")),
            .vkCmdEndRenderPass2KHR   = reinterpret_cast<PFN_vkCmdEndRenderPass2KHR>(vkGetDeviceProcAddr(device, "vkCmdEndRenderPass2KHR")),
            .vkCmdNextSubpass2KHR     = reinterpret_cast<PFN_vkCmdNextSubpass2KHR>(vkGetDeviceProcAddr(device, "vkCmdNextSubpass2KHR")),
            .vkCreateRenderPass2KHR   = reinterpret_cast<PFN_vkCreateRenderPass2KHR>(vkGetDeviceProcAddr(device, "vkCreateRenderPass2KHR")),
        #endif
        #if defined(VK_KHR_deferred_host_operations)
            .vkCreateDeferredOperationKHR            = reinterpret_cast<PFN_vkCreateDeferredOperationKHR>(vkGetDeviceProcAddr(device, "vkCreateDeferredOperationKHR")),
            .vkDeferredOperationJoinKHR              = reinterpret_cast<PFN_vkDeferredOperationJoinKHR>(vkGetDeviceProcAddr(device, "vkDeferredOperationJoinKHR")),
            .vkDestroyDeferredOperationKHR           = reinterpret_cast<PFN_vkDestroyDeferredOperationKHR>(vkGetDeviceProcAddr(device, "vkDestroyDeferredOperationKHR")),
            .vkGetDeferredOperationMaxConcurrencyKHR = reinterpret_cast<PFN_vkGetDeferredOperationMaxConcurrencyKHR>(vkGetDeviceProcAddr(device, "vkGetDeferredOperationMaxConcurrencyKHR")),
            .vkGetDeferredOperationResultKHR         = reinterpret_cast<PFN_vkGetDeferredOperationResultKHR>(vkGetDeviceProcAddr(device, "vkGetDeferredOperationResultKHR")),
        #endif
        #if defined(VK_KHR_descriptor_update_template)
            .vkCreateDescriptorUpdateTemplateKHR  = reinterpret_cast<PFN_vkCreateDescriptorUpdateTemplateKHR>(vkGetDeviceProcAddr(device, "vkCreateDescriptorUpdateTemplateKHR")),
            .vkDestroyDescriptorUpdateTemplateKHR = reinterpret_cast<PFN_vkDestroyDescriptorUpdateTemplateKHR>(vkGetDeviceProcAddr(device, "vkDestroyDescriptorUpdateTemplateKHR")),
            .vkUpdateDescriptorSetWithTemplateKHR = reinterpret_cast<PFN_vkUpdateDescriptorSetWithTemplateKHR>(vkGetDeviceProcAddr(device, "vkUpdateDescriptorSetWithTemplateKHR")),
        #endif
        #if defined(VK_KHR_device_group)
            .vkCmdDispatchBaseKHR                  = reinterpret_cast<PFN_vkCmdDispatchBaseKHR>(vkGetDeviceProcAddr(device, "vkCmdDispatchBaseKHR")),
            .vkCmdSetDeviceMaskKHR                 = reinterpret_cast<PFN_vkCmdSetDeviceMaskKHR>(vkGetDeviceProcAddr(device, "vkCmdSetDeviceMaskKHR")),
            .vkGetDeviceGroupPeerMemoryFeaturesKHR = reinterpret_cast<PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceGroupPeerMemoryFeaturesKHR")),
        #endif
        #if defined(VK_KHR_display_swapchain)
            .vkCreateSharedSwapchainsKHR = reinterpret_cast<PFN_vkCreateSharedSwapchainsKHR>(vkGetDeviceProcAddr(device, "vkCreateSharedSwapchainsKHR")),
        #endif
        #if defined(VK_KHR_draw_indirect_count)
            .vkCmdDrawIndexedIndirectCountKHR = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCountKHR>(vkGetDeviceProcAddr(device, "vkCmdDrawIndexedIndirectCountKHR")),
            .vkCmdDrawIndirectCountKHR        = reinterpret_cast<PFN_vkCmdDrawIndirectCountKHR>(vkGetDeviceProcAddr(device, "vkCmdDrawIndirectCountKHR")),
        #endif
        #if defined(VK_KHR_dynamic_rendering)
            .vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(vkGetDeviceProcAddr(device, "vkCmdBeginRenderingKHR")),
            .vkCmdEndRenderingKHR   = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(vkGetDeviceProcAddr(device, "vkCmdEndRenderingKHR")),
        #endif
        #if defined(VK_KHR_external_fence_fd)
            .vkGetFenceFdKHR    = reinterpret_cast<PFN_vkGetFenceFdKHR>(vkGetDeviceProcAddr(device, "vkGetFenceFdKHR")),
            .vkImportFenceFdKHR = reinterpret_cast<PFN_vkImportFenceFdKHR>(vkGetDeviceProcAddr(device, "vkImportFenceFdKHR")),
        #endif
        #if defined(VK_KHR_external_fence_win32)
            .vkGetFenceWin32HandleKHR    = reinterpret_cast<PFN_vkGetFenceWin32HandleKHR>(vkGetDeviceProcAddr(device, "vkGetFenceWin32HandleKHR")),
            .vkImportFenceWin32HandleKHR = reinterpret_cast<PFN_vkImportFenceWin32HandleKHR>(vkGetDeviceProcAddr(device, "vkImportFenceWin32HandleKHR")),
        #endif
        #if defined(VK_KHR_external_memory_fd)
            .vkGetMemoryFdKHR           = reinterpret_cast<PFN_vkGetMemoryFdKHR>(vkGetDeviceProcAddr(device, "vkGetMemoryFdKHR")),
            .vkGetMemoryFdPropertiesKHR = reinterpret_cast<PFN_vkGetMemoryFdPropertiesKHR>(vkGetDeviceProcAddr(device, "vkGetMemoryFdPropertiesKHR")),
        #endif
        #if defined(VK_KHR_external_memory_win32)
            .vkGetMemoryWin32HandleKHR           = reinterpret_cast<PFN_vkGetMemoryWin32HandleKHR>(vkGetDeviceProcAddr(device, "vkGetMemoryWin32HandleKHR")),
            .vkGetMemoryWin32HandlePropertiesKHR = reinterpret_cast<PFN_vkGetMemoryWin32HandlePropertiesKHR>(vkGetDeviceProcAddr(device, "vkGetMemoryWin32HandlePropertiesKHR")),
        #endif
        #if defined(VK_KHR_external_semaphore_fd)
            .vkGetSemaphoreFdKHR    = reinterpret_cast<PFN_vkGetSemaphoreFdKHR>(vkGetDeviceProcAddr(device, "vkGetSemaphoreFdKHR")),
            .vkImportSemaphoreFdKHR = reinterpret_cast<PFN_vkImportSemaphoreFdKHR>(vkGetDeviceProcAddr(device, "vkImportSemaphoreFdKHR")),
        #endif
        #if defined(VK_KHR_external_semaphore_win32)
            .vkGetSemaphoreWin32HandleKHR    = reinterpret_cast<PFN_vkGetSemaphoreWin32HandleKHR>(vkGetDeviceProcAddr(device, "vkGetSemaphoreWin32HandleKHR")),
            .vkImportSemaphoreWin32HandleKHR = reinterpret_cast<PFN_vkImportSemaphoreWin32HandleKHR>(vkGetDeviceProcAddr(device, "vkImportSemaphoreWin32HandleKHR")),
        #endif
        #if defined(VK_KHR_fragment_shading_rate)
            .vkCmdSetFragmentShadingRateKHR = reinterpret_cast<PFN_vkCmdSetFragmentShadingRateKHR>(vkGetDeviceProcAddr(device, "vkCmdSetFragmentShadingRateKHR")),
        #endif
        #if defined(VK_KHR_get_memory_requirements2)
            .vkGetBufferMemoryRequirements2KHR      = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2KHR>(vkGetDeviceProcAddr(device, "vkGetBufferMemoryRequirements2KHR")),
            .vkGetImageMemoryRequirements2KHR       = reinterpret_cast<PFN_vkGetImageMemoryRequirements2KHR>(vkGetDeviceProcAddr(device, "vkGetImageMemoryRequirements2KHR")),
            .vkGetImageSparseMemoryRequirements2KHR = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements2KHR>(vkGetDeviceProcAddr(device, "vkGetImageSparseMemoryRequirements2KHR")),
        #endif
        #if defined(VK_KHR_maintenance1)
            .vkTrimCommandPoolKHR = reinterpret_cast<PFN_vkTrimCommandPoolKHR>(vkGetDeviceProcAddr(device, "vkTrimCommandPoolKHR")),
        #endif
        #if defined(VK_KHR_maintenance3)
            .vkGetDescriptorSetLayoutSupportKHR = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSupportKHR>(vkGetDeviceProcAddr(device, "vkGetDescriptorSetLayoutSupportKHR")),
        #endif
        #if defined(VK_KHR_maintenance4)
            .vkGetDeviceBufferMemoryRequirementsKHR      = reinterpret_cast<PFN_vkGetDeviceBufferMemoryRequirementsKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceBufferMemoryRequirementsKHR")),
            .vkGetDeviceImageMemoryRequirementsKHR       = reinterpret_cast<PFN_vkGetDeviceImageMemoryRequirementsKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceImageMemoryRequirementsKHR")),
            .vkGetDeviceImageSparseMemoryRequirementsKHR = reinterpret_cast<PFN_vkGetDeviceImageSparseMemoryRequirementsKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceImageSparseMemoryRequirementsKHR")),
        #endif
        #if defined(VK_KHR_maintenance5)
            .vkCmdBindIndexBuffer2KHR             = reinterpret_cast<PFN_vkCmdBindIndexBuffer2KHR>(vkGetDeviceProcAddr(device, "vkCmdBindIndexBuffer2KHR")),
            .vkGetDeviceImageSubresourceLayoutKHR = reinterpret_cast<PFN_vkGetDeviceImageSubresourceLayoutKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceImageSubresourceLayoutKHR")),
            .vkGetImageSubresourceLayout2KHR      = reinterpret_cast<PFN_vkGetImageSubresourceLayout2KHR>(vkGetDeviceProcAddr(device, "vkGetImageSubresourceLayout2KHR")),
            .vkGetRenderingAreaGranularityKHR     = reinterpret_cast<PFN_vkGetRenderingAreaGranularityKHR>(vkGetDeviceProcAddr(device, "vkGetRenderingAreaGranularityKHR")),
        #endif
        #if defined(VK_KHR_map_memory2)
            .vkMapMemory2KHR   = reinterpret_cast<PFN_vkMapMemory2KHR>(vkGetDeviceProcAddr(device, "vkMapMemory2KHR")),
            .vkUnmapMemory2KHR = reinterpret_cast<PFN_vkUnmapMemory2KHR>(vkGetDeviceProcAddr(device, "vkUnmapMemory2KHR")),
        #endif
        #if defined(VK_KHR_performance_query)
            .vkAcquireProfilingLockKHR = reinterpret_cast<PFN_vkAcquireProfilingLockKHR>(vkGetDeviceProcAddr(device, "vkAcquireProfilingLockKHR")),
            .vkReleaseProfilingLockKHR = reinterpret_cast<PFN_vkReleaseProfilingLockKHR>(vkGetDeviceProcAddr(device, "vkReleaseProfilingLockKHR")),
        #endif
        #if defined(VK_KHR_pipeline_executable_properties)
            .vkGetPipelineExecutableInternalRepresentationsKHR = reinterpret_cast<PFN_vkGetPipelineExecutableInternalRepresentationsKHR>(vkGetDeviceProcAddr(device, "vkGetPipelineExecutableInternalRepresentationsKHR")),
            .vkGetPipelineExecutablePropertiesKHR              = reinterpret_cast<PFN_vkGetPipelineExecutablePropertiesKHR>(vkGetDeviceProcAddr(device, "vkGetPipelineExecutablePropertiesKHR")),
            .vkGetPipelineExecutableStatisticsKHR              = reinterpret_cast<PFN_vkGetPipelineExecutableStatisticsKHR>(vkGetDeviceProcAddr(device, "vkGetPipelineExecutableStatisticsKHR")),
        #endif
        #if defined(VK_KHR_present_wait)
            .vkWaitForPresentKHR = reinterpret_cast<PFN_vkWaitForPresentKHR>(vkGetDeviceProcAddr(device, "vkWaitForPresentKHR")),
        #endif
        #if defined(VK_KHR_push_descriptor)
            .vkCmdPushDescriptorSetKHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(vkGetDeviceProcAddr(device, "vkCmdPushDescriptorSetKHR")),
        #endif
        #if defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline)
            .vkCmdTraceRaysIndirect2KHR = reinterpret_cast<PFN_vkCmdTraceRaysIndirect2KHR>(vkGetDeviceProcAddr(device, "vkCmdTraceRaysIndirect2KHR")),
        #endif
        #if defined(VK_KHR_ray_tracing_pipeline)
            .vkCmdSetRayTracingPipelineStackSizeKHR            = reinterpret_cast<PFN_vkCmdSetRayTracingPipelineStackSizeKHR>(vkGetDeviceProcAddr(device, "vkCmdSetRayTracingPipelineStackSizeKHR")),
            .vkCmdTraceRaysIndirectKHR                         = reinterpret_cast<PFN_vkCmdTraceRaysIndirectKHR>(vkGetDeviceProcAddr(device, "vkCmdTraceRaysIndirectKHR")),
            .vkCmdTraceRaysKHR                                 = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(vkGetDeviceProcAddr(device, "vkCmdTraceRaysKHR")),
            .vkCreateRayTracingPipelinesKHR                    = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(vkGetDeviceProcAddr(device, "vkCreateRayTracingPipelinesKHR")),
            .vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR>(vkGetDeviceProcAddr(device, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR")),
            .vkGetRayTracingShaderGroupHandlesKHR              = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesKHR")),
            .vkGetRayTracingShaderGroupStackSizeKHR            = reinterpret_cast<PFN_vkGetRayTracingShaderGroupStackSizeKHR>(vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupStackSizeKHR")),
        #endif
        #if defined(VK_KHR_sampler_ycbcr_conversion)
            .vkCreateSamplerYcbcrConversionKHR  = reinterpret_cast<PFN_vkCreateSamplerYcbcrConversionKHR>(vkGetDeviceProcAddr(device, "vkCreateSamplerYcbcrConversionKHR")),
            .vkDestroySamplerYcbcrConversionKHR = reinterpret_cast<PFN_vkDestroySamplerYcbcrConversionKHR>(vkGetDeviceProcAddr(device, "vkDestroySamplerYcbcrConversionKHR")),
        #endif
        #if defined(VK_KHR_shared_presentable_image)
            .vkGetSwapchainStatusKHR = reinterpret_cast<PFN_vkGetSwapchainStatusKHR>(vkGetDeviceProcAddr(device, "vkGetSwapchainStatusKHR")),
        #endif
        #if defined(VK_KHR_swapchain)
            .vkAcquireNextImageKHR   = reinterpret_cast<PFN_vkAcquireNextImageKHR>(vkGetDeviceProcAddr(device, "vkAcquireNextImageKHR")),
            .vkCreateSwapchainKHR    = reinterpret_cast<PFN_vkCreateSwapchainKHR>(vkGetDeviceProcAddr(device, "vkCreateSwapchainKHR")),
            .vkDestroySwapchainKHR   = reinterpret_cast<PFN_vkDestroySwapchainKHR>(vkGetDeviceProcAddr(device, "vkDestroySwapchainKHR")),
            .vkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(vkGetDeviceProcAddr(device, "vkGetSwapchainImagesKHR")),
            .vkQueuePresentKHR       = reinterpret_cast<PFN_vkQueuePresentKHR>(vkGetDeviceProcAddr(device, "vkQueuePresentKHR")),
        #endif
        #if defined(VK_KHR_synchronization2)
            .vkCmdPipelineBarrier2KHR = reinterpret_cast<PFN_vkCmdPipelineBarrier2KHR>(vkGetDeviceProcAddr(device, "vkCmdPipelineBarrier2KHR")),
            .vkCmdResetEvent2KHR      = reinterpret_cast<PFN_vkCmdResetEvent2KHR>(vkGetDeviceProcAddr(device, "vkCmdResetEvent2KHR")),
            .vkCmdSetEvent2KHR        = reinterpret_cast<PFN_vkCmdSetEvent2KHR>(vkGetDeviceProcAddr(device, "vkCmdSetEvent2KHR")),
            .vkCmdWaitEvents2KHR      = reinterpret_cast<PFN_vkCmdWaitEvents2KHR>(vkGetDeviceProcAddr(device, "vkCmdWaitEvents2KHR")),
            .vkCmdWriteTimestamp2KHR  = reinterpret_cast<PFN_vkCmdWriteTimestamp2KHR>(vkGetDeviceProcAddr(device, "vkCmdWriteTimestamp2KHR")),
            .vkQueueSubmit2KHR        = reinterpret_cast<PFN_vkQueueSubmit2KHR>(vkGetDeviceProcAddr(device, "vkQueueSubmit2KHR")),
        #endif
        #if defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker)
            .vkCmdWriteBufferMarker2AMD = reinterpret_cast<PFN_vkCmdWriteBufferMarker2AMD>(vkGetDeviceProcAddr(device, "vkCmdWriteBufferMarker2AMD")),
        #endif
        #if defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints)
            .vkGetQueueCheckpointData2NV = reinterpret_cast<PFN_vkGetQueueCheckpointData2NV>(vkGetDeviceProcAddr(device, "vkGetQueueCheckpointData2NV")),
        #endif
        #if defined(VK_KHR_timeline_semaphore)
            .vkGetSemaphoreCounterValueKHR = reinterpret_cast<PFN_vkGetSemaphoreCounterValueKHR>(vkGetDeviceProcAddr(device, "vkGetSemaphoreCounterValueKHR")),
            .vkSignalSemaphoreKHR          = reinterpret_cast<PFN_vkSignalSemaphoreKHR>(vkGetDeviceProcAddr(device, "vkSignalSemaphoreKHR")),
            .vkWaitSemaphoresKHR           = reinterpret_cast<PFN_vkWaitSemaphoresKHR>(vkGetDeviceProcAddr(device, "vkWaitSemaphoresKHR")),
        #endif
        #if defined(VK_KHR_video_decode_queue)
            .vkCmdDecodeVideoKHR = reinterpret_cast<PFN_vkCmdDecodeVideoKHR>(vkGetDeviceProcAddr(device, "vkCmdDecodeVideoKHR")),
        #endif
        #if defined(VK_KHR_video_queue)
            .vkBindVideoSessionMemoryKHR            = reinterpret_cast<PFN_vkBindVideoSessionMemoryKHR>(vkGetDeviceProcAddr(device, "vkBindVideoSessionMemoryKHR")),
            .vkCmdBeginVideoCodingKHR               = reinterpret_cast<PFN_vkCmdBeginVideoCodingKHR>(vkGetDeviceProcAddr(device, "vkCmdBeginVideoCodingKHR")),
            .vkCmdControlVideoCodingKHR             = reinterpret_cast<PFN_vkCmdControlVideoCodingKHR>(vkGetDeviceProcAddr(device, "vkCmdControlVideoCodingKHR")),
            .vkCmdEndVideoCodingKHR                 = reinterpret_cast<PFN_vkCmdEndVideoCodingKHR>(vkGetDeviceProcAddr(device, "vkCmdEndVideoCodingKHR")),
            .vkCreateVideoSessionKHR                = reinterpret_cast<PFN_vkCreateVideoSessionKHR>(vkGetDeviceProcAddr(device, "vkCreateVideoSessionKHR")),
            .vkCreateVideoSessionParametersKHR      = reinterpret_cast<PFN_vkCreateVideoSessionParametersKHR>(vkGetDeviceProcAddr(device, "vkCreateVideoSessionParametersKHR")),
            .vkDestroyVideoSessionKHR               = reinterpret_cast<PFN_vkDestroyVideoSessionKHR>(vkGetDeviceProcAddr(device, "vkDestroyVideoSessionKHR")),
            .vkDestroyVideoSessionParametersKHR     = reinterpret_cast<PFN_vkDestroyVideoSessionParametersKHR>(vkGetDeviceProcAddr(device, "vkDestroyVideoSessionParametersKHR")),
            .vkGetVideoSessionMemoryRequirementsKHR = reinterpret_cast<PFN_vkGetVideoSessionMemoryRequirementsKHR>(vkGetDeviceProcAddr(device, "vkGetVideoSessionMemoryRequirementsKHR")),
            .vkUpdateVideoSessionParametersKHR      = reinterpret_cast<PFN_vkUpdateVideoSessionParametersKHR>(vkGetDeviceProcAddr(device, "vkUpdateVideoSessionParametersKHR")),
        #endif
        #if defined(VK_NVX_binary_import)
            .vkCmdCuLaunchKernelNVX = reinterpret_cast<PFN_vkCmdCuLaunchKernelNVX>(vkGetDeviceProcAddr(device, "vkCmdCuLaunchKernelNVX")),
            .vkCreateCuFunctionNVX  = reinterpret_cast<PFN_vkCreateCuFunctionNVX>(vkGetDeviceProcAddr(device, "vkCreateCuFunctionNVX")),
            .vkCreateCuModuleNVX    = reinterpret_cast<PFN_vkCreateCuModuleNVX>(vkGetDeviceProcAddr(device, "vkCreateCuModuleNVX")),
            .vkDestroyCuFunctionNVX = reinterpret_cast<PFN_vkDestroyCuFunctionNVX>(vkGetDeviceProcAddr(device, "vkDestroyCuFunctionNVX")),
            .vkDestroyCuModuleNVX   = reinterpret_cast<PFN_vkDestroyCuModuleNVX>(vkGetDeviceProcAddr(device, "vkDestroyCuModuleNVX")),
        #endif
        #if defined(VK_NVX_image_view_handle)
            .vkGetImageViewAddressNVX = reinterpret_cast<PFN_vkGetImageViewAddressNVX>(vkGetDeviceProcAddr(device, "vkGetImageViewAddressNVX")),
            .vkGetImageViewHandleNVX  = reinterpret_cast<PFN_vkGetImageViewHandleNVX>(vkGetDeviceProcAddr(device, "vkGetImageViewHandleNVX")),
        #endif
        #if defined(VK_NV_clip_space_w_scaling)
            .vkCmdSetViewportWScalingNV = reinterpret_cast<PFN_vkCmdSetViewportWScalingNV>(vkGetDeviceProcAddr(device, "vkCmdSetViewportWScalingNV")),
        #endif
        #if defined(VK_NV_copy_memory_indirect)
            .vkCmdCopyMemoryIndirectNV        = reinterpret_cast<PFN_vkCmdCopyMemoryIndirectNV>(vkGetDeviceProcAddr(device, "vkCmdCopyMemoryIndirectNV")),
            .vkCmdCopyMemoryToImageIndirectNV = reinterpret_cast<PFN_vkCmdCopyMemoryToImageIndirectNV>(vkGetDeviceProcAddr(device, "vkCmdCopyMemoryToImageIndirectNV")),
        #endif
        #if defined(VK_NV_device_diagnostic_checkpoints)
            .vkCmdSetCheckpointNV       = reinterpret_cast<PFN_vkCmdSetCheckpointNV>(vkGetDeviceProcAddr(device, "vkCmdSetCheckpointNV")),
            .vkGetQueueCheckpointDataNV = reinterpret_cast<PFN_vkGetQueueCheckpointDataNV>(vkGetDeviceProcAddr(device, "vkGetQueueCheckpointDataNV")),
        #endif
        #if defined(VK_NV_device_generated_commands)
            .vkCmdBindPipelineShaderGroupNV             = reinterpret_cast<PFN_vkCmdBindPipelineShaderGroupNV>(vkGetDeviceProcAddr(device, "vkCmdBindPipelineShaderGroupNV")),
            .vkCmdExecuteGeneratedCommandsNV            = reinterpret_cast<PFN_vkCmdExecuteGeneratedCommandsNV>(vkGetDeviceProcAddr(device, "vkCmdExecuteGeneratedCommandsNV")),
            .vkCmdPreprocessGeneratedCommandsNV         = reinterpret_cast<PFN_vkCmdPreprocessGeneratedCommandsNV>(vkGetDeviceProcAddr(device, "vkCmdPreprocessGeneratedCommandsNV")),
            .vkCreateIndirectCommandsLayoutNV           = reinterpret_cast<PFN_vkCreateIndirectCommandsLayoutNV>(vkGetDeviceProcAddr(device, "vkCreateIndirectCommandsLayoutNV")),
            .vkDestroyIndirectCommandsLayoutNV          = reinterpret_cast<PFN_vkDestroyIndirectCommandsLayoutNV>(vkGetDeviceProcAddr(device, "vkDestroyIndirectCommandsLayoutNV")),
            .vkGetGeneratedCommandsMemoryRequirementsNV = reinterpret_cast<PFN_vkGetGeneratedCommandsMemoryRequirementsNV>(vkGetDeviceProcAddr(device, "vkGetGeneratedCommandsMemoryRequirementsNV")),
        #endif
        #if defined(VK_NV_device_generated_commands_compute)
            .vkCmdUpdatePipelineIndirectBufferNV       = reinterpret_cast<PFN_vkCmdUpdatePipelineIndirectBufferNV>(vkGetDeviceProcAddr(device, "vkCmdUpdatePipelineIndirectBufferNV")),
            .vkGetPipelineIndirectDeviceAddressNV      = reinterpret_cast<PFN_vkGetPipelineIndirectDeviceAddressNV>(vkGetDeviceProcAddr(device, "vkGetPipelineIndirectDeviceAddressNV")),
            .vkGetPipelineIndirectMemoryRequirementsNV = reinterpret_cast<PFN_vkGetPipelineIndirectMemoryRequirementsNV>(vkGetDeviceProcAddr(device, "vkGetPipelineIndirectMemoryRequirementsNV")),
        #endif
        #if defined(VK_NV_external_memory_rdma)
            .vkGetMemoryRemoteAddressNV = reinterpret_cast<PFN_vkGetMemoryRemoteAddressNV>(vkGetDeviceProcAddr(device, "vkGetMemoryRemoteAddressNV")),
        #endif
        #if defined(VK_NV_external_memory_win32)
            .vkGetMemoryWin32HandleNV = reinterpret_cast<PFN_vkGetMemoryWin32HandleNV>(vkGetDeviceProcAddr(device, "vkGetMemoryWin32HandleNV")),
        #endif
        #if defined(VK_NV_fragment_shading_rate_enums)
            .vkCmdSetFragmentShadingRateEnumNV = reinterpret_cast<PFN_vkCmdSetFragmentShadingRateEnumNV>(vkGetDeviceProcAddr(device, "vkCmdSetFragmentShadingRateEnumNV")),
        #endif
        #if defined(VK_NV_memory_decompression)
            .vkCmdDecompressMemoryIndirectCountNV = reinterpret_cast<PFN_vkCmdDecompressMemoryIndirectCountNV>(vkGetDeviceProcAddr(device, "vkCmdDecompressMemoryIndirectCountNV")),
            .vkCmdDecompressMemoryNV              = reinterpret_cast<PFN_vkCmdDecompressMemoryNV>(vkGetDeviceProcAddr(device, "vkCmdDecompressMemoryNV")),
        #endif
        #if defined(VK_NV_mesh_shader)
            .vkCmdDrawMeshTasksIndirectCountNV = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectCountNV>(vkGetDeviceProcAddr(device, "vkCmdDrawMeshTasksIndirectCountNV")),
            .vkCmdDrawMeshTasksIndirectNV      = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectNV>(vkGetDeviceProcAddr(device, "vkCmdDrawMeshTasksIndirectNV")),
            .vkCmdDrawMeshTasksNV              = reinterpret_cast<PFN_vkCmdDrawMeshTasksNV>(vkGetDeviceProcAddr(device, "vkCmdDrawMeshTasksNV")),
        #endif
        #if defined(VK_NV_optical_flow)
            .vkBindOpticalFlowSessionImageNV = reinterpret_cast<PFN_vkBindOpticalFlowSessionImageNV>(vkGetDeviceProcAddr(device, "vkBindOpticalFlowSessionImageNV")),
            .vkCmdOpticalFlowExecuteNV       = reinterpret_cast<PFN_vkCmdOpticalFlowExecuteNV>(vkGetDeviceProcAddr(device, "vkCmdOpticalFlowExecuteNV")),
            .vkCreateOpticalFlowSessionNV    = reinterpret_cast<PFN_vkCreateOpticalFlowSessionNV>(vkGetDeviceProcAddr(device, "vkCreateOpticalFlowSessionNV")),
            .vkDestroyOpticalFlowSessionNV   = reinterpret_cast<PFN_vkDestroyOpticalFlowSessionNV>(vkGetDeviceProcAddr(device, "vkDestroyOpticalFlowSessionNV")),
        #endif
        #if defined(VK_NV_ray_tracing)
            .vkBindAccelerationStructureMemoryNV            = reinterpret_cast<PFN_vkBindAccelerationStructureMemoryNV>(vkGetDeviceProcAddr(device, "vkBindAccelerationStructureMemoryNV")),
            .vkCmdBuildAccelerationStructureNV              = reinterpret_cast<PFN_vkCmdBuildAccelerationStructureNV>(vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructureNV")),
            .vkCmdCopyAccelerationStructureNV               = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureNV>(vkGetDeviceProcAddr(device, "vkCmdCopyAccelerationStructureNV")),
            .vkCmdTraceRaysNV                               = reinterpret_cast<PFN_vkCmdTraceRaysNV>(vkGetDeviceProcAddr(device, "vkCmdTraceRaysNV")),
            .vkCmdWriteAccelerationStructuresPropertiesNV   = reinterpret_cast<PFN_vkCmdWriteAccelerationStructuresPropertiesNV>(vkGetDeviceProcAddr(device, "vkCmdWriteAccelerationStructuresPropertiesNV")),
            .vkCompileDeferredNV                            = reinterpret_cast<PFN_vkCompileDeferredNV>(vkGetDeviceProcAddr(device, "vkCompileDeferredNV")),
            .vkCreateAccelerationStructureNV                = reinterpret_cast<PFN_vkCreateAccelerationStructureNV>(vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureNV")),
            .vkCreateRayTracingPipelinesNV                  = reinterpret_cast<PFN_vkCreateRayTracingPipelinesNV>(vkGetDeviceProcAddr(device, "vkCreateRayTracingPipelinesNV")),
            .vkDestroyAccelerationStructureNV               = reinterpret_cast<PFN_vkDestroyAccelerationStructureNV>(vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureNV")),
            .vkGetAccelerationStructureHandleNV             = reinterpret_cast<PFN_vkGetAccelerationStructureHandleNV>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureHandleNV")),
            .vkGetAccelerationStructureMemoryRequirementsNV = reinterpret_cast<PFN_vkGetAccelerationStructureMemoryRequirementsNV>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureMemoryRequirementsNV")),
            .vkGetRayTracingShaderGroupHandlesNV            = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesNV>(vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesNV")),
        #endif
        #if defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2
            .vkCmdSetExclusiveScissorEnableNV = reinterpret_cast<PFN_vkCmdSetExclusiveScissorEnableNV>(vkGetDeviceProcAddr(device, "vkCmdSetExclusiveScissorEnableNV")),
        #endif
        #if defined(VK_NV_scissor_exclusive)
            .vkCmdSetExclusiveScissorNV = reinterpret_cast<PFN_vkCmdSetExclusiveScissorNV>(vkGetDeviceProcAddr(device, "vkCmdSetExclusiveScissorNV")),
        #endif
        #if defined(VK_NV_shading_rate_image)
            .vkCmdBindShadingRateImageNV          = reinterpret_cast<PFN_vkCmdBindShadingRateImageNV>(vkGetDeviceProcAddr(device, "vkCmdBindShadingRateImageNV")),
            .vkCmdSetCoarseSampleOrderNV          = reinterpret_cast<PFN_vkCmdSetCoarseSampleOrderNV>(vkGetDeviceProcAddr(device, "vkCmdSetCoarseSampleOrderNV")),
            .vkCmdSetViewportShadingRatePaletteNV = reinterpret_cast<PFN_vkCmdSetViewportShadingRatePaletteNV>(vkGetDeviceProcAddr(device, "vkCmdSetViewportShadingRatePaletteNV")),
        #endif
        #if defined(VK_QCOM_tile_properties)
            .vkGetDynamicRenderingTilePropertiesQCOM = reinterpret_cast<PFN_vkGetDynamicRenderingTilePropertiesQCOM>(vkGetDeviceProcAddr(device, "vkGetDynamicRenderingTilePropertiesQCOM")),
            .vkGetFramebufferTilePropertiesQCOM      = reinterpret_cast<PFN_vkGetFramebufferTilePropertiesQCOM>(vkGetDeviceProcAddr(device, "vkGetFramebufferTilePropertiesQCOM")),
        #endif
        #if defined(VK_QNX_external_memory_screen_buffer)
            .vkGetScreenBufferPropertiesQNX = reinterpret_cast<PFN_vkGetScreenBufferPropertiesQNX>(vkGetDeviceProcAddr(device, "vkGetScreenBufferPropertiesQNX")),
        #endif
        #if defined(VK_VALVE_descriptor_set_host_mapping)
            .vkGetDescriptorSetHostMappingVALVE           = reinterpret_cast<PFN_vkGetDescriptorSetHostMappingVALVE>(vkGetDeviceProcAddr(device, "vkGetDescriptorSetHostMappingVALVE")),
            .vkGetDescriptorSetLayoutHostMappingInfoVALVE = reinterpret_cast<PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE>(vkGetDeviceProcAddr(device, "vkGetDescriptorSetLayoutHostMappingInfoVALVE")),
        #endif
        #if (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object))
            .vkCmdBindVertexBuffers2EXT       = reinterpret_cast<PFN_vkCmdBindVertexBuffers2EXT>(vkGetDeviceProcAddr(device, "vkCmdBindVertexBuffers2EXT")),
            .vkCmdSetCullModeEXT              = reinterpret_cast<PFN_vkCmdSetCullModeEXT>(vkGetDeviceProcAddr(device, "vkCmdSetCullModeEXT")),
            .vkCmdSetDepthBoundsTestEnableEXT = reinterpret_cast<PFN_vkCmdSetDepthBoundsTestEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthBoundsTestEnableEXT")),
            .vkCmdSetDepthCompareOpEXT        = reinterpret_cast<PFN_vkCmdSetDepthCompareOpEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthCompareOpEXT")),
            .vkCmdSetDepthTestEnableEXT       = reinterpret_cast<PFN_vkCmdSetDepthTestEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthTestEnableEXT")),
            .vkCmdSetDepthWriteEnableEXT      = reinterpret_cast<PFN_vkCmdSetDepthWriteEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthWriteEnableEXT")),
            .vkCmdSetFrontFaceEXT             = reinterpret_cast<PFN_vkCmdSetFrontFaceEXT>(vkGetDeviceProcAddr(device, "vkCmdSetFrontFaceEXT")),
            .vkCmdSetPrimitiveTopologyEXT     = reinterpret_cast<PFN_vkCmdSetPrimitiveTopologyEXT>(vkGetDeviceProcAddr(device, "vkCmdSetPrimitiveTopologyEXT")),
            .vkCmdSetScissorWithCountEXT      = reinterpret_cast<PFN_vkCmdSetScissorWithCountEXT>(vkGetDeviceProcAddr(device, "vkCmdSetScissorWithCountEXT")),
            .vkCmdSetStencilOpEXT             = reinterpret_cast<PFN_vkCmdSetStencilOpEXT>(vkGetDeviceProcAddr(device, "vkCmdSetStencilOpEXT")),
            .vkCmdSetStencilTestEnableEXT     = reinterpret_cast<PFN_vkCmdSetStencilTestEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetStencilTestEnableEXT")),
            .vkCmdSetViewportWithCountEXT     = reinterpret_cast<PFN_vkCmdSetViewportWithCountEXT>(vkGetDeviceProcAddr(device, "vkCmdSetViewportWithCountEXT")),
        #endif
        #if (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object))
            .vkCmdSetDepthBiasEnableEXT         = reinterpret_cast<PFN_vkCmdSetDepthBiasEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthBiasEnableEXT")),
            .vkCmdSetLogicOpEXT                 = reinterpret_cast<PFN_vkCmdSetLogicOpEXT>(vkGetDeviceProcAddr(device, "vkCmdSetLogicOpEXT")),
            .vkCmdSetPatchControlPointsEXT      = reinterpret_cast<PFN_vkCmdSetPatchControlPointsEXT>(vkGetDeviceProcAddr(device, "vkCmdSetPatchControlPointsEXT")),
            .vkCmdSetPrimitiveRestartEnableEXT  = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetPrimitiveRestartEnableEXT")),
            .vkCmdSetRasterizerDiscardEnableEXT = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetRasterizerDiscardEnableEXT")),
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object))
            .vkCmdSetAlphaToCoverageEnableEXT            = reinterpret_cast<PFN_vkCmdSetAlphaToCoverageEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetAlphaToCoverageEnableEXT")),
            .vkCmdSetAlphaToOneEnableEXT                 = reinterpret_cast<PFN_vkCmdSetAlphaToOneEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetAlphaToOneEnableEXT")),
            .vkCmdSetColorBlendAdvancedEXT               = reinterpret_cast<PFN_vkCmdSetColorBlendAdvancedEXT>(vkGetDeviceProcAddr(device, "vkCmdSetColorBlendAdvancedEXT")),
            .vkCmdSetColorBlendEnableEXT                 = reinterpret_cast<PFN_vkCmdSetColorBlendEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetColorBlendEnableEXT")),
            .vkCmdSetColorBlendEquationEXT               = reinterpret_cast<PFN_vkCmdSetColorBlendEquationEXT>(vkGetDeviceProcAddr(device, "vkCmdSetColorBlendEquationEXT")),
            .vkCmdSetColorWriteMaskEXT                   = reinterpret_cast<PFN_vkCmdSetColorWriteMaskEXT>(vkGetDeviceProcAddr(device, "vkCmdSetColorWriteMaskEXT")),
            .vkCmdSetConservativeRasterizationModeEXT    = reinterpret_cast<PFN_vkCmdSetConservativeRasterizationModeEXT>(vkGetDeviceProcAddr(device, "vkCmdSetConservativeRasterizationModeEXT")),
            .vkCmdSetDepthClampEnableEXT                 = reinterpret_cast<PFN_vkCmdSetDepthClampEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthClampEnableEXT")),
            .vkCmdSetDepthClipEnableEXT                  = reinterpret_cast<PFN_vkCmdSetDepthClipEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthClipEnableEXT")),
            .vkCmdSetDepthClipNegativeOneToOneEXT        = reinterpret_cast<PFN_vkCmdSetDepthClipNegativeOneToOneEXT>(vkGetDeviceProcAddr(device, "vkCmdSetDepthClipNegativeOneToOneEXT")),
            .vkCmdSetExtraPrimitiveOverestimationSizeEXT = reinterpret_cast<PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT>(vkGetDeviceProcAddr(device, "vkCmdSetExtraPrimitiveOverestimationSizeEXT")),
            .vkCmdSetLineRasterizationModeEXT            = reinterpret_cast<PFN_vkCmdSetLineRasterizationModeEXT>(vkGetDeviceProcAddr(device, "vkCmdSetLineRasterizationModeEXT")),
            .vkCmdSetLineStippleEnableEXT                = reinterpret_cast<PFN_vkCmdSetLineStippleEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetLineStippleEnableEXT")),
            .vkCmdSetLogicOpEnableEXT                    = reinterpret_cast<PFN_vkCmdSetLogicOpEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetLogicOpEnableEXT")),
            .vkCmdSetPolygonModeEXT                      = reinterpret_cast<PFN_vkCmdSetPolygonModeEXT>(vkGetDeviceProcAddr(device, "vkCmdSetPolygonModeEXT")),
            .vkCmdSetProvokingVertexModeEXT              = reinterpret_cast<PFN_vkCmdSetProvokingVertexModeEXT>(vkGetDeviceProcAddr(device, "vkCmdSetProvokingVertexModeEXT")),
            .vkCmdSetRasterizationSamplesEXT             = reinterpret_cast<PFN_vkCmdSetRasterizationSamplesEXT>(vkGetDeviceProcAddr(device, "vkCmdSetRasterizationSamplesEXT")),
            .vkCmdSetRasterizationStreamEXT              = reinterpret_cast<PFN_vkCmdSetRasterizationStreamEXT>(vkGetDeviceProcAddr(device, "vkCmdSetRasterizationStreamEXT")),
            .vkCmdSetSampleLocationsEnableEXT            = reinterpret_cast<PFN_vkCmdSetSampleLocationsEnableEXT>(vkGetDeviceProcAddr(device, "vkCmdSetSampleLocationsEnableEXT")),
            .vkCmdSetSampleMaskEXT                       = reinterpret_cast<PFN_vkCmdSetSampleMaskEXT>(vkGetDeviceProcAddr(device, "vkCmdSetSampleMaskEXT")),
            .vkCmdSetTessellationDomainOriginEXT         = reinterpret_cast<PFN_vkCmdSetTessellationDomainOriginEXT>(vkGetDeviceProcAddr(device, "vkCmdSetTessellationDomainOriginEXT")),
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling))
            .vkCmdSetViewportWScalingEnableNV = reinterpret_cast<PFN_vkCmdSetViewportWScalingEnableNV>(vkGetDeviceProcAddr(device, "vkCmdSetViewportWScalingEnableNV")),
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle))
            .vkCmdSetViewportSwizzleNV = reinterpret_cast<PFN_vkCmdSetViewportSwizzleNV>(vkGetDeviceProcAddr(device, "vkCmdSetViewportSwizzleNV")),
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color))
            .vkCmdSetCoverageToColorEnableNV   = reinterpret_cast<PFN_vkCmdSetCoverageToColorEnableNV>(vkGetDeviceProcAddr(device, "vkCmdSetCoverageToColorEnableNV")),
            .vkCmdSetCoverageToColorLocationNV = reinterpret_cast<PFN_vkCmdSetCoverageToColorLocationNV>(vkGetDeviceProcAddr(device, "vkCmdSetCoverageToColorLocationNV")),
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples))
            .vkCmdSetCoverageModulationModeNV        = reinterpret_cast<PFN_vkCmdSetCoverageModulationModeNV>(vkGetDeviceProcAddr(device, "vkCmdSetCoverageModulationModeNV")),
            .vkCmdSetCoverageModulationTableEnableNV = reinterpret_cast<PFN_vkCmdSetCoverageModulationTableEnableNV>(vkGetDeviceProcAddr(device, "vkCmdSetCoverageModulationTableEnableNV")),
            .vkCmdSetCoverageModulationTableNV       = reinterpret_cast<PFN_vkCmdSetCoverageModulationTableNV>(vkGetDeviceProcAddr(device, "vkCmdSetCoverageModulationTableNV")),
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image))
            .vkCmdSetShadingRateImageEnableNV = reinterpret_cast<PFN_vkCmdSetShadingRateImageEnableNV>(vkGetDeviceProcAddr(device, "vkCmdSetShadingRateImageEnableNV")),
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test))
            .vkCmdSetRepresentativeFragmentTestEnableNV = reinterpret_cast<PFN_vkCmdSetRepresentativeFragmentTestEnableNV>(vkGetDeviceProcAddr(device, "vkCmdSetRepresentativeFragmentTestEnableNV")),
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode))
            .vkCmdSetCoverageReductionModeNV = reinterpret_cast<PFN_vkCmdSetCoverageReductionModeNV>(vkGetDeviceProcAddr(device, "vkCmdSetCoverageReductionModeNV")),
        #endif
        #if (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1))
            .vkGetDeviceGroupSurfacePresentModes2EXT = reinterpret_cast<PFN_vkGetDeviceGroupSurfacePresentModes2EXT>(vkGetDeviceProcAddr(device, "vkGetDeviceGroupSurfacePresentModes2EXT")),
        #endif
        #if (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control))
            .vkGetImageSubresourceLayout2EXT = reinterpret_cast<PFN_vkGetImageSubresourceLayout2EXT>(vkGetDeviceProcAddr(device, "vkGetImageSubresourceLayout2EXT")),
        #endif
        #if (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state))
            .vkCmdSetVertexInputEXT = reinterpret_cast<PFN_vkCmdSetVertexInputEXT>(vkGetDeviceProcAddr(device, "vkCmdSetVertexInputEXT")),
        #endif
        #if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template))
            .vkCmdPushDescriptorSetWithTemplateKHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetWithTemplateKHR>(vkGetDeviceProcAddr(device, "vkCmdPushDescriptorSetWithTemplateKHR")),
        #endif
        #if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
            .vkGetDeviceGroupPresentCapabilitiesKHR = reinterpret_cast<PFN_vkGetDeviceGroupPresentCapabilitiesKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceGroupPresentCapabilitiesKHR")),
            .vkGetDeviceGroupSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetDeviceGroupSurfacePresentModesKHR>(vkGetDeviceProcAddr(device, "vkGetDeviceGroupSurfacePresentModesKHR")),
        #endif
        #if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
            .vkAcquireNextImage2KHR = reinterpret_cast<PFN_vkAcquireNextImage2KHR>(vkGetDeviceProcAddr(device, "vkAcquireNextImage2KHR")),
        #endif
       };
    }
}

