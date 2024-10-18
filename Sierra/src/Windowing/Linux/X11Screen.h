//
// Created by Nikolay Kanchevski on 10.31.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the X11Screen.h file is only allowed in Linux builds!"
#endif

#include "../Screen.h"

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

namespace Sierra
{

    struct X11ScreenCreateInfo
    {
        XRRScreenResources* screenResources = nullptr;
        XRRCrtcInfo* crtcInfo = nullptr;
        XRROutputInfo* outputInfo = nullptr;
        const Vector4UInt& workAreaExtents = { 0, 0, 0, 0 };
    };

    class SIERRA_API X11Screen final : public Screen
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit X11Screen(const X11ScreenCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }
        [[nodiscard]] ScreenOrientation GetOrientation() const noexcept override { return size.x >= size.y ? ScreenOrientation::Landscape : ScreenOrientation::Portrait; }
        [[nodiscard]] uint16 GetRefreshRate() const noexcept override { return refreshRate; }

        [[nodiscard]] Vector2Int GetOrigin() const noexcept override { return origin; }
        [[nodiscard]] uint32 GetWidth() const noexcept override { return size.x; }
        [[nodiscard]] uint32 GetHeight() const noexcept override { return size.y; }

        [[nodiscard]] Vector2Int GetWorkAreaOrigin() const noexcept override { return workAreaOrigin; }
        [[nodiscard]] uint32 GetWorkAreaWidth() const noexcept override { return workAreaSize.x; }
        [[nodiscard]] uint32 GetWorkAreaHeight() const noexcept override { return workAreaSize.y; }
        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override { return WindowingBackendType::X11; }

        /* --- COPY SEMANTICS --- */
        X11Screen(const X11Screen&) = delete;
        X11Screen& operator=(const X11Screen&) = delete;

        /* --- MOVE SEMANTICS --- */
        X11Screen(X11Screen&&) noexcept = default;
        X11Screen& operator=(X11Screen&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~X11Screen() noexcept override = default;

    private:
        std::string name;
        Vector2Int origin = { 0, 0 };
        Vector2UInt size = { 0, 0 };
        Vector2Int workAreaOrigin = { 0, 0 };
        Vector2UInt workAreaSize = { 0, 0 };
        uint16 refreshRate = 0;

    };

}
