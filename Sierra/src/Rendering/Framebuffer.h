//
// Created by Nikolay Kanchevski on 13.11.24.
//

#pragma once

#include "RenderingResource.h"
#include "RenderPass.h"

namespace Sierra
{

    struct FramebufferAttachment
    {
        const Image& image;
        const Image* resolveImage = nullptr;

        Color32 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        float32 clearDepth = 1.0f;
    };

    struct FramebufferCreateInfo
    {
        std::string_view name = "Framebuffer";
        uint32 width = 0;
        uint32 height = 0;

        const RenderPass& templateRenderPass;
        std::span<const FramebufferAttachment> attachments = { };
    };

    class SIERRA_API Framebuffer : public virtual RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetWidth() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetHeight() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;

        /* --- DESTRUCTOR --- */
        ~Framebuffer() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit Framebuffer(const FramebufferCreateInfo& createInfo);

        /* --- MOVE SEMANTICS --- */
        Framebuffer(Framebuffer&&) noexcept = default;
        Framebuffer& operator=(Framebuffer&&) noexcept = default;

    };

}