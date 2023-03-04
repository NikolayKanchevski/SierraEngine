//
// Created by Nikolay Kanchevski on 1.03.23.
//

#pragma once

#include "Image.h"
#include "CommandBuffer.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    struct DynamicRendererAttachment
    {
    public:
        UniquePtr<Image>& image;
        LoadOp loadOp = LoadOp::CLEAR;
        StoreOp storeOp = StoreOp::STORE;
        Vector4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

        UniquePtr<Image>* resolveImage = nullptr;
        ResolveMode resolveMode = ResolveMode::NONE;

    private:
        friend class DynamicRenderer;
        inline bool IsDepth() const { return IS_FLAG_PRESENT(image->GetUsage(), ImageUsage::DEPTH_STENCIL_ATTACHMENT); }

    };

    struct DynamicRendererCreateInfo
    {
        const std::vector<DynamicRendererAttachment> &attachments;
    };

    class DynamicRenderer
    {
    public:
        /* --- CONSTRUCTORS --- */
        DynamicRenderer(const DynamicRendererCreateInfo &createInfo);
        static UniquePtr<DynamicRenderer> Create(DynamicRendererCreateInfo createInfo);

        /* --- POLLING METHODS --- */
        void Begin(const UniquePtr<CommandBuffer> &commandBuffer);
        void End(const UniquePtr<CommandBuffer> &commandBuffer);

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(DynamicRenderer);

    private:
        VkRenderingInfoKHR renderingInfo{};
        VkRenderingAttachmentInfoKHR* colorAttachments = nullptr;
        VkRenderingAttachmentInfoKHR* depthStencilAttachment = nullptr;

        inline static PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR = nullptr;
        inline static PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR = nullptr;

    };

}