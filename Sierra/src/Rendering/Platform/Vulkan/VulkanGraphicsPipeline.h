//
// Created by Nikolay Kanchevski on 28.12.23.
//

#pragma once

#include "../../GraphicsPipeline.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanGraphicsPipeline final : public GraphicsPipeline, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanGraphicsPipeline(const VulkanDevice &device, const GraphicsPipelineCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint16 GetPushConstantSize() const { return pushConstantSize; }
        [[nodiscard]] inline VkPipeline GetVulkanPipeline() const { return pipeline; }
        [[nodiscard]] inline VkPipelineLayout GetVulkanPipelineLayout() const { return device.GetPipelineLayout(pushConstantSize); }

        /* --- DESTRUCTOR --- */
        ~VulkanGraphicsPipeline() override;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static VkCullModeFlags CullModeToVkCullMode(CullMode cullMode);
        [[nodiscard]] static VkPolygonMode ShadeModeToVkPolygonMode(ShadeMode shadeMode);
        [[nodiscard]] static VkFrontFace FrontFaceModeToVkFrontFace(FrontFaceMode frontFaceMode);

    private:
        const VulkanDevice &device;

        VkPipeline pipeline = VK_NULL_HANDLE;
        uint16 pushConstantSize = 0;

    };

}
