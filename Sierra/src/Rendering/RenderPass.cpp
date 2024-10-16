//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "RenderPass.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    RenderPass::RenderPass(const RenderPassCreateInfo& createInfo)
    {
        SR_THROW_IF(createInfo.name.empty(), InvalidValueError("Cannot create render pass, as specified name must not be empty"));
        SR_THROW_IF(createInfo.attachments.empty(), InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified attachments must not be empty", createInfo.name)));
        SR_THROW_IF(createInfo.subpassDescriptions.empty(), InvalidValueError(SR_FORMAT("Cannot create render pass [{0}], as specified subpass descriptions must not be empty", createInfo.name)));
    }

    /* --- POLLING METHODS --- */

    void RenderPass::Resize(const uint32 width, const uint32 height)
    {
        SR_THROW_IF(width <= 0, InvalidValueError(SR_FORMAT("Cannot resize render pass [{0}], as specified width must not be equal to [0]", GetName())));
        SR_THROW_IF(height <= 0, InvalidValueError(SR_FORMAT("Cannot resize render pass [{0}], as specified height must not be equal to [0]", GetName())));
    }

}