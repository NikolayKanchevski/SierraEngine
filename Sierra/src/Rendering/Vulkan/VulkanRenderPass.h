//
// Created by Nikolay Kanchevski on 10.12.23.
//

#pragma once

#include "../RenderPass.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API VkAttachmentLoadOp AttachmentLoadOperationToVkAttachmentLoadOp(RenderPassAttachmentLoadOperation loadOperation) noexcept;
    [[nodiscard]] SIERRA_API VkAttachmentStoreOp AttachmentStoreOperationToVkAttachmentStoreOp(RenderPassAttachmentStoreOperation storeOperation) noexcept;

    class SIERRA_API VulkanRenderPass final : public RenderPass, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanRenderPass(const VulkanDevice& device, const RenderPassCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Resize(uint32 width, uint32 height) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }
        [[nodiscard]] uint32 GetSubpassCount() const noexcept override { return subpassCount; }

        [[nodiscard]] uint32 GetColorAttachmentCount() const noexcept override { return framebufferImageAttachments.size() - (resolveAttachmentCount + hasDepthAttachment); }
        [[nodiscard]] bool HasDepthAttachment() const noexcept override { return hasDepthAttachment; }

        [[nodiscard]] VkFramebuffer GetVulkanFramebuffer() const noexcept { return framebuffer; }
        [[nodiscard]] VkRenderPass GetVulkanRenderPass() const noexcept { return renderPass; }

        /* --- COPY SEMANTICS --- */
        VulkanRenderPass(const VulkanRenderPass&) = delete;
        VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanRenderPass(VulkanRenderPass&&) = delete;
        VulkanRenderPass& operator=(VulkanRenderPass&&) = delete;

        /* --- DESTRUCTOR --- */
        ~VulkanRenderPass() noexcept override;

    private:
        const VulkanDevice& device;
        const std::string name;

        const uint32 subpassCount = 0;
        std::vector<VkFormat> framebufferAttachmentImageFormats;
        std::vector<VkFramebufferAttachmentImageInfo> framebufferImageAttachments;

        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        VkRenderPass renderPass = VK_NULL_HANDLE;

        bool hasDepthAttachment = false;
        uint32 resolveAttachmentCount = 0;

    };

}
