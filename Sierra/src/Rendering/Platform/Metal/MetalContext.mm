//
// Created by Nikolay Kanchevski on 21.11.23.
//

#include "MetalContext.h"

#include "MetalBuffer.h"
#include "MetalImage.h"
#include "MetalSampler.h"
#include "MetalRenderPass.h"
#include "MetalSwapchain.h"
#include "MetalShader.h"
#include "MetalPipelineLayout.h"
#include "MetalGraphicsPipeline.h"
#include "MetalComputePipeline.h"
#include "MetalCommandBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalContext::MetalContext(const RenderingContextCreateInfo &createInfo)
        : RenderingContext(createInfo), MetalResource(createInfo.name), device({ .name = "Default Metal Device" })
    {
        SR_INFO("Metal context created successfully! Device in use: [{0}].", device.GetDeviceName());
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Buffer> MetalContext::CreateBuffer(const BufferCreateInfo &createInfo) const
    {
        return std::make_unique<MetalBuffer>(device, createInfo);
    }

    std::unique_ptr<Image> MetalContext::CreateImage(const ImageCreateInfo &createInfo) const
    {
        return std::make_unique<MetalImage>(device, createInfo);
    }

    std::unique_ptr<Sampler> MetalContext::CreateSampler(const SamplerCreateInfo &createInfo) const
    {
        return std::make_unique<MetalSampler>(device, createInfo);
    }

    std::unique_ptr<RenderPass> MetalContext::CreateRenderPass(const RenderPassCreateInfo &createInfo) const
    {
        return std::make_unique<MetalRenderPass>(device, createInfo);
    }

    std::unique_ptr<Swapchain> MetalContext::CreateSwapchain(const SwapchainCreateInfo &createInfo) const
    {
        return std::make_unique<MetalSwapchain>(device, createInfo);
    }

    std::unique_ptr<Shader> MetalContext::CreateShader(const ShaderCreateInfo &createInfo) const
    {
        return std::make_unique<MetalShader>(device, createInfo);
    }

    std::unique_ptr<PipelineLayout> MetalContext::CreatePipelineLayout(const PipelineLayoutCreateInfo &createInfo) const
    {
        return std::make_unique<MetalPipelineLayout>(device, createInfo);
    }

    std::unique_ptr<GraphicsPipeline> MetalContext::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo &createInfo) const
    {
        return std::make_unique<MetalGraphicsPipeline>(device, createInfo);
    }

    std::unique_ptr<ComputePipeline> MetalContext::CreateComputePipeline(const ComputePipelineCreateInfo &createInfo) const
    {
        return std::make_unique<MetalComputePipeline>(device, createInfo);
    }

    std::unique_ptr<CommandBuffer> MetalContext::CreateCommandBuffer(const CommandBufferCreateInfo &createInfo) const
    {
        return std::make_unique<MetalCommandBuffer>(device, createInfo);
    }

}