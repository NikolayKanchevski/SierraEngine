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
                    .loadOp = LOAD_OP_CLEAR,
                    .storeOp = STORE_OP_STORE,
                    .finalLayout = LAYOUT_PRESENT_SRC
                }
            },
            { { .renderTargets = { 0 } } }
        });
    }

}