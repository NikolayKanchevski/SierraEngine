//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "Image.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    class RenderPass;

    struct FramebufferCreateInfo
    {
        uint width = 0;
        uint height = 0;
        const UniquePtr<RenderPass> &renderPass;
        const std::vector<ReferenceWrapper<UniquePtr<Image>>> &attachments;
    };

    class Framebuffer
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Framebuffer(const FramebufferCreateInfo &createInfo);
        static UniquePtr<Framebuffer> Create(const FramebufferCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint GetWidth() const { return width; }
        [[nodiscard]] inline uint GetHeight() const { return height; }
        [[nodiscard]] inline VkFramebuffer GetVulkanFramebuffer() const { return vkFramebuffer; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Framebuffer);

    private:
        VkFramebuffer vkFramebuffer = VK_NULL_HANDLE;
        uint width, height;

    };

}