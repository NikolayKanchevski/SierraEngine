//
// Created by Nikolay Kanchevski on 13.11.24.
//

#include "Framebuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Framebuffer::Framebuffer(const FramebufferCreateInfo& createInfo)
    {
        SR_THROW_IF(createInfo.name.empty(), InvalidValueError("Cannot create framebuffer, as specified name must not be empty"));
        SR_THROW_IF(createInfo.width == 0, InvalidValueError("Cannot create framebuffer, as specified width must not be [0]"));
        SR_THROW_IF(createInfo.height == 0, InvalidValueError("Cannot create framebuffer, as specified height must not be [0]"));
        SR_THROW_IF(createInfo.attachments.empty(), InvalidValueError("Cannot create framebuffer, as specified attachments must not be empty"));
    }

}