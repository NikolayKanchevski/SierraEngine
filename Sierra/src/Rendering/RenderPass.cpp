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

}