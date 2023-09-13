//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#include <dwmapi.h>
#include "WindowsInstance.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    WindowsInstance::WindowsInstance(const PlatformInstanceCreateInfo &createInfo)
        : PlatformInstance(createInfo), hInstance(GetModuleHandle(nullptr))
    {

    }

    /* --- GETTER METHODS --- */

    bool WindowsInstance::IsWindowsVersionOrGreater(const DWORD major, const DWORD minor, const DWORD servicePack) const
    {
        OSVERSIONINFOEX versionInfo;
        ZeroMemory(&versionInfo, sizeof(OSVERSIONINFOEX));

        versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        versionInfo.dwMajorVersion = major;          // Desired major version
        versionInfo.dwMinorVersion = minor;          // Desired minor version
        versionInfo.wServicePackMajor = servicePack; // Desired service pack major version

        DWORDLONG conditionMask = 0;
        VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
        VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
        VER_SET_CONDITION(conditionMask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

        return VerifyVersionInfo(&versionInfo, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, conditionMask);
    }

    /* --- DESTRUCTOR --- */

    WindowsInstance::~WindowsInstance()
    {

    }

}