//
// Created by Nikolay Kanchevski on 8.26.23.
//

#include <X11/Xutil.h>

#include "X11Window.h"
#include "LinuxContext.h"

namespace Sierra
{
    /* --- CONSTRUCTORS --- */

    X11Window::X11Window(const X11Context& x11Context, const WindowCreateInfo& createInfo)
        : Window(createInfo),
            x11Context(x11Context),
            window(x11Context.CreateWindow(createInfo.title, createInfo.width, createInfo.height)),
            inputManager(x11Context.GetXkbExtension(), { }),
            cursorManager(x11Context, window, { }),
            title(createInfo.title),
            extents(x11Context.GetWindowExtents(window)),
            lastMaximizedState(createInfo.maximize),
            resizable(createInfo.resizable)
    {
        // Create entry in event queue map
        unhandledEventQueues.push_back({ .window = window });

        if (!createInfo.hide) x11Context.ShowWindow(window);

        // Handle resizing and maximizing
        if (!createInfo.resizable)
        {
            if (createInfo.maximize)
            {
                const X11Screen& screen = x11Context.GetWindowScreen(window);
                const Vector2UInt sizeLimits = Vector2UInt(screen.GetWorkAreaWidth(), screen.GetWorkAreaHeight()) - Vector2UInt(extents.x, extents.z);
                x11Context.SetWindowSizeLimits(window, sizeLimits, sizeLimits);
            }
            else
            {
                const Vector2UInt sizeLimits = { createInfo.width, createInfo.height };
                x11Context.SetWindowSizeLimits(window, sizeLimits, sizeLimits);
            }
        }
        else
        {
            if (createInfo.maximize)
            {
                shouldMaximizeOnShow = createInfo.hide;
                x11Context.MaximizeWindow(window);
            }
        }

        cursorManager.OnWindowInitialize();
    }

    /* --- POLLING METHODS --- */

    void X11Window::Update()
    {
        if (closed) return;

        inputManager.Update();
        cursorManager.Update();

        // Continue handling events until there are none queried
        while (!x11Context.IsEventQueueEmpty())
        {
            // If the current event does not apply to this window, we save it, so it can later access it
            XEvent event = x11Context.PollNextEvent();
            if (event.xany.window == window)
            {
                HandleX11Event(event);
            }
            else
            {
                auto iterator = std::ranges::find_if(unhandledEventQueues, [event](const WindowEventQueue& item) { return item.window == event.xany.window; });
                if (iterator != unhandledEventQueues.end())
                {
                    iterator->queue.push(event);
                }
            }
        }

        // Check if there are unhandled events pending for this window and if so, handle them
        auto iterator = std::ranges::find_if(unhandledEventQueues, [this](const WindowEventQueue& item) { return item.window == window; });
        while (!iterator->queue.empty())
        {
            XEvent& event = iterator->queue.front();
            if (event.xany.window == window) HandleX11Event(event);
            iterator->queue.pop();
        }

        // A closing event may have been handled, so we need to check again
        if (!closed) cursorManager.PostUpdate();

        x11Context.Flush();
    }

    void X11Window::Minimize()
    {
        x11Context.MinimizeWindow(window);
    }

    void X11Window::Maximize()
    {
        if (!resizable) return; // Maximizing an unresizable window will do nothing, but an event would still get emitted
        x11Context.MaximizeWindow(window);
    }

    void X11Window::Show()
    {
        x11Context.ShowWindow(window);
        if (shouldMaximizeOnShow)
        {
            Maximize();
            shouldMaximizeOnShow = false;

            GetWindowMaximizeDispatcher().DispatchEvent();
            GetWindowResizeDispatcher().DispatchEvent(GetWidth(), GetHeight());
        }
    }

    void X11Window::Hide()
    {
        x11Context.HideWindow(window);
    }

    void X11Window::Focus()
    {
        x11Context.FocusWindow(window);
    }

    void X11Window::Close()
    {
        if (closed) return;
        closed = true;

        // Remove XID from unhandled event map
        unhandledEventQueues.erase(std::ranges::find_if(unhandledEventQueues, [this](const WindowEventQueue& item) { return item.window == window; }));

        // Destroy window
        x11Context.DestroyWindow(window);
        window = 0;
    }

    /* --- SETTER METHODS --- */

    void X11Window::SetTitle(const std::string_view newTitle)
    {
        title = newTitle;
        x11Context.SetWindowTitle(window, newTitle);
    }

    void X11Window::SetPosition(const Vector2Int& newPosition)
    {
        x11Context.SetWindowPosition(window, { newPosition.x + extents.x, newPosition.y });
    }

    void X11Window::SetSize(const Vector2UInt& newSize)
    {
        x11Context.SetWindowSize(window, newSize);
    }

    void X11Window::SetOpacity(const float32 opacity)
    {
        x11Context.SetWindowOpacity(window, opacity);
    }

    /* --- GETTER METHODS --- */

    std::string_view X11Window::GetTitle() const
    {
        return title;
    }

    Vector2Int X11Window::GetPosition() const
    {
        const X11Screen& screen = x11Context.GetWindowScreen(window);
        
        // X11 does not take window extents into account when not maximized and uses -Y, so we manually handle that
        Vector2Int position = x11Context.GetWindowPosition(window);
        position.x -= !IsMaximized() * extents.x;
        position.y -= !IsMaximized() * extents.z;
        position.y = static_cast<int32>(screen.GetHeight()) - position.y;
        return position;
    }

    uint32 X11Window::GetWidth() const
    {
        return x11Context.GetWindowSize(window).x;
    }

    uint32 X11Window::GetHeight() const
    {
        return x11Context.GetWindowSize(window).y;
    }

    uint32 X11Window::GetFramebufferWidth() const
    {
        return lastSize.x;
    }

    uint32 X11Window::GetFramebufferHeight() const
    {
        return lastSize.y;
    }

    float32 X11Window::GetOpacity() const
    {
        return x11Context.GetWindowOpacity(window);
    }

    bool X11Window::IsClosed() const
    {
        return closed;
    }

    bool X11Window::IsMinimized() const
    {
        return x11Context.IsWindowMinimized(window);
    }

    bool X11Window::IsMaximized() const
    {
        return x11Context.IsWindowMaximized(window);
    }

    bool X11Window::IsFocused() const
    {
        return x11Context.IsWindowFocused(window);
    }

    bool X11Window::IsHidden() const
    {
        return x11Context.IsWindowHidden(window);
    }

    Screen& X11Window::GetScreen() const
    {
        return x11Context.GetWindowScreen(window);
    }

    InputManager* X11Window::GetInputManager()
    {
        return &inputManager;
    }

    CursorManager* X11Window::GetCursorManager()
    {
        return &cursorManager;
    }

    TouchManager* X11Window::GetTouchManager()
    {
        return nullptr;
    }

    PlatformAPI X11Window::GetAPI() const
    {
        return PlatformAPI::X11;
    }

    /* --- PRIVATE METHODS --- */

    void X11Window::HandleX11Event(XEvent& event)
    {
        // Discard event if filtered out
        if (x11Context.IsEventFiltered(event)) return;

        // Handle event accordingly
        switch (event.type)
        {
            case PropertyNotify:
            {
                if (event.xproperty.state != PropertyNewValue) break;

                // Handle minimize/maximize events
                if (event.xproperty.atom == x11Context.GetAtom(AtomType::WM_STATE))
                {
                    const bool newMinimizedState = IsMinimized();
                    if (lastMinimizedState != newMinimizedState)
                    {
                        if (newMinimizedState) GetWindowMinimizeDispatcher().DispatchEvent();
                        lastMinimizedState = newMinimizedState;
                    }
                }
                else if (event.xproperty.atom == x11Context.GetAtom(AtomType::NET_WM_STATE))
                {
                    const bool newMaximizedState = IsMaximized();
                    if (lastMaximizedState != newMaximizedState && !IsHidden() && IsFocused())
                    {
                        if (newMaximizedState)
                        {
                            GetWindowMaximizeDispatcher().DispatchEvent();
                            GetWindowResizeDispatcher().DispatchEvent(GetWidth(), GetHeight());
                        }
                        lastMaximizedState = newMaximizedState;
                    }
                }

                break;
            }
            case ConfigureNotify:
            {
                if (IsMinimized() || IsMaximized()) break;

                // Handle resizing
                {
                    const Vector2UInt newSize = Vector2UInt(event.xconfigure.width, event.xconfigure.height);
                    if (lastSize != newSize)
                    {
                        // This is just to check if size has not been set yet, because resize event is called upon creation, and we do not want it to be registered
                        if (lastSize.y != std::numeric_limits<uint32>::max())
                        {
                            GetWindowResizeDispatcher().DispatchEvent(GetWidth(), GetHeight());
                        }

                        lastSize = newSize;
                        break;
                    }
                }

                // Handle moving
                {
                    const Vector2Int newPosition = Vector2Int(event.xconfigure.x - extents.x, event.xconfigure.y - extents.z);
                    if (lastPosition != newPosition)
                    {
                        // This is just to check if position has not been set yet, because position event is called upon creation, and we do not want it to be registered
                        if (lastPosition.y != std::numeric_limits<int32>::max())
                        {
                            GetWindowMoveDispatcher().DispatchEvent(GetPosition());
                        }

                        lastPosition = newPosition;
                        break;
                    }
                }

                break;
            }
            case ClientMessage:
            {
                if (event.xclient.message_type == None) break;

                if (event.xclient.message_type == x11Context.GetAtom(AtomType::WM_PROTOCOLS))
                {
                    const Atom protocol = event.xclient.data.l[0];
                    if (protocol == None)
                    {
                        break;
                    }

                    // Handle window closing request
                    if (protocol == x11Context.GetAtom(AtomType::WM_DELETE_WINDOW))
                    {
                        GetWindowCloseDispatcher().DispatchEvent();
                        Close();
                    }
                    // Handle window pinging (tested by the window manager to see if it continues to responds to events)
                    else if (protocol == x11Context.GetAtom(AtomType::NET_WM_PING))
                    {
                        x11Context.RegisterRootWindowEvent(None, 0, 0, 0, 0, SubstructureNotifyMask | SubstructureRedirectMask);
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

                GetWindowFocusDispatcher().DispatchEvent(true);
                break;
            }
            case FocusOut:
            {
                // Only register actual user-created window focus events
                if (event.xfocus.mode == NotifyGrab || event.xfocus.mode == NotifyUngrab)
                {
                    break;
                }

                GetWindowFocusDispatcher().DispatchEvent(false);
                break;
            }
            case KeyPress:
            {
                inputManager.KeyPressEvent(event);
                break;
            }
            case KeyRelease:
            {
                // Repeated press events will also give out a release event, so we need a way to filter out non-physical events
                if (!x11Context.GetXkbExtension().IsAutoRepeatDetectable() && !x11Context.IsEventQueueEmpty())
                {
                    // Check if next event is of KeyPress type, key information matches that of the current event, and the time in between is less than 20 ms (world record for most presses is for one every 62.5ms, so a delay of 20ms is guaranteed to not be physical)
                    const XEvent nextEvent = x11Context.PeekNextEvent();
                    if (nextEvent.type == KeyPress && nextEvent.xkey.window == event.xkey.window && nextEvent.xkey.keycode == event.xkey.keycode && (nextEvent.xkey.time - event.xkey.time) < 20)
                    {
                        return;
                    }
                }
                
                inputManager.KeyReleaseEvent(event);
                break;
            }
            case ButtonPress:
            {
                inputManager.ButtonPressEvent(event);
                break;
            }
            case ButtonRelease:
            {
                inputManager.ButtonReleaseEvent(event);
                break;
            }
            case MotionNotify:
            {
                cursorManager.MotionNotifyEvent(event);
                break;
            }
        }
    }

    /* --- DESTRUCTOR --- */

    X11Window::~X11Window()
    {
        if (closed) return;

        // Destroy window
        XEvent closeEvent = x11Context.QueryWindowDestruction(window);
        HandleX11Event(closeEvent);
    }
}