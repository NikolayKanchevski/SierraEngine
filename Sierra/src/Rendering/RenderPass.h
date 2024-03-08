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
        const std::optional<std::reference_wrapper<const std::unique_ptr<Image>>> &resolveImage = std::nullopt;
        const std::unique_ptr<Image> &templateImage;
        RenderPassAttachmentType type = RenderPassAttachmentType::Color;
        RenderPassAttachmentLoadOperation loadOperation = RenderPassAttachmentLoadOperation::Clear;
        RenderPassAttachmentStoreOperation storeOperation = RenderPassAttachmentStoreOperation::Store;
    };

    struct SubpassDescription
    {
        const std::initializer_list<uint32> &renderTargets = { };
        const std::initializer_list<uint32> &inputs = { };
    };

    struct RenderPassCreateInfo
    {
        const std::string &name = "Render Pass";
        const std::initializer_list<RenderPassAttachment> &attachments = { };
        const std::initializer_list<SubpassDescription> &subpassDescriptions = { };
    };

    struct RenderPassBeginAttachment
    {
        const std::unique_ptr<Image> &image;
        const ColorRGBA32 &clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    };

    class SIERRA_API RenderPass : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Resize(uint32 width, uint32 height) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetColorAttachmentCount() const = 0;
        [[nodiscard]] virtual uint32 GetResolveAttachmentCount() const = 0;
        [[nodiscard]] virtual bool HasDepthAttachment() const = 0;

        [[nodiscard]] inline uint32 GetAttachmentCount() const { return GetColorAttachmentCount() + GetResolveAttachmentCount() + HasDepthAttachment(); }

        /* --- OPERATORS --- */
        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~RenderPass() = default;

    protected:
        explicit RenderPass(const RenderPassCreateInfo &createInfo);

    };
}