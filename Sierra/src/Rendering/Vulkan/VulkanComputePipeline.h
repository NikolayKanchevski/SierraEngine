//
// Created by Nikolay Kanchevski on 27.01.24.
//

#pragma once

#include "../ComputePipeline.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanComputePipeline final : public ComputePipeline, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanComputePipeline(const VulkanDevice& device, const ComputePipelineCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }

        [[nodiscard]] VkPipeline GetVulkanPipeline() const noexcept { return pipeline; }
        [[nodiscard]] VkPipelineLayout GetVulkanPipelineLayout() const noexcept { return device.GetPipelineLayout(pushConstantSize); }

        /* --- COPY SEMANTICS --- */
        VulkanComputePipeline(const VulkanComputePipeline&) = delete;
        VulkanComputePipeline& operator=(const VulkanComputePipeline&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanComputePipeline(VulkanComputePipeline&&) = delete;
        VulkanComputePipeline& operator=(VulkanComputePipeline&&) = delete;

        /* --- DESTRUCTOR --- */
        ~VulkanComputePipeline() noexcept override;

    private:
        const VulkanDevice& device;
        const std::string name;

        VkPipeline pipeline = VK_NULL_HANDLE;
        uint16 pushConstantSize = 0;

    };

}
