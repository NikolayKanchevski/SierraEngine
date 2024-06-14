//
// Created by Nikolay Kanchevski on 11.19.2023.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the Win32Context.h file is only allowed in Windows builds!"
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#pragma region Windows Defines // Some of these are not present in all Windows API variants, so we define them manually
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
    #if defined(CreateWindow)
        #undef CreateWindow
    #endif
#pragma endregion

#include "Win32Screen.h"

namespace Sierra
{

    struct Win32ContextCreateInfo
    {
        HINSTANCE hInstance = nullptr;
    };

    class SIERRA_API Win32Context final
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] HWND CreateWindow(std::string_view title, UINT width, UINT height, DWORD style, WNDPROC windowProc) const;
        void DestroyWindow(HWND window) const;

        [[nodiscard]] bool EventQueueEmpty(HWND window) const;
        MSG PollNextEvent(HWND window) const;
        MSG PeekNextEvent(HWND window) const;
        [[nodiscard]] bool IsEventFiltered(HWND, const UINT message, const WPARAM wParam, const LPARAM);

        void AdjustWindowRectForDPI(HWND window, RECT &rect) const;
        [[nodiscard]] bool IsWindowsVersionOrGreater(DWORD major, DWORD minor, WORD servicePack) const;

        void ReloadScreens();

        /* --- GETTER METHODS --- */
        [[nodiscard]] HINSTANCE GetHInstance() const { return hInstance; }

        [[nodiscard]] Win32Screen& GetPrimaryScreen();
        [[nodiscard]] Win32Screen& GetWindowScreen(HWND window);

        /* --- DESTRUCTOR --- */
        ~Win32Context();

    private:
        friend class WindowsContext;
        explicit Win32Context(const Win32ContextCreateInfo &createInfo);

        HINSTANCE hInstance;
        std::vector<Win32Screen> screens;

        HANDLE process;
        HICON processIcon;

        static BOOL CALLBACK EnumDisplayMonitorsProc(HMONITOR hMonitor, HDC hdc, LPRECT lrpcMonitor, LPARAM dwData);

    };

}
