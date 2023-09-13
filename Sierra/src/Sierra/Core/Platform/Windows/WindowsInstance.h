//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#pragma once

#if !PLATFORM_WINDOWS
    #error "Including the WindowsInstance.h file is only allowed in Windows builds!"
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#pragma region Windows Defines
    // NOTE: Some of these are not present in all Windows API variants, so we define them manually
    #if !defined(WM_MOUSEHWHEEL)
        #define WM_MOUSEHWHEEL 0x020E
    #endif
    #if !defined(WM_DWMCOMPOSITIONCHANGED)
        #define WM_DWMCOMPOSITIONCHANGED 0x031E
    #endif
    #if !defined(WM_DWMCOLORIZATIONCOLORCHANGED)
        #define WM_DWMCOLORIZATIONCOLORCHANGED 0x0320
    #endif
    #if !defined(WM_COPYGLOBALDATA)
        #define WM_COPYGLOBALDATA 0x0049
    #endif
    #if !defined(WM_UNICHAR)
        #define WM_UNICHAR 0x0109
    #endif
    #if !defined(UNICODE_NOCHAR)
        #define UNICODE_NOCHAR 0xFFFF
    #endif
    #if !defined(WM_DPICHANGED)
        #define WM_DPICHANGED 0x02E0
    #endif
    #if !defined(GET_XBUTTON_WPARAM)
        #define GET_XBUTTON_WPARAM(w) (HIWORD(w))
    #endif
    #if !defined(EDS_ROTATEDMODE)
        #define EDS_ROTATEDMODE 0x00000004
    #endif
    #if !defined(DISPLAY_DEVICE_ACTIVE)
        #define DISPLAY_DEVICE_ACTIVE 0x00000001
    #endif
    #if !defined(_WIN32_WINNT_WINBLUE)
        #define _WIN32_WINNT_WINBLUE 0x0603
    #endif
    #if !defined(_WIN32_WINNT_WIN8)
        #define _WIN32_WINNT_WIN8 0x0602
    #endif
    #if !defined(WM_GETDPISCALEDSIZE)
        #define WM_GETDPISCALEDSIZE 0x02e4
    #endif
    #if !defined(USER_DEFAULT_SCREEN_DPI)
        #define USER_DEFAULT_SCREEN_DPI 96
    #endif
    #if !defined(OCR_HAND)
        #define OCR_HAND 32649
    #endif
#pragma endregion

#include "../../PlatformInstance.h"

namespace Sierra
{

    class SIERRA_API WindowsInstance final : public PlatformInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        WindowsInstance(const PlatformInstanceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline HINSTANCE GetHInstance() const { return hInstance; }
        [[nodiscard]] bool IsWindowsVersionOrGreater(DWORD major, DWORD minor, DWORD servicePack) const;
        [[nodiscard]] inline PlatformType GetType() const override { return PlatformType::Windows; }

        /* --- DESTRUCTOR --- */
        ~WindowsInstance();

    private:
        HINSTANCE hInstance;

    };

}
