//
// Created by Nikolay Kanchevski on 8.26.23.
//

#include "X11Window.h"

#include <X11/Xmd.h>
#include <X11/Xatom.h>

#define NET_WM_STATE_ADD 1

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    X11Window::X11Window(const X11Context& x11Context, const WindowCreateInfo& createInfo)
        : Window(createInfo), x11Context(x11Context),
            window(x11Context.CreateWindow(createInfo.title, createInfo.width, createInfo.height, WindowEventCallback, this)),
            inputManager(x11Context.GetXkbExtension()), cursorManager(x11Context, window),
            title(createInfo.title), extents(GetExtents()), lastMaximizedState(createInfo.maximize)
    {
        if (!createInfo.hide) XMapWindow(x11Context.GetDisplay(), window);

        // Handle resizing and maximizing
        if (!createInfo.resizable)
        {
            if (createInfo.maximize)
            {
                const X11Screen& screen = x11Context.GetWindowScreen(window);
                const Vector2UInt sizeLimits = Vector2UInt(screen.GetWorkAreaWidth(), screen.GetWorkAreaHeight()) - Vector2UInt(extents.x, extents.z);

                const std::unique_ptr<XSizeHints, std::function<void(void*)>> sizeHints = { XAllocSizeHints(), XFree };
                sizeHints->flags |= PWinGravity | PMinSize | PMaxSize;
                sizeHints->win_gravity = StaticGravity;
                sizeHints->min_width = static_cast<int>(sizeLimits.x);
                sizeHints->max_width = static_cast<int>(sizeLimits.x);
                sizeHints->min_height = static_cast<int>(sizeLimits.y);
                sizeHints->max_height = static_cast<int>(sizeLimits.y);
                XSetWMNormalHints(x11Context.GetDisplay(), window, sizeHints.get());
            }
            else
            {
                const Vector2UInt sizeLimits = { createInfo.width, createInfo.height };

                const std::unique_ptr<XSizeHints, std::function<void(void*)>> sizeHints = { XAllocSizeHints(), XFree };
                sizeHints->flags |= PWinGravity | PMinSize | PMaxSize;
                sizeHints->win_gravity = StaticGravity;
                sizeHints->min_width = static_cast<int>(sizeLimits.x);
                sizeHints->max_width = static_cast<int>(sizeLimits.x);
                sizeHints->min_height = static_cast<int>(sizeLimits.y);
                sizeHints->max_height = static_cast<int>(sizeLimits.y);
                XSetWMNormalHints(x11Context.GetDisplay(), window, sizeHints.get());
            }
        }
        else
        {
            if (createInfo.maximize)
            {
                shouldMaximizeOnShow = createInfo.hide;
                Maximize();
            }
        }

        // Assign it now, so an initial Maximize() call does not fail for non-resizable windows
        resizable = createInfo.resizable;
        XFlush(x11Context.GetDisplay());
    }

    /* --- POLLING METHODS --- */

    void X11Window::Update()
    {
        if (closed) return;

        inputManager.Update();
        cursorManager.Update();
    }

    void X11Window::Minimize()
    {
        XIconifyWindow(x11Context.GetDisplay(), window, x11Context.GetScreen());
        XFlush(x11Context.GetDisplay());

        while (!IsMinimized());
    }

    void X11Window::Maximize()
    {
        // Maximizing a non-resizable window will only emmit an event
        if (!resizable) return;

        // NOTE: We are manually maximizing the window (sending a maximize event while correctly registered, for some reason, it does not change the dimensions of the window)
        XMoveWindow(x11Context.GetDisplay(), window, 0, 0);

        ::Screen* const screen = ScreenOfDisplay(x11Context.GetDisplay(), x11Context.GetScreen());
        XResizeWindow(x11Context.GetDisplay(), window, screen->width, screen->height);

        if (!x11Context.GetAtom(AtomType::NET_WM_STATE) || !x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_VERT)  || !x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_HORZ))
        {
            SR_WARNING("Could not dispatch an X11 window maximize event, as the required atoms are unsupported!");
            return;
        }

        // Retrieve window states
        Atom* states = nullptr;
        const ulong windowStateCount = x11Context.GetWindowProperty(window, x11Context.GetAtom(AtomType::NET_WM_STATE), XA_ATOM, reinterpret_cast<uchar**>(&states));
        const std::unique_ptr<Atom, std::function<void(void*)>> windowStates = { states, XFree };

        // Check if all required states are present, and add these, which are not
        std::vector<Atom> missingStates = { x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_VERT), x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_HORZ) };
        for (size i = 0; i < windowStateCount; i++)
        {
            for (auto iterator = missingStates.begin(); iterator != missingStates.end();)
            {
                if (states[i] == *iterator) iterator = missingStates.erase(iterator);
                else iterator++;
            }
        }

        // If no missing states are present, then window must be maximized, so we return
        if (missingStates.empty()) return;

        // Otherwise we apply the missing states
        XChangeProperty(x11Context.GetDisplay(), window, x11Context.GetAtom(AtomType::NET_WM_STATE), XA_ATOM, 32, PropModeAppend, reinterpret_cast<const uchar*>(missingStates.data()), missingStates.size());
        XFlush(x11Context.GetDisplay());

        struct ConfigureNotifyEventData
        {
            XID window;
        };
        ConfigureNotifyEventData configureNotifyEventData = { .window = window };

        // Wait until request has been processed
        x11Context.WaitForEvent(
            [](Display*, XEvent* event, XPointer configureNotifyEventDataPointer) -> int
            {
                return event->type == ConfigureNotify && event->xproperty.window == reinterpret_cast<ConfigureNotifyEventData*>(configureNotifyEventDataPointer)->window;
            },
            &configureNotifyEventData
        );

    }

    void X11Window::Show()
    {
        XMapWindow(x11Context.GetDisplay(), window);
        if (shouldMaximizeOnShow)
        {
            Maximize();
            shouldMaximizeOnShow = false;

            GetWindowMaximizeDispatcher().DispatchEvent();
            GetWindowResizeDispatcher().DispatchEvent(GetWidth(), GetHeight());
        }

        XFlush(x11Context.GetDisplay());
    }

    void X11Window::Hide()
    {
        XUnmapWindow(x11Context.GetDisplay(), window);
        XFlush(x11Context.GetDisplay());
    }

    void X11Window::Focus()
    {
        if (x11Context.GetAtom(AtomType::NET_ACTIVE_WINDOW))
        {
            x11Context.RegisterWindowEvent(window, x11Context.GetAtom(AtomType::NET_ACTIVE_WINDOW), NET_WM_STATE_ADD, 0, 0, 0, SubstructureNotifyMask | SubstructureRedirectMask);
        }
        else if (!IsHidden())
        {
            XRaiseWindow(x11Context.GetDisplay(), window);
            XSetInputFocus(x11Context.GetDisplay(), window, RevertToParent, CurrentTime);
        }
        else
        {
            return;
        }

        XFlush(x11Context.GetDisplay());
    }

    void X11Window::Close()
    {
        if (closed) return;
        closed = true;

        x11Context.DestroyWindow(window);
        window = 0;
    }

    /* --- SETTER METHODS --- */

    void X11Window::SetTitle(const std::string_view newTitle)
    {
        title = newTitle;
        XStoreName(x11Context.GetDisplay(), window, title.data());
    }

    void X11Window::SetPosition(const Vector2Int position)
    {
        XMoveWindow(x11Context.GetDisplay(), window, position.x + extents.x, position.y);
    }

    void X11Window::SetSize(const Vector2UInt newSize)
    {
        XResizeWindow(x11Context.GetDisplay(), window, newSize.x, newSize.y);
    }

    void X11Window::SetOpacity(const float32 opacity)
    {
        CARD32 value = static_cast<CARD32>(std::numeric_limits<uint32>::max() * static_cast<float64>(opacity));
        XChangeProperty(x11Context.GetDisplay(), window, x11Context.GetAtom(AtomType::NET_WM_WINDOW_OPACITY), XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<uchar*>(&value), 1);
    }

    /* --- GETTER METHODS --- */

    std::string_view X11Window::GetTitle() const noexcept
    {
        return title;
    }

    Vector2Int X11Window::GetPosition() const noexcept
    {
        const X11Screen& screen = x11Context.GetWindowScreen(window);

        XID child;
        int xPosition, yPosition;
        XWindowAttributes windowAttributes;
        XTranslateCoordinates(x11Context.GetDisplay(), window, x11Context.GetRootWindow(), 0, 0, &xPosition, &yPosition, &child);
        XGetWindowAttributes(x11Context.GetDisplay(), window, &windowAttributes);
        
        // X11 does not take window extents into account when not maximized and uses -Y, so we manually handle that
        xPosition -= !IsMaximized() * extents.x;
        yPosition -= static_cast<int32>(screen.GetHeight()) - (!IsMaximized() * extents.z);

        return { xPosition, yPosition };
    }

    uint32 X11Window::GetWidth() const noexcept
    {
        XWindowAttributes windowAttributes;
        XGetWindowAttributes(x11Context.GetDisplay(), window, &windowAttributes);
        return windowAttributes.width;
    }

    uint32 X11Window::GetHeight() const noexcept
    {
        XWindowAttributes windowAttributes;
        XGetWindowAttributes(x11Context.GetDisplay(), window, &windowAttributes);
        return windowAttributes.height;
    }

    uint32 X11Window::GetFramebufferWidth() const noexcept
    {
        return lastSize.x;
    }

    uint32 X11Window::GetFramebufferHeight() const noexcept
    {
        return lastSize.y;
    }

    float32 X11Window::GetOpacity() const noexcept
    {
        float32 opacity = 1.0f;
        CARD32* value = nullptr;
        if (x11Context.GetWindowProperty(window, x11Context.GetAtom(AtomType::NET_WM_WINDOW_OPACITY), XA_CARDINAL, reinterpret_cast<uchar**>(&value)))
        {
            opacity = static_cast<float32>(*value / static_cast<float64>(std::numeric_limits<uint32>::max()));
            XFree(value);
        }

        return opacity;
    }

    bool X11Window::IsClosed() const noexcept
    {
        return closed;
    }

    bool X11Window::IsMinimized() const noexcept
    {
        struct WindowState
        {
            CARD32 state;
            XID icon;
        };
        WindowState* state = nullptr;

        uint32 result = WithdrawnState;
        if (x11Context.GetWindowProperty(window, x11Context.GetAtom(AtomType::WM_STATE), x11Context.GetAtom(AtomType::WM_STATE), reinterpret_cast<uchar**>(&state)) >= 2)
        {
            result = state->state;
        }

        if (state != nullptr) XFree(state);
        return result == IconicState;
    }

    bool X11Window::IsMaximized() const noexcept
    {
        if (!x11Context.GetAtom(AtomType::NET_WM_STATE) || !x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_VERT) || !x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_HORZ))
        {
            return false;
        }

        Atom* states = nullptr;
        const ulong stateCount = x11Context.GetWindowProperty(window, x11Context.GetAtom(AtomType::NET_WM_STATE), XA_ATOM, reinterpret_cast<uchar**>(&states));
        const std::unique_ptr<Atom, std::function<void(void*)>> windowStates = { states, XFree };

        bool success = false;
        for (ulong i = stateCount; i--;)
        {
            if (states[i] == x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_VERT) || states[i] == x11Context.GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_HORZ))
            {
                success = true;
                break;
            }
        }

        return success;
    }

    bool X11Window::IsFocused() const noexcept
    {
        XID focusedWindow;
        int focusedWindowState;

        XGetInputFocus(x11Context.GetDisplay(), &focusedWindow, &focusedWindowState);
        return window == focusedWindow;
    }

    bool X11Window::IsHidden() const noexcept
    {
        XWindowAttributes windowAttributes;
        XGetWindowAttributes(x11Context.GetDisplay(), window, &windowAttributes);
        return windowAttributes.map_state != IsViewable;
    }

    InputManager* X11Window::GetInputManager() noexcept
    {
        return &inputManager;
    }

    CursorManager* X11Window::GetCursorManager() noexcept
    {
        return &cursorManager;
    }

    TouchManager* X11Window::GetTouchManager() noexcept
    {
        return nullptr;
    }

    WindowingBackendType X11Window::GetBackendType() const noexcept
    {
        return WindowingBackendType::X11;
    }

    /* --- PRIVATE METHODS --- */

    Vector4UInt X11Window::GetExtents()
    {
        Vector4UInt extents = { 0, 0, 0, 0 };
        if (x11Context.GetAtom(AtomType::NET_FRAME_EXTENTS))
        {
            // Temporarily show window
            bool windowWasHidden = IsHidden();
            Show();

            // Register extent request
            XEvent event;
            x11Context.RegisterWindowEvent(window, x11Context.GetAtom(AtomType::NET_REQUEST_FRAME_EXTENTS));

            // Construct data struct to use when checking events
            struct ExtentEventData
            {
                XID window;
                Atom frameExtentsAtom;
            };
            ExtentEventData extentEventData = { .window = window, .frameExtentsAtom = x11Context.GetAtom(AtomType::NET_FRAME_EXTENTS) };

            // Wait until request has been processed
            x11Context.WaitForEvent(
                [](Display*, XEvent* event, XPointer extentEventDataPointer) -> int
                {
                    return event->type == PropertyNotify && event->xproperty.state == PropertyNewValue && event->xproperty.window == reinterpret_cast<ExtentEventData*>(extentEventDataPointer)->window && event->xproperty.atom == reinterpret_cast<ExtentEventData*>(extentEventDataPointer)->frameExtentsAtom;
                },
                &extentEventData
            );

            // Save extent data
            Atom* extentData = nullptr;
            if (x11Context.GetWindowProperty(window, x11Context.GetAtom(AtomType::NET_FRAME_EXTENTS), XA_CARDINAL, reinterpret_cast<uchar**>(&extentData)) == 4)
            {
                extents.x = static_cast<uint32>(extentData[0]);
                extents.y = static_cast<uint32>(extentData[1]);
                extents.z = static_cast<uint32>(extentData[2]);
                extents.w = static_cast<uint32>(extentData[3]);
                XFree(extentData);
            }

            // Hide window back
            if (windowWasHidden) Hide();

            // Discard all events, that have occurred during extent extraction
            XSync(x11Context.GetDisplay(), False);
            while(QLength(x11Context.GetDisplay()) >0)
            {
                XEvent discardedEvent = { };
                XNextEvent(x11Context.GetDisplay(), &discardedEvent);
            }
        }

        return extents;
    }

    void X11Window::WindowEventCallback(const XEvent& event, const XID windowID, void* userData)
    {
        if (event.xany.window != windowID || userData == nullptr) return;
        X11Window* window = reinterpret_cast<X11Window*>(userData);

        // Handle event accordingly
        switch (event.type)
        {
            case PropertyNotify:
            {
                if (event.xproperty.state != PropertyNewValue) break;

                // Handle minimize/maximize events
                if (event.xproperty.atom == window->x11Context.GetAtom(AtomType::WM_STATE))
                {
                    const bool newMinimizedState = window->IsMinimized();
                    if (window->lastMinimizedState != newMinimizedState)
                    {
                        if (newMinimizedState) window->GetWindowMinimizeDispatcher().DispatchEvent();
                        window->lastMinimizedState = newMinimizedState;
                    }
                }
                else if (event.xproperty.atom == window->x11Context.GetAtom(AtomType::NET_WM_STATE))
                {
                    const bool newMaximizedState = window->IsMaximized();
                    if (window->lastMaximizedState != newMaximizedState && !window->IsHidden() && window->IsFocused())
                    {
                        if (newMaximizedState)
                        {
                            window->GetWindowMaximizeDispatcher().DispatchEvent();
                            window->GetWindowResizeDispatcher().DispatchEvent(window->GetWidth(), window->GetHeight());
                        }
                        window->lastMaximizedState = newMaximizedState;
                    }
                }

                break;
            }
            case ConfigureNotify:
            {
                if (window->IsMinimized() || window->IsMaximized()) break;

                // Handle resizing
                {
                    const Vector2UInt newSize = Vector2UInt(event.xconfigure.width, event.xconfigure.height);
                    if (window->lastSize != newSize)
                    {
                        // This is just to check if size has not been set yet, because resize event is called upon creation, and we do not want it to be registered
                        if (window->lastSize.y != std::numeric_limits<uint32>::max())
                        {
                            window->GetWindowResizeDispatcher().DispatchEvent(window->GetWidth(), window->GetHeight());
                        }

                        window->lastSize = newSize;
                        break;
                    }
                }

                // Handle moving
                {
                    const Vector2Int newPosition = Vector2Int(event.xconfigure.x - window->extents.x, event.xconfigure.y - window->extents.z);
                    if (window->lastPosition != newPosition)
                    {
                        // This is just to check if position has not been set yet, because position event is called upon creation, and we do not want it to be registered
                        if (window->lastPosition.y != std::numeric_limits<int32>::max())
                        {
                            window->GetWindowMoveDispatcher().DispatchEvent(window->GetPosition());
                        }

                        window->lastPosition = newPosition;
                        break;
                    }
                }

                break;
            }
            case ClientMessage:
            {
                if (event.xclient.message_type == None) break;

                if (event.xclient.message_type == window->x11Context.GetAtom(AtomType::WM_PROTOCOLS))
                {
                    const Atom protocol = event.xclient.data.l[0];
                    if (protocol == None)
                    {
                        break;
                    }

                    // Handle window closing request
                    if (protocol == window->x11Context.GetAtom(AtomType::WM_DELETE_WINDOW))
                    {
                        window->GetWindowCloseDispatcher().DispatchEvent();
                        window->Close();
                    }
                        // Handle window pinging (tested by the window manager to see if it continues to responds to events)
                    else if (protocol == window->x11Context.GetAtom(AtomType::NET_WM_PING))
                    {
                        window->x11Context.RegisterWindowEvent(window->x11Context.GetRootWindow(), None, 0, 0, 0, 0, SubstructureNotifyMask | SubstructureRedirectMask);
                    }
                }
                break;
            }
            case FocusIn:
            {
                // Only register actual user-created window focus events
                if (event.xfocus.mode == NotifyGrab || event.xfocus.mode == NotifyUngrab)
                {
                    break;
                }

                window->GetWindowFocusDispatcher().DispatchEvent(true);
                break;
            }
            case FocusOut:
            {
                // Only register actual user-created window focus events
                if (event.xfocus.mode == NotifyGrab || event.xfocus.mode == NotifyUngrab)
                {
                    break;
                }

                window->GetWindowFocusDispatcher().DispatchEvent(false);
                break;
            }
            case KeyPress:
            {
                window->inputManager.RegisterKeyPress(window->inputManager.XKeyCodeToKey(event.xkey.keycode));
                break;
            }
            case KeyRelease:
            {
                // Repeated press events will also give out a release event, so we need a way to filter out non-physical events
                if (!window->x11Context.GetXkbExtension().IsAutoRepeatDetectable() && QLength(window->x11Context.GetDisplay()) > 0)
                {
                    // Check if next event is of KeyPress type, key information matches that of the current event, and the time in between is less than 20 ms (world record for most presses is for one every 62.5ms, so a delay of 20ms is guaranteed to not be physical)
                    const XEvent nextEvent = window->x11Context.PeekNextEvent();
                    if (nextEvent.type == KeyPress && nextEvent.xkey.window == event.xkey.window && nextEvent.xkey.keycode == event.xkey.keycode && (nextEvent.xkey.time - event.xkey.time) < 20)
                    {
                        return;
                    }
                }

                window->inputManager.RegisterKeyRelease(window->inputManager.XKeyCodeToKey(event.xkey.keycode));
                break;
            }
            case ButtonPress:
            {
                window->inputManager.RegisterMouseButtonPress(window->inputManager.XButtonToMouseButton(event.xbutton.button));
                break;
            }
            case ButtonRelease:
            {
                window->inputManager.RegisterMouseButtonRelease(window->inputManager.XButtonToMouseButton(event.xbutton.button));
                break;
            }
            case MotionNotify:
            {
                window->cursorManager.RegisterCursorMove({ event.xmotion.x, window->GetHeight() - event.xmotion.y });
                break;
            }
        }
    }

    /* --- DESTRUCTOR --- */

    X11Window::~X11Window() noexcept
    {
        if (closed) return;

        x11Context.DestroyWindow(window);
        window = 0;
    }

}