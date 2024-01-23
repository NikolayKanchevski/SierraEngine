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
        [[nodiscard]] bool HasDepthAttachment() const override { return depthAttachmentIndex.has_value(); };
        [[nodiscard]] inline const std::optional<uint32>& GetDepthAttachmentIndex() const { return depthAttachmentIndex; }

        [[nodiscard]] inline MTL::RenderPassDescriptor* GetSubpass(const uint32 subpassIndex) const { return renderPassDescriptors[subpassIndex]; }
        [[nodiscard]] inline uint32 GetSubpassCount() const { return renderPassDescriptors.size(); }

        /* --- DESTRUCTOR --- */
        ~MetalRenderPass() override;

        /* --- CONVERSIONS --- */
        static MTL::LoadAction AttachmentLoadOperationToLoadAction(AttachmentLoadOperation loadOperation);
        static MTL::StoreAction AttachmentStoreOperationToStoreAction(AttachmentStoreOperation storeOperation);

    private:
        std::vector<MTL::RenderPassDescriptor*> renderPassDescriptors;

        std::optional<uint32> depthAttachmentIndex = std::nullopt;
        uint32 colorAttachmentCount = 0;

    };

}
