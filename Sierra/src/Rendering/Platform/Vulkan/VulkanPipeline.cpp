//
// Created by Nikolay Kanchevski on 8.01.24.
//

#include "VulkanPipeline.h"

#include "VulkanPipelineLayout.h"
#include "VulkanCommandBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanPipeline::VulkanPipeline(const VulkanDevice &device, const VulkanPipelineCreateInfo &createInfo)
        : VulkanResource(createInfo.name), device(device), pipelineLayout(static_cast<const VulkanPipelineLayout&>(*createInfo.layout)), bindPoint(createInfo.bindPoint), concurrentFrameCount(createInfo.concurrentFrameCount)
    {
        SR_ERROR_IF(createInfo.layout->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot create pipeline [{0}] using pipeline layout [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!");
        if (pipelineLayout.GetVulkanDescriptorSetLayout() != VK_NULL_HANDLE)
        {
            // Create descriptor set for every concurrent frame
            descriptorSets.resize(concurrentFrameCount);
            for (uint32 i = 0; i < createInfo.concurrentFrameCount; i++)
            {
                for (uint32 j = 0; j < pipelineLayout.GetBindings().size(); j++)
                {
                    descriptorSets[i] = std::make_unique<VulkanPushDescriptorSet>(VulkanDescriptorSetCreateInfo());
                }
            }
        }
    }

    /* --- POLLING METHODS --- */

    void VulkanPipeline::Begin(std::unique_ptr<CommandBuffer> &commandBuffer) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot begin graphics pipeline [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(*commandBuffer);

        // Bind pipeline
        device.GetFunctionTable().vkCmdBindPipeline(vulkanCommandBuffer.GetVulkanCommandBuffer(), bindPoint, pipeline);
        resourcesBound = descriptorSets.empty();
    }

    void VulkanPipeline::End(std::unique_ptr<CommandBuffer> &commandBuffer) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot end graphics pipeline [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        currentFrame = (currentFrame + 1) % concurrentFrameCount;
    }

    void VulkanPipeline::PushConstants(std::unique_ptr<CommandBuffer> &commandBuffer, const void* data, const uint16 memoryRange, const uint16 offset) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot push constants to pipeline [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(*commandBuffer);

        SR_ERROR_IF(memoryRange > pipelineLayout.GetPushConstantSize(), "[Vulkan]: Cannot push [{0}] bytes of push constant data to pipeline [{1}], as specified memory range is bigger than specified in the corresponding pipeline layout, which is [{2}] bytes!", memoryRange, GetName(), pipelineLayout.GetPushConstantSize());

        device.GetFunctionTable().vkCmdPushConstants(vulkanCommandBuffer.GetVulkanCommandBuffer(), pipelineLayout.GetVulkanPipelineLayout(), VK_SHADER_STAGE_ALL, offset, memoryRange, data);
    }

    void VulkanPipeline::BindBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, const uint32 binding, const std::unique_ptr<Buffer> &buffer, const uint32 arrayIndex, const uint64 memoryRange, const uint64 offset) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind buffer [{0}] to binding [{1}] within pipeline [{2}] using command buffer [{3}] as its graphics API differs from [GraphicsAPI::Vulkan]!", buffer->GetName(), binding, GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(*commandBuffer);

        SR_ERROR_IF(buffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind buffer [{0}] to binding [{1}] within pipeline [{2}], as its graphics API differs from [GraphicsAPI::Vulkan]!", buffer->GetName(), binding, GetName());
        const VulkanBuffer &vulkanBuffer = static_cast<const VulkanBuffer&>(*buffer);

        SR_ERROR_IF(offset + memoryRange > buffer->GetMemorySize(), "[Vulkan]: Cannot bind [{0}] bytes (offset by another [{1}] bytes) from buffer [{2}] to pipeline [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryRange, offset, buffer->GetName(), GetName(), offset + memoryRange, buffer->GetMemorySize());

        // Bind buffer
        descriptorSets[currentFrame]->BindBuffer(binding, vulkanBuffer, arrayIndex, memoryRange != 0 ? memoryRange : buffer->GetMemorySize(), offset);
        resourcesBound = false;
    }

    void VulkanPipeline::BindImage(std::unique_ptr<CommandBuffer> &commandBuffer, const uint32 binding, const std::unique_ptr<Image> &image, const uint32 arrayIndex) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind image [{0}] to binding [{1}] within pipeline [{2}] using command buffer [{3}] as its graphics API differs from [GraphicsAPI::Vulkan]!", image->GetName(), binding, GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(*commandBuffer);

        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot bind image [{0}] to binding [{1}] within pipeline [{2}], as its graphics API differs from [GraphicsAPI::Vulkan]!", image->GetName(), binding, GetName());
        const VulkanImage &vulkanImage = static_cast<const VulkanImage&>(*image);

        // Bind image
        descriptorSets[currentFrame]->BindImage(binding, vulkanImage, arrayIndex);
        resourcesBound = false;
    }

    /* --- PROTECTED METHODS --- */

    void VulkanPipeline::BindResources(const VulkanCommandBuffer &commandBuffer) const
    {
        if (!resourcesBound)
        {
            if (!descriptorSets.empty()) device.GetFunctionTable().vkCmdPushDescriptorSetKHR(commandBuffer.GetVulkanCommandBuffer(), bindPoint, pipelineLayout.GetVulkanPipelineLayout(), 0, descriptorSets[currentFrame]->GetWriteDescriptorSets().size(), descriptorSets[currentFrame]->GetWriteDescriptorSets().data());
            resourcesBound = true;
        }

    }

}