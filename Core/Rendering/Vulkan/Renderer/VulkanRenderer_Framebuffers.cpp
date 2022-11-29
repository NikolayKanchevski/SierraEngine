//
// Created by Nikolay Kanchevski on 9.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateFramebuffers()
    {
        // Resize framebuffers vectors to store one image for each concurrent frame
        offscreenFramebuffers.resize(maxConcurrentFrames);
        swapchainFramebuffers.resize(maxConcurrentFrames);

        // Set up image attachments for framebufferss
        std::vector<VkImageView> offscreenAttachments(msaaSamplingEnabled + 2);   // Color Resolve + Depth + Color
        std::vector<VkImageView> swapchainAttachments(1);                         // Color image only (which is a combination of offscreen images)

        // Set color attachment if sampling enabled
        uint32_t colorAttachmentBinding = 0;
        if (msaaSamplingEnabled)
        {
            offscreenAttachments[0] = this->colorImage->GetVulkanImageView();
            colorAttachmentBinding = 2;
        }

        // Set depth attachment
        offscreenAttachments[1] = this->depthImage->GetVulkanImageView();

        // Create a framebuffer for each concurrent frame
        for (int i = maxConcurrentFrames; i--;)
        {
            // Set color image of corresponding frame
            offscreenAttachments[colorAttachmentBinding] = this->offscreenImages[i]->GetVulkanImageView();

            // Create a framebuffer from the stored attachments
            offscreenFramebuffers[i] = Framebuffer::Builder()
                .SetRenderPass(this->offscreenRenderPass->GetVulkanRenderPass())
                .AddAttachments(offscreenAttachments)
            .Build();

            // Set color image of corresponding frame
            swapchainAttachments[0] = this->swapchainImages[i]->GetVulkanImageView();

            // Create a framebuffer from the stored attachments
            swapchainFramebuffers[i] = Framebuffer::Builder()
                .SetRenderPass(this->swapchainRenderPass->GetVulkanRenderPass())
                .AddAttachments(swapchainAttachments)
            .Build();
        }
    }

}