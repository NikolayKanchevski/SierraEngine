//
// Created by Nikolay Kanchevski on 10.12.23.
//

#pragma once

#include "../../RenderPass.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalRenderPass : public RenderPass, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalRenderPass(const MetalDevice &device, const RenderPassCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Resize(uint32 width, uint32 height) override;
        void Begin(std::unique_ptr<CommandBuffer> &commandBuffer, const std::initializer_list<RenderPassBeginAttachment> &attachments) const override;
        void BeginNextSubpass(std::unique_ptr<CommandBuffer> &commandBuffer) const override;
        void End(std::unique_ptr<CommandBuffer> &commandBuffer) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetColorAttachmentCount() const override { return static_cast<uint32>(attachmentTable.size()) - hasDepthAttachment; };
        [[nodiscard]] bool HasDepthAttachment() const override { return hasDepthAttachment; };

        [[nodiscard]] inline const auto& GetAttachmentTable() const { return attachmentTable; }

        /* --- DESTRUCTOR --- */
        ~MetalRenderPass() override;

        /* --- CONVERSIONS --- */
        static MTL::LoadAction AttachmentLoadOperationToLoadAction(AttachmentLoadOperation loadOperation);
        static MTL::StoreAction AttachmentStoreOperationToStoreAction(AttachmentStoreOperation storeOperation);

    private:
        std::vector<MTL::RenderPassDescriptor*> renderPassDescriptors;

        struct AttachmentTableEntry
        {
            MTL::RenderPassAttachmentDescriptor* attachmentDescriptor = nullptr;
            AttachmentType attachmentType = AttachmentType::Undefined;
        };
        std::vector<AttachmentTableEntry> attachmentTable; // Attachment index tells to which subpass it is tied
        bool hasDepthAttachment = false;

        mutable uint32 currentSubpass = 0;

    };

}
