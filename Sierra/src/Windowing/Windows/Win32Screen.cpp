//
// Created by Nikolay Kanchevski on 11.25.2023.
//

#include "Win32Screen.h"

namespace Sierra
{

    namespace
    {
        [[nodiscard]] std::string WideStringToUTF8(const std::wstring_view string)
        {
            if (string.empty()) return { };

            const int byteCount = WideCharToMultiByte(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), nullptr, 0, nullptr, nullptr);
            if (byteCount <= 0) return { };

            std::string utf8String(static_cast<size>(byteCount), '\0');
            WideCharToMultiByte(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), utf8String.data(), byteCount, nullptr, nullptr);
            return utf8String;
        }

        [[nodiscard]] Vector2Int ToLogicalPosition(const LONG x, const LONG y, const float32 scaling) noexcept
        {
            return { static_cast<int32>(glm::round(static_cast<float32>(x) / scaling)), static_cast<int32>(glm::round(static_cast<float32>(y) / scaling)) };
        }

        [[nodiscard]] Vector2UInt ToLogicalSize(const LONG width, const LONG height, const float32 scaling) noexcept
        {
            return { static_cast<uint32>(glm::round(static_cast<float32>(glm::max(width, 0L)) / scaling)), static_cast<uint32>(glm::round(static_cast<float32>(glm::max(height, 0L)) / scaling)) };
        }
    }

    /* --- CONSTRUCTORS --- */

    Win32Screen::Win32Screen(const Win32ScreenCreateInfo& createInfo)
        : hMonitor(createInfo.hMonitor), scaling(createInfo.scaling > 0.0f ? createInfo.scaling : 1.0f)
    {
        MONITORINFOEXW monitorInfo = { };
        monitorInfo.cbSize = sizeof(MONITORINFOEXW);
        if (!GetMonitorInfoW(hMonitor, &monitorInfo)) return;

        primary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;

        // Windows reports monitors in physical pixels, while the desktop is laid out in logical units, which are the physical ones divided by the monitor's scale
        const RECT& monitorRect = monitorInfo.rcMonitor;
        origin = ToLogicalPosition(monitorRect.left, monitorRect.top, scaling);
        size = ToLogicalSize(monitorRect.right - monitorRect.left, monitorRect.bottom - monitorRect.top, scaling);

        // Work area is the part of the screen, that is not reserved by the desktop (such as for the taskbar)
        const RECT& workAreaRect = monitorInfo.rcWork;
        workAreaOrigin = ToLogicalPosition(workAreaRect.left, workAreaRect.top, scaling);
        workAreaSize = ToLogicalSize(workAreaRect.right - workAreaRect.left, workAreaRect.bottom - workAreaRect.top, scaling);

        // Get refresh rate
        DEVMODEW displaySettings = { };
        displaySettings.dmSize = sizeof(DEVMODEW);
        if (EnumDisplaySettingsW(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &displaySettings))
        {
            refreshRate = static_cast<uint16>(displaySettings.dmDisplayFrequency);
        }

        // Monitor attached to the display adapter's output carries the name of the device (the output itself is only named after the adapter)
        DISPLAY_DEVICEW displayDevice = { };
        displayDevice.cb = sizeof(DISPLAY_DEVICEW);
        if (EnumDisplayDevicesW(monitorInfo.szDevice, 0, &displayDevice, 0) && displayDevice.DeviceString[0] != L'\0')
        {
            name = WideStringToUTF8(displayDevice.DeviceString);
        }
        else
        {
            name = WideStringToUTF8(monitorInfo.szDevice);
        }
    }

}
