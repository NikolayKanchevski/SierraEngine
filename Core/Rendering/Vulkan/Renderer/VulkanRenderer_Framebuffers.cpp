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
        const uint32_t attachmentCount = msaaSamplingEnabled ? 3 : 2;
        std::vector<VkImageView> attachments(attachmentCount);

        // If MSAA is enabled attach the sampled color image
        if (msaaSamplingEnabled) attachments[0] = this->colorImage->GetVulkanImageView();
        attachments[1] = this->depthImage->GetVulkanImageView();

        // Get index of swapchain image without sampling based on whether a sampled image exists
        const uint32_t swapchainAttachmentIndex = msaaSamplingEnabled ? 2 : 0;

        // Create a framebuffer for each swapchain image
        for (int i = swapchainImages.size(); i--;)
        {
            // Assign the dynamic attachments
            attachments[swapchainAttachmentIndex] = this->swapchainImages[i]->GetVulkanImageView();

            // Add the attachments to the framebuffer and then create it
            swapchainFramebuffers[i] = Framebuffer::Builder()
                .SetRenderPass(this->renderPass->GetVulkanRenderPass())
                .AddAttachments(attachments)
            .Build();
        }
    }

}