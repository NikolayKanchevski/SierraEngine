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
        const XRRScreenResources* screenResources = nullptr;
        const XRRCrtcInfo* crtcInfo = nullptr;
        const XRROutputInfo* outputInfo = nullptr;
        const Vector4UInt &workAreaExtents = nullptr;
    };

    class SIERRA_API X11Screen final : public Screen
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit X11Screen(const X11ScreenCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const override { return name; };
        [[nodiscard]] uint32 GetRefreshRate() const override { return refreshRate; };
        [[nodiscard]] ScreenOrientation GetOrientation() const override { return size.x >= size.y ? ScreenOrientation::Landscape : ScreenOrientation::Portrait; }

        [[nodiscard]] Vector2Int GetOrigin() const override { return origin; };
        [[nodiscard]] uint32 GetWidth() const override { return size.x; }
        [[nodiscard]] uint32 GetHeight() const override { return size.y; }

        [[nodiscard]] Vector2Int GetWorkAreaOrigin() const override { return workAreaOrigin; };
        [[nodiscard]] uint32 GetWorkAreaWidth() const override { return workAreaSize.x; }
        [[nodiscard]] uint32 GetWorkAreaHeight() const override { return workAreaSize.y; }

        /* --- MOVE SEMANTICS --- */
        X11Screen(X11Screen&& other);

    private:
        std::string name;
        Vector2Int origin = { 0, 0 };
        Vector2UInt size = { 0, 0 };
        Vector2Int workAreaOrigin = { 0, 0 };
        Vector2UInt workAreaSize = { 0, 0 };
        uint32 refreshRate = 0;

    };

}
