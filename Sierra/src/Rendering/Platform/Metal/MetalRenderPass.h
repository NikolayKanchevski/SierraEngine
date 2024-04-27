//
// Created by Nikolay Kanchevski on 10.12.23.
//

#pragma once

#include "../../RenderPass.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalRenderPass final : public RenderPass, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalRenderPass(const MetalDevice &device, const RenderPassCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Resize(uint32 width, uint32 height) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetColorAttachmentCount() const override { return colorAttachmentCount; };
        [[nodiscard]] bool HasDepthAttachment() const override { return hasDepthAttachment; };

        [[nodiscard]] inline MTLRenderPassDescriptor* GetSubpass(const uint32 subpassIndex) const { return subpasses[subpassIndex]; }
        [[nodiscard]] inline std::span<MTLRenderPassAttachmentDescriptor* const> GetAttachment(const uint32 attachmentIndex) const { return attachmentMap[attachmentIndex]; }

        /* --- DESTRUCTOR --- */
        ~MetalRenderPass() override;

        /* --- CONVERSIONS --- */
        static MTLLoadAction AttachmentLoadOperationToLoadAction(RenderPassAttachmentLoadOperation loadOperation);
        static MTLStoreAction AttachmentStoreOperationToStoreAction(RenderPassAttachmentStoreOperation storeOperation);

    private:
        std::vector<MTLRenderPassDescriptor*> subpasses = { };
        std::vector<std::vector<MTLRenderPassAttachmentDescriptor*>> attachmentMap = { };

        uint32 colorAttachmentCount = 0;
        bool hasDepthAttachment = false;

    };

}
