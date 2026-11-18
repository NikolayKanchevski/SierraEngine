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
        constexpr long NET_WM_STATE_REMOVE = 0;
        constexpr long NET_WM_STATE_ADD = 1;

        // Largest size a window can have along an axis, which stands in for axes without a maximum size limit
        constexpr int MAXIMUM_WINDOW_EXTENT = std::numeric_limits<int16>::max();

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
            // Nothing else remembers where the window was, so that is saved here, for it to be restored to later
            if (!maximized)
            {
                unmaximizedPosition = QueryPosition();
                unmaximizedSize = size;
            }

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

    void X11Window::Restore()
    {
        if (closed) return;

        Display* const display = x11Context.GetDisplay();
        if (minimized)
        {
            // Mapping a minimized window is what asks the window manager to bring it back
            XMapWindow(display, window);
        }
        else if (maximized)
        {
            if (IsWindowStateSupported())
            {
                // Window manager returns the window to its previous geometry itself, and the outcome is reported through the state of the window
                x11Context.RegisterWindowEvent(window, x11Context.GetAtom(AtomType::NET_WM_STATE), NET_WM_STATE_REMOVE, x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_VERT), x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_HORZ), SOURCE_APPLICATION);
            }
            else
            {
                XMoveResizeWindow(display, window, unmaximizedPosition.x, unmaximizedPosition.y, unmaximizedSize.x, unmaximizedSize.y);
                maximized = false;
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

    void X11Window::RequestFocus(const WindowFocusRequestType requestType)
    {
        if (closed || focused) return;

        // Window manager highlights the window (such as in the taskbar), without taking focus away from the user
        if (x11Context.GetAtom(AtomType::NET_WM_STATE) != None && x11Context.GetAtom(AtomType::NET_WM_STATE_DEMANDS_ATTENTION) != None)
        {
            x11Context.RegisterWindowEvent(window, x11Context.GetAtom(AtomType::NET_WM_STATE), NET_WM_STATE_ADD, x11Context.GetAtom(AtomType::NET_WM_STATE_DEMANDS_ATTENTION), 0, SOURCE_APPLICATION);
        }

        // Critical requests are also marked as urgent, which window managers keep signaling (such as by flashing), until the window is focused
        if (requestType == WindowFocusRequestType::Critical) SetUrgencyHint(true);

        attentionRequested = true;
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

    void X11Window::SetSizeLimits(const Vector2UInt newMinimumSize, const Vector2UInt newMaximumSize)
    {
        // NOTE: A limit of zero stands for no limit along that axis
        minimumSize = newMinimumSize;
        maximumSize = newMaximumSize;
        if (closed) return;

        SetSizeHints(size);

        // Window managers only have to respect limits, once the window gets resized, so it is brought within them right away
        if (resizable && !maximized)
        {
            if (const Vector2UInt constrainedSize = GetConstrainedSize(GetSize()); constrainedSize != GetSize()) SetSize(constrainedSize);
        }

        XFlush(x11Context.GetDisplay());
    }

    void X11Window::SetAspectRatio(const uint32 numerator, const uint32 denominator)
    {
        // NOTE: A ratio of zero stands for no ratio at all
        aspectRatio = { numerator, denominator };
        if (closed) return;

        SetSizeHints(size);

        // Window is fitted to the ratio right away, the same way as on Wayland
        if (resizable && !maximized)
        {
            if (const Vector2UInt constrainedSize = GetConstrainedSize(GetSize()); constrainedSize != GetSize()) SetSize(constrainedSize);
        }

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

                // Attention, which was requested, has now been given, so the window manager is told to stop signaling it
                if (attentionRequested)
                {
                    if (x11Context.GetAtom(AtomType::NET_WM_STATE) != None && x11Context.GetAtom(AtomType::NET_WM_STATE_DEMANDS_ATTENTION) != None)
                    {
                        x11Context.RegisterWindowEvent(window, x11Context.GetAtom(AtomType::NET_WM_STATE), NET_WM_STATE_REMOVE, x11Context.GetAtom(AtomType::NET_WM_STATE_DEMANDS_ATTENTION), 0, SOURCE_APPLICATION);
                    }

                    SetUrgencyHint(false);
                    attentionRequested = false;
                }

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

    Vector2UInt X11Window::GetSize() const noexcept
    {
        return x11Context.GetLogicalSize(size);
    }

    Vector2UInt X11Window::GetFramebufferSize() const noexcept
    {
        // Framebuffer is the content of the window in physical pixels, which is what the server reports
        return size;
    }

    float32 X11Window::GetScaling() const noexcept
    {
        // X11 has no scale of its own per window, so it is that of the desktop
        return x11Context.GetScaling();
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
        else
        {
            if (minimumSize.x != 0 || minimumSize.y != 0)
            {
                const Vector2UInt physicalMinimumSize = x11Context.GetPhysicalSize(minimumSize);

                sizeHints->flags |= PMinSize;
                sizeHints->min_width = static_cast<int>(physicalMinimumSize.x);
                sizeHints->min_height = static_cast<int>(physicalMinimumSize.y);
            }

            if (maximumSize.x != 0 || maximumSize.y != 0)
            {
                const Vector2UInt physicalMaximumSize = x11Context.GetPhysicalSize(maximumSize);

                sizeHints->flags |= PMaxSize;
                sizeHints->max_width = maximumSize.x != 0 ? static_cast<int>(physicalMaximumSize.x) : MAXIMUM_WINDOW_EXTENT;
                sizeHints->max_height = maximumSize.y != 0 ? static_cast<int>(physicalMaximumSize.y) : MAXIMUM_WINDOW_EXTENT;
            }

            if (aspectRatio.x != 0 && aspectRatio.y != 0)
            {
                sizeHints->flags |= PAspect;
                sizeHints->min_aspect.x = sizeHints->max_aspect.x = static_cast<int>(aspectRatio.x);
                sizeHints->min_aspect.y = sizeHints->max_aspect.y = static_cast<int>(aspectRatio.y);
            }
        }

        XSetWMNormalHints(x11Context.GetDisplay(), window, sizeHints.get());
    }

    void X11Window::SetUrgencyHint(const bool urgent) const noexcept
    {
        Display* const display = x11Context.GetDisplay();

        // Other hints, which may already be set, are kept as they are
        std::unique_ptr<XWMHints, std::function<void(void*)>> hints = { XGetWMHints(display, window), XFree };
        if (hints == nullptr) hints.reset(XAllocWMHints());
        if (hints == nullptr) return;

        if (urgent) hints->flags |= XUrgencyHint;
        else hints->flags &= ~XUrgencyHint;

        XSetWMHints(display, window, hints.get());
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

    Vector2UInt X11Window::GetConstrainedSize(Vector2UInt targetSize) const noexcept
    {
        // Ratio is matched by shrinking whichever side is too long, the same way as on Wayland
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
