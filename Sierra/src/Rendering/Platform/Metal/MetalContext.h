//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include "../../RenderingContext.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalContext final : public RenderingContext, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MetalContext(const RenderingContextCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Buffer> CreateBuffer(const BufferCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<Image> CreateImage(const ImageCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<Sampler> CreateSampler(const SamplerCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<RenderPass> CreateRenderPass(const RenderPassCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<Swapchain> CreateSwapchain(const SwapchainCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<Shader> CreateShader(const ShaderCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<PipelineLayout> CreatePipelineLayout(const PipelineLayoutCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo &createInfo) const override;
        [[nodiscard]] std::unique_ptr<CommandBuffer> CreateCommandBuffer(const CommandBufferCreateInfo &createInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const Device& GetDevice() const override { return device; }

        /* --- DESTRUCTOR --- */
        ~MetalContext() = default;

    private:
        MetalDevice device;

    };

}
