//
// Created by Nikolay Kanchevski on 9.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateRenderPass()
    {
        // Create the swapchain's render pass
        swapchainRenderPass = RenderPass::Create({
            {
                {
                    .imageAttachment = swapchainImages[0],
                    .loadOp = LoadOp::CLEAR,
                    .storeOp = StoreOp::STORE,
                    .finalLayout = ImageLayout::PRESENT_SRC
                }
            },
            { { .renderTargets = { 0 } } }
        });
    }

}