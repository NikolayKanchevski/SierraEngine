//
// Created by Nikolay Kanchevski on 10.29.23.
//

#include "X11Context.h"

#include <unistd.h>
#include <X11/Xmd.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/XInput2.h>

#define NET_WM_STATE_ADD    1

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    X11Context::X11Context(const X11ContextCreateInfo &createInfo)
        : display(XOpenDisplay(nullptr)), screen((SR_ERROR_IF(display == nullptr, "Could not open X11 display!"), XDefaultScreen(display))), rootWindow(XRootWindow(display, screen)),
          xkbExtension(XkbExtension({ .display = display })), xrandrExtension(XrandrExtension({ .display = display }))
    {
        // Set error handler and initialize X11
        #if SR_ENABLE_LOGGING
            XSetErrorHandler([](Display* faultyDisplay, XErrorEvent* errorEvent)
            {
                // Retrieve error message
                char errorText[256];
                XGetErrorText(faultyDisplay, errorEvent->error_code, errorText, sizeof(errorText));

                // Log message
                SR_ERROR("An X11 error occurred: {0}", errorText);
                return 0;
            });
        #endif
        XInitThreads();

        // Declare supported atoms
        ulong supportedAtomCount = 0;
        Atom* supportedAtoms = nullptr;

        // Retrieve supported atoms
        atomTable[GetAtomTypeIndex(AtomType::NET_SUPPORTED)] = XInternAtom(display, "_NET_SUPPORTED", False);
        supportedAtomCount = GetWindowProperty(rootWindow, GetAtom(AtomType::NET_SUPPORTED), XA_ATOM, reinterpret_cast<uchar**>(&supportedAtoms));

        // Query support for each atom, that the application needs
        atomTable[GetAtomTypeIndex(AtomType::NET_SUPPORTING_WM_CHECK)]             = XInternAtom(display, "_NET_SUPPORTING_WM_CHECK", False);
        atomTable[GetAtomTypeIndex(AtomType::WM_PROTOCOLS)]                        = XInternAtom(display, "WM_PROTOCOLS", False);
        atomTable[GetAtomTypeIndex(AtomType::WM_STATE)]                            = XInternAtom(display, "WM_STATE", False);
        atomTable[GetAtomTypeIndex(AtomType::WM_DELETE_WINDOW)]                    = XInternAtom(display, "WM_DELETE_WINDOW", False);
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_NAME)]                         = XInternAtom(display, "_NET_WM_NAME", False);
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_ICON)]                         = XInternAtom(display, "_NET_WM_ICON", False);
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_PING)]                         = XInternAtom(display, "_NET_WM_PING", False);
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_PID)]                          = XInternAtom(display, "_NET_WM_PID", False);
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_ICON_NAME)]                    = XInternAtom(display, "_NET_WM_ICON_NAME", False);
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_BYPASS_COMPOSITOR)]            = XInternAtom(display, "_NET_WM_BYPASS_COMPOSITOR", False);
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_WINDOW_OPACITY)]               = XInternAtom(display, "_NET_WM_WINDOW_OPACITY", False);
        atomTable[GetAtomTypeIndex(AtomType::MOTIF_WM_HINTS)]                      = XInternAtom(display, "_MOTIF_WM_HINTS", False);
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_STATE)]                        = TryRetrieveAtom(supportedAtoms, supportedAtomCount, "_NET_WM_STATE");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_STATE_ABOVE)]                  = TryRetrieveAtom(supportedAtoms, supportedAtomCount, "_NET_WM_STATE_ABOVE");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_STATE_FULLSCREEN)]             = TryRetrieveAtom(supportedAtoms, supportedAtomCount, "_NET_WM_STATE_FULLSCREEN");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_STATE_MAXIMIZED_VERT)]         = TryRetrieveAtom(supportedAtoms, supportedAtomCount, "_NET_WM_STATE_MAXIMIZED_VERT");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_STATE_MAXIMIZED_HORZ)]         = TryRetrieveAtom(supportedAtoms, supportedAtomCount, "_NET_WM_STATE_MAXIMIZED_HORZ");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_STATE_DEMANDS_ATTENTION)]      = TryRetrieveAtom(supportedAtoms, supportedAtomCount, "_NET_WM_STATE_DEMANDS_ATTENTION");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_FULLSCREEN_MONITORS)]          = TryRetrieveAtom(supportedAtoms, supportedAtomCount, "_NET_WM_FULLSCREEN_MONITORS");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_CM_Sx)]                        = TryRetrieveAtom(supportedAtoms, supportedAtomCount, "_NET_WM_CM_Sx");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_WINDOW_TYPE)]                  = TryRetrieveAtom(supportedAtoms, supportedAtomCount, "_NET_WM_WINDOW_TYPE");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_WINDOW_TYPE_NORMAL)]           = TryRetrieveAtom(supportedAtoms, supportedAtomCount, "_NET_WM_WINDOW_TYPE_NORMAL");
        atomTable[GetAtomTypeIndex(AtomType::NET_WORKAREA)]                        = TryRetrieveAtom(supportedAtoms, supportedAtomCount, "_NET_WORKAREA");
        atomTable[GetAtomTypeIndex(AtomType::NET_CURRENT_DESKTOP)]                 = TryRetrieveAtom(supportedAtoms, supportedAtomCount, "_NET_CURRENT_DESKTOP");
        atomTable[GetAtomTypeIndex(AtomType::NET_ACTIVE_WINDOW)]                   = TryRetrieveAtom(supportedAtoms, supportedAtomCount, "_NET_ACTIVE_WINDOW");
        atomTable[GetAtomTypeIndex(AtomType::NET_FRAME_EXTENTS)]                   = TryRetrieveAtom(supportedAtoms, supportedAtomCount, "_NET_FRAME_EXTENTS");
        atomTable[GetAtomTypeIndex(AtomType::NET_REQUEST_FRAME_EXTENTS)]           = TryRetrieveAtom(supportedAtoms, supportedAtomCount, "_NET_REQUEST_FRAME_EXTENTS");

        // Deallocate atom data
        if (supportedAtomCount > 0) XFree(supportedAtoms);

        // Detect screens
        ReloadScreens();

        // Create invisible cursor
        XcursorImage* emptyCursorImage = XcursorImageCreate(1, 1);
        invisibleCursor = XcursorImageLoadCursor(display, emptyCursorImage);
        XcursorImageDestroy(emptyCursorImage);
    }

    /* --- POLLING METHODS --- */

    XID X11Context::CreateWindow(const std::string_view title, const uint32 width, const uint32 height) const
    {
        // Assign attributes
        XSetWindowAttributes setAttributes = { 0 };
        setAttributes.override_redirect = true;
        setAttributes.background_pixel = WhitePixel(display, screen);
        setAttributes.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | FocusChangeMask | VisibilityChangeMask | EnterWindowMask | LeaveWindowMask | PropertyChangeMask;

        // Create window
        XID window = XCreateWindow(
            display, rootWindow, 0, 0,
            width, height, 0, DefaultDepth(display, screen),
            InputOutput, DefaultVisual(display, screen), CWBackPixel | (setAttributes.event_mask != 0 ? CWEventMask : 0), &setAttributes
        );
        SR_ERROR_IF(window == 0, "Could not create X11 window!");

        // Set window manager protocols
        std::array<Atom, 2> protocols = { GetAtom(AtomType::WM_DELETE_WINDOW), GetAtom(AtomType::NET_WM_PING) };
        XSetWMProtocols(display, window, protocols.data(), protocols.size());

        // Set the process ID
        auto processID = getpid();
        XChangeProperty(display, window, GetAtom(AtomType::NET_WM_PID), XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<uchar*>(&processID), 1);

        // If window supports typing and the normal type, assign it
        if (GetAtom(AtomType::NET_WM_WINDOW_TYPE) && GetAtom(AtomType::NET_WM_WINDOW_TYPE_NORMAL))
        {
            Atom type = GetAtom(AtomType::NET_WM_WINDOW_TYPE_NORMAL);
            XChangeProperty(display, window, GetAtom(AtomType::NET_WM_WINDOW_TYPE), XA_ATOM, 32, PropModeReplace, reinterpret_cast<uchar*>(&type), 1);
        }

        // Handle window manager hints
        {
            XWMHints* wmHints = XAllocWMHints();
            SR_ERROR_IF(wmHints == nullptr, "Could not allocate X11 window manager hints!");

            wmHints->flags = StateHint;
            wmHints->initial_state = NormalState;
            XSetWMHints(display, window, wmHints);

            XFree(wmHints);
        }

        // Handle class hints
        {
            XClassHint* classHint = XAllocClassHint();
            char instanceName[12 + 1] = "SierraEngine";

            classHint->res_name = instanceName;
            XSetClassHint(display, window, classHint);

            XFree(classHint);
        }

        // Assign size hints
        {
            XSizeHints* sizeHints = XAllocSizeHints();

            sizeHints->flags |= PWinGravity;
            sizeHints->win_gravity = StaticGravity;
            XSetWMNormalHints(display, window, sizeHints);

            XFree(sizeHints);
        }

        // Set window title
        SetWindowTitle(window, title);
        Flush();

        return window;
    }

     void X11Context::DestroyWindow(const XID window) const
    {
        // Hide window away
        XUnmapWindow(display, window);
        Flush();
    }

    XEvent X11Context::QueryWindowDestruction(const XID window) const
    {
        // Register window closing
        XEvent event = RegisterWindowEvent(window, GetAtom(AtomType::WM_PROTOCOLS), GetAtom(AtomType::WM_DELETE_WINDOW), CurrentTime);
        return event;
    }

    bool X11Context::IsEventQueueEmpty() const
    {
        return QLength(display) == 0;
    }

    XEvent X11Context::PollNextEvent() const
    {
        XEvent event;
        XNextEvent(display, &event);
        return event;
    }

    XEvent X11Context::PeekNextEvent() const
    {
        XEvent event;
        XPeekEvent(display, &event);
        return event;
    }

    bool X11Context::IsEventFiltered(XEvent &event) const
    {
        if (XFilterEvent(&event, None)) return true;

        // Check if the given event is for a change in screens (not a window event, so pretend it is filtered)
        if (event.type == xrandrExtension.GetEventBase() + RRNotify)
        {
            ReloadScreens(&event);
            return true;
        }

        return false;
    }

    XEvent X11Context::WaitForEvent(void* eventData, int(EventCheck)(Display*, XEvent*, XPointer)) const
    {
        XEvent event;
        XIfEvent(display, &event, EventCheck, reinterpret_cast<XPointer>(eventData));
        return event;
    }

    XEvent X11Context::RegisterWindowEvent(const XID window, const Atom type, const long a, const long b, const long c, const long d, const int eventMask) const
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
        XSendEvent(display, window, False, eventMask, &event);
        return event;
    }

    XEvent X11Context::RegisterRootWindowEvent(const Atom type, const long a, const long b, const long c, const long d, const int eventMask) const
    {
        return RegisterWindowEvent(rootWindow, type, a, b, c, d, eventMask);
    }

    void X11Context::Flush() const
    {
        XFlush(display);
    }

    void X11Context::MinimizeWindow(const XID window) const
    {
        // Minimize window
        XIconifyWindow(display, window, screen);
        Flush();

        // Wait until window thread has registered window as minimized
        while (!IsWindowMinimized(window))
        {
            continue;
        }
    }

    void X11Context::MaximizeWindow(const XID window) const
    {
        // Manually maximize window (not sure why, but sending a maximize event while correctly registered, it does not in any way change the dimensions of the window)
        XMoveWindow(display, window, 0, 0);
        XResizeWindow(display, window, ScreenOfDisplay(display, screen)->width, ScreenOfDisplay(display, screen)->height);

        if (!GetAtom(AtomType::NET_WM_STATE) || !GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_VERT)  || !GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_HORZ))
        {
            SR_WARNING("Could not dispatch an X11 window maximize event, as the required atoms are unsupported!");
            return;
        }

        // Manually send out a maximize event
        {
            // Retrieve window states
            Atom* windowStates = nullptr;
            ulong windowStateCount = GetWindowProperty(window, GetAtom(AtomType::NET_WM_STATE), XA_ATOM, reinterpret_cast<uchar**>(&windowStates));

            // Check if all required states are present, and add these, which are not
            std::vector<Atom> missingStates = { GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_VERT), GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_HORZ) };
            for (uint32 i = windowStateCount; i--;)
            {
                for (auto iterator = missingStates.begin(); iterator != missingStates.end();)
                {
                    if (windowStates[i] == *iterator) iterator = missingStates.erase(iterator);
                    else iterator++;
                }
            }

            if (windowStates != nullptr) XFree(windowStates);

            // If no missing states are present, then window must be maximized, so we return
            if (missingStates.empty()) return;

            // Otherwise we apply the missing states
            XChangeProperty(display, window, GetAtom(AtomType::NET_WM_STATE), XA_ATOM, 32, PropModeAppend, reinterpret_cast<uchar*>(missingStates.data()), missingStates.size());
            Flush();
        }

        // Wait until request has been processed
        struct ConfigureNotifyEventData { XID window; } configureNotifyEventData = { .window = window };
        WaitForEvent(&configureNotifyEventData, [](Display*, XEvent* event, XPointer configureNotifyEventDataPtr) -> int
        {
            return event->type == ConfigureNotify && event->xproperty.window == reinterpret_cast<ConfigureNotifyEventData*>(configureNotifyEventDataPtr)->window;
        });
    }

    void X11Context::ShowWindow(const XID window) const
    {
        XMapWindow(display, window);
        Flush();
    }

    void X11Context::HideWindow(const XID window) const
    {
        XUnmapWindow(display, window);
        Flush();
    }

    void X11Context::FocusWindow(const XID window) const
    {
        if (GetAtom(AtomType::NET_ACTIVE_WINDOW))
        {
            RegisterWindowEvent(window, GetAtom(AtomType::NET_ACTIVE_WINDOW), NET_WM_STATE_ADD, 0, 0, 0, SubstructureNotifyMask | SubstructureRedirectMask);
        }
        else if (!IsWindowHidden(window))
        {
            XRaiseWindow(display, window);
            XSetInputFocus(display, window, RevertToParent, CurrentTime);
        }
        else
        {
            return;
        }

        Flush();
    }

    void X11Context::ShowWindowCursor(const XID window) const
    {
        // Clear invisible cursor
        XUndefineCursor(display, window);
    }

    void X11Context::HideWindowCursor(const XID window) const
    {
        // Assign invisible cursor
        XDefineCursor(display, window, invisibleCursor);
    }

    void X11Context::SetWindowCursorPosition(const XID window, const Vector2Int &position) const
    {
        XWarpPointer(display, None, window, 0, 0, 0, 0, position.x, position.y);
        Flush();
    }

    void X11Context::SetWindowTitle(const XID window, const std::string_view title) const
    {
        XStoreName(display, window, title.data());
    }

    void X11Context::SetWindowPosition(const XID window, const Vector2Int &position) const
    {
        XMoveWindow(display, window, position.x, position.y);
    }

    void X11Context::SetWindowSize(const XID window, const Vector2UInt &size) const
    {
        XResizeWindow(display, window, size.x, size.y);
    }

    void X11Context::SetWindowSizeLimits(const XID window, const Vector2UInt &minimumSize, const Vector2UInt &maximumSize) const
    {
        // Define size hints
        XSizeHints* sizeHints = XAllocSizeHints();
        sizeHints->flags |= PWinGravity | PMinSize | PMaxSize;
        sizeHints->win_gravity = StaticGravity;
        sizeHints->min_width = static_cast<int>(minimumSize.x);
        sizeHints->max_width = static_cast<int>(maximumSize.x);
        sizeHints->min_height = static_cast<int>(minimumSize.y);
        sizeHints->max_height = static_cast<int>(maximumSize.y);
        XSetWMNormalHints(display, window, sizeHints);

        // Free memory
        XFree(sizeHints);
        Flush();
    }

    void X11Context::SetWindowOpacity(const XID window, const float32 opacity) const
    {
        CARD32 value = static_cast<CARD32>(std::numeric_limits<uint32>::max() * static_cast<float64>(opacity));
        XChangeProperty(display, window, GetAtom(AtomType::NET_WM_WINDOW_OPACITY), XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<uchar*>(&value), 1);
    }

    /* --- GETTER METHODS --- */

    const X11Screen& X11Context::GetPrimaryScreen() const
    {
        return screens[0];
    }

    const X11Screen& X11Context::GetWindowScreen(const XID window) const
    {
        const Vector2Int windowPosition = GetWindowPosition(window);
        for (const X11Screen &screen : screens)
        {
            const Vector2Int screenOrigin = screen.GetOrigin();
            const Vector2Int screenSize = { screen.GetWidth(), screen.GetHeight() };
            if (windowPosition.x >= screenOrigin.x && windowPosition.x <= screenOrigin.x + screenSize.x && windowPosition.y >= screenOrigin.y && windowPosition.y <= screenOrigin.y + screenSize.y)
            {
                return screen;
            }
        }
        return screens[0];
    }

    std::optional<XID> X11Context::GetFocusedWindow() const
    {
        XID focusedWindow;
        int focusedWindowState;
        XGetInputFocus(display, &focusedWindow, &focusedWindowState);
        if (focusedWindow == 0) return std::nullopt;
        return focusedWindow;
    }

    std::string X11Context::GetWindowTitle(const XID window) const
    {
        char* title = nullptr;
        XFetchName(display, window, &title);

        auto result = std::string(title);
        XFree(title);

        return result;
    }

    Vector2Int X11Context::GetWindowPosition(const XID window) const
    {
        XID child;
        int xPosition, yPosition;
        XWindowAttributes windowAttributes;
        XTranslateCoordinates(display, window, rootWindow, 0, 0, &xPosition, &yPosition, &child);
        XGetWindowAttributes(display, window, &windowAttributes);
        return { xPosition, yPosition };
    }

    Vector2UInt X11Context::GetWindowSize(const XID window) const
    {
        XID child;
        int xPosition, yPosition;
        XWindowAttributes windowAttributes;
        XTranslateCoordinates(display, window, rootWindow, 0, 0, &xPosition, &yPosition, &child);
        XGetWindowAttributes(display, window, &windowAttributes);
        return { windowAttributes.width, windowAttributes.height };
    }

    float32 X11Context::GetWindowOpacity(const XID window) const
    {
        float32 opacity = 1.0f;

        CARD32* value = nullptr;
        if (GetWindowProperty(window, GetAtom(AtomType::NET_WM_WINDOW_OPACITY), XA_CARDINAL, reinterpret_cast<uchar**>(&value)))
        {
            opacity = static_cast<float32>(*value / static_cast<float64>(std::numeric_limits<uint32>::max()));
            XFree(value);
        }

        return opacity;
    }

    bool X11Context::IsWindowMinimized(const XID window) const
    {
        int32 result = WithdrawnState;
        struct
        {
            CARD32 state;
            XID icon;
        }* state = nullptr;

        if (GetWindowProperty(window, GetAtom(AtomType::WM_STATE), GetAtom(AtomType::WM_STATE), reinterpret_cast<uchar**>(&state)) >= 2)
        {
            result = state->state;
        }

        if (state != nullptr) XFree(state);
        return result == IconicState;
    }

    bool X11Context::IsWindowMaximized(const XID window) const
    {
        if (!GetAtom(AtomType::NET_WM_STATE) || !GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_VERT) || !GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_HORZ))
        {
            return false;
        }

        Atom* states;
        ulong stateCount = GetWindowProperty(window, GetAtom(AtomType::NET_WM_STATE), XA_ATOM, reinterpret_cast<uchar**>(&states));

        bool result = false;
        for (ulong i = stateCount; i--;)
        {
            if (states[i] == GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_VERT) || states[i] == GetAtom(AtomType::NET_WM_STATE_MAXIMIZED_HORZ))
            {
                result = true;
                break;
            }
        }

        if (states != nullptr) XFree(states);
        return result;
    }

    bool X11Context::IsWindowFocused(const XID window) const
    {
        XID focusedWindow;
        int focusedWindowState;
        XGetInputFocus(display, &focusedWindow, &focusedWindowState);
        return window == focusedWindow;
    }

    bool X11Context::IsWindowHidden(const XID window) const
    {
        XWindowAttributes windowAttributes;
        XGetWindowAttributes(display, window, &windowAttributes);
        return windowAttributes.map_state != IsViewable;
    }

    Vector2Int X11Context::GetWindowCursorPosition(const XID window) const
    {
        XID rootWindow, childWindow;
        int xRoot, yRoot;
        int xChild, yChild;
        uint mask;
        XQueryPointer(display, window, &rootWindow, &childWindow, &xRoot, &yRoot, &xChild, &yChild, &mask);
        return { xChild, yChild };
    }

    Vector4UInt X11Context::GetWindowExtents(const XID window) const
    {
        Vector4UInt extents;
        if (GetAtom(AtomType::NET_FRAME_EXTENTS))
        {
            // Temporarily show window
            bool windowWasHidden = IsWindowHidden(window);
            ShowWindow(window);

            // Register extent request
            XEvent event;
            RegisterWindowEvent(window, GetAtom(AtomType::NET_REQUEST_FRAME_EXTENTS));

            // Construct data struct to use when checking events
            struct ExtentEventData
            {
                XID window;
                Atom frameExtentsAtom;
            };
            ExtentEventData extentEventData = { .window = window, .frameExtentsAtom = GetAtom(AtomType::NET_FRAME_EXTENTS) };

            // Wait until request has been processed
            WaitForEvent(&extentEventData, [](Display*, XEvent* event, XPointer extentEventDataPtr) -> int
            {
                return event->type == PropertyNotify && event->xproperty.state == PropertyNewValue && event->xproperty.window == reinterpret_cast<ExtentEventData*>(extentEventDataPtr)->window && event->xproperty.atom == reinterpret_cast<ExtentEventData*>(extentEventDataPtr)->frameExtentsAtom;
            });

            // Save extent data
            Atom* extentData = nullptr;
            if (GetWindowProperty(window, GetAtom(AtomType::NET_FRAME_EXTENTS), XA_CARDINAL, reinterpret_cast<uchar**>(&extentData)) == 4)
            {
                extents.x = static_cast<uint32>(extentData[0]);
                extents.y = static_cast<uint32>(extentData[1]);
                extents.z = static_cast<uint32>(extentData[2]);
                extents.w = static_cast<uint32>(extentData[3]);
                XFree(extentData);
            }

            // Hide window back
            if (windowWasHidden) HideWindow(window);

            // Discard all events, that have occurred during extent extraction
            XSync(display, False);
            while (!IsEventQueueEmpty())
            {
                PollNextEvent();
            }
        }

        return extents;
    }

    /* --- PRIVATE METHODS --- */

    Atom X11Context::TryRetrieveAtom(const Atom* supportedAtoms, const ulong atomCount, const char* atomName)
    {
        // Retrieve atom
        Atom atom = XInternAtom(display, atomName, False);

        // Check if it is supported and return it
        for (uint32 i = atomCount; i--;)
        {
            if (supportedAtoms[i] == atom) return atom;
        }

        return None;
    }

    void X11Context::ReloadScreens(XEvent* notifyEvent) const
    {
        // Clear screens
        screens.clear();
        if (notifyEvent != nullptr) XRRUpdateConfiguration(notifyEvent);

        // Get work area extents
        Vector4UInt workAreaExtents = { 0, 0, 0, 0 };
        if (GetAtom(AtomType::NET_WORKAREA))
        {
            Atom* workAreaExtentData = nullptr;
            if (GetWindowProperty(rootWindow, GetAtom(AtomType::NET_WORKAREA), XA_CARDINAL, reinterpret_cast<uchar**>(&workAreaExtentData)))
            {
                workAreaExtents.x = static_cast<int32>(workAreaExtentData[0]);
                workAreaExtents.y = static_cast<int32>(workAreaExtentData[1]);
                workAreaExtents.z = static_cast<int32>(workAreaExtentData[2]);
                workAreaExtents.w = static_cast<int32>(workAreaExtentData[3]);
                XFree(workAreaExtentData);
            }
        }

        // Load screens
        XRRScreenResources* screenResources = XRRGetScreenResources(display, rootWindow);
        for (uint32 i = 0; i < screenResources->ncrtc; i++)
        {
            XRRCrtcInfo* crtcInfo = XRRGetCrtcInfo(display, screenResources, screenResources->crtcs[i]);
            if (crtcInfo->noutput > 0)
            {
                XRROutputInfo* outputInfo = XRRGetOutputInfo(display, screenResources, crtcInfo->outputs[0]);
                screens.emplace_back(X11ScreenCreateInfo{ .screenResources = screenResources, .crtcInfo = crtcInfo, .outputInfo = outputInfo, .workAreaExtents = workAreaExtents });
                XRRFreeOutputInfo(outputInfo);
            }
            XRRFreeCrtcInfo(crtcInfo);
        }
        XRRFreeScreenResources(screenResources);
    }

    int32 X11Context::GetWindowProperty(const XID window, const Atom property, const Atom type, uchar** value) const
    {
        // Arbitrary values, needed to call XGetWindowProperty()
        Atom actualType;
        int actualFormat;
        ulong itemCount;
        ulong bytesAfter;

        // Get the requested property
        XGetWindowProperty(display, window, property, 0, LONG_MAX, False, type, &actualType, &actualFormat, &itemCount, &bytesAfter, value);
        return static_cast<int32>(itemCount);
    }

    /* --- DESTRUCTOR --- */

    X11Context::~X11Context()
    {
        // Close display
        XCloseDisplay(display);
        display = nullptr;

        // Invalidate objects relying on the display
        rootWindow = 0;
        screen = 0;
    }

}