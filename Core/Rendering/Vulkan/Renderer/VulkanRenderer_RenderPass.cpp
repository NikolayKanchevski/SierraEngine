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
                    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                }
            },
            { { .renderTargets = { 0 } } }
        });
    }

}