//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <vector>
#include <array>
#include "Image.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    class Framebuffer
    {
    public:
        /* --- CONSTRUCTORS --- */
        Framebuffer(VkRenderPass givenRenderPass, std::vector<VkImageView> givenAttachments);

        class Builder
        {
        public:
            Builder& SetRenderPass(VkRenderPass givenRenderPass);
            Builder& AddAttachments(std::vector<VkImageView> &givenAttachments);
            [[nodiscard]] std::unique_ptr<Framebuffer> Build() const;

        private:
            VkRenderPass vkRenderPass;
            std::vector<VkImageView> attachments;
        };

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkFramebuffer GetVulkanFramebuffer() const { return this->vkFramebuffer; }

        /* --- DESTRUCTOR --- */
        ~Framebuffer();
        Framebuffer(const Framebuffer &) = delete;
        Framebuffer &operator=(const Framebuffer &) = delete;
    private:
        VkFramebuffer vkFramebuffer;

    };

}