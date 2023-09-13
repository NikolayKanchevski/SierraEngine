//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#include <dwmapi.h>
#include "Win32Instance.h"

#include "../../../Engine/RNG.h"

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

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Win32Instance::Win32Instance()
        : hInstance(GetModuleHandle(nullptr))
    {

    }

    UniquePtr<Win32Instance> Win32Instance::Create()
    {
        return std::make_unique<Win32Instance>();
    }

    /* --- POLLING METHODS --- */

    HWND Win32Instance::CreateWindow(LPCSTR title, LONG xPosition, LONG yPosition, ulong width, ulong height, bool resizable, bool maximize, const WNDPROC &proc)
    {
        // Generate random class name
        char className[11 + 1];
        sprintf_s(className, "%i", RNG().Random<int32>());

        // Set up window class
        WNDCLASS windowClass{};
        windowClass.lpszClassName = className;
        windowClass.hInstance = hInstance;
        windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.lpfnWndProc = proc;
        windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        RegisterClass(&windowClass);

        // Set up window style
        DWORD exStyle = WS_EX_APPWINDOW;
        DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | (resizable ? (WS_SIZEBOX | WS_MAXIMIZEBOX) : 0) | (maximize ? WS_MAXIMIZE : 0);

        // Create window size rect
        RECT rect;
        rect.left = xPosition;
        rect.top = yPosition;
        rect.right = width;
        rect.bottom = height;
        AdjustWindowRectEx(&rect, style, FALSE, exStyle);

        // Create window
        HWND window = CreateWindowEx(
            exStyle, className,
            title, style,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right - rect.left, rect.bottom - rect.top,
            NULL, NULL, hInstance, NULL
        );
        SR_ERROR_IF(window == nullptr, "Could not create Windows window!");

        // Enable Windows >= 7 events
        if (IsWindows7OrGreater())
        {
            ChangeWindowMessageFilterEx(window, WM_DROPFILES, MSGFLT_ALLOW, NULL);
            ChangeWindowMessageFilterEx(window, WM_COPYDATA, MSGFLT_ALLOW, NULL);
            ChangeWindowMessageFilterEx(window, WM_COPYGLOBALDATA, MSGFLT_ALLOW, NULL);
        }

        // Adjust window rect to account for DPI scaling
        if (IsWindows10Version1607OrGreater())
        {
            AdjustWindowRectExForDpi(&rect, style, FALSE, exStyle, GetDpiForWindow(window));
        }
        else
        {
            AdjustWindowRectEx(&rect, style, FALSE, exStyle);
        }

        // Get window rect
        WINDOWPLACEMENT windowPlacement(sizeof(windowPlacement));
        GetWindowPlacement(window, &windowPlacement);
        OffsetRect(&rect, windowPlacement.rcNormalPosition.left - rect.left, windowPlacement.rcNormalPosition.top - rect.top);

        // Re-place window and hide it
        windowPlacement.rcNormalPosition = rect;
        windowPlacement.showCmd = SW_HIDE;
        SetWindowPlacement(window, &windowPlacement);

        return window;
    }

    void Win32Instance::CloseWindow(const HWND window)
    {
        ::CloseWindow(window);

        char className[11 + 1];
        GetClassName(window, className, sizeof(className));
        UnregisterClass(className, hInstance);

        SetWindowLongPtr(window, GWLP_USERDATA, 0);
        DestroyWindow(window);
    }

    /* --- GETTER METHODS --- */

    DWORD Win32Instance::GetWindowStyle(const HWND window)
    {
        return static_cast<DWORD>(GetWindowLong(window, GWL_STYLE));
    }

    bool Win32Instance::IsWindowsVersionOrGreater(const DWORD major, const DWORD minor, const DWORD servicePack) const
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

    Win32Instance::~Win32Instance()
    {

    }

}