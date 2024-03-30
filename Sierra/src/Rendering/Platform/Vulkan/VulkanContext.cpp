//
// Created by Nikolay Kanchevski on 9.09.23.
//

#include "VulkanContext.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanSampler.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"
#include "VulkanShader.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanComputePipeline.h"
#include "VulkanResourceTable.h"
#include "VulkanCommandBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanContext::VulkanContext(const RenderingContextCreateInfo &createInfo)
        : RenderingContext(createInfo), VulkanResource(createInfo.name), instance(VulkanInstance({ })), device(instance, { .name = "Default Vulkan Device" })
    {
        SR_INFO("Vulkan context created successfully! Device in use: [{0}].", device.GetDeviceName());
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Buffer> VulkanContext::CreateBuffer(const BufferCreateInfo &createInfo) const
    {
        return std::make_unique<VulkanBuffer>(device, createInfo);
    }

    std::unique_ptr<Image> VulkanContext::CreateImage(const ImageCreateInfo &createInfo) const
    {
        return std::make_unique<VulkanImage>(device, createInfo);
    }

    std::unique_ptr<Sampler> VulkanContext::CreateSampler(const SamplerCreateInfo &createInfo) const
    {
        return std::make_unique<VulkanSampler>(device, createInfo);
    }

    std::unique_ptr<RenderPass> VulkanContext::CreateRenderPass(const RenderPassCreateInfo &createInfo) const
    {
        return std::make_unique<VulkanRenderPass>(device, createInfo);
    }

    std::unique_ptr<Swapchain> VulkanContext::CreateSwapchain(const SwapchainCreateInfo &createInfo) const
    {
        return std::make_unique<VulkanSwapchain>(instance, device, createInfo);
    }

    std::unique_ptr<Shader> VulkanContext::CreateShader(const ShaderCreateInfo &createInfo) const
    {
        return std::make_unique<VulkanShader>(device, createInfo);
    }

    std::unique_ptr<GraphicsPipeline> VulkanContext::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo &createInfo) const
    {
        return std::make_unique<VulkanGraphicsPipeline>(device, createInfo);
    }

    std::unique_ptr<ComputePipeline> VulkanContext::CreateComputePipeline(const ComputePipelineCreateInfo &createInfo) const
    {
        return std::make_unique<VulkanComputePipeline>(device, createInfo);
    }

    std::unique_ptr<ResourceTable> VulkanContext::CreateResourceTable(const ResourceTableCreateInfo &createInfo) const
    {
        return std::make_unique<VulkanResourceTable>(device, createInfo);
    }

    std::unique_ptr<CommandBuffer> VulkanContext::CreateCommandBuffer(const CommandBufferCreateInfo &createInfo) const
    {
        return std::make_unique<VulkanCommandBuffer>(device, createInfo);
    }

}
