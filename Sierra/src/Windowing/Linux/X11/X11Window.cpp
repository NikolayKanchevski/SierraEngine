//
// Created by Nikolay Kanchevski on 8.26.23.
//

#include "X11Window.h"

#include <X11/Xmd.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

namespace Sierra
{

    namespace
    {
        // Buttons, which X11 reports scrolling as (Xlib only defines the first five buttons)
        constexpr uint32 SCROLL_UP_BUTTON = 4;
        constexpr uint32 SCROLL_DOWN_BUTTON = 5;
        constexpr uint32 SCROLL_LEFT_BUTTON = 6;
        constexpr uint32 SCROLL_RIGHT_BUTTON = 7;

        // Action field of _NET_WM_STATE client messages
        constexpr long NET_WM_STATE_ADD = 1;

        // Source indication of client messages, stating that they come from an application (as opposed to a pager or taskbar)
        constexpr long SOURCE_APPLICATION = 1;

        // Scrolling is reported as presses of buttons 4 to 7, which are not buttons of their own
        [[nodiscard]] bool IsScrollButton(const uint32 button) noexcept
        {
            return button >= SCROLL_UP_BUTTON && button <= SCROLL_RIGHT_BUTTON;
        }

        // Vertical scroll is positive when scrolling up, and horizontal is positive when scrolling right, as on the other platforms
        [[nodiscard]] Vector2 GetScrollDelta(const uint32 button) noexcept
        {
            switch (button)
            {
                case SCROLL_UP_BUTTON:   return { 0.0f, 1.0f };
                case SCROLL_DOWN_BUTTON:   return { 0.0f, -1.0f };
                case SCROLL_LEFT_BUTTON:   return { -1.0f, 0.0f };
                case SCROLL_RIGHT_BUTTON:   return { 1.0f, 0.0f };
                default:        return { 0.0f, 0.0f };
            }
        }
    }

    /* --- CONSTRUCTORS --- */

    X11Window::X11Window(X11Context& x11Context, const XID window, const WindowCreateInfo& createInfo)
        : Window(createInfo), x11Context(x11Context), window(window),
            inputManager(x11Context.GetXkbExtension()), cursorManager(x11Context, window, x11Context.GetPhysicalSize(Vector2UInt(createInfo.width, createInfo.height))),
            title(createInfo.title), resizable(createInfo.resizable), size(x11Context.GetPhysicalSize(Vector2UInt(createInfo.width, createInfo.height)))
    {
        Display* const display = x11Context.GetDisplay();

        SetSizeHints(size);
        ApplyTitle();

        // Maximized state is requested before mapping, so that the window manager applies it, when it manages the window
        const bool maximizeRequested = createInfo.maximize && resizable;
        if (maximizeRequested && IsWindowStateSupported())
        {
            const std::array<Atom, 2> states = { x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_VERT), x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_HORZ) };
            XChangeProperty(display, window, x11Context.GetAtom(AtomType::NET_WM_STATE), XA_ATOM, 32, PropModeReplace, reinterpret_cast<const uchar*>(states.data()), static_cast<int>(states.size()));
        }

        // Decorations are announced by the window manager through a property, which is read whenever it changes
        ReadFrameExtents();
        if (x11Context.GetAtom(AtomType::NET_REQUEST_FRAME_EXTENTS)) x11Context.RegisterWindowEvent(window, x11Context.GetAtom(AtomType::NET_REQUEST_FRAME_EXTENTS));

        // Events are routed to this instance from now on, which is done before mapping, so that none of them are missed
        XSaveContext(display, window, x11Context.GetContext(), reinterpret_cast<XPointer>(this));

        XMapWindow(display, window);
        XFlush(display);

        // Without a window manager there is nothing to apply the maximized state, so the window covers its screen itself
        if (maximizeRequested && !IsWindowStateSupported()) Maximize();

        minimized = ReadMinimized();
        if (IsWindowStateSupported()) maximized = ReadMaximized();
        lastPosition = QueryPosition();
    }

    /* --- POLLING METHODS --- */

    void X11Window::Update()
    {
        if (closed) return;

        // Events are read here as well, so the state of the window stays current, even while it is being waited on (such as while it is minimized)
        x11Context.Update();
        if (closed) return;

        inputManager.Update();
        cursorManager.Update(focused);
    }

    void X11Window::Minimize()
    {
        if (closed) return;

        // Result is reported through the state of the window, once the window manager has changed it
        XIconifyWindow(x11Context.GetDisplay(), window, x11Context.GetScreen());
        XFlush(x11Context.GetDisplay());
    }

    void X11Window::Maximize()
    {
        if (closed || !resizable) return;

        Display* const display = x11Context.GetDisplay();
        if (IsWindowStateSupported())
        {
            // Window manager resizes and moves the window itself, and the outcome is reported through the state of the window
            x11Context.RegisterWindowEvent(window, x11Context.GetAtom(AtomType::NET_WM_STATE), NET_WM_STATE_ADD, x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_VERT), x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_HORZ), SOURCE_APPLICATION);
        }
        else
        {
            const X11Screen& screen = x11Context.GetWindowScreen(window);
            const Vector2Int physicalOrigin = x11Context.GetPhysicalPosition(screen.GetWorkAreaOrigin());
            const Vector2UInt physicalSize = x11Context.GetPhysicalSize({ screen.GetWorkAreaWidth(), screen.GetWorkAreaHeight() });
            XMoveResizeWindow(display, window, physicalOrigin.x, physicalOrigin.y, physicalSize.x, physicalSize.y);

            // Nothing else reports the state of such a window, so it is the state set here
            if (!maximized)
            {
                maximized = true;
                GetWindowMaximizeDispatcher().DispatchEvent();
            }
        }

        XFlush(display);
    }

    void X11Window::Show()
    {
        if (closed) return;

        XMapWindow(x11Context.GetDisplay(), window);
        XFlush(x11Context.GetDisplay());
    }

    void X11Window::Hide()
    {
        if (closed) return;

        XUnmapWindow(x11Context.GetDisplay(), window);
        XFlush(x11Context.GetDisplay());
    }

    void X11Window::Focus()
    {
        if (closed) return;

        Display* const display = x11Context.GetDisplay();
        if (x11Context.GetAtom(AtomType::NET_ACTIVE_WINDOW))
        {
            // Window manager activates the window, which also raises it and gives it keyboard focus
            x11Context.RegisterWindowEvent(window, x11Context.GetAtom(AtomType::NET_ACTIVE_WINDOW), SOURCE_APPLICATION, CurrentTime, 0, 0);
        }
        else if (mapped)
        {
            XRaiseWindow(display, window);
            XSetInputFocus(display, window, RevertToParent, CurrentTime);
        }

        XFlush(display);
    }

    void X11Window::Close()
    {
        if (closed) return;

        Destroy();
        GetWindowCloseDispatcher().DispatchEvent();
    }

    /* --- SETTER METHODS --- */

    void X11Window::SetTitle(const std::string_view newTitle)
    {
        title = newTitle;
        if (!closed) ApplyTitle();
    }

    void X11Window::SetPosition(const Vector2Int position)
    {
        if (closed) return;

        // Position is that of the window's frame, so the decorations around the content are taken into account
        const Vector2Int physicalPosition = x11Context.GetPhysicalPosition(position);
        XMoveWindow(x11Context.GetDisplay(), window, physicalPosition.x + extents.x, physicalPosition.y + extents.z);
        XFlush(x11Context.GetDisplay());
    }

    void X11Window::SetSize(const Vector2UInt newSize)
    {
        if (closed) return;

        const Vector2UInt clampedSize = glm::max(newSize, Vector2UInt(1, 1));
        const Vector2UInt physicalSize = x11Context.GetPhysicalSize(clampedSize);
        SetSizeHints(physicalSize);

        XResizeWindow(x11Context.GetDisplay(), window, physicalSize.x, physicalSize.y);
        XFlush(x11Context.GetDisplay());
    }

    void X11Window::SetOpacity(const float32 opacity)
    {
        if (closed) return;

        CARD32 value = static_cast<CARD32>(std::numeric_limits<uint32>::max() * static_cast<float64>(opacity));
        XChangeProperty(x11Context.GetDisplay(), window, x11Context.GetAtom(AtomType::NET_WM_WINDOW_OPACITY), XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<uchar*>(&value), 1);
        XFlush(x11Context.GetDisplay());
    }

    /* --- POLLING METHODS --- */

    void X11Window::HandleX11Event(XEvent& event)
    {
        switch (event.type)
        {
            case ConfigureNotify:
            {
                // Size is reported directly, while position is relative to the root window, and has to be queried
                const Vector2UInt newSize = { static_cast<uint32>(event.xconfigure.width), static_cast<uint32>(event.xconfigure.height) };
                if (newSize != size)
                {
                    size = newSize;
                    cursorManager.SetWindowSize(size);
                    const Vector2UInt logicalSize = x11Context.GetLogicalSize(size);
                    GetWindowResizeDispatcher().DispatchEvent(logicalSize.x, logicalSize.y);
                }

                const Vector2Int newPosition = QueryPosition();
                if (newPosition != lastPosition)
                {
                    lastPosition = newPosition;
                    GetWindowMoveDispatcher().DispatchEvent(x11Context.GetLogicalPosition(newPosition));
                }

                break;
            }
            case PropertyNotify:
            {
                if (event.xproperty.state != PropertyNewValue) break;

                const Atom atom = event.xproperty.atom;
                if (atom == x11Context.GetAtom(AtomType::WM_STATE))
                {
                    const bool nowMinimized = ReadMinimized();
                    if (nowMinimized != minimized)
                    {
                        minimized = nowMinimized;
                        if (minimized) GetWindowMinimizeDispatcher().DispatchEvent();
                    }
                }
                else if (atom == x11Context.GetAtom(AtomType::NET_WM_STATE))
                {
                    const bool nowMaximized = ReadMaximized();
                    if (nowMaximized != maximized)
                    {
                        maximized = nowMaximized;
                        if (maximized) GetWindowMaximizeDispatcher().DispatchEvent();
                    }
                }
                else if (atom == x11Context.GetAtom(AtomType::NET_FRAME_EXTENTS))
                {
                    ReadFrameExtents();
                }

                break;
            }
            case MapNotify:
            {
                mapped = true;
                break;
            }
            case UnmapNotify:
            {
                mapped = false;
                break;
            }
            case ClientMessage:
            {
                if (event.xclient.message_type != x11Context.GetAtom(AtomType::WM_PROTOCOLS)) break;

                const Atom protocol = static_cast<Atom>(event.xclient.data.l[0]);
                if (protocol == x11Context.GetAtom(AtomType::WM_DELETE_WINDOW))
                {
                    // Window is only closed, once the application has been asked to do so by the window manager
                    Close();
                }
                else if (protocol == x11Context.GetAtom(AtomType::NET_WM_PING))
                {
                    // Window manager pings windows, to see whether they still respond, and expects the ping back at the root window
                    x11Context.ReplyToRootEvent(event);
                }

                break;
            }
            case FocusIn:
            {
                // Grabs are not focus changes made by the user
                if (event.xfocus.mode == NotifyGrab || event.xfocus.mode == NotifyUngrab) break;
                if (focused) break;

                focused = true;
                GetWindowFocusDispatcher().DispatchEvent(true);
                break;
            }
            case FocusOut:
            {
                if (event.xfocus.mode == NotifyGrab || event.xfocus.mode == NotifyUngrab) break;
                if (!focused) break;

                focused = false;

                // Releases are not reported to a window, which is not focused, so keys and buttons, which were still held, would otherwise remain held
                inputManager.ReleaseKeys();
                inputManager.ReleaseMouseButtons();

                GetWindowFocusDispatcher().DispatchEvent(false);
                break;
            }
            case KeyPress:
            {
                inputManager.RegisterKeyPress(inputManager.XKeyCodeToKey(event.xkey.keycode));
                break;
            }
            case KeyRelease:
            {
                // Without detectable auto-repeat, holding a key produces a release, which is immediately followed by a press of the same key. Those are not physical releases
                if (!x11Context.GetXkbExtension().IsAutoRepeatDetectable() && QLength(GetDisplay()) > 0)
                {
                    // No human can press a key again within 20 ms, so a press that close is treated as the auto-repeat of this one
                    const XEvent nextEvent = x11Context.PeekNextEvent();
                    if (nextEvent.type == KeyPress && nextEvent.xkey.window == event.xkey.window && nextEvent.xkey.keycode == event.xkey.keycode && (nextEvent.xkey.time - event.xkey.time) < 20)
                    {
                        break;
                    }
                }

                inputManager.RegisterKeyRelease(inputManager.XKeyCodeToKey(event.xkey.keycode));
                break;
            }
            case ButtonPress:
            {
                if (IsScrollButton(event.xbutton.button))
                {
                    inputManager.RegisterMouseScroll(GetScrollDelta(event.xbutton.button));
                    break;
                }

                inputManager.RegisterMouseButtonPress(inputManager.XButtonToMouseButton(event.xbutton.button));
                break;
            }
            case ButtonRelease:
            {
                if (IsScrollButton(event.xbutton.button)) break;

                inputManager.RegisterMouseButtonRelease(inputManager.XButtonToMouseButton(event.xbutton.button));
                break;
            }
            case MotionNotify:
            {
                // Cursor's Y coordinate is measured upwards from the bottom of the window
                cursorManager.RegisterCursorMove({ event.xmotion.x, static_cast<int32>(size.y) - event.xmotion.y });
                break;
            }
            default:
            {
                break;
            }
        }
    }

    /* --- GETTER METHODS --- */

    std::string X11Window::GetTitle() const noexcept
    {
        return title;
    }

    Vector2Int X11Window::GetPosition() const noexcept
    {
        if (closed) return { 0, 0 };
        return x11Context.GetLogicalPosition(QueryPosition());
    }

    uint32 X11Window::GetWidth() const noexcept
    {
        return x11Context.GetLogicalSize(size).x;
    }

    uint32 X11Window::GetHeight() const noexcept
    {
        return x11Context.GetLogicalSize(size).y;
    }

    uint32 X11Window::GetFramebufferWidth() const noexcept
    {
        // Framebuffer is the content of the window in physical pixels, which is what the server reports
        return size.x;
    }

    uint32 X11Window::GetFramebufferHeight() const noexcept
    {
        return size.y;
    }

    float32 X11Window::GetOpacity() const noexcept
    {
        if (closed) return 1.0f;

        float32 opacity = 1.0f;
        uchar* data = nullptr;
        if (x11Context.GetWindowProperty(window, x11Context.GetAtom(AtomType::NET_WM_WINDOW_OPACITY), XA_CARDINAL, &data) > 0)
        {
            opacity = static_cast<float32>(*reinterpret_cast<const ulong*>(data) / static_cast<float64>(std::numeric_limits<uint32>::max()));
        }
        if (data != nullptr) XFree(data);

        return opacity;
    }

    bool X11Window::IsClosed() const noexcept
    {
        return closed;
    }

    bool X11Window::IsMinimized() const noexcept
    {
        return minimized;
    }

    bool X11Window::IsMaximized() const noexcept
    {
        return maximized;
    }

    bool X11Window::IsFocused() const noexcept
    {
        return focused;
    }

    bool X11Window::IsVisible() const noexcept
    {
        return mapped;
    }

    bool X11Window::IsTransparent() const noexcept
    {
        // Windows are created with an opaque visual, so they can never be transparent
        return false;
    }

    /* --- DESTRUCTOR --- */

    X11Window::~X11Window() noexcept
    {
        if (!closed) Destroy();
    }

    /* --- PRIVATE METHODS --- */

    void X11Window::SetSizeHints(const Vector2UInt fixedSize) const noexcept
    {
        const std::unique_ptr<XSizeHints, std::function<void(void*)>> sizeHints = { XAllocSizeHints(), XFree };
        sizeHints->flags = PWinGravity;

        // Static gravity keeps the content in place, when the window manager adds its decorations around it
        sizeHints->win_gravity = StaticGravity;

        // Windows, which are not resizable, are fixed to their size, which the window manager has to respect
        if (!resizable)
        {
            sizeHints->flags |= PMinSize | PMaxSize;
            sizeHints->min_width = sizeHints->max_width = static_cast<int>(fixedSize.x);
            sizeHints->min_height = sizeHints->max_height = static_cast<int>(fixedSize.y);
        }

        XSetWMNormalHints(x11Context.GetDisplay(), window, sizeHints.get());
    }

    void X11Window::ApplyTitle() const noexcept
    {
        Display* const display = x11Context.GetDisplay();
        XStoreName(display, window, title.c_str());

        // Window managers prefer the UTF-8 title, which is what allows non-ASCII titles to be displayed
        XChangeProperty(display, window, x11Context.GetAtom(AtomType::NET_WM_NAME), XInternAtom(display, "UTF8_STRING", False), 8, PropModeReplace, reinterpret_cast<const uchar*>(title.data()), static_cast<int>(title.size()));
        XFlush(display);
    }

    void X11Window::ReadFrameExtents() noexcept
    {
        if (x11Context.GetAtom(AtomType::NET_FRAME_EXTENTS) == None) return;

        uchar* data = nullptr;
        if (x11Context.GetWindowProperty(window, x11Context.GetAtom(AtomType::NET_FRAME_EXTENTS), XA_CARDINAL, &data) == 4)
        {
            const ulong* const values = reinterpret_cast<const ulong*>(data);
            extents = { static_cast<int32>(values[0]), static_cast<int32>(values[1]), static_cast<int32>(values[2]), static_cast<int32>(values[3]) };
        }
        if (data != nullptr) XFree(data);
    }

    bool X11Window::ReadMinimized() const noexcept
    {
        // WM_STATE is maintained by the window manager, and starts with the state of the window
        uchar* data = nullptr;
        bool iconic = false;
        if (x11Context.GetWindowProperty(window, x11Context.GetAtom(AtomType::WM_STATE), x11Context.GetAtom(AtomType::WM_STATE), &data) >= 1)
        {
            iconic = reinterpret_cast<const ulong*>(data)[0] == static_cast<ulong>(IconicState);
        }
        if (data != nullptr) XFree(data);

        return iconic;
    }

    bool X11Window::ReadMaximized() const noexcept
    {
        if (!IsWindowStateSupported()) return false;

        uchar* data = nullptr;
        const uint32 stateCount = x11Context.GetWindowProperty(window, x11Context.GetAtom(AtomType::NET_WM_STATE), XA_ATOM, &data);
        const Atom* const states = reinterpret_cast<const Atom*>(data);

        // Window is only considered maximized, when it is maximized along both axes
        bool vertical = false;
        bool horizontal = false;
        for (uint32 i = 0; i < stateCount; i++)
        {
            if (states[i] == x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_VERT)) vertical = true;
            else if (states[i] == x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_HORZ)) horizontal = true;
        }
        if (data != nullptr) XFree(data);

        return vertical && horizontal;
    }

    bool X11Window::IsWindowStateSupported() const noexcept
    {
        return x11Context.GetAtom(AtomType::NET_WM_STATE) != None && x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_VERT) != None && x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_HORZ) != None;
    }

    Vector2Int X11Window::QueryPosition() const noexcept
    {
        int xPosition = 0;
        int yPosition = 0;
        XID child = None;
        XTranslateCoordinates(x11Context.GetDisplay(), window, x11Context.GetRootWindow(), 0, 0, &xPosition, &yPosition, &child);

        // Position of the window is that of its frame, which lies the extents of the decorations away from its content
        return { xPosition - extents.x, yPosition - extents.z };
    }

    void X11Window::Destroy() noexcept
    {
        Display* const display = x11Context.GetDisplay();

        // Cursor is shown again, so that it is not left hidden, once the window is gone
        cursorManager.SetCursorVisibility(true);

        XDeleteContext(display, window, x11Context.GetContext());
        XDestroyWindow(display, window);
        XFlush(display);

        window = None;
        closed = true;
        mapped = false;
        focused = false;
    }

}
