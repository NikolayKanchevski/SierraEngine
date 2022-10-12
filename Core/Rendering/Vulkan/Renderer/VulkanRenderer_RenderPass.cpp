//
// Created by Nikolay Kanchevski on 9.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateRenderPass()
    {
        auto builer = Subpass::Builder()
            .SetPipelineBindPoint(VK_PIPELINE_BIND_POINT_GRAPHICS)
            .AddColorAttachment(0, swapchainImages[0], VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, true)
            .SetDepthAttachment(1, depthImage, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE);

        if (msaaSamplingEnabled)
        {
            builer.AddResolveAttachment(
                    2, swapchainImages[0], VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                    VK_ATTACHMENT_STORE_OP_STORE, VK_SAMPLE_COUNT_1_BIT,
                    VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_NONE_KHR);
        }

        subpass = builer.Build();

        renderPass = RenderPass::Builder().Build(subpass);
    }

}