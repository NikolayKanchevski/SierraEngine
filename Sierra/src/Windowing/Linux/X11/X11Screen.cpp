//
// Created by Nikolay Kanchevski on 10.31.23.
//

#include "X11Screen.h"

#include <X11/extensions/Xrandr.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    X11Screen::X11Screen(const X11ScreenCreateInfo& createInfo)
        : name(createInfo.outputInfo->name, createInfo.outputInfo->nameLen), origin(createInfo.crtcInfo->x, createInfo.crtcInfo->y)
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

        // Work area is the part of the screen, that is not reserved by the desktop (for panels and the like), and is reported as an area of the whole desktop. Without such report, the whole screen is usable
        const Vector4UInt& desktopArea = createInfo.workAreaExtents; // [ X | Y | Width | Height ]
        if (desktopArea.z == 0 || desktopArea.w == 0)
        {
            workAreaOrigin = origin;
            workAreaSize = size;
        }
        else
        {
            const int32 left = glm::max(origin.x, static_cast<int32>(desktopArea.x));
            const int32 top = glm::max(origin.y, static_cast<int32>(desktopArea.y));
            const int32 right = glm::min(origin.x + static_cast<int32>(size.x), static_cast<int32>(desktopArea.x + desktopArea.z));
            const int32 bottom = glm::min(origin.y + static_cast<int32>(size.y), static_cast<int32>(desktopArea.y + desktopArea.w));

            workAreaOrigin = { left, top };
            workAreaSize = { static_cast<uint32>(glm::max(right - left, 0)), static_cast<uint32>(glm::max(bottom - top, 0)) };
        }

        // Get refresh rate
        for (int i = 0; i < createInfo.screenResources->nmode; i++)
        {
            const XRRModeInfo& mode_info = createInfo.screenResources->modes[i];
            if (mode_info.id == createInfo.crtcInfo->mode)
            {
                // Some modes (such as those of virtual displays) carry no timings, from which no refresh rate can be computed
                const unsigned long totalPixelCount = static_cast<unsigned long>(mode_info.hTotal) * static_cast<unsigned long>(mode_info.vTotal);
                if (totalPixelCount > 0) refreshRate = static_cast<uint16>(mode_info.dotClock / totalPixelCount);
                break;
            }
        }

        // Desktop is laid out in logical units, which are the physical ones divided by its scale
        const float32 scaling = createInfo.scaling;
        origin = { static_cast<int32>(glm::round(origin.x / scaling)), static_cast<int32>(glm::round(origin.y / scaling)) };
        size = { static_cast<uint32>(glm::round(size.x / scaling)), static_cast<uint32>(glm::round(size.y / scaling)) };
        workAreaOrigin = { static_cast<int32>(glm::round(workAreaOrigin.x / scaling)), static_cast<int32>(glm::round(workAreaOrigin.y / scaling)) };
        workAreaSize = { static_cast<uint32>(glm::round(workAreaSize.x / scaling)), static_cast<uint32>(glm::round(workAreaSize.y / scaling)) };
    }

}