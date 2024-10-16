//
// Created by Nikolay Kanchevski on 11.25.2023.
//

#include "Win32Screen.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Win32Screen::Win32Screen(const Win32ScreenCreateInfo& createInfo)
        : hMonitor(createInfo.hMonitor)
    {
        // Allocate monitor info
        MONITORINFOEX monitorInfo = { };
        monitorInfo.cbSize = sizeof(MONITORINFOEX);

        // Get monitor info
        if (GetMonitorInfo(createInfo.hMonitor, &monitorInfo))
        {
            // Get screen dimensions
            origin = { monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top };
            size = { monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top };

            // Get work area dimensions
            workAreaSize = { monitorInfo.rcWork.right - monitorInfo.rcWork.left, monitorInfo.rcWork.bottom - monitorInfo.rcWork.top };
            workAreaOrigin = { monitorInfo.rcMonitor.left + (size.x - workAreaSize.x), monitorInfo.rcMonitor.top + (size.y - workAreaSize.y) };

            // Allocate display settings
            DEVMODE displaySettings
            {
                .dmSize = sizeof(DEVMODE),
                .dmDriverExtra = 0
            };

            // Retrieve display settings
            if (EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &displaySettings)) refreshRate = displaySettings.dmDisplayFrequency;

            // Allocate device info
            DISPLAY_DEVICEA displayDevice = { .cb = sizeof(DISPLAY_DEVICEA) };

            // Retrieve device info
            if (EnumDisplayDevicesA(monitorInfo.szDevice, 0, &displayDevice, 0))
            {
                name = displayDevice.DeviceString;
            }
        }
    }

}