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
        [[nodiscard]] std::string_view GetName() const override;

        [[nodiscard]] uint32 GetColorAttachmentCount() const override { return colorAttachmentCount; }
        [[nodiscard]] bool HasDepthAttachment() const override { return hasDepthAttachment; }

        [[nodiscard]] MTLRenderPassDescriptor* GetSubpass(const size subpassIndex) const { return subpasses[subpassIndex]; }
        [[nodiscard]] std::span<MTLRenderPassAttachmentDescriptor* const> GetAttachment(const size attachmentIndex) const { return attachmentMap[attachmentIndex]; }

        /* --- DESTRUCTOR --- */
        ~MetalRenderPass() override;

        /* --- CONVERSIONS --- */
        static MTLLoadAction AttachmentLoadOperationToLoadAction(RenderPassAttachmentLoadOperation loadOperation);
        static MTLStoreAction AttachmentStoreOperationToStoreAction(RenderPassAttachmentStoreOperation storeOperation);

    private:
        std::string name;

        std::vector<MTLRenderPassDescriptor*> subpasses = { };
        std::vector<std::vector<MTLRenderPassAttachmentDescriptor*>> attachmentMap = { };

        uint32 colorAttachmentCount = 0;
        bool hasDepthAttachment = false;

    };

}
