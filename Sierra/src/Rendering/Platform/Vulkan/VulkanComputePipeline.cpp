//
// Created by Nikolay Kanchevski on 27.01.24.
//

#include "VulkanComputePipeline.h"

#include "VulkanShader.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanComputePipeline::VulkanComputePipeline(const VulkanDevice &device, const ComputePipelineCreateInfo &createInfo)
        : ComputePipeline(createInfo), VulkanResource(createInfo.name), device(device), pushConstantSize(createInfo.pushConstantSize)
    {
        SR_ERROR_IF(createInfo.computeShader->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot create compute pipeline [{0}] with compute shader [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), createInfo.computeShader->GetName());
        const VulkanShader &vulkanComputeShader = static_cast<const VulkanShader&>(*createInfo.computeShader);

        // Set up only shader stage
        const VkPipelineShaderStageCreateInfo shaderStageCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .module = vulkanComputeShader.GetVulkanShaderModule(),
            .pName = "main"
        };

        // Set up compute pipeline create info
        const VkComputePipelineCreateInfo computePipelineCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            .stage = shaderStageCreateInfo,
            .layout = device.GetPipelineLayout(createInfo.pushConstantSize),
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
        };

        // Create pipeline
        const VkResult result = device.GetFunctionTable().vkCreateComputePipelines(device.GetLogicalDevice(), VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &pipeline);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create compute pipeline [{0}]! Error code: {1}.", GetName(), static_cast<int32>(result));
    }

    /* --- DESTRUCTOR --- */

    VulkanComputePipeline::~VulkanComputePipeline()
    {
        device.GetFunctionTable().vkDestroyPipeline(device.GetLogicalDevice(), pipeline, nullptr);
    }

}