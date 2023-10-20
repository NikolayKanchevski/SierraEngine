//
// Created by Nikolay Kanchevski on 8.21.2023.
//

#include "Win32Window.h"

#include "WindowsInstance.h"
#include "../../../Engine/RNG.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Win32Window::Win32Window(const WindowCreateInfo &createInfo)
        : Window(createInfo), windowsInstance(*static_cast<WindowsInstance*>(createInfo.platformInstance.get())), inputManager(Win32InputManager({ })), cursorManager(Win32CursorManager({ })), title(createInfo.title)
    {
        SR_ERROR_IF(createInfo.platformInstance->GetType() !=+ PlatformType::Windows, "Cannot create Win32 window using a platform instance of type [{0}]!", createInfo.platformInstance->GetType()._to_string());

        // Generate random class name
        char className[11 + 1];
        sprintf_s(className, "%i", RNG().Random<int32>());

        // Set up window class
        WNDCLASS windowClass{};
        windowClass.lpszClassName = className;
        windowClass.hInstance = windowsInstance.GetHInstance();
        windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.lpfnWndProc = Win32Window::WindowProc;
        windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        RegisterClass(&windowClass);

        // Set up window style
        DWORD exStyle = WS_EX_APPWINDOW;
        DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | (createInfo.resizable ? (WS_SIZEBOX | WS_MAXIMIZEBOX) : 0) | (createInfo.maximize && createInfo.resizable ? WS_MAXIMIZE : 0);

        // Create window size rect
        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = static_cast<LONG>(createInfo.width);
        rect.bottom = static_cast<LONG>(createInfo.height);
        AdjustWindowRectEx(&rect, style, FALSE, exStyle);

        // Create window
        window = CreateWindowEx(
            exStyle, className,
            title.c_str(), style,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right - rect.left, rect.bottom - rect.top,
            NULL, NULL, windowsInstance.GetHInstance(), NULL
        );
        SR_ERROR_IF(window == nullptr, "Could not create Win32 window!");

        // Manually resize window if needed
        bool adjustedForDPI = false;
        if (createInfo.maximize && !createInfo.resizable)
        {
            // Set up monitor info
            MONITORINFO monitorInfo;
            ZeroMemory(&monitorInfo, sizeof(MONITORINFO));

            // Get monitor & its dimensions
            HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
            if (GetMonitorInfo(monitor, &monitorInfo))
            {
                // Set rect to be the same size as monitor work area, and adjust that to account for DPI scaling
                rect = monitorInfo.rcWork;
                AdjustWindowSizeForDPI(rect);
                adjustedForDPI = true;

                // Move window down, so title bar is not outside the monitor
                rect.top += GetTitleBarHeight();
                SetWindowPos(window, nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_NOZORDER);
            }
        }

        // Account for DPI scaling
        if (!adjustedForDPI) AdjustWindowSizeForDPI(rect);

        // Enable Windows >= 7 events
        if (windowsInstance.IsWindowsVersionOrGreater(6, 1, 0))
        {
            ChangeWindowMessageFilterEx(window, WM_DROPFILES, MSGFLT_ALLOW, NULL);
            ChangeWindowMessageFilterEx(window, WM_COPYDATA, MSGFLT_ALLOW, NULL);
            ChangeWindowMessageFilterEx(window, WM_COPYGLOBALDATA, MSGFLT_ALLOW, NULL);
        }

        // Set window's icon to that of the binary
        SendMessage(window, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(windowsInstance.GetProcessIcon()));

        // Connect this window instance to the Windows window handle
        SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        // Show window
        if (!createInfo.hide)
        {
            ShowWindow(window, SW_SHOW);
        }
    }

    /* --- POLLING METHODS --- */

    LRESULT CALLBACK Win32Window::WindowProc(const HWND callingWindow, const UINT message, const WPARAM wParam, const LPARAM lParam)
    {
        Win32Window* window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(callingWindow, GWLP_USERDATA));
        if (window != nullptr)
        {
            switch (message)
            {
                case WM_SYSCOMMAND:
                {
                    switch (wParam)
                    {
                        case SC_SCREENSAVE:
                        case SC_MONITORPOWER:
                        {
                            return 0;
                        }
                        default:
                        {
                            break;
                        }
                    }
                    break;
                }
                case WM_SHOWWINDOW:
                {
                    window->justBecameShown = wParam;
                    break;
                }
                case WM_MOVE:
                {
                    if (window->nextMoveEventBlocked || window->justBecameShown || window->IsClosed()) break;

                    window->GetWindowMoveDispatcher().DispatchEvent(window->GetPosition());
                    break;
                }
                case WM_SIZE:
                {
                    if (window->justBecameShown) break;

                    if (wParam == SIZE_MINIMIZED)
                    {
                        if (window->IsClosed()) break;

                        window->GetWindowMinimizeDispatcher().DispatchEvent();
                        break;
                    }

                    if (wParam == SIZE_MAXIMIZED)
                    {
                        if (!window->IsMaximized()) window->GetWindowMaximizeDispatcher().DispatchEvent();
                        break;
                    }

                    if (window->nextMoveEventBlocked)
                    {
                        window->nextMoveEventBlocked = false;
                        break;
                    }

                    window->GetWindowResizeDispatcher().DispatchEvent(window->GetSize());
                    break;
                }
                case WM_SETFOCUS:
                {
                    if (window->IsClosed()) break;

                    window->GetWindowFocusDispatcher().DispatchEvent(true);
                    window->nextMoveEventBlocked = false;
                    break;
                }
                case WM_KILLFOCUS:
                {
                    if (window->IsClosed()) break;

                    window->GetWindowFocusDispatcher().DispatchEvent(false);
                    window->nextMoveEventBlocked = true;
                    break;
                }
                case WM_CLOSE:
                {
                    window->GetWindowCloseDispatcher().DispatchEvent();
                    window->Close();
                    return 0;
                }
                case WM_DESTROY:
                {
                    PostQuitMessage(0);
                    return 0;
                }
                case WM_KEYDOWN:
                case WM_SYSKEYDOWN:
                case WM_KEYUP:
                case WM_SYSKEYUP:
                {
                    window->inputManager.KeyMessage(message, wParam, lParam);
                    break;
                }
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                case WM_XBUTTONDOWN:
                case WM_XBUTTONUP:
                {
                    window->inputManager.MouseButtonMessage(message, wParam, lParam);
                    break;
                }
                case WM_MOUSEHWHEEL:
                {
                    window->inputManager.MouseWheelMessage(message, wParam, lParam);
                    break;
                }
                case WM_MOUSEMOVE:
                {
                    window->cursorManager.MouseMoveMessage(message, wParam, lParam);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        return DefWindowProc(callingWindow, message, wParam, lParam);
    }

    void Win32Window::OnUpdate()
    {
        cursorManager.OnUpdate();
        inputManager.OnUpdate();

        MSG message;
        while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        justBecameShown = false;
    }

    void Win32Window::Minimize()
    {
        ShowWindow(window, SW_MINIMIZE);
    }

    void Win32Window::Maximize()
    {
        ShowWindow(window, SW_MAXIMIZE);
    }

    void Win32Window::Show()
    {
        ShowWindow(window, SW_SHOW);
    }

    void Win32Window::Hide()
    {
        ShowWindow(window, SW_HIDE);
    }

    void Win32Window::Focus()
    {
        SetFocus(window);
        SetForegroundWindow(window);
    }

    void Win32Window::Close()
    {
        if (closed) return;
        closed = true;

        CloseWindow(window);

        char className[11 + 1];
        GetClassName(window, className, sizeof(className));
        UnregisterClass(className, windowsInstance.GetHInstance());
        SetWindowLongPtr(window, GWLP_USERDATA, 0);

        DestroyWindow(window);
        window = nullptr;
    }

    /* --- SETTER METHODS --- */

    void Win32Window::SetTitle(const String &newTitle)
    {
        title = newTitle;
        SetWindowText(window, newTitle.c_str());
    }

    void Win32Window::SetPosition(const Vector2Int &position)
    {
        RECT rect = { position.x, position.y + GetTitleBarHeight(), position.x, position.y + GetTitleBarHeight() };
        rect.left -= GetSystemMetrics(SM_CXBORDER);
        rect.right -= GetSystemMetrics(SM_CXBORDER);
        rect.top -= GetSystemMetrics(SM_CYBORDER);
        rect.bottom -= GetSystemMetrics(SM_CYBORDER);

        AdjustWindowSizeForDPI(rect);

        // Update window position
        SetWindowPos(window, nullptr, rect.left, rect.top, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
    }

    void Win32Window::SetSize(const Vector2UInt &size)
    {
        RECT rect = { 0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y) };
        AdjustWindowSizeForDPI(rect);

        // Update window rect
        SetWindowPos(window, HWND_TOP, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
    }

    void Win32Window::SetOpacity(const float32 opacity)
    {
        LONG exStyle = GetWindowLongW(window, GWL_EXSTYLE);
        if (GetOpacity() < 1.0f || (exStyle & WS_EX_TRANSPARENT))
        {
            BYTE alpha = (BYTE) (255 * opacity);
            exStyle |= WS_EX_LAYERED;
            SetWindowLongW(window, GWL_EXSTYLE, exStyle);
            SetLayeredWindowAttributes(window, 0, alpha, LWA_ALPHA);
        }
        else if (exStyle & WS_EX_TRANSPARENT)
        {
            SetLayeredWindowAttributes(window, 0, 0, 0);
        }
        else
        {
            exStyle &= ~WS_EX_LAYERED;
            SetWindowLongW(window, GWL_EXSTYLE, exStyle);
        }
    }

    /* --- GETTER METHODS --- */

    String Win32Window::GetTitle() const
    {
        return title;
    }

    Vector2Int Win32Window::GetPosition() const
    {
        // Get position
        POINT position = { 0, 0 };
        ClientToScreen(window, &position);

        return { position.x, position.y - GetTitleBarHeight() };
    }

    Vector2UInt Win32Window::GetSize() const
    {
        RECT rect;
        GetWindowRect(window, &rect);
        return { rect.right - rect.left, rect.bottom - rect.top };
    }

    Vector2UInt Win32Window::GetFramebufferSize() const
    {
        RECT rect;
        GetClientRect(window, &rect);
        return { rect.right - rect.left, rect.bottom - rect.top };
    }

    float32 Win32Window::GetOpacity() const
    {
        BYTE alpha;
        DWORD flags;

        // Get alpha
        if ((GetWindowLongW(window, GWL_EXSTYLE) & WS_EX_LAYERED) && GetLayeredWindowAttributes(window, NULL, &alpha, &flags))
        {
            if (flags & LWA_ALPHA) return static_cast<float32>(alpha) / 255.f;
        }

        return 1.0f;
    }

    bool Win32Window::IsClosed() const
    {
        return closed;
    }

    bool Win32Window::IsMinimized() const
    {
        return IsIconic(window);
    }

    bool Win32Window::IsMaximized() const
    {
        return IsZoomed(window);
    }

    bool Win32Window::IsFocused() const
    {
        return GetForegroundWindow() == window;
    }

    bool Win32Window::IsHidden() const
    {
        return !IsWindowVisible(window);
    }

    InputManager& Win32Window::GetInputManager()
    {
        return inputManager;
    }

    CursorManager& Win32Window::GetCursorManager()
    {
        return cursorManager;
    }

    WindowAPI Win32Window::GetAPI() const
    {
        return WindowAPI::Win32;
    }

    /* --- PRIVATE METHODS --- */

    void Win32Window::AdjustWindowSizeForDPI(RECT &rect)
    {
        if (windowsInstance.IsWindowsVersionOrGreater(10, 0, 14393)) // Windows 10 1607
        {
            AdjustWindowRectExForDpi(&rect, GetWindowLong(window, GWL_STYLE), FALSE, GetWindowLong(window, GWL_STYLE), GetDpiForWindow(window));
        }
        else
        {
            AdjustWindowRectEx(&rect, GetWindowLong(window, GWL_STYLE), FALSE, GetWindowLong(window, GWL_STYLE));
        }
    }

    /* --- DESTRUCTOR --- */

    Win32Window::~Win32Window()
    {
        if (closed) return;
        WindowProc(window, WM_CLOSE, 0, 0);
    }

}