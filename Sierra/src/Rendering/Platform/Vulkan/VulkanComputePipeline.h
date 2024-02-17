//
// Created by Nikolay Kanchevski on 27.01.24.
//

#pragma once

#include "../../ComputePipeline.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"
#include "VulkanPipelineLayout.h"

namespace Sierra
{

    class SIERRA_API VulkanComputePipeline final : public ComputePipeline, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanComputePipeline(const VulkanDevice &device, const ComputePipelineCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkPipeline GetVulkanPipeline() const { return pipeline; }
        [[nodiscard]] inline const VulkanPipelineLayout& GetLayout() const { return layout; }

        /* --- DESTRUCTOR --- */
        ~VulkanComputePipeline() override;

    private:
        const VulkanDevice &device;
        const VulkanPipelineLayout &layout;

        VkPipeline pipeline = VK_NULL_HANDLE;

    };

}
