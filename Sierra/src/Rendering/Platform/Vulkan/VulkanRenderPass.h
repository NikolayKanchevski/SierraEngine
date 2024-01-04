//
// Created by Nikolay Kanchevski on 10.12.23.
//

#pragma once

#include "../../RenderPass.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanRenderPass : public RenderPass, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanRenderPass(const VulkanDevice &device, const RenderPassCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Resize(uint32 width, uint32 height) override;
        void Begin(std::unique_ptr<CommandBuffer> &commandBuffer, const std::initializer_list<RenderPassBeginAttachment> &attachments) const override;
        void BeginNextSubpass(std::unique_ptr<CommandBuffer> &commandBuffer) const override;
        void End(std::unique_ptr<CommandBuffer> &commandBuffer) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetColorAttachmentCount() const override { return colorAttachmentCount; };
        [[nodiscard]] bool HasDepthAttachment() const override { return hasDepthAttachment; };

        [[nodiscard]] VkFramebuffer GetVulkanFramebuffer() const { return framebuffer; }
        [[nodiscard]] VkRenderPass GetVulkanRenderPass() const { return renderPass; }

        /* --- DESTRUCTOR --- */
        ~VulkanRenderPass();

        /* --- CONVERSIONS --- */
        static VkAttachmentLoadOp AttachmentLoadOperationToVkAttachmentLoadOp(AttachmentLoadOperation loadOperation);
        static VkAttachmentStoreOp AttachmentStoreOperationToVkAttachmentStoreOp(AttachmentStoreOperation storeOperation);

    private:
        const VulkanDevice &device;

        std::vector<VkFormat> framebufferImageAttachmentFormats;
        std::vector<VkFramebufferAttachmentImageInfo> framebufferImageAttachments;

        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        VkRenderPass renderPass = VK_NULL_HANDLE;

        uint32 colorAttachmentCount = 0;
        bool hasDepthAttachment = false;

    };

}
