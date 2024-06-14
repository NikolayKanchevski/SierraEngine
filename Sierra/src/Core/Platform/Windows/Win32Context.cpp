//
// Created by Nikolay Kanchevski on 11.19.2023.
//

#include "Win32Context.h"

#include <shellapi.h>
#include "../../../Utilities/RNG.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Win32Context::Win32Context(const Win32ContextCreateInfo &createInfo)
        : hInstance(createInfo.hInstance), process(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, GetCurrentProcessId()))
    {
        SR_ERROR_IF(createInfo.hInstance == nullptr, "HINSTANCE pointer passed upon creation of Win32Context must not be null!");

        // Get binary name
        CHAR executableName[MAX_PATH];
        DWORD binaryNameLength = sizeof(CHAR) * MAX_PATH;
        SR_ERROR_IF(!QueryFullProcessImageNameA(process, 0, executableName, &binaryNameLength), "Could not get the name of the application's Windows process!");

        // Load binary icon
        processIcon = ExtractIconA(hInstance, executableName, 0);

        // Detect screens
        ReloadScreens();
    }

    /* --- POLLING METHODS --- */

    HWND Win32Context::CreateWindow(const std::string_view title, const UINT width, const UINT height, const DWORD style, WNDPROC windowProc) const
    {
        // Generate random class name
        char className[11 + 1];
        sprintf_s(className, "%i", RNG().Random<int32>());

        // Set up window class
        WNDCLASS windowClass{ };
        windowClass.lpszClassName = className;
        windowClass.hInstance = hInstance;
        windowClass.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
        windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        windowClass.lpfnWndProc = windowProc;
        windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        RegisterClass(&windowClass);

        // Set up window style
        DWORD exStyle = WS_EX_APPWINDOW;

        // Create window size rect
        RECT rect
        {
            .left = 0,
            .top = 0,
            .right = static_cast<LONG>(width),
            .bottom = static_cast<LONG>(height)
        };
        AdjustWindowRectEx(&rect, style, FALSE, exStyle);

        // Create window
        HWND window = CreateWindowEx(
            exStyle, className,
            title.data(), style,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right - rect.left, rect.bottom - rect.top,
            nullptr, nullptr, hInstance, NULL
        );
        SR_ERROR_IF(window == nullptr, "Could not create Win32 window!");

        // Account for DPI scaling
        AdjustWindowRectForDPI(window, rect);

        // Enable Windows >= 7 events
        if (IsWindowsVersionOrGreater(6, 1, 0))
        {
            ChangeWindowMessageFilterEx(window, WM_DROPFILES, MSGFLT_ALLOW, nullptr);
            ChangeWindowMessageFilterEx(window, WM_COPYDATA, MSGFLT_ALLOW, nullptr);
            ChangeWindowMessageFilterEx(window, WM_COPYGLOBALDATA, MSGFLT_ALLOW, nullptr);
        }

        // Set window's icon to that of the binary
        SendMessage(window, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(processIcon));

        return window;
    }

    void Win32Context::DestroyWindow(HWND window) const
    {
        CloseWindow(window);

        char className[11 + 1];
        GetClassName(window, className, sizeof(className));
        UnregisterClass(className, hInstance);
        SetWindowLongPtr(window, GWLP_USERDATA, 0);

        ::DestroyWindow(window);
    }

    bool Win32Context::EventQueueEmpty(HWND window) const
    {
        MSG message = { };
        return !PeekMessage(&message, window, 0, 0, PM_NOREMOVE);
    }

    MSG Win32Context::PollNextEvent(HWND window) const
    {
        MSG message = { };
        if (PeekMessage(&message, window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        return message;
    }

    MSG Win32Context::PeekNextEvent(HWND window) const
    {
        MSG message = { };
        PeekMessage(&message, window, 0, 0, PM_NOREMOVE);
        return message;
    }

    bool Win32Context::IsEventFiltered(HWND, const UINT message, const WPARAM wParam, const LPARAM)
    {
        switch (message)
        {
            case WM_DISPLAYCHANGE:
            {
                ReloadScreens();
                return true;
            }
            case WM_SYSCOMMAND:
            {
                switch (wParam)
                {
                    case SC_SCREENSAVE:
                    case SC_MONITORPOWER:
                    {
                        return true;
                    }
                    default:
                    {
                        break;
                    }
                }
                break;
            }
            default:
            {
                break;
            }
        }

        return false;
    }

    void Win32Context::AdjustWindowRectForDPI(HWND window, RECT &rect) const
    {
        if (IsWindowsVersionOrGreater(10, 0, 14393)) // Windows 10 1607
        {
            AdjustWindowRectExForDpi(&rect, GetWindowLong(window, GWL_STYLE), FALSE, GetWindowLong(window, GWL_STYLE), GetDpiForWindow(window));
        }
        else
        {
            AdjustWindowRectEx(&rect, GetWindowLong(window, GWL_STYLE), FALSE, GetWindowLong(window, GWL_STYLE));
        }
    }

    bool Win32Context::IsWindowsVersionOrGreater(const DWORD major, const DWORD minor, const WORD servicePack) const
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

    /* --- GETTER METHODS --- */

    Win32Screen& Win32Context::GetPrimaryScreen()
    {
        return screens[0];
    }

    Win32Screen& Win32Context::GetWindowScreen(HWND window)
    {
        HMONITOR windowMonitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
        return *std::find_if(screens.begin(), screens.end(), [windowMonitor](const Win32Screen &item) { return item.GetHMonitor() == windowMonitor; });
    }

    /* --- PRIVATE METHODS --- */

    void Win32Context::ReloadScreens()
    {
        EnumDisplayMonitors(nullptr, nullptr, EnumDisplayMonitorsProc, reinterpret_cast<LPARAM>(this));
    }

    BOOL Win32Context::EnumDisplayMonitorsProc(HMONITOR hMonitor, HDC hdc, LPRECT lrpcMonitor, const LPARAM dwData)
    {
        // Get passed context
        Win32Context* context = reinterpret_cast<Win32Context*>(dwData);
        context->screens.emplace_back(Win32Screen({ .hMonitor = hMonitor }));
        return TRUE;
    }

    /* --- DESTRUCTOR --- */

    Win32Context::~Win32Context()
    {
        CloseHandle(process);
    }

}