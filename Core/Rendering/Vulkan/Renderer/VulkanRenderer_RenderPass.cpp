//
// Created by Nikolay Kanchevski on 9.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateRenderPasses()
    {
        // Set up fixed offscreen attachments
        auto offscreenSubpassBuilder = Subpass::Builder()
            .AddColorAttachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, msaaSamplingEnabled ? colorImage : offscreenImages[0], VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE)
            .SetDepthAttachment(1, depthImage, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE);

        // Add sampling resolve attachment if MSAA is enabled
        if (msaaSamplingEnabled)
        {
            offscreenSubpassBuilder.AddResolveAttachment(
                2, offscreenImages[0],
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE, VK_SAMPLE_COUNT_1_BIT,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_NONE_KHR
            );
        }

        // Create offscreen subpass and render pass
        offscreenSubpass = offscreenSubpassBuilder.Build();
        offscreenRenderPass = RenderPass::Builder().Build(offscreenSubpass);

        // Create swapchain subpass and render pass
        swapchainSubpass = Subpass::Builder()
            .AddColorAttachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, swapchainImages[0], VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE)
        .Build();

        swapchainRenderPass = RenderPass::Builder().Build(swapchainSubpass);
    }

}