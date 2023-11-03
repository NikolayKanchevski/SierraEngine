//
// Created by Nikolay Kanchevski on 10.31.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the X11Screen.h file is only allowed in Linux builds!"
#endif

#include "../../Screen.h"

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

namespace Sierra
{

    struct X11ScreenCreateInfo
    {
        const XRRScreenResources* screenResources;
        const XRRCrtcInfo* crtcInfo;
        const XRROutputInfo* outputInfo;
        const Vector4UInt &workAreaExtents;
    };

    class SIERRA_API X11Screen final : public Screen
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit X11Screen(X11Screen&& other);
        explicit X11Screen(const X11ScreenCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        inline String GetName() const override { return name; };
        inline Vector2Int GetOrigin() const override { return origin; };
        inline Vector2UInt GetSize() const override { return size; };
        inline Vector2Int GetWorkAreaOrigin() const override { return workAreaOrigin; };
        inline Vector2UInt GetWorkAreaSize() const override { return workAreaSize; };
        inline int32 GetRefreshRate() const override { return refreshRate; };

    private:
        String name;
        Vector2Int origin = { 0, 0 };
        Vector2UInt size = { 0, 0 };
        Vector2Int workAreaOrigin = { 0, 0 };
        Vector2UInt workAreaSize = { 0, 0 };
        int32 refreshRate = 0;

    };

}
