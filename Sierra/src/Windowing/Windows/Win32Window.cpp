//
// Created by Nikolay Kanchevski on 8.21.2023.
//

#include "Win32Window.h"

namespace Sierra
{

    namespace
    {
        constexpr DWORD WINDOW_EX_STYLE = WS_EX_APPWINDOW;
        constexpr DWORD MINIMIZED_MESSAGE_WAIT_MILLISECONDS = 16;

        [[nodiscard]] DWORD GetWindowStyle(const bool resizable) noexcept
        {
            DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
            if (resizable) style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
            return style;
        }

        [[nodiscard]] HWND CreateWin32Window(const Win32Context& win32Context, const WindowCreateInfo& createInfo)
        {
            const std::wstring title = Win32Context::UTF8ToWideString(createInfo.title);

            // Window is created hidden, and only gets its final size and position, once the DPI of the monitor Windows places it on is known
            HWND window = CreateWindowExW(
                WINDOW_EX_STYLE, win32Context.GetWindowClassName(), title.c_str(), GetWindowStyle(createInfo.resizable),
                CW_USEDEFAULT, CW_USEDEFAULT, static_cast<int>(createInfo.width), static_cast<int>(createInfo.height),
                nullptr, nullptr, win32Context.GetHInstance(), nullptr
            );

            SR_THROW_IF(window == nullptr, UnsupportedFeatureError(SR_FORMAT("Cannot create Win32 window [{0}], as CreateWindowExW() failed with error code [{1}]", createInfo.title, GetLastError())));
            return window;
        }
    }

    /* --- CONSTRUCTORS --- */

    Win32Window::Win32Window(Win32Context& win32Context, const WindowCreateInfo& createInfo)
        : Window(createInfo), win32Context(win32Context), window(CreateWin32Window(win32Context, createInfo)), cursorManager(window, { createInfo.width, createInfo.height }),
            title(createInfo.title), resizable(createInfo.resizable), dpi(win32Context.GetWindowDPI(window))
    {
        // Content is sized in logical units, which are scaled by the DPI of the monitor the window has been placed on
        const Vector2UInt frameSize = GetFrameSize(ToPhysicalSize({ createInfo.width, createInfo.height }));

        // Window is centered within the work area of its monitor
        Vector2Int framePosition = QueryPosition();
        MONITORINFO monitorInfo = { };
        monitorInfo.cbSize = sizeof(MONITORINFO);
        if (GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitorInfo))
        {
            const RECT& workArea = monitorInfo.rcWork;
            framePosition.x = workArea.left + glm::max((workArea.right - workArea.left - static_cast<LONG>(frameSize.x)) / 2, 0L);
            framePosition.y = workArea.top + glm::max((workArea.bottom - workArea.top - static_cast<LONG>(frameSize.y)) / 2, 0L);
        }

        SetWindowPos(window, nullptr, framePosition.x, framePosition.y, static_cast<int>(frameSize.x), static_cast<int>(frameSize.y), SWP_NOZORDER | SWP_NOACTIVATE);

        size = QuerySize();
        lastPosition = QueryPosition();
        cursorManager.SetWindowSize(size);

        if (createInfo.opacity < 1.0f) SetOpacity(createInfo.opacity);

        // Messages are routed to this instance from now on, which is done before showing the window, so that none of the ones showing it sends are missed
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        // NOTE: Windows, which are not resizable, cannot be maximized, as on the other platforms
        ShowWindow(window, createInfo.maximize && resizable ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL);
        SetForegroundWindow(window);

        minimized = IsIconic(window) != FALSE;
        maximized = IsZoomed(window) != FALSE;
        focused = GetFocus() == window;
    }

    /* --- POLLING METHODS --- */

    void Win32Window::Update()
    {
        if (closed) return;

        // State of the current frame becomes that of the last one before messages are read, so that changes, which happen in between two updates, are compared against it
        inputManager.Update();
        cursorManager.Update(focused);

        // Minimized windows have nothing to present, so instead of spinning, the thread waits for a message (with a time limit, so control still returns to the caller regularly)
        if (minimized) MsgWaitForMultipleObjects(0, nullptr, FALSE, MINIMIZED_MESSAGE_WAIT_MILLISECONDS, QS_ALLINPUT);

        // Messages are read here as well, so the state of the window stays current, even while it is being waited on (such as while it is minimized)
        win32Context.Update();
        if (closed) return;

        ReleaseStuckKeys();
    }

    void Win32Window::Minimize()
    {
        if (closed) return;
        ShowWindow(window, SW_MINIMIZE);
    }

    void Win32Window::Maximize()
    {
        if (closed || !resizable) return;
        ShowWindow(window, SW_MAXIMIZE);
    }

    void Win32Window::Restore()
    {
        if (closed) return;

        // Restoring a minimized window brings it back to the state it was in before (which may be maximized), while restoring a maximized one unmaximizes it, as on the other platforms
        ShowWindow(window, SW_RESTORE);
    }

    void Win32Window::Show()
    {
        if (closed) return;
        ShowWindow(window, SW_SHOW);
    }

    void Win32Window::Hide()
    {
        if (closed) return;
        ShowWindow(window, SW_HIDE);
    }

    void Win32Window::RequestFocus(const WindowFocusRequestType requestType)
    {
        if (closed || focused) return;

        // Taskbar highlights the window, without taking focus away from the user, while critical requests keep flashing it (along with its title bar), until it is focused
        FLASHWINFO flashInfo = { };
        flashInfo.cbSize = sizeof(FLASHWINFO);
        flashInfo.hwnd = window;
        flashInfo.dwFlags = requestType == WindowFocusRequestType::Critical ? FLASHW_ALL | FLASHW_TIMERNOFG : FLASHW_TRAY;
        flashInfo.uCount = requestType == WindowFocusRequestType::Critical ? 0 : 1;
        FlashWindowEx(&flashInfo);
    }

    void Win32Window::Focus()
    {
        if (closed) return;

        if (minimized) ShowWindow(window, SW_RESTORE);
        BringWindowToTop(window);
        SetForegroundWindow(window);
        SetFocus(window);
    }

    void Win32Window::Close()
    {
        if (closed) return;

        Destroy();
        GetWindowCloseDispatcher().DispatchEvent();
    }

    /* --- SETTER METHODS --- */

    void Win32Window::SetTitle(const std::string_view newTitle)
    {
        title = newTitle;
        if (closed) return;

        const std::wstring wideTitle = Win32Context::UTF8ToWideString(title);
        SetWindowTextW(window, wideTitle.c_str());
    }

    void Win32Window::SetPosition(const Vector2Int position)
    {
        if (closed) return;

        // Position is that of the window's frame, as on the other platforms
        const Vector2Int physicalPosition = ToPhysicalPosition(position);
        SetWindowPos(window, nullptr, physicalPosition.x, physicalPosition.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void Win32Window::SetSize(const Vector2UInt newSize)
    {
        if (closed) return;

        // Size is that of the window's content, so the decorations around it are added on top
        const Vector2UInt frameSize = GetFrameSize(ToPhysicalSize(glm::max(newSize, Vector2UInt(1, 1))));
        SetWindowPos(window, nullptr, 0, 0, static_cast<int>(frameSize.x), static_cast<int>(frameSize.y), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void Win32Window::SetSizeLimits(const Vector2UInt newMinimumSize, const Vector2UInt newMaximumSize)
    {
        // NOTE: A limit of zero stands for no limit along that axis, and limits are applied whenever Windows asks for them (see WM_GETMINMAXINFO)
        minimumSize = newMinimumSize;
        maximumSize = newMaximumSize;
        if (closed) return;

        // Windows only applies limits, once the window gets resized, so it is brought within them right away
        if (resizable && !maximized)
        {
            if (const Vector2UInt constrainedSize = GetConstrainedSize(GetSize()); constrainedSize != GetSize()) SetSize(constrainedSize);
        }
    }

    void Win32Window::SetAspectRatio(const uint32 numerator, const uint32 denominator)
    {
        // NOTE: A ratio of zero stands for no ratio at all, and the ratio is applied whenever the user resizes the window (see WM_SIZING)
        aspectRatio = { numerator, denominator };
        if (closed) return;

        // Window is fitted to the ratio right away, the same way as on the other platforms
        if (resizable && !maximized)
        {
            if (const Vector2UInt constrainedSize = GetConstrainedSize(GetSize()); constrainedSize != GetSize()) SetSize(constrainedSize);
        }
    }

    void Win32Window::SetOpacity(const float32 opacity)
    {
        if (closed) return;

        // Only layered windows can be translucent, which they are only made into, when needed, as composing them is slower
        const LONG_PTR exStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);
        if (opacity < 1.0f)
        {
            if ((exStyle & WS_EX_LAYERED) == 0) SetWindowLongPtrW(window, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
            SetLayeredWindowAttributes(window, 0, static_cast<BYTE>(glm::round(255.0f * glm::clamp(opacity, 0.0f, 1.0f))), LWA_ALPHA);
        }
        else if ((exStyle & WS_EX_LAYERED) != 0)
        {
            SetWindowLongPtrW(window, GWL_EXSTYLE, exStyle & ~static_cast<LONG_PTR>(WS_EX_LAYERED));
        }
    }

    /* --- GETTER METHODS --- */

    std::string Win32Window::GetTitle() const noexcept
    {
        return title;
    }

    Vector2Int Win32Window::GetPosition() const noexcept
    {
        if (closed) return { 0, 0 };
        return ToLogicalPosition(QueryPosition());
    }

    Vector2UInt Win32Window::GetSize() const noexcept
    {
        return ToLogicalSize(size);
    }

    Vector2UInt Win32Window::GetFramebufferSize() const noexcept
    {
        return size;
    }

    float32 Win32Window::GetScaling() const noexcept
    {
        return GetDPIScaling();
    }

    float32 Win32Window::GetOpacity() const noexcept
    {
        if (closed) return 1.0f;

        BYTE alpha = 255;
        DWORD flags = 0;
        if ((GetWindowLongPtrW(window, GWL_EXSTYLE) & WS_EX_LAYERED) != 0 && GetLayeredWindowAttributes(window, nullptr, &alpha, &flags) && (flags & LWA_ALPHA) != 0)
        {
            return static_cast<float32>(alpha) / 255.0f;
        }

        return 1.0f;
    }

    bool Win32Window::IsVisible() const noexcept
    {
        // Minimized windows are not visible, as on the other platforms (while Windows considers them to be so)
        return !closed && !minimized && IsWindowVisible(window) != FALSE;
    }

    bool Win32Window::IsTransparent() const noexcept
    {
        // Content of the window is always opaque (only the whole window can be made translucent, through its opacity)
        return false;
    }

    bool Win32Window::IsMinimized() const noexcept
    {
        return minimized;
    }

    bool Win32Window::IsMaximized() const noexcept
    {
        return maximized;
    }

    bool Win32Window::IsFocused() const noexcept
    {
        return focused;
    }

    bool Win32Window::IsClosed() const noexcept
    {
        return closed;
    }

    /* --- DESTRUCTOR --- */

    Win32Window::~Win32Window() noexcept
    {
        if (!closed) Destroy();
    }

    /* --- PRIVATE METHODS --- */

    DWORD Win32Window::GetStyle() const noexcept
    {
        return static_cast<DWORD>(GetWindowLongPtrW(window, GWL_STYLE));
    }

    DWORD Win32Window::GetExStyle() const noexcept
    {
        return static_cast<DWORD>(GetWindowLongPtrW(window, GWL_EXSTYLE));
    }

    float32 Win32Window::GetDPIScaling() const noexcept
    {
        return static_cast<float32>(dpi) / static_cast<float32>(USER_DEFAULT_SCREEN_DPI);
    }

    Vector2UInt Win32Window::ToPhysicalSize(const Vector2UInt logicalSize) const noexcept
    {
        const float32 scaling = GetDPIScaling();
        return { static_cast<uint32>(glm::round(static_cast<float32>(logicalSize.x) * scaling)), static_cast<uint32>(glm::round(static_cast<float32>(logicalSize.y) * scaling)) };
    }

    Vector2UInt Win32Window::ToLogicalSize(const Vector2UInt physicalSize) const noexcept
    {
        const float32 scaling = GetDPIScaling();
        return { static_cast<uint32>(glm::round(static_cast<float32>(physicalSize.x) / scaling)), static_cast<uint32>(glm::round(static_cast<float32>(physicalSize.y) / scaling)) };
    }

    Vector2Int Win32Window::ToPhysicalPosition(const Vector2Int logicalPosition) const noexcept
    {
        const float32 scaling = GetDPIScaling();
        return { static_cast<int32>(glm::round(static_cast<float32>(logicalPosition.x) * scaling)), static_cast<int32>(glm::round(static_cast<float32>(logicalPosition.y) * scaling)) };
    }

    Vector2Int Win32Window::ToLogicalPosition(const Vector2Int physicalPosition) const noexcept
    {
        const float32 scaling = GetDPIScaling();
        return { static_cast<int32>(glm::round(static_cast<float32>(physicalPosition.x) / scaling)), static_cast<int32>(glm::round(static_cast<float32>(physicalPosition.y) / scaling)) };
    }

    Vector2UInt Win32Window::GetFrameSize(const Vector2UInt physicalSize) const noexcept
    {
        RECT rect = { 0, 0, static_cast<LONG>(physicalSize.x), static_cast<LONG>(physicalSize.y) };
        win32Context.AdjustWindowRectForDPI(rect, GetStyle(), GetExStyle(), dpi);
        return { static_cast<uint32>(rect.right - rect.left), static_cast<uint32>(rect.bottom - rect.top) };
    }

    Vector2UInt Win32Window::GetConstrainedSize(Vector2UInt targetSize) const noexcept
    {
        // Ratio is matched by shrinking whichever side is too long, the same way as on the other platforms
        if (aspectRatio.x != 0 && aspectRatio.y != 0 && targetSize.x != 0 && targetSize.y != 0)
        {
            const float32 currentAspectRatio = static_cast<float32>(targetSize.x) / static_cast<float32>(targetSize.y);
            const float32 targetAspectRatio = static_cast<float32>(aspectRatio.x) / static_cast<float32>(aspectRatio.y);

            if (currentAspectRatio < targetAspectRatio) targetSize.y = static_cast<uint32>(static_cast<float32>(targetSize.x) / targetAspectRatio);
            else if (currentAspectRatio > targetAspectRatio) targetSize.x = static_cast<uint32>(static_cast<float32>(targetSize.y) * targetAspectRatio);
        }

        // Limits take precedence over the ratio, when the two cannot both be met
        if (minimumSize.x != 0) targetSize.x = glm::max(targetSize.x, minimumSize.x);
        if (minimumSize.y != 0) targetSize.y = glm::max(targetSize.y, minimumSize.y);
        if (maximumSize.x != 0) targetSize.x = glm::min(targetSize.x, maximumSize.x);
        if (maximumSize.y != 0) targetSize.y = glm::min(targetSize.y, maximumSize.y);

        return glm::max(targetSize, Vector2UInt(1, 1));
    }

    Vector2Int Win32Window::QueryPosition() const noexcept
    {
        RECT rect = { };
        GetWindowRect(window, &rect);
        return { rect.left, rect.top };
    }

    Vector2UInt Win32Window::QuerySize() const noexcept
    {
        RECT rect = { };
        GetClientRect(window, &rect);
        return { static_cast<uint32>(glm::max(rect.right - rect.left, 0L)), static_cast<uint32>(glm::max(rect.bottom - rect.top, 0L)) };
    }

    void Win32Window::ApplyAspectRatio(const WPARAM edge, RECT& frameRect) const noexcept
    {
        // Ratio applies to the content, so the decorations around it are left out of it
        RECT decorations = { 0, 0, 0, 0 };
        win32Context.AdjustWindowRectForDPI(decorations, GetStyle(), GetExStyle(), dpi);

        const LONG decorationWidth = decorations.right - decorations.left;
        const LONG decorationHeight = decorations.bottom - decorations.top;
        const float32 ratio = static_cast<float32>(aspectRatio.x) / static_cast<float32>(aspectRatio.y);

        // Side, which the user is dragging, is kept as it is, and the other one is fitted to the ratio
        switch (edge)
        {
            case WMSZ_LEFT:
            case WMSZ_RIGHT:
            case WMSZ_BOTTOMLEFT:
            case WMSZ_BOTTOMRIGHT:
            {
                frameRect.bottom = frameRect.top + decorationHeight + static_cast<LONG>(static_cast<float32>(frameRect.right - frameRect.left - decorationWidth) / ratio);
                break;
            }
            case WMSZ_TOPLEFT:
            case WMSZ_TOPRIGHT:
            {
                frameRect.top = frameRect.bottom - decorationHeight - static_cast<LONG>(static_cast<float32>(frameRect.right - frameRect.left - decorationWidth) / ratio);
                break;
            }
            case WMSZ_TOP:
            case WMSZ_BOTTOM:
            {
                frameRect.right = frameRect.left + decorationWidth + static_cast<LONG>(static_cast<float32>(frameRect.bottom - frameRect.top - decorationHeight) * ratio);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    void Win32Window::ReleaseStuckKeys()
    {
        if (!focused) return;

        // Windows does not report every release: when both Shift keys are held, releasing one of them reports nothing, and Windows keys are not reported as released, when they trigger shortcuts of the system (such as Win+V)
        constexpr std::array<std::pair<int, Key>, 4> POLLED_KEYS
        {{
            { VK_LSHIFT, Key::LeftShift },
            { VK_RSHIFT, Key::RightShift },
            { VK_LWIN, Key::LeftSystem },
            { VK_RWIN, Key::RightSystem }
        }};

        for (const auto& [virtualKeyCode, key] : POLLED_KEYS)
        {
            if (inputManager.IsKeyDown(key) && (GetKeyState(virtualKeyCode) & 0x8000) == 0) inputManager.RegisterKeyRelease(key);
        }
    }

    void Win32Window::Destroy() noexcept
    {
        if (GetCapture() == window) ReleaseCapture();

        // Messages are no longer routed to this instance, as it is about to stop representing a window, and those sent while destroying it are handled by default
        SetWindowLongPtrW(window, GWLP_USERDATA, 0);
        DestroyWindow(window);

        window = nullptr;
        closed = true;
        focused = false;
        minimized = false;
        maximized = false;
    }

    LRESULT Win32Window::HandleWin32Message(const UINT message, const WPARAM wParam, const LPARAM lParam)
    {
        // NOTE: Handle is copied, as handling a message may close the window, after which it is still the one the message is to be finished with
        HWND const hwnd = window;

        switch (message)
        {
            case WM_CLOSE:
            {
                // Window is only closed, once the application has been asked to do so (such as by its close button, or by Alt+F4)
                Close();
                return 0;
            }
            case WM_SYSCOMMAND:
            {
                // Pressing Alt (or F10) would otherwise enter the menu loop of the window, which has no menu, and would hold up the application, until released
                if ((wParam & 0xFFF0) == SC_KEYMENU) return 0;
                break;
            }
            case WM_MOVE:
            {
                // Minimized windows are moved far off screen (to around [-32000, -32000]), which is not a move made by the user
                if (minimized || IsIconic(hwnd)) return 0;

                const Vector2Int newPosition = QueryPosition();
                if (newPosition != lastPosition)
                {
                    lastPosition = newPosition;
                    GetWindowMoveDispatcher().DispatchEvent(ToLogicalPosition(newPosition));
                }

                return 0;
            }
            case WM_SIZE:
            {
                // Minimizing a maximized window keeps it maximized, so that it is restored as such
                const bool nowMinimized = wParam == SIZE_MINIMIZED;
                const bool nowMaximized = wParam == SIZE_MAXIMIZED || (nowMinimized && maximized);

                if (nowMinimized != minimized)
                {
                    minimized = nowMinimized;
                    if (minimized) GetWindowMinimizeDispatcher().DispatchEvent();
                }

                if (nowMaximized != maximized)
                {
                    maximized = nowMaximized;
                    if (maximized) GetWindowMaximizeDispatcher().DispatchEvent();
                }

                // Minimized windows are reported to have no size, while their content keeps the one it had, as on the other platforms
                if (minimized) return 0;

                const Vector2UInt newSize = { LOWORD(lParam), HIWORD(lParam) };
                if (newSize != size)
                {
                    size = newSize;
                    cursorManager.SetWindowSize(size);

                    const Vector2UInt logicalSize = ToLogicalSize(size);
                    GetWindowResizeDispatcher().DispatchEvent(logicalSize.x, logicalSize.y);
                }

                return 0;
            }
            case WM_SETFOCUS:
            {
                if (focused) return 0;
                focused = true;

                // Attention, which may have been requested, has now been given, so the taskbar is told to stop signaling it
                FLASHWINFO flashInfo = { };
                flashInfo.cbSize = sizeof(FLASHWINFO);
                flashInfo.hwnd = hwnd;
                flashInfo.dwFlags = FLASHW_STOP;
                FlashWindowEx(&flashInfo);

                GetWindowFocusDispatcher().DispatchEvent(true);
                return 0;
            }
            case WM_KILLFOCUS:
            {
                if (!focused) return 0;
                focused = false;

                // Releases are not reported to a window, which is not focused, so keys and buttons, which were still held, would otherwise remain held
                if (GetCapture() == hwnd) ReleaseCapture();
                inputManager.ReleaseKeys();
                inputManager.ReleaseMouseButtons();

                GetWindowFocusDispatcher().DispatchEvent(false);
                return 0;
            }
            case WM_GETMINMAXINFO:
            {
                // Windows, which are not resizable, already cannot be resized by the user
                if (!resizable) break;

                // Limits are of the window's content, so the decorations around it are added on top
                MINMAXINFO* const minMaxInfo = reinterpret_cast<MINMAXINFO*>(lParam);
                if (minimumSize.x != 0 || minimumSize.y != 0)
                {
                    const Vector2UInt minimumFrameSize = GetFrameSize(ToPhysicalSize(glm::max(minimumSize, Vector2UInt(1, 1))));
                    if (minimumSize.x != 0) minMaxInfo->ptMinTrackSize.x = static_cast<LONG>(minimumFrameSize.x);
                    if (minimumSize.y != 0) minMaxInfo->ptMinTrackSize.y = static_cast<LONG>(minimumFrameSize.y);
                }

                if (maximumSize.x != 0 || maximumSize.y != 0)
                {
                    const Vector2UInt maximumFrameSize = GetFrameSize(ToPhysicalSize(glm::max(maximumSize, Vector2UInt(1, 1))));
                    if (maximumSize.x != 0) minMaxInfo->ptMaxTrackSize.x = static_cast<LONG>(maximumFrameSize.x);
                    if (maximumSize.y != 0) minMaxInfo->ptMaxTrackSize.y = static_cast<LONG>(maximumFrameSize.y);
                }

                return 0;
            }
            case WM_SIZING:
            {
                if (aspectRatio.x == 0 || aspectRatio.y == 0) break;

                ApplyAspectRatio(wParam, *reinterpret_cast<RECT*>(lParam));
                return TRUE;
            }
            case WM_DPICHANGED:
            {
                // Window has been moved to a monitor of another scale, so its content is rescaled to keep its logical size, using the rectangle Windows suggests for that
                dpi = LOWORD(wParam);

                const RECT* const suggestedRect = reinterpret_cast<const RECT*>(lParam);
                SetWindowPos(hwnd, nullptr, suggestedRect->left, suggestedRect->top, suggestedRect->right - suggestedRect->left, suggestedRect->bottom - suggestedRect->top, SWP_NOZORDER | SWP_NOACTIVATE);

                return 0;
            }
            case WM_ERASEBKGND:
            {
                // Content is drawn by the swapchain, so erasing it beforehand would only make it flicker
                return TRUE;
            }
            case WM_SETCURSOR:
            {
                // Hidden cursor is only hidden over the content of the window, and is shown as usual over its decorations
                if (LOWORD(lParam) == HTCLIENT && !cursorManager.IsCursorVisible())
                {
                    SetCursor(nullptr);
                    return TRUE;
                }

                break;
            }
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                HandleKeyMessage(message, wParam, lParam);

                // Messages are still handled by default, so that shortcuts of the system (such as Alt+F4) keep working
                break;
            }
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_XBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            case WM_XBUTTONUP:
            {
                MouseButton mouseButton = MouseButton::Unknown;
                switch (message)
                {
                    case WM_LBUTTONDOWN:
                    case WM_LBUTTONUP:      { mouseButton = MouseButton::Left; break; }
                    case WM_RBUTTONDOWN:
                    case WM_RBUTTONUP:      { mouseButton = MouseButton::Right; break; }
                    case WM_MBUTTONDOWN:
                    case WM_MBUTTONUP:      { mouseButton = MouseButton::Middle; break; }
                    default:                { mouseButton = GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? MouseButton::Extra1 : MouseButton::Extra2; break; }
                }

                const bool pressed = message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_XBUTTONDOWN;
                if (pressed)
                {
                    // Mouse is captured while any of its buttons is held, so that releases outside of the window are still reported to it
                    if (!inputManager.IsAnyMouseButtonDown()) SetCapture(hwnd);
                    inputManager.RegisterMouseButtonPress(mouseButton);
                }
                else
                {
                    inputManager.RegisterMouseButtonRelease(mouseButton);
                    if (!inputManager.IsAnyMouseButtonDown() && GetCapture() == hwnd) ReleaseCapture();
                }

                // Extra buttons are expected to be reported as handled with TRUE, unlike other buttons
                return message == WM_XBUTTONDOWN || message == WM_XBUTTONUP ? TRUE : 0;
            }
            case WM_MOUSEMOVE:
            {
                // Coordinates are signed, as they reach outside of the window while the mouse is captured, and the Y one is measured upwards from the bottom of the window
                const int32 x = static_cast<int16>(LOWORD(lParam));
                const int32 y = static_cast<int16>(HIWORD(lParam));
                cursorManager.RegisterCursorMove({ x, static_cast<int32>(size.y) - y });
                return 0;
            }
            case WM_MOUSEWHEEL:
            {
                // Each notch of a wheel is reported as WHEEL_DELTA, and vertical scroll is positive when scrolling up, as on the other platforms
                inputManager.RegisterMouseScroll({ 0.0f, static_cast<float32>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float32>(WHEEL_DELTA) });
                return 0;
            }
            case WM_MOUSEHWHEEL:
            {
                // Horizontal scroll is positive when scrolling right, as on the other platforms (and as Windows reports it)
                inputManager.RegisterMouseScroll({ static_cast<float32>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float32>(WHEEL_DELTA), 0.0f });
                return 0;
            }
            default:
            {
                break;
            }
        }

        return DefWindowProcW(hwnd, message, wParam, lParam);
    }

    void Win32Window::HandleKeyMessage(const UINT message, const WPARAM wParam, const LPARAM lParam)
    {
        const UINT virtualKeyCode = static_cast<UINT>(wParam);
        const WORD keyFlags = HIWORD(lParam);

        const bool pressed = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
        const bool extended = (keyFlags & KF_EXTENDED) != 0;

        // Keys, which are being composed by an input method editor, are not keys of their own
        if (virtualKeyCode == VK_PROCESSKEY) return;

        // Keys, which share a virtual key code with another key, are told apart by their scan code
        Key key = Key::Unknown;
        switch (virtualKeyCode)
        {
            case VK_SHIFT:
            {
                key = MapVirtualKeyW(LOBYTE(keyFlags), MAPVK_VSC_TO_VK_EX) == VK_RSHIFT ? Key::RightShift : Key::LeftShift;
                break;
            }
            case VK_CONTROL:
            {
                if (extended)
                {
                    key = Key::RightControl;
                    break;
                }

                // AltGr is reported as a press of Left Control, immediately followed by one of Right Alt (with the same time), the former of which is not a physical key press
                MSG nextMessage = { };
                if (PeekMessageW(&nextMessage, nullptr, 0, 0, PM_NOREMOVE))
                {
                    const bool nextIsKeyMessage = nextMessage.message == WM_KEYDOWN || nextMessage.message == WM_SYSKEYDOWN || nextMessage.message == WM_KEYUP || nextMessage.message == WM_SYSKEYUP;
                    if (nextIsKeyMessage && nextMessage.wParam == VK_MENU && (HIWORD(nextMessage.lParam) & KF_EXTENDED) != 0 && nextMessage.time == static_cast<DWORD>(GetMessageTime())) return;
                }

                key = Key::LeftControl;
                break;
            }
            case VK_MENU:
            {
                key = extended ? Key::RightAlt : Key::LeftAlt;
                break;
            }
            case VK_RETURN:
            {
                key = extended ? Key::KeypadEnter : Key::Enter;
                break;
            }
            default:
            {
                key = VirtualKeyCodeToKey(virtualKeyCode);
                break;
            }
        }

        if (key == Key::Unknown) return;

        // Print Screen is only reported when released, so it is pressed and released at once
        if (key == Key::PrintScreen)
        {
            if (!pressed)
            {
                inputManager.RegisterKeyPress(key);
                inputManager.RegisterKeyRelease(key);
            }
            return;
        }

        if (pressed) inputManager.RegisterKeyPress(key);
        else inputManager.RegisterKeyRelease(key);
    }

}
