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

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }
        [[nodiscard]] uint32 GetSubpassCount() const noexcept override { return subpassCount; }

        [[nodiscard]] uint32 GetColorAttachmentCount() const noexcept override { return colorAttachmentCount; }
        [[nodiscard]] bool HasDepthAttachment() const noexcept override { return hasDepthAttachment; }

        [[nodiscard]] VkRenderPass GetVulkanRenderPass() const noexcept { return renderPass; }

        /* --- COPY SEMANTICS --- */
        VulkanRenderPass(const VulkanRenderPass&) = delete;
        VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanRenderPass(VulkanRenderPass&&) noexcept = default;
        VulkanRenderPass& operator=(VulkanRenderPass&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~VulkanRenderPass() noexcept override;

    private:
        const VulkanDevice* device = nullptr;
        std::string name = { };

        uint32 subpassCount = 0;
        VkRenderPass renderPass = VK_NULL_HANDLE;

        uint32 colorAttachmentCount = 0;
        bool hasDepthAttachment = false;

    };

}
