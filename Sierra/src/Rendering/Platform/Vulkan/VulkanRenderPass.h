//
// Created by Nikolay Kanchevski on 10.12.23.
//

#pragma once

#include "../../RenderPass.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanRenderPass final : public RenderPass, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanRenderPass(const VulkanDevice &device, const RenderPassCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Resize(uint32 width, uint32 height) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetColorAttachmentCount() const override { return framebufferImageAttachments.size() - (resolveAttachmentCount + hasDepthAttachment); }
        [[nodiscard]] inline bool HasDepthAttachment() const override { return hasDepthAttachment; };

        [[nodiscard]] inline VkFramebuffer GetVulkanFramebuffer() const { return framebuffer; }
        [[nodiscard]] inline VkRenderPass GetVulkanRenderPass() const { return renderPass; }

        /* --- DESTRUCTOR --- */
        ~VulkanRenderPass() override;

        /* --- CONVERSIONS --- */
        static VkAttachmentLoadOp AttachmentLoadOperationToVkAttachmentLoadOp(RenderPassAttachmentLoadOperation loadOperation);
        static VkAttachmentStoreOp AttachmentStoreOperationToVkAttachmentStoreOp(RenderPassAttachmentStoreOperation storeOperation);

    private:
        const VulkanDevice &device;

        std::vector<VkFormat> framebufferAttachmentImageFormats;
        std::vector<VkFramebufferAttachmentImageInfo> framebufferImageAttachments;

        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        VkRenderPass renderPass = VK_NULL_HANDLE;

        bool hasDepthAttachment = false;
        uint32 resolveAttachmentCount = 0;

    };

}
