//
// Created by Nikolay Kanchevski on 10.12.23.
//

#pragma once

#include "RenderingResource.h"

#include "Image.h"
#include "CommandBuffer.h"

namespace Sierra
{

    enum class AttachmentLoadOperation : bool
    {
        Clear,
        Load
    };

    enum class AttachmentStoreOperation : bool
    {
        Store,
        Discard
    };

    enum class AttachmentType : uint8
    {
        Undefined      = 0x0000,
        Color          = 0x0001,
        Depth          = 0x0002
    };
    SR_DEFINE_ENUM_FLAG_OPERATORS(AttachmentType);

    struct RenderPassAttachment
    {
        const std::unique_ptr<Image> &templateImage;
        AttachmentType type = AttachmentType::Undefined;
        AttachmentLoadOperation loadOperation = AttachmentLoadOperation::Clear;
        AttachmentStoreOperation storeOperation = AttachmentStoreOperation::Store;
    };

    struct SubpassDescription
    {
        const std::initializer_list<uint32> &renderTargets = { };
        const std::initializer_list<uint32> &inputs = { };
    };

    struct RenderPassCreateInfo
    {
        const std::string &name = "Render Pass";
        const std::initializer_list<RenderPassAttachment> &attachments;
        const std::initializer_list<SubpassDescription> &subpassDescriptions;
    };

    struct RenderPassBeginAttachment
    {
        const std::unique_ptr<Image> &image;
        const Vector4 &clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    };

    class SIERRA_API RenderPass : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Resize(uint32 width, uint32 height) = 0;
        virtual void Begin(std::unique_ptr<CommandBuffer> &commandBuffer, const std::initializer_list<RenderPassBeginAttachment> &attachments) const = 0;
        virtual void BeginNextSubpass(std::unique_ptr<CommandBuffer> &commandBuffer) const = 0;
        virtual void End(std::unique_ptr<CommandBuffer> &commandBuffer) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetColorAttachmentCount() const = 0;
        [[nodiscard]] virtual bool HasDepthAttachment() const = 0;

        /* --- OPERATORS --- */
        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;

    protected:
        explicit RenderPass(const RenderPassCreateInfo &createInfo);

    };
}