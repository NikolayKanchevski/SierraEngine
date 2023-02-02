//
// Created by Nikolay Kanchevski on 12.12.22.
//

#pragma once

#include "Image.h"
#include "Framebuffer.h"
#include "../VulkanTypes.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    struct RenderPassAttachment
    {
        /* --- FRAMEBUFFER PROPERTIES --- */
        UniquePtr<Image>& imageAttachment;

        /* --- RENDERPASS PROPERTIES --- */
        LoadOp loadOp =  LoadOp::UNDEFINED;
        StoreOp storeOp = StoreOp::UNDEFINED;
        ImageLayout finalLayout = ImageLayout::UNDEFINED;
        bool isResolve = false;

        bool IsDepth() const { return finalLayout == ImageLayout::DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL; }
    };

    struct SubpassInfo
    {
        std::vector<uint> renderTargets{};
        std::vector<uint> subpassInputs{};
    };

    struct RenderPassCreateInfo
    {
        const std::vector<RenderPassAttachment> &attachments;
        const std::vector<SubpassInfo> &subpassInfos;
    };

    class RenderPass
    {

    public:
        /* --- CONSTRUCTORS --- */
        RenderPass(const RenderPassCreateInfo &createInfo);
        static UniquePtr<RenderPass> Create(RenderPassCreateInfo createInfo);

        /* --- POLLING METHODS --- */
        void NextSubpass(VkCommandBuffer commandBuffer);
        void Begin(const UniquePtr<Framebuffer> &framebuffer, VkCommandBuffer commandBuffer);
        void End(VkCommandBuffer commandBuffer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkRenderPass GetVulkanRenderPass() const { return renderPass; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        RenderPass(const RenderPass &) = delete;
        RenderPass &operator=(const RenderPass &) = delete;

    private:
        VkRenderPass renderPass;
        std::vector<VkClearValue> clearValues;

        struct SubpassDescription
        {
            VkAttachmentReference depthReference{};
            std::vector<VkAttachmentReference> colorReferences{};
            std::vector<VkAttachmentReference> resolveReferences{};
            std::vector<VkAttachmentReference> inputReferences{};
            VkSubpassDescription data{};
        };

    };

}
