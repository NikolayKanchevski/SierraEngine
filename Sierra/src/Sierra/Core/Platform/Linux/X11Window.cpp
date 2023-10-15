//
// Created by Nikolay Kanchevski on 8.26.23.
//

#include <X11/Xmd.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>

#include "X11Window.h"
#define NET_WM_STATE_ADD    1

namespace Sierra
{

    namespace
    {
        /*
         * Since you cannot get events for a specific window with X11 (you instead get all pending events for all windows),
         * we store the events, which are not for the current window, so they can be handled later - when the corresponding window is updated
        */
        std::unordered_map<XID, std::vector<XEvent>> unhandledEvents;
    }

    /* --- CONSTRUCTORS --- */

    X11Window::X11Window(const WindowCreateInfo &createInfo)
        : Window(createInfo), linuxInstance(*static_cast<LinuxInstance*>(createInfo.platformInstance.get())), title(createInfo.title), maximized(createInfo.maximize)
    {
        SR_ERROR_IF(createInfo.platformInstance->GetType() !=+ PlatformType::Linux, "Cannot create X11 window using a platform instance of type [{0}]!", createInfo.platformInstance->GetType()._to_string());

        // Assign attributes
        XSetWindowAttributes setAttributes = { 0 };
        setAttributes.override_redirect = true;
        setAttributes.background_pixel = WhitePixel(linuxInstance.GetDisplay(), linuxInstance.GetScreen());
        setAttributes.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | FocusChangeMask | VisibilityChangeMask | EnterWindowMask | LeaveWindowMask | PropertyChangeMask;

        // Create window
        window = XCreateWindow(
            linuxInstance.GetDisplay(), linuxInstance.GetRootWindow(), 0, 0,
            createInfo.width, createInfo.height, 0, DefaultDepth(linuxInstance.GetDisplay(), linuxInstance.GetScreen()),
            InputOutput, DefaultVisual(linuxInstance.GetDisplay(), linuxInstance.GetScreen()), CWBackPixel | (setAttributes.event_mask != 0 ? CWEventMask : 0), &setAttributes
        );
        SR_ERROR_IF(window == 0, "Could not create X11 window!");

        // Maximize window (manually, because it won't work if window is not resizable)
        if (createInfo.maximize)
        {
            XMoveWindow(linuxInstance.GetDisplay(), window, 0, 0);
            XResizeWindow(linuxInstance.GetDisplay(), window, ScreenOfDisplay(linuxInstance.GetDisplay(), linuxInstance.GetScreen())->width, ScreenOfDisplay(linuxInstance.GetDisplay(), linuxInstance.GetScreen())->height);
        }

        // Set window manager protocols
        Atom protocols[] = { linuxInstance.GetAtoms().WM_DELETE_WINDOW, linuxInstance.GetAtoms().NET_WM_PING };
        XSetWMProtocols(linuxInstance.GetDisplay(), window, protocols, sizeof(protocols) / sizeof(Atom));

        // Set the process ID
        int64 processID = getpid();
        XChangeProperty(linuxInstance.GetDisplay(), window, linuxInstance.GetAtoms().NET_WM_PID, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<uchar*>(&processID), 1);

        // If window supports typing and the normal type, assign it
        if (linuxInstance.GetAtoms().NET_WM_WINDOW_TYPE && linuxInstance.GetAtoms().NET_WM_WINDOW_TYPE_NORMAL)
        {
            Atom type = linuxInstance.GetAtoms().NET_WM_WINDOW_TYPE_NORMAL;
            XChangeProperty(linuxInstance.GetDisplay(), window, linuxInstance.GetAtoms().NET_WM_WINDOW_TYPE, XA_ATOM, 32, PropModeReplace, reinterpret_cast<uchar*>(&type), 1);
        }

        // Handle OS' window manager hints
        {
            XWMHints* wmHints = XAllocWMHints();
            SR_ERROR_IF(wmHints == nullptr, "Could not allocate X11 window manager hints!");

            wmHints->flags = StateHint;
            wmHints->initial_state = NormalState;

            XSetWMHints(linuxInstance.GetDisplay(), window, wmHints);
            XFree(wmHints);
        }

        // Handle class hints
        {
            XClassHint* classHint = XAllocClassHint();

            char instanceName[12 + 1];
            std::snprintf(instanceName, sizeof(instanceName), "%s", "SierraEngine");
            classHint->res_name = instanceName;

            XSetClassHint(linuxInstance.GetDisplay(), window, classHint);
            XFree(classHint);
        }


        // Set window title
        XStoreName(linuxInstance.GetDisplay(), window, createInfo.title.c_str());
        XFlush(linuxInstance.GetDisplay());

        // Assign size hints
        XSizeHints* sizeHints = XAllocSizeHints();
        {
            SR_ERROR_IF(sizeHints == nullptr, "Could not allocate X11 window size sizeHints!");

            sizeHints->flags |= PWinGravity;
            sizeHints->win_gravity = StaticGravity;

            if (!createInfo.resizable)
            {
                sizeHints->flags |= (PMinSize | PMaxSize);
                if (createInfo.maximize)
                {
                    // Size is limited to just the work area of the screen
                    long* workAreaData = nullptr;
                    if (GetRootProperty(linuxInstance.GetAtoms().NET_WORKAREA, XA_CARDINAL, reinterpret_cast<uchar**>(&workAreaData)))
                    {
                        sizeHints->min_width = sizeHints->max_width = workAreaData[2];
                        sizeHints->min_height = sizeHints->max_height = workAreaData[3];
                        XFree(workAreaData);
                    }
                    else
                    {
                        SR_WARNING("Cannot get work area for X11 screen, as the [NET_WORKAREA] atom is not supported, making maximizing a non-resizable window impossible!");
                    }
                }
                else
                {
                    // Size is limited to the window's dimensions
                    sizeHints->min_width = sizeHints->max_width = createInfo.width;
                    sizeHints->min_height = sizeHints->max_height = createInfo.height;
                }
            }

            // Apply sizing policies
            XSetWMNormalHints(linuxInstance.GetDisplay(), window, sizeHints);
        }

        // Retrieve extents
        {
            if (linuxInstance.GetAtoms().NET_FRAME_EXTENTS)
            {
                // Temporarily show window
                XMapWindow(linuxInstance.GetDisplay(), window);

                // Register extent request
                XEvent event;
                RegisterX11Event(linuxInstance.GetAtoms().NET_REQUEST_FRAME_EXTENTS);

                // Create lambda to check wether a given event is of type NET_FRAME_EXTENTS
                auto IsFrameExtentEventLambda = [](Display* display, XEvent* event, XPointer windowPtr) -> Bool
                {
                    return static_cast<Bool>(event->type == PropertyNotify && event->xproperty.state == PropertyNewValue && event->xproperty.window == reinterpret_cast<X11Window*>(windowPtr)->window && event->xproperty.atom == reinterpret_cast<X11Window*>(windowPtr)->linuxInstance.GetAtoms().NET_FRAME_EXTENTS);
                };

                // Wait until request has been processed
                XIfEvent(linuxInstance.GetDisplay(), &event, IsFrameExtentEventLambda, reinterpret_cast<XPointer>(this));

                // Save extent data
                long* extentData = nullptr;
                if (GetProperty(linuxInstance.GetAtoms().NET_FRAME_EXTENTS, XA_CARDINAL, reinterpret_cast<uchar**>(&extentData)) == 4)
                {
                    extents.x = extentData[0];
                    extents.y = extentData[1];
                    extents.z = extentData[2];
                    extents.w = extentData[3];
                    XFree(extentData);
                }

                // Hide window back
                XUnmapWindow(linuxInstance.GetDisplay(), window);

                // Discard all events, that have occurred during extent extraction
                XSync(linuxInstance.GetDisplay(), False);
                while (QLength(linuxInstance.GetDisplay()) != 0)
                {
                    XNextEvent(linuxInstance.GetDisplay(), &event);
                }
            }
        }

        if (createInfo.maximize && !createInfo.resizable)
        {
            // If window was maximized manually, its limits need to be re-adjusted, such that they exclude all decorations
            position = { 0, 0 };
            sizeHints->min_width = sizeHints->max_width -= (extents.x + extents.y);
            sizeHints->min_height = sizeHints->max_height -= (extents.z + extents.w);
            XSetWMNormalHints(linuxInstance.GetDisplay(), window, sizeHints);

            // Flush all pending events
            XFlush(linuxInstance.GetDisplay());
            XSync(linuxInstance.GetDisplay(), False);

            // Discard them
            XEvent event;
            while (QLength(linuxInstance.GetDisplay()) != 0)
            {
                XNextEvent(linuxInstance.GetDisplay(), &event);
            }
        }
        XFree(sizeHints);

        // Show window
        if (!createInfo.hide)
        {
            XMapWindow(linuxInstance.GetDisplay(), window);
        }
    }

    /* --- POLLING METHODS --- */

    void X11Window::OnUpdate()
    {
        if (closed) return;

        // Continue handling events until there are none queried
        while (QLength(linuxInstance.GetDisplay()) != 0)
        {
            XEvent event;
            XNextEvent(linuxInstance.GetDisplay(), &event);

            // If the current event does not apply to this window, we save it, so it can later access it
            if (event.xany.window != window)
            {
                unhandledEvents[event.xclient.window].push_back(event);
            }
            else
            {
                HandleX11Event(event);
            }
        }

        XFlush(linuxInstance.GetDisplay());

        // Check if there are unhandled events pending for this window and if so, handle them
        auto iterator = unhandledEvents.find(window);
        if (iterator != unhandledEvents.end())
        {
            for (auto &event : iterator->second)
            {
                HandleX11Event(event);
            }
            iterator->second.clear();
        }

        XFlush(linuxInstance.GetDisplay());
    }

    void X11Window::Minimize()
    {
        XIconifyWindow(linuxInstance.GetDisplay(), window, linuxInstance.GetScreen());
        XFlush(linuxInstance.GetDisplay());

        while (!IsMinimized())
        {
            continue;
        }
    }

    void X11Window::Maximize()
    {
        if (!linuxInstance.GetAtoms().NET_WM_STATE || !linuxInstance.GetAtoms().NET_WM_STATE_MAXIMIZED_VERT || !linuxInstance.GetAtoms().NET_WM_STATE_MAXIMIZED_HORZ)
        {
            return;
        }

        if (!IsHidden())
        {
            RegisterX11Event(linuxInstance.GetAtoms().NET_WM_STATE, NET_WM_STATE_ADD, linuxInstance.GetAtoms().NET_WM_STATE_MAXIMIZED_VERT, linuxInstance.GetAtoms().NET_WM_STATE_MAXIMIZED_HORZ, 1, SubstructureNotifyMask | SubstructureRedirectMask);
        }
        else
        {
            Atom* states = NULL;
            ulong count = GetProperty(linuxInstance.GetAtoms().NET_WM_STATE, XA_ATOM, reinterpret_cast<uchar**>(&states));

            Atom missing[2] =
            {
                linuxInstance.GetAtoms().NET_WM_STATE_MAXIMIZED_VERT,
                linuxInstance.GetAtoms().NET_WM_STATE_MAXIMIZED_HORZ
            };

            ulong missingCount = 2;
            for (ulong i = 0; i < count; i++)
            {
                for (ulong j = 0;  j < missingCount;  j++)
                {
                    if (states[i] == missing[j])
                    {
                        missing[j] = missing[missingCount - 1];
                        missingCount--;
                    }
                }
            }

            if (states) XFree(states);
            if (!missingCount) return;

            XChangeProperty(linuxInstance.GetDisplay(), window, linuxInstance.GetAtoms().NET_WM_STATE, XA_ATOM, 32, PropModeAppend, reinterpret_cast<uchar*>(missing), missingCount);
        }

        XFlush(linuxInstance.GetDisplay());

        while (!IsMaximized())
        {
            continue;
        }
    }

    void X11Window::Show()
    {
        XMapWindow(linuxInstance.GetDisplay(), window);
    }

    void X11Window::Hide()
    {
        XUnmapWindow(linuxInstance.GetDisplay(), window);
        XFlush(linuxInstance.GetDisplay());
    }

    void X11Window::Focus()
    {
        if (linuxInstance.GetAtoms().NET_ACTIVE_WINDOW)
        {
            RegisterX11Event(linuxInstance.GetAtoms().NET_ACTIVE_WINDOW, NET_WM_STATE_ADD, 0, 0, 0, SubstructureNotifyMask | SubstructureRedirectMask);
        }
        else if (!IsHidden())
        {
            XRaiseWindow(linuxInstance.GetDisplay(), window);
            XSetInputFocus(linuxInstance.GetDisplay(), window, RevertToParent, CurrentTime);
        }

        XFlush(linuxInstance.GetDisplay());
    }

    void X11Window::Close()
    {
        if (closed) return;
        closed = true;

        // Register XID closing
        RegisterX11Event(linuxInstance.GetAtoms().WM_PROTOCOLS, linuxInstance.GetAtoms().WM_DELETE_WINDOW, CurrentTime);

        // Hide and destroy window
        XUnmapWindow(linuxInstance.GetDisplay(), window);
        XDestroyWindow(linuxInstance.GetDisplay(), window);

        // Remove XID from unhandled event map
        auto iterator = unhandledEvents.find(window);
        if (iterator != unhandledEvents.end())
        {
            unhandledEvents.erase(iterator);
        }

        // Dispatch all queried X11 events
        XFlush(linuxInstance.GetDisplay());
        window = 0;
    }

    /* --- SETTER METHODS --- */

    void X11Window::SetTitle(const String &newTitle)
    {
        title = newTitle;
        XStoreName(linuxInstance.GetDisplay(), window, newTitle.c_str());
    }

    void X11Window::SetPosition(const Vector2Int &newPosition)
    {
        XMoveWindow(linuxInstance.GetDisplay(), window, newPosition.x + extents.x, newPosition.y);
    }

    void X11Window::SetSize(const Vector2UInt &newSize)
    {
        XResizeWindow(linuxInstance.GetDisplay(), window, newSize.x, newSize.y);
    }

    void X11Window::SetOpacity(float32 opacity)
    {
        CARD32 value = static_cast<CARD32>(UINT32_MAX * static_cast<double>(opacity));
        XChangeProperty(linuxInstance.GetDisplay(), window, linuxInstance.GetAtoms().NET_WM_WINDOW_OPACITY, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<uchar*>(&value), 1);
    }

    /* --- GETTER METHODS --- */

    String X11Window::GetTitle() const
    {
        return title;
    }

    Vector2Int X11Window::GetPosition() const
    {
        return position;
    }

    Vector2UInt X11Window::GetSize() const
    {
        return size;
    }

    Vector2UInt X11Window::GetFramebufferSize() const
    {
        return size;
    }

    float32 X11Window::GetOpacity() const
    {
        float32 opacity = 1.0f;

        CARD32* value = NULL;
        if (GetProperty(linuxInstance.GetAtoms().NET_WM_WINDOW_OPACITY, XA_CARDINAL, reinterpret_cast<uchar**>(&value)))
        {
            opacity = static_cast<float32>(*value / static_cast<double>(UINT32_MAX));
            XFree(value);
        }

        return opacity;
    }

    bool X11Window::IsClosed() const
    {
        return closed;
    }

    bool X11Window::IsMinimized() const
    {
        int32 result = WithdrawnState;
        struct
        {
            CARD32 state;
            XID icon;
        }* state = nullptr;

        if (GetProperty(linuxInstance.GetAtoms().WM_STATE, linuxInstance.GetAtoms().WM_STATE, reinterpret_cast<uchar**>(&state)) >= 2)
        {
            result = state->state;
        }

        if (state != nullptr) XFree(state);
        return result == IconicState;
    }

    bool X11Window::IsMaximized() const
    {
        if (!linuxInstance.GetAtoms().NET_WM_STATE || !linuxInstance.GetAtoms().NET_WM_STATE_MAXIMIZED_VERT || !linuxInstance.GetAtoms().NET_WM_STATE_MAXIMIZED_HORZ)
        {
            return false;
        }

        Atom* states;
        ulong stateCount = GetProperty(linuxInstance.GetAtoms().NET_WM_STATE, XA_ATOM, reinterpret_cast<uchar**>(&states));

        bool result = false;
        for (ulong i = stateCount; i--;)
        {
            if (states[i] == linuxInstance.GetAtoms().NET_WM_STATE_MAXIMIZED_VERT || states[i] == linuxInstance.GetAtoms().NET_WM_STATE_MAXIMIZED_HORZ)
            {
                result = true;
                break;
            }
        }

        if (states != nullptr) XFree(states);
        return result;
    }

    bool X11Window::IsFocused() const
    {
        int focusedWindowState;
        XID focusedWindow;

        XGetInputFocus(linuxInstance.GetDisplay(), &focusedWindow, &focusedWindowState);
        return window == focusedWindow;
    }

    bool X11Window::IsHidden() const
    {
        XWindowAttributes windowAttributes;
        XGetWindowAttributes(linuxInstance.GetDisplay(), window, &windowAttributes);
        return windowAttributes.map_state != IsViewable;
    }

    WindowAPI X11Window::GetAPI() const
    {
        return WindowAPI::X11;
    }

    /* --- PRIVATE METHODS --- */

    int32 X11Window::GetProperty(Atom property, Atom type, uchar** value) const
    {
        // Arbitrary values, so the method below can be called
        Atom actualType;
        int actualFormat;
        ulong itemCount;
        ulong bytesAfter;

        // Get the requested property
        XGetWindowProperty(linuxInstance.GetDisplay(), window, property, 0, LONG_MAX, False, type, &actualType, &actualFormat, &itemCount, &bytesAfter, value);
        return itemCount;
    }

    int32 X11Window::GetRootProperty(Atom property, Atom type, uchar** value) const
    {
        // Arbitrary values, so the method below can be called
        Atom actualType;
        int actualFormat;
        ulong itemCount;
        ulong bytesAfter;

        // Get the requested property
        XGetWindowProperty(linuxInstance.GetDisplay(), linuxInstance.GetRootWindow(), property, 0, LONG_MAX, False, type, &actualType, &actualFormat, &itemCount, &bytesAfter, value);
        return itemCount;
    }

    void X11Window::RegisterX11Event(const Atom type, const long a, const long b, const long c, const long d, const int eventMask)
    {
        XEvent event;
        event.type = ClientMessage;
        event.xclient.window = window;
        event.xclient.format = 32;
        event.xclient.message_type = type;
        event.xclient.data.l[0] = a;
        event.xclient.data.l[1] = b;
        event.xclient.data.l[2] = c;
        event.xclient.data.l[3] = d;
        XSendEvent(linuxInstance.GetDisplay(), window, False, eventMask, &event);
    }

    void X11Window::HandleX11Event(XEvent &event)
    {
        // Save event states
        bool filtered = XFilterEvent(&event, None);

        // Handle event accordingly
        switch (event.type)
        {
            case PropertyNotify:
            {
                if (event.xproperty.state != PropertyNewValue) break;

                // Handle minimize/maximize events
                if (event.xproperty.atom == linuxInstance.GetAtoms().WM_STATE)
                {
                    bool newMinimizedState = IsMinimized();
                    if (minimized != newMinimizedState)
                    {
                        if (newMinimizedState) GetWindowMinimizeDispatcher().DispatchEvent();
                        minimized = newMinimizedState;
                    }
                }
                else if (event.xproperty.atom == linuxInstance.GetAtoms().NET_WM_STATE)
                {
                    bool newMaximizedState = IsMaximized();
                    if (maximized != newMaximizedState && !IsHidden() && IsFocused())
                    {
                        if (newMaximizedState) GetWindowMaximizeDispatcher().DispatchEvent();
                        maximized = newMaximizedState;
                    }
                }

                break;
            }
            case ConfigureNotify:
            {
                if (IsMinimized() || IsMaximized()) break;

                // Handle resizing
                {
                    Vector2UInt newSize = Vector2UInt(event.xconfigure.width, event.xconfigure.height);
                    if (size != newSize)
                    {
                        // This is just to check if size has not been set yet, because resize event is called upon creation, and we do not want it to be registered
                        if (size.y != std::numeric_limits<uint32>::max())
                        {
                            GetWindowResizeDispatcher().DispatchEvent(newSize);
                        }

                        size = newSize;
                        break;
                    }
                }

                // Handle moving
                {
                    Vector2Int newPosition = Vector2Int(event.xconfigure.x - extents.x, event.xconfigure.y - extents.z);
                    if (position != newPosition)
                    {
                        // This is just to check if position has not been set yet, because position event is called upon creation, and we do not want it to be registered
                        if (position.y != std::numeric_limits<int32>::max())
                        {
                            GetWindowMoveDispatcher().DispatchEvent(newPosition);
                        }

                        position = newPosition;
                        break;
                    }
                }

                break;
            }
            case ClientMessage:
            {
                if (filtered || event.xclient.message_type == None) break;

                if (event.xclient.message_type == linuxInstance.GetAtoms().WM_PROTOCOLS)
                {
                    Atom protocol = event.xclient.data.l[0];
                    if (protocol == None)
                    {
                        break;
                    }

                    // Handle window closing request
                    if (protocol == linuxInstance.GetAtoms().WM_DELETE_WINDOW)
                    {
                        GetWindowCloseDispatcher().DispatchEvent();
                        Close();
                    }
                        // Handle window pinging (tested by the window manager to see if it continues to responds to events)
                    else if (protocol == linuxInstance.GetAtoms().NET_WM_PING)
                    {
                        XEvent reply = event;
                        reply.xclient.window = linuxInstance.GetRootWindow();
                        XSendEvent(linuxInstance.GetDisplay(), linuxInstance.GetRootWindow(), False, SubstructureNotifyMask | SubstructureRedirectMask, &reply);
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
        }
    }

    /* --- DESTRUCTOR --- */

    X11Window::~X11Window()
    {
        if (closed) return;

        // Register window close event
        XEvent event;
        RegisterX11Event(linuxInstance.GetAtoms().WM_PROTOCOLS, linuxInstance.GetAtoms().WM_DELETE_WINDOW, CurrentTime);
        XNextEvent(linuxInstance.GetDisplay(), &event);

        // Handle closing event
        HandleX11Event(event);
        XFlush(linuxInstance.GetDisplay());
    }
}