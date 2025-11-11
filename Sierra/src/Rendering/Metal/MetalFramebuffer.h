//
// Created by Nikolay Kanchevski on 27.12.24.
//

#pragma once

#include "MetalResource.h"
#include "../Framebuffer.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalFramebuffer final : public Framebuffer, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalFramebuffer(const MetalDevice& device, const FramebufferCreateInfo& createInfo);

        /* --- GETTER METHODS --- */

        [[nodiscard]] uint32 GetWidth() const noexcept override { return attachments[0].image.GetWidth(); }
        [[nodiscard]] uint32 GetHeight() const noexcept override { return attachments[0].image.GetHeight(); }

        [[nodiscard]] std::span<const FramebufferAttachment> GetAttachments() const noexcept { return attachments; }

        /* --- COPY SEMANTICS --- */
        MetalFramebuffer(const MetalFramebuffer&) = delete;
        MetalFramebuffer& operator=(const MetalFramebuffer&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalFramebuffer(MetalFramebuffer&&) noexcept = default;
        MetalFramebuffer& operator=(MetalFramebuffer&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~MetalFramebuffer() noexcept override = default;

    private:
        std::vector<FramebufferAttachment> attachments = { };

    };

}