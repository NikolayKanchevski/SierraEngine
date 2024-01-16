//
// Created by Nikolay Kanchevski on 28.12.23.
//

#pragma once

#include "../../GraphicsPipeline.h"

#include "VulkanDevice.h"
#include "VulkanPipeline.h"

namespace Sierra
{

    class SIERRA_API VulkanGraphicsPipeline final : public GraphicsPipeline, public VulkanPipeline
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanGraphicsPipeline(const VulkanDevice &device, const GraphicsPipelineCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void BindVertexBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Buffer> &vertexBuffer, uint64 offset = 0) const override;
        void BindIndexBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Buffer> &indexBuffer, uint64 offset = 0) const override;
        void Draw(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 vertexCount) const override;
        void DrawIndexed(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 indexCount, uint64 indexOffset = 0, uint64 vertexOffset = 0) const override;

        /* --- DESTRUCTOR --- */
        ~VulkanGraphicsPipeline() override;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static VkCullModeFlags CullModeToVkCullMode(CullMode cullMode);
        [[nodiscard]] static VkPolygonMode ShadeModeToVkPolygonMode(ShadeMode shadeMode);
        [[nodiscard]] static VkFrontFace FrontFaceModeToVkFrontFace(FrontFaceMode frontFaceMode);

    };

}
