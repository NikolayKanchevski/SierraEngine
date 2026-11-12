//
// Created by Nikolay Kanchevski on 11.12.26.
//

#include "WaylandScreen.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    WaylandScreen::WaylandScreen(const WaylandScreenCreateInfo& createInfo)
        : output(createInfo.output), registryName(createInfo.registryName)
    {
        SR_THROW_IF(createInfo.output == nullptr, InvalidValueError("Cannot create Wayland screen, as specified output must not be a null pointer"));
    }

    /* --- DESTRUCTOR --- */

    WaylandScreen::~WaylandScreen() noexcept
    {
        if (output == nullptr) return;

        if (wl_output_get_version(output) >= WL_OUTPUT_RELEASE_SINCE_VERSION)
        {
            wl_output_release(output);
        }
        else
        {
            wl_output_destroy(output);
        }
    }

}
