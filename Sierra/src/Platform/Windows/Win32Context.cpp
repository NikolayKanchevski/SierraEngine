//
// Created by Nikolay Kanchevski on 11.19.2023.
//

#include "Win32Context.h"

#include <shellapi.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Win32Context::Win32Context(const Win32ContextCreateInfo& createInfo)
        : hInstance(createInfo.hInstance), process(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, GetCurrentProcessId()))
    {
        SR_THROW_IF(createInfo.hInstance == nullptr, InvalidValueError("Cannot create Win32 context, as specified hInstance must not be null"));

        CHAR executableName[MAX_PATH];
        DWORD binaryNameLength = sizeof(CHAR) * MAX_PATH;

        if (QueryFullProcessImageNameA(process, 0, executableName, &binaryNameLength))
        {
            processIcon = ExtractIconA(hInstance, executableName, 0);
        }
        else
        {
            SR_ERROR("Could not get the name of the application's Windows process!");
        }

        ReloadScreens();
    }

    /* --- POLLING METHODS --- */

    HWND Win32Context::CreateWindow(const std::string_view title, const UINT width, const UINT height, const DWORD style, WNDPROC windowProc) const
    {
        // Generate random class name
        char className[11 + 1];
        sprintf_s(className, "%i", RNG().Random<int32>());

        const WNDCLASS windowClass =
        {
            .lpfnWndProc = windowProc,
            .hInstance = hInstance,
            .hIcon = LoadIcon(nullptr, IDI_WINLOGO),
            .hCursor = LoadCursor(nullptr, IDC_ARROW),
            .hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
            .lpszClassName = className,
        };

        // Set up window class
        RegisterClass(&windowClass);

        // Set up window style
        const DWORD exStyle = WS_EX_APPWINDOW;

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
        HWND window = CreateWindowExA(
            exStyle, className,
            title.data(), style,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right - rect.left, rect.bottom - rect.top,
            nullptr, nullptr, hInstance, NULL
        );

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

    void Win32Context::ReloadScreens()
    {
        screens.clear();
        EnumDisplayMonitors(nullptr, nullptr, EnumDisplayMonitorsProc, reinterpret_cast<LPARAM>(this));
    }

    void Win32Context::AdjustWindowRectForDPI(HWND window, RECT& rect) const noexcept
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

    /* --- GETTER METHODS --- */

    bool Win32Context::IsWindowsVersionOrGreater(const DWORD major, const DWORD minor, const WORD servicePack) const noexcept
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

    Win32Screen& Win32Context::GetWindowScreen(HWND window)
    {
        return const_cast<Win32Screen&>(const_cast<const Win32Context*>(this)->GetWindowScreen(window));
    }

    const Win32Screen& Win32Context::GetWindowScreen(HWND window) const
    {
        HMONITOR windowMonitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
        return *std::ranges::find_if(screens, [windowMonitor](const Win32Screen& item) { return item.GetHMonitor() == windowMonitor; });
    }

    /* --- PRIVATE METHODS --- */

    BOOL Win32Context::EnumDisplayMonitorsProc(HMONITOR hMonitor, HDC hdc, LPRECT lrpcMonitor, const LPARAM dwData)
    {
        // Get passed context
        Win32Context* context = reinterpret_cast<Win32Context*>(dwData);
        context->screens.emplace_back(Win32Screen({ .hMonitor = hMonitor }));
        return TRUE;
    }

    /* --- PRIVATE METHODS --- */

    void Win32Context::Update()
    {
        MSG message = { };
        while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
        {
            switch (message.message)
            {
                case WM_DISPLAYCHANGE:
                {
                    ReloadScreens();
                    continue;
                }
                case WM_SYSCOMMAND:
                {
                    switch (message.wParam)
                    {
                        case SC_SCREENSAVE:
                        case SC_MONITORPOWER:       continue;
                        default:                    break;
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }

            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }

    /* --- DESTRUCTOR --- */

    Win32Context::~Win32Context() noexcept
    {
        CloseHandle(process);
    }

}