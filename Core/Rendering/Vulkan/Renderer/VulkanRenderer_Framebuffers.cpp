//
// Created by Nikolay Kanchevski on 9.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateFramebuffers()
    {
        // Resize the frame buffers array to be of the same size as the swapchainImages array
        swapchainFramebuffers.resize(swapchainImages.size());

        // Assign the static attachments
        std::vector<VkImageView> attachments(3);
        attachments[0] = this->colorImage->GetVulkanImageView();
        attachments[1] = this->depthImage->GetVulkanImageView();

        // Create a framebuffer for each swapchain image
        for (int i = swapchainImages.size(); i--;)
        {
            // Assign the dynamic attachments
            attachments[2] = this->swapchainImages[i]->GetVulkanImageView();

            // Add the attachments to the framebuffer and then create it
            swapchainFramebuffers[i] = Framebuffer::Builder()
                .SetRenderPass(this->renderPass->GetVulkanRenderPass())
                .AddAttachments(attachments)
            .Build();
        }
    }

}