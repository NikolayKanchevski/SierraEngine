//
// Created by Nikolay Kanchevski on 27.01.24.
//

#include "VulkanComputePipeline.h"

#include "VulkanShader.h"

#include "VulkanResultHandler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanComputePipeline::VulkanComputePipeline(const VulkanDevice& device, const ComputePipelineCreateInfo& createInfo)
        : ComputePipeline(createInfo), device(device), name(createInfo.name), pushConstantSize(createInfo.pushConstantSize)
    {
        SR_THROW_IF(createInfo.computeShader.GetBackendType() != RenderingBackendType::Vulkan, UnexpectedTypeError(SR_FORMAT("Cannot create compute pipeline [{0}] with compute shader [{1}], as its backend type differs from [RenderingBackendType::Vulkan]", name, createInfo.computeShader.GetName())));
        const VulkanShader& vulkanComputeShader = static_cast<const VulkanShader&>(createInfo.computeShader);

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
        const VkResult result = device.GetFunctionTable().vkCreateComputePipelines(device.GetVulkanDevice(), VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &pipeline);
        if (result != VK_SUCCESS) HandleVulkanResult(result, SR_FORMAT("Could not create compute pipeline [{0}]", name));
    }

    /* --- DESTRUCTOR --- */

    VulkanComputePipeline::~VulkanComputePipeline() noexcept
    {
        device.GetFunctionTable().vkDestroyPipeline(device.GetVulkanDevice(), pipeline, nullptr);
    }

}