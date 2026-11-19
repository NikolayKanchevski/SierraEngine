//
// Created by Nikolay Kanchevski on 11.19.2023.
//

#include "Win32Context.h"

#include "../../Windowing/Windows/Win32Window.h"

namespace Sierra
{

    namespace
    {
        constexpr const wchar_t* WINDOW_CLASS_NAME = L"SierraWindow";
        constexpr const wchar_t* HELPER_WINDOW_CLASS_NAME = L"SierraHelperWindow";

        // Name, which application icons are embedded into executables under (see Windows.cmake)
        constexpr const wchar_t* APPLICATION_ICON_RESOURCE_NAME = L"IDR_MAINFRAME";

        // Values from headers, which are not present in all Windows SDKs, and are thus defined manually
        constexpr int PROCESS_PER_MONITOR_DPI_AWARE_VALUE = 2;   // PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE
        constexpr int MDT_EFFECTIVE_DPI_VALUE = 0;               // MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI
        constexpr UINT SPI_SETLOGICALDPIOVERRIDE_VALUE = 0x009F; // Sent when the scale of the desktop gets changed

        // Exceptions must not travel through the frames of Windows, which call window procedures, so they are stored and thrown once dispatching messages is done
        thread_local std::exception_ptr pendingException = nullptr;

        template<typename T>
        [[nodiscard]] T LoadFunction(HMODULE module, const char* name) noexcept
        {
            if (module == nullptr) return nullptr;
            return reinterpret_cast<T>(reinterpret_cast<void*>(GetProcAddress(module, name)));
        }
    }

    /* --- CONSTRUCTORS --- */

    Win32Context::Win32Context(const Win32ContextCreateInfo& createInfo)
        : hInstance(createInfo.hInstance)
    {
        SR_THROW_IF(hInstance == nullptr, InvalidValueError("Cannot create Win32 context, as specified hInstance must not be null"));

        // Awareness of DPI has to be declared before any window is created, as Windows would otherwise stretch their contents, which blurs them
        LoadDPIFunctions();
        SetDPIAwareness();
        LoadIcons();

        // All windows share a single class, and the context routes their messages to the instances of the windows they concern
        const WNDCLASSEXW windowClassInfo
        {
            .cbSize = sizeof(WNDCLASSEXW),
            .style = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc = WindowProc,
            .hInstance = hInstance,
            .hIcon = icon,
            .hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512)), // IDC_ARROW
            .hbrBackground = nullptr, // Contents are drawn by the swapchain, so they are never erased
            .lpszClassName = WINDOW_CLASS_NAME,
            .hIconSm = smallIcon
        };

        windowClass = RegisterClassExW(&windowClassInfo);
        SR_THROW_IF(windowClass == 0, UnsupportedFeatureError("Cannot create Win32 context, as its window class could not be registered"));

        // Changes of displays are only broadcast to top-level windows, so an invisible one is kept, for screens to stay current, even while there are no other windows
        const WNDCLASSEXW helperWindowClassInfo
        {
            .cbSize = sizeof(WNDCLASSEXW),
            .lpfnWndProc = HelperWindowProc,
            .hInstance = hInstance,
            .lpszClassName = HELPER_WINDOW_CLASS_NAME
        };

        helperWindowClass = RegisterClassExW(&helperWindowClassInfo);
        if (helperWindowClass != 0)
        {
            helperWindow = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, HELPER_WINDOW_CLASS_NAME, L"Sierra Helper Window", WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 1, 1, nullptr, nullptr, hInstance, this);

            // The first ShowWindow() call of a process is ignored, when its parent has specified how it is to be shown, so that call is used up here, for windows to be shown as requested
            if (helperWindow != nullptr) ShowWindow(helperWindow, SW_HIDE);
        }

        ReloadScreens();
    }

    /* --- POLLING METHODS --- */

    void Win32Context::Update()
    {
        // PeekMessageW() returns immediately, once there are no messages left, so it never blocks
        MSG message = { };
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
        {
            // Quitting is up to the application, so there is nothing to dispatch
            if (message.message == WM_QUIT) continue;

            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        if (pendingException != nullptr) std::rethrow_exception(std::exchange(pendingException, nullptr));
    }

    void Win32Context::ReloadScreens() noexcept
    {
        screens.clear();
        EnumDisplayMonitors(nullptr, nullptr, EnumDisplayMonitorsProc, reinterpret_cast<LPARAM>(this));

        // Monitors are not enumerated in any particular order, so the primary one is moved to the front
        if (const auto primaryScreen = std::ranges::find_if(screens, [](const Win32Screen& screen) -> bool { return screen.IsPrimary(); }); primaryScreen != screens.end() && primaryScreen != screens.begin())
        {
            std::iter_swap(screens.begin(), primaryScreen);
        }

        // There is always a primary monitor (even if no display is connected), which is used, when enumeration yields none
        if (screens.empty())
        {
            const HMONITOR primaryMonitor = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
            screens.emplace_back(Win32ScreenCreateInfo { .hMonitor = primaryMonitor, .scaling = static_cast<float32>(GetMonitorDPI(primaryMonitor)) / USER_DEFAULT_SCREEN_DPI });
        }
    }

    void Win32Context::AdjustWindowRectForDPI(RECT& rect, const DWORD style, const DWORD exStyle, const UINT dpi) const noexcept
    {
        // Decorations are scaled by the DPI of the window's monitor, which only Windows 10 1607 and later can account for
        if (adjustWindowRectExForDpi != nullptr) adjustWindowRectExForDpi(&rect, style, FALSE, exStyle, dpi);
        else AdjustWindowRectEx(&rect, style, FALSE, exStyle);
    }

    /* --- GETTER METHODS --- */

    LPCWSTR Win32Context::GetWindowClassName() const noexcept
    {
        return WINDOW_CLASS_NAME;
    }

    UINT Win32Context::GetWindowDPI(HWND window) const noexcept
    {
        if (getDpiForWindow != nullptr)
        {
            if (const UINT dpi = getDpiForWindow(window); dpi != 0) return dpi;
        }

        return GetMonitorDPI(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST));
    }

    UINT Win32Context::GetMonitorDPI(HMONITOR monitor) const noexcept
    {
        // Monitors only have DPIs of their own as of Windows 8.1, before which the whole desktop has the same one
        if (getDpiForMonitor != nullptr)
        {
            UINT xDPI = 0;
            UINT yDPI = 0;
            if (SUCCEEDED(getDpiForMonitor(monitor, MDT_EFFECTIVE_DPI_VALUE, &xDPI, &yDPI)) && xDPI != 0) return xDPI;
        }

        UINT dpi = USER_DEFAULT_SCREEN_DPI;
        if (const HDC deviceContext = GetDC(nullptr); deviceContext != nullptr)
        {
            if (const int logicalPixels = GetDeviceCaps(deviceContext, LOGPIXELSX); logicalPixels > 0) dpi = static_cast<UINT>(logicalPixels);
            ReleaseDC(nullptr, deviceContext);
        }

        return dpi;
    }

    const Win32Screen& Win32Context::GetWindowScreen(HWND window) const noexcept
    {
        const HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
        for (const Win32Screen& screen : screens)
        {
            if (screen.GetHMonitor() == monitor) return screen;
        }

        // Screens are never empty, as ReloadScreens() always provides at least one
        return screens[0];
    }

    /* --- CONVERSIONS --- */

    std::wstring Win32Context::UTF8ToWideString(const std::string_view string)
    {
        if (string.empty()) return { };

        const int characterCount = MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), nullptr, 0);
        if (characterCount <= 0) return { };

        std::wstring wideString(static_cast<size>(characterCount), L'\0');
        MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), wideString.data(), characterCount);

        return wideString;
    }

    std::string Win32Context::WideStringToUTF8(const std::wstring_view string)
    {
        if (string.empty()) return { };

        const int byteCount = WideCharToMultiByte(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), nullptr, 0, nullptr, nullptr);
        if (byteCount <= 0) return { };

        std::string utf8String(static_cast<size>(byteCount), '\0');
        WideCharToMultiByte(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), utf8String.data(), byteCount, nullptr, nullptr);

        return utf8String;
    }

    /* --- DESTRUCTOR --- */

    Win32Context::~Win32Context() noexcept
    {
        if (helperWindow != nullptr)
        {
            SetWindowLongPtrW(helperWindow, GWLP_USERDATA, 0);
            DestroyWindow(helperWindow);
        }

        if (helperWindowClass != 0) UnregisterClassW(HELPER_WINDOW_CLASS_NAME, hInstance);
        if (windowClass != 0) UnregisterClassW(WINDOW_CLASS_NAME, hInstance);

        // NOTE: Icons are shared resources (loaded with LR_SHARED), which must not be destroyed
        if (shcore != nullptr) FreeLibrary(shcore);
    }

    /* --- PRIVATE METHODS --- */

    void Win32Context::LoadIcons() noexcept
    {
        // Applications embed their icon into their executable, and windows without one are given the default icon by Windows
        // icon = static_cast<HICON>(LoadImageW(hInstance, APPLICATION_ICON_RESOURCE_NAME, IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR | LR_SHARED));
        // smallIcon = static_cast<HICON>(LoadImageW(hInstance, APPLICATION_ICON_RESOURCE_NAME, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR | LR_SHARED));
    }

    void Win32Context::LoadDPIFunctions() noexcept
    {
        // NOTE: User32 is always loaded into Windows processes, so it does not need to be loaded (and freed) manually
        user32 = GetModuleHandleW(L"user32.dll");
        shcore = LoadLibraryExW(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

        getDpiForWindow = LoadFunction<decltype(getDpiForWindow)>(user32, "GetDpiForWindow");
        adjustWindowRectExForDpi = LoadFunction<decltype(adjustWindowRectExForDpi)>(user32, "AdjustWindowRectExForDpi");
        getDpiForMonitor = LoadFunction<decltype(getDpiForMonitor)>(shcore, "GetDpiForMonitor");
    }

    void Win32Context::SetDPIAwareness() const noexcept
    {
        // Each window is scaled by the DPI of the monitor it is on (and rescaled when moved to another), which Windows 10 1703 and later fully support. Older versions only support parts of that, or a single DPI for the whole desktop
        // NOTE: Awareness may have already been declared (such as by the executable's manifest), in which case it cannot be changed, and these calls all fail
        if (const auto setProcessDpiAwarenessContext = LoadFunction<BOOL(WINAPI*)(HANDLE)>(user32, "SetProcessDpiAwarenessContext"); setProcessDpiAwarenessContext != nullptr)
        {
            const HANDLE perMonitorAwareV2 = reinterpret_cast<HANDLE>(static_cast<LONG_PTR>(-4)); // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
            const HANDLE perMonitorAware = reinterpret_cast<HANDLE>(static_cast<LONG_PTR>(-3));   // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE
            if (setProcessDpiAwarenessContext(perMonitorAwareV2) || setProcessDpiAwarenessContext(perMonitorAware)) return;
        }

        if (const auto setProcessDpiAwareness = LoadFunction<HRESULT(WINAPI*)(int)>(shcore, "SetProcessDpiAwareness"); setProcessDpiAwareness != nullptr)
        {
            if (SUCCEEDED(setProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE_VALUE))) return;
        }

        SetProcessDPIAware();
    }

    LRESULT CALLBACK Win32Context::WindowProc(HWND window, const UINT message, const WPARAM wParam, const LPARAM lParam)
    {
        // Instances of windows are only attached to them, once fully constructed, so messages, which are sent while creating them, are handled by default
        Win32Window* const win32Window = reinterpret_cast<Win32Window*>(GetWindowLongPtrW(window, GWLP_USERDATA));
        if (win32Window == nullptr) return DefWindowProcW(window, message, wParam, lParam);

        return win32Window->HandleWin32Message(message, wParam, lParam);
    }

    LRESULT CALLBACK Win32Context::HelperWindowProc(HWND window, const UINT message, const WPARAM wParam, const LPARAM lParam)
    {
        switch (message)
        {
            case WM_NCCREATE:
            {
                // Context is passed on creation, and is attached to the window, so that it can be retrieved, when monitors change
                const CREATESTRUCTW* const createStruct = reinterpret_cast<const CREATESTRUCTW*>(lParam);
                SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
                break;
            }
            case WM_SETTINGCHANGE:
            {
                // Work area changes (such as the taskbar being moved or resized), as do logical sizes of monitors, when the scale of the desktop does
                if (wParam != SPI_SETWORKAREA && wParam != SPI_SETLOGICALDPIOVERRIDE_VALUE) break;
                [[fallthrough]];
            }
            case WM_DISPLAYCHANGE:
            {
                if (Win32Context* const context = reinterpret_cast<Win32Context*>(GetWindowLongPtrW(window, GWLP_USERDATA)); context != nullptr) context->ReloadScreens();
                break;
            }
            default:
            {
                break;
            }
        }

        return DefWindowProcW(window, message, wParam, lParam);
    }

    BOOL CALLBACK Win32Context::EnumDisplayMonitorsProc(HMONITOR monitor, HDC, LPRECT, const LPARAM data)
    {
        Win32Context* const context = reinterpret_cast<Win32Context*>(data);
        context->screens.emplace_back(Win32ScreenCreateInfo { .hMonitor = monitor, .scaling = static_cast<float32>(context->GetMonitorDPI(monitor)) / USER_DEFAULT_SCREEN_DPI });
        return TRUE;
    }

}
