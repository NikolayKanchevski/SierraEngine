//
// Created by Nikolay Kanchevski on 11.25.2023.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the Win32Screen.h file is only allowed in Windows builds!"
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../Screen.h"

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
        explicit Win32Screen(const Win32ScreenCreateInfo& createInfo);

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
        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override { return WindowingBackendType::Win32; }

        [[nodiscard]] HMONITOR GetHMonitor() const { return hMonitor; }

        /* --- COPY SEMANTICS --- */
        Win32Screen(const Win32Screen&) = delete;
        Win32Screen& operator=(const Win32Screen&) = delete;

        /* --- MOVE SEMANTICS --- */
        Win32Screen(Win32Screen&&) noexcept = default;
        Win32Screen& operator=(Win32Screen&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~Win32Screen() noexcept override = default;

    private:
        HMONITOR hMonitor = nullptr;

        std::string name = "Unknown";
        uint16 refreshRate = 0;

        Vector2Int origin = { 0, 0 };
        Vector2UInt size = { 0, 0 };
        Vector2Int workAreaOrigin = { 0, 0 };
        Vector2UInt workAreaSize = { 0, 0 };

    };

}
