//
// Created by Nikolay Kanchevski on 11.25.2023.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the Win32Screen.h file is only allowed in Windows builds!"
#endif

#include "../../Screen.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Sierra
{

    struct Win32ScreenCreateInfo
    {
        HMONITOR hMonitor = nullptr;
    };

    class SIERRA_API Win32Screen final : public Screen
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Win32Screen(const Win32ScreenCreateInfo &createInfo);

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
        Win32Screen(Win32Screen&& other);

    private:
        std::string name;
        Vector2Int origin = { 0, 0 };
        Vector2UInt size = { 0, 0 };
        Vector2Int workAreaOrigin = { 0, 0 };
        Vector2UInt workAreaSize = { 0, 0 };
        uint32 refreshRate = 0;

        // On MSVC, only a move constructor is not enough to allow Win32Context to hold a std::vector<Win32Screen>
        friend class Win32Context;
        explicit Win32Screen(Win32Screen& other);
        Win32Screen& operator=(const Win32Screen &other);

    };

}
