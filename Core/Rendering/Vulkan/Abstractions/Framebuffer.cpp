//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "../VulkanCore.h"
#include "Framebuffer.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    /* --- CONSTRUCTORS --- */

    Framebuffer::Framebuffer(VkRenderPass givenRenderPass, std::vector<VkImageView> givenAttachments)
    {
        // Set up the framebuffer creation info
        VkFramebufferCreateInfo framebufferCreateInfo;
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = givenRenderPass;
        framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(givenAttachments.size());
        framebufferCreateInfo.width = VulkanCore::GetSwapchainExtent().width;
        framebufferCreateInfo.height = VulkanCore::GetSwapchainExtent().height;
        framebufferCreateInfo.layers = 1;
        framebufferCreateInfo.pAttachments = givenAttachments.data();

        // Create the Vulkan framebuffer
        VulkanDebugger::CheckResults(
            vkCreateFramebuffer(VulkanCore::GetLogicalDevice(), &framebufferCreateInfo, nullptr, &vkFramebuffer),
            "Failed to create a framebuffer with attachment count of [" + std::to_string(givenAttachments.size()) + "]"
        );
    }

    std::unique_ptr<Framebuffer> Framebuffer::Builder::Build() const
    {
        return std::make_unique<Framebuffer>(vkRenderPass, attachments);
    }

    Framebuffer::Builder &Framebuffer::Builder::SetRenderPass(VkRenderPass givenRenderPass)
    {
        // Save the provided render pass
        this->vkRenderPass = givenRenderPass;
        return *this;
    }

    Framebuffer::Builder &Framebuffer::Builder::AddAttachments(std::vector<VkImageView> &givenAttachments)
    {
        // Add the given attachments to the local list
        this->attachments.insert(attachments.end(), givenAttachments.begin(), givenAttachments.end());
        return *this;
    }

    /* --- DESTRUCTOR --- */

    Framebuffer::~Framebuffer()
    {
        // Destroy the Vulkan framebuffer
        vkDestroyFramebuffer(VulkanCore::GetLogicalDevice(), this->vkFramebuffer, nullptr);
    }
}
