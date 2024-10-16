//
// Created by Nikolay Kanchevski on 25.09.24.
//

#include "SwapchainEvents.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    SwapchainResizeEvent::SwapchainResizeEvent(const uint32 width, const uint32 height, const uint32 scaling) noexcept
        : scaling(scaling), width(width), height(height)
    {

    }

}