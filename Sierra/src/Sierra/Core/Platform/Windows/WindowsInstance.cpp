//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#include "WindowsInstance.h"

#include <shellapi.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    WindowsInstance::WindowsInstance(const PlatformInstanceCreateInfo &createInfo)
        : PlatformInstance(createInfo), hInstance(GetModuleHandle(nullptr)), process(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, GetCurrentProcessId()))
    {
        // Get binary name
        CHAR executableName[MAX_PATH];
        DWORD binaryNameLength = sizeof(CHAR) * MAX_PATH;
        SR_ERROR_IF(!QueryFullProcessImageNameA(process, 0, executableName, &binaryNameLength), "Could not get the name of the application's Windows process!");

        // Load binary icon
        processIcon = ExtractIconA(hInstance, executableName, 0);
    }

    /* --- GETTER METHODS --- */

    bool WindowsInstance::IsWindowsVersionOrGreater(const DWORD major, const DWORD minor, const DWORD servicePack) const
    {
        OSVERSIONINFOEX versionInfo;
        ZeroMemory(&versionInfo, sizeof(OSVERSIONINFOEX));

        versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        versionInfo.dwMajorVersion = major;          // Desired major version
        versionInfo.dwMinorVersion = minor;          // Desired minor version
        versionInfo.wServicePackMajor = static_cast<WORD>(servicePack); // Desired service pack major version

        DWORDLONG conditionMask = 0;
        VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
        VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
        VER_SET_CONDITION(conditionMask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

        return VerifyVersionInfo(&versionInfo, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, conditionMask);
    }

    /* --- DESTRUCTOR --- */

    WindowsInstance::~WindowsInstance()
    {
        CloseHandle(process);
    }

}