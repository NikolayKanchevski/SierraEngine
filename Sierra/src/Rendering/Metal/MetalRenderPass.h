//
// Created by Nikolay Kanchevski on 10.12.23.
//

#pragma once

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLRenderPassDescriptor = void;
        using MTLRenderPassAttachmentDescriptor = void;
    }
#endif

#include "../RenderPass.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API MTLLoadAction AttachmentLoadOperationToLoadAction(RenderPassAttachmentLoadOperation loadOperation) noexcept;
    [[nodiscard]] SIERRA_API MTLStoreAction AttachmentStoreOperationToStoreAction(RenderPassAttachmentStoreOperation storeOperation) noexcept;

    class SIERRA_API MetalRenderPass final : public RenderPass, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalRenderPass(const MetalDevice& device, const RenderPassCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetSubpassCount() const noexcept override { return subpasses.size(); }

        [[nodiscard]] uint32 GetColorAttachmentCount() const noexcept override { return colorAttachmentCount; }
        [[nodiscard]] bool HasDepthAttachment() const noexcept override { return hasDepthAttachment; }

        [[nodiscard]] MTLRenderPassDescriptor* GetSubpassRenderPass(size subpassIndex) const;
        [[nodiscard]] ImageFormat GetSubpassColorAttachmentFormat(size subpassIndex, size attachmentIndex) const;
        [[nodiscard]] ImageFormat GetSubpassDepthAttachmentFormat(size subpassIndex) const;
        [[nodiscard]] std::span<MTLRenderPassAttachmentDescriptor* const> GetAttachmentReferences(size attachmentIndex) const;

        /* --- COPY SEMANTICS --- */
        MetalRenderPass(const MetalRenderPass&) = delete;
        MetalRenderPass& operator=(const MetalRenderPass&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalRenderPass(MetalRenderPass&&) noexcept = default;
        MetalRenderPass& operator=(MetalRenderPass&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~MetalRenderPass() noexcept override;

    private:

        struct MetalSubpass
        {
            std::vector<ImageFormat> colorFormats = { };
            ImageFormat depthFormat = ImageFormat::Undefined;
            MTLRenderPassDescriptor* renderPass = nil;
        };

        std::vector<MetalSubpass> subpasses = { };
        std::vector<std::vector<MTLRenderPassAttachmentDescriptor*>> attachmentReferences = { };

        uint32 colorAttachmentCount = 0;
        bool hasDepthAttachment = false;

    };

}
