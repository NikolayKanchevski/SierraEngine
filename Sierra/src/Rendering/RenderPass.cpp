//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "RenderPass.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    RenderPass::RenderPass(const RenderPassCreateInfo &createInfo)
    {
        SR_ERROR_IF(createInfo.attachments.size() == 0, "Cannot create render pass [{0}] with no attachments!", createInfo.name);
        SR_ERROR_IF(createInfo.subpassDescriptions.size() == 0, "Cannot create render pass [{0}] with no subpasses specified!", createInfo.name);
        #if SR_ENABLE_LOGGING
            const uint32 expectedWidth = createInfo.attachments.begin()->templateImage->GetWidth();
            const uint32 expectedHeight = createInfo.attachments.begin()->templateImage->GetHeight();
            for (uint32 i = 0; i < createInfo.subpassDescriptions.size(); i++)
            {
                for (const auto renderTargetIndex : (createInfo.subpassDescriptions.begin() + i)->renderTargets)
                {
                    const RenderPassAttachment &attachment = *(createInfo.attachments.begin() + renderTargetIndex);
                    SR_ERROR_IF(attachment.type == AttachmentType::Undefined, "Could not create render pass [{0}], as attachment type of attachment [{1}] with image [{2}] must not be [AttachmentType::Undefined]!", createInfo.name, renderTargetIndex, attachment.templateImage->GetName());
                    SR_ERROR_IF(attachment.templateImage->GetWidth() != expectedWidth || attachment.templateImage->GetHeight() != expectedHeight, "Could not create render pass [{0}], because not all attachments share the same dimensions, and they must!", createInfo.name);

                    const SubpassDescription &subpassDescription = *(createInfo.subpassDescriptions.begin() + i);
                    SR_ERROR_IF(std::find_if(subpassDescription.inputs.begin(), subpassDescription.inputs.end(), [&renderTargetIndex](const uint32 input) { return input == renderTargetIndex; }), "Could not create render pass [{0}], as attachment [{1}] is specified both as a render target and input attachment for subpass [{2}]! It must be either, but not both.", createInfo.name, renderTargetIndex, i);
                }
            }
        #endif

    }

}