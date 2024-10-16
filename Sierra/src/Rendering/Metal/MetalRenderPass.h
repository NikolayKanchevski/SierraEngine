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

        /* --- POLLING METHODS --- */
        void Resize(uint32 width, uint32 height) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }
        [[nodiscard]] uint32 GetSubpassCount() const noexcept override { return subpasses.size(); }

        [[nodiscard]] uint32 GetColorAttachmentCount() const noexcept override { return colorAttachmentCount; }
        [[nodiscard]] bool HasDepthAttachment() const noexcept override { return hasDepthAttachment; }

        [[nodiscard]] MTLRenderPassDescriptor* GetSubpass(const size subpassIndex) const { SR_THROW_IF(subpassIndex >= subpasses.size(), ValueOutOfRangeError(SR_FORMAT("Cannot get invalid subpass [{0}] of Metal render pass [{1}]", subpassIndex, name), subpassIndex, size(0), subpasses.size() - 1)); return subpasses[subpassIndex]; }
        [[nodiscard]] std::span<MTLRenderPassAttachmentDescriptor* const> GetAttachment(const size attachmentIndex) const { SR_THROW_IF(attachmentIndex >= attachmentMap.size(), ValueOutOfRangeError(SR_FORMAT("Cannot get invalid attachment [{0}] of Metal render pass [{1}]", attachmentIndex, name), attachmentIndex, size(0), attachmentMap.size() - 1)); return attachmentMap[attachmentIndex]; }

        /* --- COPY SEMANTICS --- */
        MetalRenderPass(const MetalRenderPass&) = delete;
        MetalRenderPass& operator=(const MetalRenderPass&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalRenderPass(MetalRenderPass&&) = delete;
        MetalRenderPass& operator=(MetalRenderPass&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MetalRenderPass() noexcept override;

    private:
        const std::string name;

        std::vector<MTLRenderPassDescriptor*> subpasses = { };
        std::vector<std::vector<MTLRenderPassAttachmentDescriptor*>> attachmentMap = { };

        uint32 colorAttachmentCount = 0;
        bool hasDepthAttachment = false;

    };

}
