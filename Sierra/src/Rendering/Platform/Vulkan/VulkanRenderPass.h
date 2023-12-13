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

        /* --- DESTRUCTOR --- */
        void Destroy() override;

        /* --- CONVERSIONS --- */
        static VkAttachmentLoadOp AttachmentLoadOperationToVkAttachmentLoadOp(AttachmentLoadOperation loadOperation);
        static VkAttachmentStoreOp AttachmentStoreOperationToVkAttachmentStoreOp(AttachmentStoreOperation storeOperation);

    private:
        const VulkanDevice &device;

        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        VkRenderPass renderPass = VK_NULL_HANDLE;
        std::vector<VkClearValue> clearValues;

    };

}
