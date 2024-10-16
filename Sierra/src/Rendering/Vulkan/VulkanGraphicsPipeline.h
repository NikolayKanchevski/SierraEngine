//
// Created by Nikolay Kanchevski on 28.12.23.
//

#pragma once

#include "../GraphicsPipeline.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API VkCullModeFlags CullModeToVkCullMode(CullMode cullMode) noexcept;
    [[nodiscard]] SIERRA_API VkPolygonMode ShadeModeToVkPolygonMode(ShadeMode shadeMode) noexcept;
    [[nodiscard]] SIERRA_API VkFrontFace FrontFaceModeToVkFrontFace(FrontFaceMode frontFaceMode) noexcept;

    class SIERRA_API VulkanGraphicsPipeline final : public GraphicsPipeline, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanGraphicsPipeline(const VulkanDevice& device, const GraphicsPipelineCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }

        [[nodiscard]] VkPipeline GetVulkanPipeline() const noexcept { return pipeline; }
        [[nodiscard]] VkPipelineLayout GetVulkanPipelineLayout() const noexcept { return device.GetPipelineLayout(pushConstantSize); }
        [[nodiscard]] size GetVertexStride() const noexcept { return vertexStride; }

        /* --- COPY SEMANTICS --- */
        VulkanGraphicsPipeline(const VulkanGraphicsPipeline&) = delete;
        VulkanGraphicsPipeline& operator=(const VulkanGraphicsPipeline&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanGraphicsPipeline(VulkanGraphicsPipeline&&) = delete;
        VulkanGraphicsPipeline& operator=(VulkanGraphicsPipeline&&) = delete;

        /* --- DESTRUCTOR --- */
        ~VulkanGraphicsPipeline() noexcept override;

    private:
        const VulkanDevice& device;
        const std::string name;

        VkPipeline pipeline = VK_NULL_HANDLE;
        size vertexStride = 0;
        uint16 pushConstantSize = 0;

    };

}
