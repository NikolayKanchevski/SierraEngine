//
// Created by Nikolay Kanchevski on 8.21.2023.
//

#include "Win32Window.h"

#include "WindowsContext.h"

#include "Win32Screen.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Win32Window::Win32Window(Win32Context &win32Context, const WindowCreateInfo &createInfo)
        : Window(createInfo),
            win32Context(win32Context),
            window(win32Context.CreateWindow(createInfo.title, createInfo.width, createInfo.height, (!createInfo.hide ? WS_VISIBLE : 0) | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | (createInfo.resizable ? (WS_SIZEBOX | WS_MAXIMIZEBOX) : 0) | (createInfo.maximize && createInfo.resizable ? WS_MAXIMIZE : 0), WindowProc)),
            inputManager(), cursorManager(window),
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

        // Connect this window instance to the Windows window handle
        SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }

    /* --- POLLING METHODS --- */

    void Win32Window::Update()
    {
        cursorManager.Update();
        inputManager.Update();

        while (!win32Context.EventQueueEmpty(window))
        {
            win32Context.PollNextEvent(window);
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
        if ((GetWindowLongW(window, GWL_EXSTYLE) & WS_EX_LAYERED) && GetLayeredWindowAttributes(window, nullptr, &alpha, &flags))
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
            if (window->win32Context.IsEventFiltered(callingWindow, message, wParam, lParam))
            {
                return 0;
            }

            if (window->win32Context.IsEventFiltered(callingWindow, message, wParam, lParam)) return 0;
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
                        window->GetWindowMaximizeDispatcher().DispatchEvent();
                    }

                    if (window->nextMoveEventBlocked)
                    {
                        window->nextMoveEventBlocked = false;
                        break;
                    }

                    window->GetWindowResizeDispatcher().DispatchEvent(window->GetWidth(), window->GetHeight());
                    break;
                }
                case WM_SETFOCUS:
                {
                    if (window->IsClosed()) break;

                    window->GetWindowFocusDispatcher().DispatchEvent(true);
                    window->nextMoveEventBlocked = false;

                    if (!window->cursorManager.IsCursorVisible()) window->cursorManager.SetCursorVisibility(false);
                    return 0;
                }
                case WM_KILLFOCUS:
                {
                    if (window->IsClosed()) break;

                    window->GetWindowFocusDispatcher().DispatchEvent(false);
                    window->nextMoveEventBlocked = true;
                    return 0;
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
                    // If key has been filtered out we return
                    if (wParam == VK_PROCESSKEY) break;

                    // Translate key
                    UINT keyCode = wParam != 0 ? static_cast<UINT>(wParam) : MapVirtualKeyW(static_cast<UINT>(wParam), MAPVK_VK_TO_VSC);
                    Key key = Win32InputManager::VirtualKeyCodeToKey(keyCode);
                    if (key == Key::Unknown) break;

                    // Distinguish double keys, as they share the same initial code
                    if (HIWORD(lParam) & KF_EXTENDED)
                    {
                        switch (key)
                        {
                            case Key::LeftControl:
                            {
                                key = Key::RightControl;
                                break;
                            }
                            case Key::LeftAlt:
                            {
                                key = Key::RightAlt;
                                break;
                            }
                            default:
                            {
                                break;
                            }
                        }
                    }

                    // Since the legacy AltGr key is not handled as a separate key, but rather as a Left Control followed by Right Alt
                    // event, we must capture the second event to not detect 2 physical presses, when a just single one has happened
                    if (wParam == VK_CONTROL)
                    {
                        MSG temporaryMessage;
                        const DWORD time = GetMessageTime();
                        if (PeekMessageW(&temporaryMessage, nullptr, 0, 0, PM_NOREMOVE))
                        {
                            switch (temporaryMessage.message)
                            {
                                case WM_KEYDOWN:
                                case WM_SYSKEYDOWN:
                                case WM_KEYUP:
                                case WM_SYSKEYUP:
                                {
                                    if (temporaryMessage.wParam == VK_MENU && (HIWORD(temporaryMessage.lParam) & KF_EXTENDED) && temporaryMessage.time == time)
                                    {
                                        // This is the Left Control message, so we exit before Right Alt gets the chance
                                        break;
                                    }
                                    break;
                                }
                                default:
                                {
                                    break;
                                }
                            }
                            if (
                                (temporaryMessage.message == WM_KEYDOWN || temporaryMessage.message == WM_SYSKEYDOWN || temporaryMessage.message == WM_KEYUP || temporaryMessage.message == WM_SYSKEYUP) &&
                                (temporaryMessage.wParam == VK_MENU && (HIWORD(temporaryMessage.lParam) & KF_EXTENDED) && temporaryMessage.time == time)
                            )
                            {
                                break;
                            }
                        }
                    }

                    // Explicitly handle PrintScreen, as it only sends out KEY_RELEASE messages
                    if (key == Key::PrintScreen)
                    {
                        window->inputManager.RegisterKeyPress(Key::PrintScreen);
                        window->inputManager.RegisterKeyRelease(Key::PrintScreen);
                        break;
                    }
                    // Both Shift keys are reported as single VK_SHIFT message, and we need to distinguish them
                    else if (key == Key::LeftShift)
                    {
                        if (message == WM_KEYDOWN)
                        {
                            if (GetKeyState(VK_LSHIFT) & KF_UP)
                            {
                                window->inputManager.RegisterKeyPress(Key::LeftShift);
                                break;
                            }
                            else if (GetKeyState(VK_RSHIFT) & KF_UP)
                            {
                                window->inputManager.RegisterKeyPress(Key::RightShift);
                                break;
                            }
                        }
                        else if (message == WM_KEYUP)
                        {
                            if (window->inputManager.IsKeyPressed(Key::LeftShift) || window->inputManager.IsKeyHeld(Key::LeftShift) && !(GetKeyState(VK_LSHIFT) & KF_UP))
                            {
                                window->inputManager.RegisterKeyRelease(Key::LeftShift);
                                break;
                            }
                            if (!window->inputManager.IsKeyPressed(Key::LeftShift) && !window->inputManager.IsKeyHeld(Key::LeftShift) && !(GetKeyState(VK_RSHIFT) & KF_UP))
                            {
                                window->inputManager.RegisterKeyRelease(Key::RightShift);
                                break;
                            }
                        }
                    }

                    // Register key
                    const bool pressed = !(HIWORD(lParam) & KF_UP);
                    if (pressed) window->inputManager.RegisterKeyPress(key);
                    else window->inputManager.RegisterKeyRelease(key);

                    break;
                }
                case WM_LBUTTONDOWN:
                {
                    window->inputManager.RegisterMouseButtonPress(MouseButton::Left);
                    break;
                }
                case WM_LBUTTONUP:
                {
                    window->inputManager.RegisterMouseButtonRelease(MouseButton::Left);
                    break;
                }
                case WM_RBUTTONDOWN:
                {
                    window->inputManager.RegisterMouseButtonPress(MouseButton::Right);
                    break;
                }
                case WM_RBUTTONUP:
                {
                    window->inputManager.RegisterMouseButtonRelease(MouseButton::Right);
                    break;
                }
                case WM_MBUTTONDOWN:
                {
                    window->inputManager.RegisterMouseButtonPress(MouseButton::Middle);
                    break;
                }
                case WM_MBUTTONUP:
                {
                    window->inputManager.RegisterMouseButtonRelease(MouseButton::Middle);
                    break;
                }
                case WM_XBUTTONDOWN:
                {
                    window->inputManager.RegisterMouseButtonPress(GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? MouseButton::Extra1 : MouseButton::Extra2);
                    break;
                }
                case WM_XBUTTONUP:
                {
                    window->inputManager.RegisterMouseButtonRelease(GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? MouseButton::Extra1 : MouseButton::Extra2);
                    break;
                }
                case WM_MOUSEWHEEL:
                {
                    window->inputManager.RegisterMouseScroll({ window->inputManager.GetMouseScroll().x, static_cast<float32>(static_cast<SHORT>(HIWORD(wParam))) / WHEEL_DELTA });
                    break;
                }
                case WM_MOUSEHWHEEL:
                {
                    window->inputManager.RegisterMouseScroll({ -static_cast<float32>(static_cast<SHORT>(HIWORD(wParam))) / WHEEL_DELTA, window->inputManager.GetMouseScroll().x });
                    break;
                }
                case WM_MOUSEMOVE:
                {
                    RECT rect = { };
                    GetClientRect(window->GetHwnd(), &rect);
                    window->cursorManager.RegisterCursorMove({ LOWORD(lParam), rect.bottom - rect.top - HIWORD(lParam) });
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