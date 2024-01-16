//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

#include "../../RenderingContext.h"
#include "VulkanResource.h"

#include "VulkanInstance.h"
#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanContext final : public RenderingContext, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit VulkanContext(const RenderingContextCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Buffer> CreateBuffer(const BufferCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<Image> CreateImage(const ImageCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<RenderPass> CreateRenderPass(const RenderPassCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<Swapchain> CreateSwapchain(const SwapchainCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<Shader> CreateShader(const ShaderCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<PipelineLayout> CreatePipelineLayout(const PipelineLayoutCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<CommandBuffer> CreateCommandBuffer(const CommandBufferCreateInfo &createInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Device& GetDevice() const override { return device; };

        /* --- DESTRUCTOR --- */
        ~VulkanContext() override = default;

    private:
        VulkanInstance instance;
        VulkanDevice device;

    };

}
