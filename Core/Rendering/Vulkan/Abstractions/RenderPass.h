//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <vector>
#include "Image.h"
#include "Framebuffer.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    class Subpass
    {
    private:
        struct ColorAttachment
        {
            VkAttachmentDescription data;
            uint32_t binding;
        };

        struct ResolveAttachment
        {
            VkAttachmentDescription data;
            VkImageLayout imageLayout;
            uint32_t binding;
        };

        struct DepthAttachment
        {
            VkAttachmentDescription data;
            uint32_t binding;
        };

    public:
        /* --- CONSTRUCTORS --- */
        Subpass(VkPipelineBindPoint givenBindPoint,
                DepthAttachment givenDepthAttachment,
                std::vector<ColorAttachment> givenColorAttachments,
                std::vector<ResolveAttachment> givenResolveAttachments,
                uint32_t srcSubpass, uint32_t dstSubpass);

        class Builder
        {
        public:
            Builder& SetPipelineBindPoint(VkPipelineBindPoint givenPipelineBindPoint);
            Builder& SetDepthAttachment(uint32_t binding, Image depthImage, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
                                        VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);
            Builder& AddColorAttachment(uint32_t binding, Image colorImage, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
                                        VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);
            Builder& AddResolveAttachment(uint32_t binding, Image image, VkImageLayout finalLayout, VkImageLayout referenceLayout,
                                          VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkSampleCountFlagBits sampling = VK_SAMPLE_COUNT_1_BIT,
                                          VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);
            std::unique_ptr<Subpass> Build(uint32_t srcSubpass = ~0U, uint32_t dstSubpass = ~0U) const;

        private:
            VkPipelineBindPoint pipelineBindPoint;
            DepthAttachment depthAttachment;

            std::vector<ColorAttachment> colorAttachments;
            std::vector<ResolveAttachment> resolveAttachments;
        };

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkSubpassDescription GetVulkanSubpass() const { return this->vkSubpass; }
        [[nodiscard]] inline VkSubpassDependency GetVulkanSubpassDependency() const { return this->vkSubpassDependency; }

        /* --- DESTRUCTOR --- */
        friend class RenderPass;

        Subpass(const Subpass &) = delete;
        Subpass &operator=(const Subpass &) = delete;

    private:
        VkSubpassDescription vkSubpass;
        VkSubpassDependency vkSubpassDependency;
        VkPipelineBindPoint bindPoint;

        bool hasDepthReference;
        VkAttachmentReference depthReference;

        std::vector<VkAttachmentDescription> attachmentDescriptions;
        std::vector<VkAttachmentReference> colorAttachmentReferences;
        std::vector<VkAttachmentReference> resolveAttachmentReferences;
    };

    class RenderPass
    {
    public:
        /* --- CONSTRUCTORS --- */
        RenderPass(Subpass *subpass);

        class Builder
        {
        public:
            Builder& SetSubpass(Subpass *givenSubpass);
            [[nodiscard]] std::unique_ptr<RenderPass> Build() const;

        private:
            Subpass *subpass;
        };

        /* --- SETTER METHODS --- */
        void SetFramebuffer(Framebuffer &givenFramebuffer);
        void SetBackgroundColor(glm::vec3 givenColor);
        void Begin(VkCommandBuffer givenCommandBuffer);
        void End(VkCommandBuffer givenCommandBuffer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkRenderPass GetVulkanRenderPass() const { return this->vkRenderPass; }

        /* --- DESTRUCTOR --- */
        ~RenderPass();
        RenderPass(const RenderPass &) = delete;
        RenderPass &operator=(const RenderPass &) = delete;

    private:
        VkRenderPass vkRenderPass;
        VkClearValue clearValues[2];
        VkFramebuffer vkFramebuffer = VK_NULL_HANDLE;
        bool renderPassBegan;

    };

}