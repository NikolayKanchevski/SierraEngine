//
// Created by Nikolay Kanchevski on 10.31.23.
//

#include "X11Screen.h"

#include <X11/extensions/Xrandr.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    X11Screen::X11Screen(const X11ScreenCreateInfo& createInfo)
        : Screen(), name(createInfo.outputInfo->name, createInfo.outputInfo->nameLen), origin(createInfo.crtcInfo->x, createInfo.crtcInfo->y)
    {
        // Save size, while taking rotation into account
        if (createInfo.crtcInfo->rotation & RR_Rotate_90 || createInfo.crtcInfo->rotation & RR_Rotate_270)
        {
            size.x = createInfo.crtcInfo->height;
            size.y = createInfo.crtcInfo->width;
        }
        else
        {
            size.x = createInfo.crtcInfo->width;
            size.y = createInfo.crtcInfo->height;
        }

        // Cut out system reserved area to get final work area
        workAreaSize = size;
        if (origin.x < createInfo.workAreaExtents.x)
        {
            workAreaSize.x -= createInfo.workAreaExtents.x - origin.x;
            origin.x = static_cast<int32>(createInfo.workAreaExtents.x);
        }

        if (origin.y < createInfo.workAreaExtents.y)
        {
            workAreaSize.y -= createInfo.workAreaExtents.y - origin.y;
            origin.y = static_cast<int32>(createInfo.workAreaExtents.y);
        }

        if (origin.x + workAreaSize.x > createInfo.workAreaExtents.x + createInfo.workAreaExtents.z)
        {
            workAreaSize.x = createInfo.workAreaExtents.x - origin.x + createInfo.workAreaExtents.z;
        }

        if (origin.y + workAreaSize.y > createInfo.workAreaExtents.y + createInfo.workAreaExtents.w)
        {
            workAreaSize.y = createInfo.workAreaExtents.y - origin.y + createInfo.workAreaExtents.w;
        }

        // Now get work origin
        workAreaOrigin.x += static_cast<int32>(size.x) - static_cast<int32>(workAreaSize.x);
        workAreaOrigin.y += static_cast<int32>(size.y) - static_cast<int32>(workAreaSize.y);

        // Get refresh rate
        for (size i = 0; i < createInfo.screenResources->nmode; i++)
        {
            const XRRModeInfo& mode_info = createInfo.screenResources->modes[i];
            if (mode_info.id == createInfo.crtcInfo->mode)
            {
                refreshRate = mode_info.dotClock / (mode_info.hTotal * mode_info.vTotal);
                break;
            }
        }
    }

    /* --- MOVE SEMANTICS --- */

    X11Screen::X11Screen(X11Screen&& other)
    : name(std::move(other.name)), origin(other.origin), size(other.size), workAreaOrigin(other.workAreaOrigin), workAreaSize(other.workAreaSize), refreshRate(other.refreshRate)
    {

    }

}