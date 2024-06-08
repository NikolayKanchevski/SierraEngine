//
// Created by Nikolay Kanchevski on 8.21.2023.
//

#include "Win32Window.h"

#include "WindowsContext.h"

#include "Win32Screen.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Win32Window::Win32Window(const Win32Context &win32Context, const WindowCreateInfo &createInfo)
        : Window(createInfo),
            win32Context(win32Context),
            window(win32Context.CreateWindow(createInfo.title, createInfo.width, createInfo.height, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | (createInfo.resizable ? (WS_SIZEBOX | WS_MAXIMIZEBOX) : 0) | (createInfo.maximize && createInfo.resizable ? WS_MAXIMIZE : 0), WindowProc)),
            inputManager(Win32InputManager({ })),
            cursorManager(Win32CursorManager(window, { })),
            title(createInfo.title)
    {
        // Manually maximize window if not resizable
        if (createInfo.maximize && !createInfo.resizable)
        {
            // Get screen
            const Win32Screen &screen = win32Context.GetWindowScreen(window);

            // Construct rect
            RECT rect
            {
                .left = screen.GetWorkAreaOrigin().x,
                .top = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYBORDER),
                .right = screen.GetWorkAreaOrigin().x + static_cast<LONG>(screen.GetWorkAreaWidth()),
                .bottom = static_cast<LONG>(screen.GetWorkAreaHeight())
            };

            // Resize
            win32Context.AdjustWindowRectForDPI(window, rect);
            SetWindowPos(window, nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_NOZORDER);
        }
        else if (createInfo.maximize)
        {
            ShowWindow(window, SW_MAXIMIZE);
        }

        // Connect this window instance to the Windows window handle
        SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        // Hide window
        if (createInfo.hide) ShowWindow(window, SW_HIDE);
    }

    /* --- POLLING METHODS --- */

    void Win32Window::Update()
    {
        cursorManager.Update();
        inputManager.Update();

        while (!win32Context.IsWindowEventQueueEmpty(window))
        {
            win32Context.PollNextWindowEvent(window);
        }

        cursorManager.PostUpdate();
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

        win32Context.DestroyWindow(window);
        window = nullptr;
    }

    /* --- SETTER METHODS --- */

    void Win32Window::SetTitle(const std::string_view newTitle)
    {
        title = newTitle;
        SetWindowText(window, newTitle.data());
    }

    void Win32Window::SetPosition(const Vector2Int &position)
    {
        RECT rect = { position.x, position.y + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYBORDER) };
        win32Context.AdjustWindowRectForDPI(window, rect);

        // Update window position
        SetWindowPos(window, nullptr, rect.left, rect.top, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
    }

    void Win32Window::SetSize(const Vector2UInt &size)
    {
        RECT rect = { 0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y) };
        win32Context.AdjustWindowRectForDPI(window, rect);

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

    std::string_view Win32Window::GetTitle() const
    {
        return title;
    }

    Vector2Int Win32Window::GetPosition() const
    {
        // Get position
        POINT position = { 0, 0 };
        ClientToScreen(window, &position);
        return { position.x, position.y - (GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYBORDER) * 2) };
    }

    uint32 Win32Window::GetWidth() const
    {
        RECT rect = { };
        GetWindowRect(window, &rect);
        return rect.right - rect.left;
    }

    uint32 Win32Window::GetHeight() const
    {
        RECT rect = { };
        GetWindowRect(window, &rect);
        return rect.bottom - rect.top;
    }

    uint32 Win32Window::GetFramebufferWidth() const
    {
        RECT rect = { };
        GetClientRect(window, &rect);
        return rect.right - rect.left;
    }

    uint32 Win32Window::GetFramebufferHeight() const
    {
        RECT rect = { };
        GetClientRect(window, &rect);
        return rect.bottom - rect.top;
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

    Screen& Win32Window::GetScreen() const
    {
        return win32Context.GetWindowScreen(window);
    }

    InputManager& Win32Window::GetInputManager()
    {
        return inputManager;
    }

    CursorManager& Win32Window::GetCursorManager()
    {
        return cursorManager;
    }

    PlatformAPI Win32Window::GetAPI() const
    {
        return PlatformAPI::Win32;
    }

    /* --- PRIVATE METHODS --- */

    LRESULT CALLBACK Win32Window::WindowProc(HWND callingWindow, const UINT message, const WPARAM wParam, const LPARAM lParam)
    {
        Win32Window* window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(callingWindow, GWLP_USERDATA));
        if (window != nullptr)
        {
            if (window->win32Context.IsWindowEventFiltered(callingWindow, message, wParam, lParam))
            {
                return 0;
            }

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

                    window.GetWindowMoveDispatcher().DispatchEvent(window.GetPosition());
                    break;
                }
                case WM_SIZE:
                {
                    if (window->justBecameShown) break;

                    if (wParam == SIZE_MINIMIZED)
                    {
                        if (window->IsClosed()) break;

                        window.GetWindowMinimizeDispatcher().DispatchEvent();
                        break;
                    }

                    if (wParam == SIZE_MAXIMIZED)
                    {
                        window.GetWindowMaximizeDispatcher().DispatchEvent();
                    }

                    if (window->nextMoveEventBlocked)
                    {
                        window->nextMoveEventBlocked = false;
                        break;
                    }

                    window.GetWindowResizeDispatcher().DispatchEvent(window.GetWidth(), window.GetHeight());
                    break;
                }
                case WM_SETFOCUS:
                {
                    if (window->IsClosed()) break;

                    window.GetWindowFocusDispatcher().DispatchEvent(true);
                    window->nextMoveEventBlocked = false;

                    if (window->cursorManager.IsCursorHidden()) window->cursorManager.HideCursor();
                    return 0;
                }
                case WM_KILLFOCUS:
                {
                    if (window->IsClosed()) break;

                    window.GetWindowFocusDispatcher().DispatchEvent(false);
                    window->nextMoveEventBlocked = true;
                    return 0;
                }
                case WM_CLOSE:
                {
                    window.GetWindowCloseDispatcher().DispatchEvent();
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
                case WM_MOUSEWHEEL:
                case WM_MOUSEHWHEEL:
                {
                    window->inputManager.MouseWheelMessage(message, wParam, lParam);
                    break;
                }
                case WM_MOUSEMOVE:
                {
                    window->cursorManager.MouseMoveMessage(message, wParam, lParam);
                    return 0;
                }
                default:
                {
                    break;
                }
            }
        }

        return DefWindowProc(callingWindow, message, wParam, lParam);
    }

    /* --- DESTRUCTOR --- */

    Win32Window::~Win32Window()
    {
        if (closed) return;
        WindowProc(window, WM_CLOSE, 0, 0);
    }

}