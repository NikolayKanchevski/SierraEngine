//
// Created by Nikolay Kanchevski on 9.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateFramebuffers()
    {
        // Resize framebuffers vectors to store one image for each concurrent frame
        swapchainFramebuffers.resize(maxConcurrentFrames);

        // Set up image attachments for framebuffers
        std::vector<VkImageView> swapchainAttachments(1);   // Color image only (which is a combination of offscreen images)
        
        // Create a framebuffer for each concurrent frame
        for (uint32_t i = maxConcurrentFrames; i--;)
        {
            // Set color image of corresponding frame
            swapchainAttachments[0] = this->swapchainImages[i]->GetVulkanImageView();

            // Create a framebuffer from the stored attachments
            swapchainFramebuffers[i] = Framebuffer::Create({
                .width = swapchainExtent.width,
                .height = swapchainExtent.height,
                .attachments = swapchainAttachments,
                .renderPass = this->swapchainRenderPass->GetVulkanRenderPass()
            });
        }
    }

}