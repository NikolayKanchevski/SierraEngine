//
// Created by Nikolay Kanchevski on 19.06.23.
//

#include "ComputePipeline.h"

#include "../VK.h"

namespace Sierra::Rendering
{
    /* --- CONSTRUCTORS --- */

    ComputePipeline::ComputePipeline(const ComputePipelineCreateInfo &createInfo)
        : Pipeline({ .maxConcurrentFrames = 1, .shaders = { createInfo.shader }, .shaderInfo = std::move(createInfo.shaderInfo) })
    {
        CreatePipeline();
    }

    UniquePtr<ComputePipeline> ComputePipeline::Create(const ComputePipelineCreateInfo &createInfo)
    {
        return std::make_unique<ComputePipeline>(createInfo);
    }

    void ComputePipeline::CreatePipeline()
    {
        // Check if recreating
        if (vkPipeline != nullptr)
        {
            VK::GetDevice()->WaitUntilIdle();
            vkDestroyPipeline(VK::GetLogicalDevice(), vkPipeline, nullptr);
        }

        // Set bind point
        bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

        // Set up create info
        VkComputePipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.stage = *shaders.begin()->second->shaderStageCreateInfo;
        pipelineCreateInfo.layout = vkPipelineLayout;

        // Create the compute pipeline
        VK_ASSERT(
            vkCreateComputePipelines(VK::GetLogicalDevice(), pipelineCache, 1, &pipelineCreateInfo, nullptr, &vkPipeline),
            "Failed to create compute pipeline"
        );
    }

    /* --- POLLING METHODS --- */

    void ComputePipeline::Dispatch(const UniquePtr<CommandBuffer> &commandBuffer, const uint xCount, const uint yCount, const uint zCount)
    {
        BindResources(commandBuffer);
        vkCmdDispatch(commandBuffer->GetVulkanCommandBuffer(), xCount, yCount, zCount);
    }

    /* --- DESTRUCTOR --- */

    void ComputePipeline::Destroy()
    {
        Pipeline::Destroy();
    }

}
