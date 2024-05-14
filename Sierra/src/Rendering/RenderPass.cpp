//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "RenderPass.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    RenderPass::RenderPass(const RenderPassCreateInfo &createInfo)
    {
        SR_ERROR_IF(createInfo.attachments.empty(), "Cannot create render pass [{0}] with no attachments!", createInfo.name);
        SR_ERROR_IF(createInfo.subpassDescriptions.empty(), "Cannot create render pass [{0}] with no subpasses specified!", createInfo.name);
        #if SR_ENABLE_LOGGING
            const uint32 expectedWidth = createInfo.attachments.begin()->templateOutputImage->GetWidth();
            const uint32 expectedHeight = createInfo.attachments.begin()->templateOutputImage->GetHeight();
            for (uint32 i = 0; i < createInfo.subpassDescriptions.size(); i++)
            {
                for (const uint32 renderTargetIndex : createInfo.subpassDescriptions[i].renderTargets)
                {
                    const RenderPassAttachment &attachment = createInfo.attachments[renderTargetIndex];

                    SR_ERROR_IF(attachment.templateOutputImage == nullptr, "Cannot create render pass [{0}], as attachment [{1}]'s template output image must not be a null pointer!", createInfo.name, i);
                    SR_ERROR_IF(attachment.templateOutputImage->GetWidth() != expectedWidth || attachment.templateOutputImage->GetHeight() != expectedHeight, "Cannot create render pass [{0}], as attachment [{1}] does not share the same dimensions as the rest!", createInfo.name, renderTargetIndex);
                }
            }
        #endif

    }

}