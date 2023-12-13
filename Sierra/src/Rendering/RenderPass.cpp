//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "RenderPass.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    RenderPass::RenderPass(const RenderPassCreateInfo &createInfo)
    {
        #if SR_ENABLE_LOGGING
            const uint32 expectedWidth = createInfo.attachments[0].image->GetWidth();
            const uint32 expectedHeight = createInfo.attachments[0].image->GetHeight();
            for (uint32 i = 0; i < createInfo.subpassDescriptions.size(); i++)
            {
                for (const auto renderTargetIndex : createInfo.subpassDescriptions[i].renderTargets)
                {
                    SR_ERROR_IF(createInfo.attachments[renderTargetIndex].type == AttachmentType::Undefined, "Could not create render pass [{0}], as attachment type of attachment [{1}] with image [{2}] must not be [AttachmentType::Undefined]!", createInfo.name, renderTargetIndex, createInfo.attachments[renderTargetIndex].image->GetName());
                    SR_ERROR_IF(createInfo.attachments[renderTargetIndex].image->GetWidth() != expectedWidth || createInfo.attachments[renderTargetIndex].image->GetHeight() != expectedHeight, "Could not create render pass [{0}], because not all attachments share the same dimensions, and they must!", createInfo.name);
                    SR_ERROR_IF(std::find_if(createInfo.subpassDescriptions[i].inputs.begin(), createInfo.subpassDescriptions[i].inputs.end(), [&renderTargetIndex](const uint32 input) { return input == renderTargetIndex; }), "Could not create render pass [{0}], as attachment [{1}] is specified both as a render target and input attachment for subpass [{2}]! It must be either, but not both.", createInfo.name, renderTargetIndex, i);
                }
            }
        #endif

    }

}