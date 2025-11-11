//
// Created by Nikolay Kanchevski on 14.11.24.
//

#pragma once

#include "VulkanResource.h"
#include "../Framebuffer.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanFramebuffer final : public Framebuffer, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanFramebuffer(const VulkanDevice& device, const FramebufferCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetWidth() const noexcept override { return width; }
        [[nodiscard]] uint32 GetHeight() const noexcept override { return height; }

        [[nodiscard]] VkFramebuffer GetVulkanFramebuffer() const noexcept { return framebuffer; }
        [[nodiscard]] std::span<const VkClearValue> GetVulkanClearValues() const noexcept { return clearValues; }

        /* --- COPY SEMANTICS --- */
        VulkanFramebuffer(const VulkanFramebuffer&) = delete;
        VulkanFramebuffer& operator=(const VulkanFramebuffer&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanFramebuffer(VulkanFramebuffer&&) noexcept = default;
        VulkanFramebuffer& operator=(VulkanFramebuffer&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~VulkanFramebuffer() noexcept override;

    private:
        const VulkanDevice* device;

        uint32 width = 0;
        uint32 height = 0;

        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        std::vector<VkClearValue> clearValues = { };

    };

}