//
// Created by Nikolay Kanchevski on 10.12.23.
//

#pragma once

#include "RenderingResource.h"

#include "Image.h"

namespace Sierra
{

    enum class RenderPassAttachmentLoadOperation : bool
    {
        Clear,
        Load
    };

    enum class RenderPassAttachmentStoreOperation : bool
    {
        Store,
        Discard
    };

    enum class RenderPassAttachmentType : bool
    {
        Color,
        Depth
    };

    struct RenderPassAttachment
    {
        RenderPassAttachmentType type = RenderPassAttachmentType::Color;

        const Image& templateOutputImage;
        const Image* templateResolverImage = nullptr;

        RenderPassAttachmentLoadOperation loadOperation = RenderPassAttachmentLoadOperation::Clear;
        RenderPassAttachmentStoreOperation storeOperation = RenderPassAttachmentStoreOperation::Store;
    };

    struct SubpassDescription
    {
        std::span<const uint32> renderTargets = { };
        std::span<const uint32> inputs = { };
    };

    struct RenderPassCreateInfo
    {
        std::string_view name = "Render Pass";
        std::span<const RenderPassAttachment> attachments = { };
        std::span<const SubpassDescription> subpassDescriptions = { };
    };

    struct RenderPassBeginAttachment
    {
        const Image& outputImage;
        const Image* resolverImage = nullptr;
        Color32 clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };
    };

    class SIERRA_API RenderPass : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Resize(uint32 width, uint32 height);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetSubpassCount() const noexcept = 0;

        [[nodiscard]] uint32 GetAttachmentCount() const noexcept { return GetColorAttachmentCount() + HasDepthAttachment(); }
        [[nodiscard]] virtual uint32 GetColorAttachmentCount() const noexcept = 0;
        [[nodiscard]] virtual bool HasDepthAttachment() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;

        /* --- MOVE SEMANTICS --- */
        RenderPass(RenderPass&&) = delete;
        RenderPass& operator=(RenderPass&&) = delete;

        /* --- DESTRUCTOR --- */
        ~RenderPass() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit RenderPass(const RenderPassCreateInfo& createInfo);

    };
}