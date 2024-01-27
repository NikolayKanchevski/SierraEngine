//
// Created by Nikolay Kanchevski on 27.01.24.
//

#include "VulkanComputePipeline.h"

#include "VulkanShader.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanComputePipeline::VulkanComputePipeline(const VulkanDevice &device, const ComputePipelineCreateInfo &createInfo)
        : ComputePipeline(createInfo), VulkanResource(createInfo.name), device(device), layout(static_cast<VulkanPipelineLayout&>(*createInfo.layout))
    {
        SR_ERROR_IF(createInfo.layout->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot create compute pipeline [{0}] with pipeline layout [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), createInfo.layout->GetName());

        SR_ERROR_IF(createInfo.computeShader->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot create compute pipeline [{0}] with compute shader [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), createInfo.computeShader->GetName());
        const VulkanShader &vulkanComputeShader = static_cast<VulkanShader&>(*createInfo.computeShader);

        // Set up only shader stage
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo = { };
        shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        shaderStageCreateInfo.module = vulkanComputeShader.GetVulkanShaderModule();
        shaderStageCreateInfo.pName = "main";

        // Set up compute pipeline create info
        VkComputePipelineCreateInfo pipelineCreateInfo = { };
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.stage = shaderStageCreateInfo;
        pipelineCreateInfo.layout = layout.GetVulkanPipelineLayout();
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = -1;

        // Create pipeline
        const VkResult result = device.GetFunctionTable().vkCreateComputePipelines(device.GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create compute pipeline [{0}]! Error code: {1}.", GetName(), result);
    }

    /* --- DESTRUCTOR --- */

    VulkanComputePipeline::~VulkanComputePipeline()
    {
        device.GetFunctionTable().vkDestroyPipeline(device.GetLogicalDevice(), pipeline, nullptr);
    }

}