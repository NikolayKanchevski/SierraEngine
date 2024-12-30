//
// Created by Nikolay Kanchevski on 27.12.24.
//

#include "MetalFramebuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalFramebuffer::MetalFramebuffer(const MetalDevice& device, const FramebufferCreateInfo& createInfo)
        : Framebuffer(createInfo), name(createInfo.name), attachments(std::vector(createInfo.attachments.begin(), createInfo.attachments.end()))
    {
        for (size i = 0; i < attachments.size(); i++)
        {
            const FramebufferAttachment& attachment = attachments[i];
            SR_THROW_IF(attachment.image.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot create framebuffer [{0}] using image [{1}] for attachment [{2}], as its backend type differs from [RenderingBackendType::Metal]", name, attachment.image.GetName(), i)));
        }
    }

}