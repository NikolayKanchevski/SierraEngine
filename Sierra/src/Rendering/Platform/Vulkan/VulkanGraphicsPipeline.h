//
// Created by Nikolay Kanchevski on 28.12.23.
//

#pragma once

#include "../../GraphicsPipeline.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanGraphicsPipeline : public GraphicsPipeline, VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanGraphicsPipeline(const VulkanDevice &device, const GraphicsPipelineCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Begin(std::unique_ptr<CommandBuffer> &commandBuffer) const override;
        void End(std::unique_ptr<CommandBuffer> &commandBuffer) const override;

        void Draw(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 vertexCount) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkPipeline GetVulkanPipeline() const { return pipeline; }

        /* --- DESTRUCTOR --- */
        ~VulkanGraphicsPipeline();

        /* --- CONVERSIONS --- */
        [[nodiscard]] static VkCullModeFlags CullModeToVkCullMode(CullMode cullMode);
        [[nodiscard]] static VkPolygonMode ShadeModeToVkPolygonMode(ShadeMode shadeMode);
        [[nodiscard]] static VkFrontFace FrontFaceModeToVkFrontFace(FrontFaceMode frontFaceMode);

    private:
        const VulkanDevice &device;

        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkPipeline pipeline = VK_NULL_HANDLE;

    };

}
