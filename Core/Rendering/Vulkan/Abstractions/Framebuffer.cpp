//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Framebuffer.h"

#include "../VulkanCore.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    /* --- CONSTRUCTORS --- */

    Framebuffer::Framebuffer(const FramebufferCreateInfo &createInfo)
        : width(createInfo.width), height(createInfo.height)
    {
        // Set up the framebuffer creation info
        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = createInfo.renderPass;
        framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(createInfo.attachments.size());
        framebufferCreateInfo.width = createInfo.width;
        framebufferCreateInfo.height = createInfo.height;
        framebufferCreateInfo.layers = 1;
        framebufferCreateInfo.pAttachments = createInfo.attachments.data();

        // Create the Vulkan framebuffer
        VK_ASSERT(
            vkCreateFramebuffer(VulkanCore::GetLogicalDevice(), &framebufferCreateInfo, nullptr, &vkFramebuffer),
            "Failed to create a framebuffer with attachment count of [" + std::to_string(createInfo.attachments.size()) + "]"
        );
    }

    std::unique_ptr<Framebuffer> Framebuffer::Create(FramebufferCreateInfo createInfo)
    {
        return std::make_unique<Framebuffer>(createInfo);
    }

    /* --- DESTRUCTOR --- */

    void Framebuffer::Destroy()
    {
        // Destroy the Vulkan framebuffer
        vkDestroyFramebuffer(VulkanCore::GetLogicalDevice(), this->vkFramebuffer, nullptr);
    }
}
