//
// Created by Nikolay Kanchevski on 10.12.23.
//

#pragma once

#include "RenderingResource.h"

#include "Image.h"

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
        const std::unique_ptr<Image> &image;
        AttachmentType type = AttachmentType::Undefined;
        AttachmentLoadOperation loadOperation = AttachmentLoadOperation::Clear;
        AttachmentStoreOperation storeOperation = AttachmentStoreOperation::Store;
    };

    struct SubpassDescription
    {
        const std::initializer_list<uint32> &inputs = { };
        const std::initializer_list<uint32> &renderTargets = { };
    };

    struct RenderPassCreateInfo
    {
        const std::string &name = "Render Pass";
        const std::vector<RenderPassAttachment> &attachments;
        const std::vector<SubpassDescription> &subpassDescriptions;
    };

    class SIERRA_API RenderPass : public virtual RenderingResource
    {
    public:
        /* --- OPERATORS --- */
        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;

    protected:
        explicit RenderPass(const RenderPassCreateInfo &createInfo);

    };
}