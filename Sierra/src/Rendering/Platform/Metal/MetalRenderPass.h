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
        [[nodiscard]] uint32 GetResolveAttachmentCount() const override { return resolvedColorAttachmentIndices.size(); };
        [[nodiscard]] bool HasDepthAttachment() const override { return depthAttachmentIndex.has_value(); };

        [[nodiscard]] inline const std::optional<uint32>& GetDepthAttachmentIndex() const { return depthAttachmentIndex; }

        [[nodiscard]] inline uint32 GetSubpassCount() const { return renderPassDescriptors.size(); }
        [[nodiscard]] inline MTL::RenderPassDescriptor* GetSubpass(const uint32 subpassIndex) const { return renderPassDescriptors[subpassIndex]; }
        [[nodiscard]] inline const std::vector<uint32>& GetSubpassRenderTargets(const uint32 subpassIndex) const { return subpassRenderTargets[subpassIndex]; }
        [[nodiscard]] inline const std::vector<uint32>& GetResolvedColorAttachmentIndices() const { return resolvedColorAttachmentIndices; }

        /* --- DESTRUCTOR --- */
        ~MetalRenderPass() override;

        /* --- CONVERSIONS --- */
        static MTL::LoadAction AttachmentLoadOperationToLoadAction(RenderPassAttachmentLoadOperation loadOperation);
        static MTL::StoreAction AttachmentStoreOperationToStoreAction(RenderPassAttachmentStoreOperation storeOperation);

    private:
        std::vector<MTL::RenderPassDescriptor*> renderPassDescriptors;
        std::vector<std::vector<uint32>> subpassRenderTargets;

        uint32 colorAttachmentCount = 0;
        std::optional<uint32> depthAttachmentIndex = std::nullopt;
        std::vector<uint32> resolvedColorAttachmentIndices;

    };

}
