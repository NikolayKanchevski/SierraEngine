//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

#include "RenderingResource.h"

#include "Device.h"
#include "Buffer.h"
#include "Image.h"
#include "Sampler.h"
#include "RenderPass.h"
#include "Swapchain.h"
#include "Shader.h"
#include "ResourceTable.h"
#include "GraphicsPipeline.h"
#include "ComputePipeline.h"
#include "CommandBuffer.h"

namespace Sierra
{

    class SIERRA_API RenderingContext : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::unique_ptr<Buffer> CreateBuffer(const BufferCreateInfo &createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<Image> CreateImage(const ImageCreateInfo &createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<Sampler> CreateSampler(const SamplerCreateInfo &createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<RenderPass> CreateRenderPass(const RenderPassCreateInfo &createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<Swapchain> CreateSwapchain(const SwapchainCreateInfo &createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<Shader> CreateShader(const ShaderCreateInfo &createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo &createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo &createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<ResourceTable> CreateResourceTable(const ResourceTableCreateInfo &createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<CommandBuffer> CreateCommandBuffer(const CommandBufferCreateInfo &createInfo) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const Device& GetDevice() const = 0;

        /* --- DESTRUCTOR --- */
        ~RenderingContext() override = default;

    protected:
        RenderingContext() = default;

    };

}
