//
// Created by Nikolay Kanchevski on 12.12.22.
//

#pragma once

#include "Image.h"
#include "Framebuffer.h"
#include "../VulkanTypes.h"
#include "CommandBuffer.h"

namespace Sierra::Rendering
{

    enum class RenderPassAttachmentType
    {
        SWAPCHAIN = 0,
        COLOR = 1,
        DEPTH = 2
    };

    struct RenderPassAttachment
    {
        UniquePtr<Image>& image;
        LoadOp loadOp =  LoadOp::CLEAR;
        StoreOp storeOp = StoreOp::STORE;
        RenderPassAttachmentType type = RenderPassAttachmentType::COLOR;
        Vector4 clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    };

    struct SubpassCreateInfo
    {
        std::vector<uint> renderTargets{};
        std::vector<uint> subpassInputs{};
        bool loadDepthAttachment = false;
    };

    struct RenderPassCreateInfo
    {
        const std::vector<RenderPassAttachment> &attachments;
        const std::vector<SubpassCreateInfo> &subpassInfos = { { .renderTargets = { 0 } } };
    };

    class RenderPass
    {
    private:
        struct SubpassAttachmentTransitionInfo
        {
            Image* image = nullptr;
            ImageLayout finalLayout = ImageLayout::UNDEFINED;
        };

        struct SubpassInfo
        {
            bool writesToDepthAttachment = false;
            bool hasDepthAttachment = false;
            uint colorAttachmentCount = 0;
            std::vector<SubpassAttachmentTransitionInfo> attachmentTransitionInfo{};
        };

        class SubpassDescription
        {
        public:
            inline SubpassDescription& AddColorReference(const VkAttachmentReference reference) { colorReferences.push_back(reference); return *this; }
            inline SubpassDescription& AddResolveReference(const VkAttachmentReference reference) { resolveReferences.push_back(reference); return *this; }
            inline SubpassDescription& AddInputReference(const VkAttachmentReference reference) { inputReferences.push_back(reference); return *this; }
            inline SubpassDescription& SetDepthReference(const VkAttachmentReference reference) { depthReference = reference; return *this; }
            [[nodiscard]] inline VkSubpassDescription const CreateVulkanSubpassDescription()
            {
                return {
                    .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                    .inputAttachmentCount = static_cast<uint>(inputReferences.size()),
                    .pInputAttachments = inputReferences.data(),
                    .colorAttachmentCount = static_cast<uint>(colorReferences.size()),
                    .pColorAttachments = colorReferences.data(),
                    .pResolveAttachments = resolveReferences.data(),
                    .pDepthStencilAttachment = depthReference.layout != VK_IMAGE_LAYOUT_UNDEFINED ? &depthReference : nullptr
                };
            }

        private:
            VkAttachmentReference depthReference{};
            std::vector<VkAttachmentReference> colorReferences{};
            std::vector<VkAttachmentReference> resolveReferences{};
            std::vector<VkAttachmentReference> inputReferences{};

        };

    public:
        /* --- CONSTRUCTORS --- */
        RenderPass(const RenderPassCreateInfo &createInfo);
        static UniquePtr<RenderPass> Create(const RenderPassCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void NextSubpass(const UniquePtr<CommandBuffer> &commandBuffer);
        void Begin(const UniquePtr<Framebuffer> &framebuffer, const UniquePtr<CommandBuffer> &commandBuffer);
        void End(const UniquePtr<CommandBuffer> &commandBuffer);

        /* --- SETTER METHODS --- */
        void SetClearColor(uint colorAttachmentIndex, const Vector4& color);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool HasDepthAttachment(const uint forSubpassIndex = 0) const { return subpasses[forSubpassIndex].hasDepthAttachment; }
        [[nodiscard]] inline uint GetColorAttachmentCount(const uint forSubpassIndex = 0) const { return subpasses[forSubpassIndex].colorAttachmentCount; }
        [[nodiscard]] inline VkRenderPass GetVulkanRenderPass() const { return renderPass; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(RenderPass);

    private:
        VkRenderPass renderPass = VK_NULL_HANDLE;
        uint currentSubpass = 0;

        std::vector<SubpassInfo> subpasses;
        std::vector<VkClearValue> clearValues;
    };

}
