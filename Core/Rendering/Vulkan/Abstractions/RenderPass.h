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
        };

        struct ResolveAttachment
        {
            VkAttachmentDescription data;
            VkImageLayout imageLayout;
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
                std::unordered_map<uint32_t, ColorAttachment> givenColorAttachments,
                std::unordered_map<uint32_t, ResolveAttachment> givenResolveAttachments,
                uint32_t srcSubpass, uint32_t dstSubpass);

        class Builder
        {
        public:
            Builder& SetPipelineBindPoint(VkPipelineBindPoint givenPipelineBindPoint);
            Builder& SetDepthAttachment(uint32_t binding, const std::unique_ptr<Image> &depthImage, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
                                        VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);
            Builder& AddColorAttachment(uint32_t binding, const std::unique_ptr<Image> &colorImage, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
                                        VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);
            Builder& AddResolveAttachment(uint32_t binding, const std::unique_ptr<Image> &image, VkImageLayout finalLayout, VkImageLayout referenceLayout,
                                          VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkSampleCountFlagBits sampling = VK_SAMPLE_COUNT_1_BIT,
                                          VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);
            std::unique_ptr<Subpass> Build(uint32_t srcSubpass = ~0U, uint32_t dstSubpass = 0) const;

        private:
            VkPipelineBindPoint pipelineBindPoint;
            DepthAttachment depthAttachment;

            std::unordered_map<uint32_t, ColorAttachment> colorAttachments;
            std::unordered_map<uint32_t, ResolveAttachment> resolveAttachments;
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
        RenderPass(const std::unique_ptr<Subpass> &subpass);

        class Builder
        {
        public:
            [[nodiscard]] std::unique_ptr<RenderPass> Build(const std::unique_ptr<Subpass> &givenSubpass) const;
        };

        /* --- SETTER METHODS --- */
        void SetFramebuffer(std::unique_ptr<Framebuffer> &givenFramebuffer);
        void SetBackgroundColor(glm::vec3 givenColor);
        void Begin(VkCommandBuffer givenCommandBuffer);
        void End(VkCommandBuffer givenCommandBuffer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkRenderPass GetVulkanRenderPass() const { return this->vkRenderPass; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        RenderPass(const RenderPass &) = delete;
        RenderPass &operator=(const RenderPass &) = delete;

    private:
        VkRenderPass vkRenderPass;
        VkClearValue clearValues[2];
        VkFramebuffer vkFramebuffer = VK_NULL_HANDLE;
        bool renderPassBegan;

    };

}