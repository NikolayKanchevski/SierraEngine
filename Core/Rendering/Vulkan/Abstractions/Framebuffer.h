//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    struct FramebufferCreateInfo
    {
        uint32_t width = 0;
        uint32_t height = 0;
        const std::vector<VkImageView> &attachments;
        VkRenderPass renderPass = VK_NULL_HANDLE;
    };

    class Framebuffer
    {
    public:
        /* --- CONSTRUCTORS --- */
        Framebuffer(const FramebufferCreateInfo &createInfo);
        static std::unique_ptr<Framebuffer> Create(FramebufferCreateInfo createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32_t GetWidth() const { return width; }
        [[nodiscard]] inline uint32_t GetHeight() const { return height; }
        [[nodiscard]] inline VkFramebuffer GetVulkanFramebuffer() const { return this->vkFramebuffer; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        Framebuffer(const Framebuffer &) = delete;
        Framebuffer &operator=(const Framebuffer &) = delete;
    private:
        VkFramebuffer vkFramebuffer;
        uint32_t width, height;

    };

}