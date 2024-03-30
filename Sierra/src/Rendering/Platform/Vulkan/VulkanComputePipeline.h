//
// Created by Nikolay Kanchevski on 27.01.24.
//

#pragma once

#include "../../ComputePipeline.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanComputePipeline final : public ComputePipeline, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanComputePipeline(const VulkanDevice &device, const ComputePipelineCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint16 GetPushConstantSize() const { return pushConstantSize; }
        [[nodiscard]] inline VkPipeline GetVulkanPipeline() const { return pipeline; }
        [[nodiscard]] inline VkPipelineLayout GetVulkanPipelineLayout() const { return device.GetPipelineLayout(pushConstantSize); }

        /* --- DESTRUCTOR --- */
        ~VulkanComputePipeline() override;

    private:
        const VulkanDevice &device;

        VkPipeline pipeline = VK_NULL_HANDLE;
        uint16 pushConstantSize = 0;

    };

}
