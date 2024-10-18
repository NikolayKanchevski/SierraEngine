//
// Created by Nikolay Kanchevski on 10.29.23.
//

#include "X11Context.h"

#include <unistd.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/extensions/Xrandr.h>

namespace Sierra
{

    namespace
    {
        struct WindowEntry
        {
            XID window = 0;
            X11Context::WindowEventCallback Callback = { };
            void* userData = nullptr;
        };
        std::vector<WindowEntry> windowMap = { };

        Atom TryRetrieveAtom(Display* display, const Atom* supportedAtoms, const ulong atomCount, const char* atomName)
        {
            // Retrieve atom
            Atom atom = XInternAtom(display, atomName, False);

            // Check if it is supported and return it
            for (size i = atomCount; i--;)
            {
                if (supportedAtoms[i] == atom) return atom;
            }

            return None;
        }
    }

    /* --- CONSTRUCTORS --- */

    X11Context::X11Context(const X11ContextCreateInfo& createInfo)
        : display(createInfo.display), screen(XDefaultScreen(display)), rootWindow(XRootWindow(display, screen)),
          xkbExtension({ .display = display }), xrandrExtension({ .display = display })
    {
        XInitThreads();
        XrmInitialize();

        #if SR_ENABLE_LOGGING
            XSetErrorHandler([](Display* faultyDisplay, XErrorEvent* errorEvent) -> int
            {
                // Retrieve error message
                std::array<char, 256> error = { };
                XGetErrorText(faultyDisplay, errorEvent->error_code, error.data(), error.size());

                // Log message
                SR_ERROR("X11 - {0}", std::string_view(error.data(), error.size()));
                return 0;
            });
        #endif

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
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_STATE)]                        = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_WM_STATE");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_STATE_ABOVE)]                  = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_WM_STATE_ABOVE");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_STATE_FULLSCREEN)]             = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_WM_STATE_FULLSCREEN");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_STATE_MAXIMIZED_VERT)]         = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_WM_STATE_MAXIMIZED_VERT");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_STATE_MAXIMIZED_HORZ)]         = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_WM_STATE_MAXIMIZED_HORZ");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_STATE_DEMANDS_ATTENTION)]      = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_WM_STATE_DEMANDS_ATTENTION");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_FULLSCREEN_MONITORS)]          = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_WM_FULLSCREEN_MONITORS");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_CM_Sx)]                        = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_WM_CM_Sx");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_WINDOW_TYPE)]                  = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_WM_WINDOW_TYPE");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_WINDOW_TYPE_NORMAL)]           = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_WM_WINDOW_TYPE_NORMAL");
        atomTable[GetAtomTypeIndex(AtomType::NET_WORKAREA)]                        = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_WORKAREA");
        atomTable[GetAtomTypeIndex(AtomType::NET_CURRENT_DESKTOP)]                 = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_CURRENT_DESKTOP");
        atomTable[GetAtomTypeIndex(AtomType::NET_ACTIVE_WINDOW)]                   = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_ACTIVE_WINDOW");
        atomTable[GetAtomTypeIndex(AtomType::NET_FRAME_EXTENTS)]                   = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_FRAME_EXTENTS");
        atomTable[GetAtomTypeIndex(AtomType::NET_REQUEST_FRAME_EXTENTS)]           = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_REQUEST_FRAME_EXTENTS");

        // Deallocate atom data
        if (supportedAtomCount > 0) XFree(supportedAtoms);

        // Detect screens
        ReloadScreens();

        // Create invisible cursor
        XcursorImage* const emptyCursorImage = XcursorImageCreate(1, 1);
        invisibleCursor = XcursorImageLoadCursor(display, emptyCursorImage);
        XcursorImageDestroy(emptyCursorImage);
    }

    /* --- POLLING METHODS --- */

    XID X11Context::CreateWindow(const std::string_view title, const uint32 width, const uint32 height, const WindowEventCallback& Callback, void* userData) const
    {
        // Assign attributes
        XSetWindowAttributes setAttributes = { 0 };
        setAttributes.override_redirect = true;
        setAttributes.background_pixel = WhitePixel(display, screen);
        setAttributes.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | FocusChangeMask | VisibilityChangeMask | EnterWindowMask | LeaveWindowMask | PropertyChangeMask;

        // Create window
        const XID window = XCreateWindow(
            display, rootWindow, 0, 0,
            width, height, 0, DefaultDepth(display, screen),
            InputOutput, DefaultVisual(display, screen), CWBackPixel | CWBorderPixel | CWEventMask, &setAttributes
        );

        // Set window manager protocols
        std::array<Atom, 2> protocols = { GetAtom(AtomType::WM_DELETE_WINDOW), GetAtom(AtomType::NET_WM_PING) };
        XSetWMProtocols(display, window, protocols.data(), protocols.size());

        // Set the process ID
        pid_t processID = getpid();
        XChangeProperty(display, window, GetAtom(AtomType::NET_WM_PID), XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<uchar*>(&processID), 1);

        // If window supports typing and the normal type, assign it
        if (GetAtom(AtomType::NET_WM_WINDOW_TYPE) && GetAtom(AtomType::NET_WM_WINDOW_TYPE_NORMAL))
        {
            Atom type = GetAtom(AtomType::NET_WM_WINDOW_TYPE_NORMAL);
            XChangeProperty(display, window, GetAtom(AtomType::NET_WM_WINDOW_TYPE), XA_ATOM, 32, PropModeReplace, reinterpret_cast<uchar*>(&type), 1);
        }

        // Set window manager hints
        {
            const std::unique_ptr<XWMHints, std::function<void(void*)>> windowManagerHints = { XAllocWMHints(), XFree };
            windowManagerHints->flags = StateHint;
            windowManagerHints->initial_state = NormalState;
            XSetWMHints(display, window, windowManagerHints.get());
        }

        // Set window manager normal hints
        {
            const std::unique_ptr<XSizeHints, std::function<void(void*)>> sizeHints = { XAllocSizeHints(), XFree };
            sizeHints->flags |= PWinGravity;
            sizeHints->win_gravity = StaticGravity;
            XSetWMNormalHints(display, window, sizeHints.get());
        }

        // Set class hint
        {
            const std::unique_ptr<XClassHint, std::function<void(void*)>> classHint = { XAllocClassHint(), XFree };
            char instanceName[12 + 1] = "SierraEngine";
            classHint->res_name = instanceName;
            XSetClassHint(display, window, classHint.get());
        }

        // Set window title
        XStoreName(display, window, title.data());
        XFlush(display);

        const WindowEntry windowEntry
        {
            .window = window,
            .Callback = Callback,
            .userData = userData
        };
        windowMap.emplace_back(windowEntry);

        return window;
    }

     void X11Context::DestroyWindow(const XID window) const
    {
        // Hide window away
        XUnmapWindow(display, window);
        XFlush(display);
    }

    void X11Context::ReloadScreens()
    {
        screens.clear();

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
        std::unique_ptr<XRRScreenResources, std::function<void(XRRScreenResources*)>> screenResources = { XRRGetScreenResources(display, rootWindow), XRRFreeScreenResources };
        for (size i = 0; i < screenResources->ncrtc; i++)
        {
            std::unique_ptr<XRRCrtcInfo, std::function<void(XRRCrtcInfo*)>> crtcInfo = { XRRGetCrtcInfo(display, screenResources.get(), screenResources->crtcs[i]), XRRFreeCrtcInfo };
            if (crtcInfo->noutput > 0)
            {
                std::unique_ptr<XRROutputInfo, std::function<void(XRROutputInfo*)>> outputInfo = { XRRGetOutputInfo(display, screenResources.get(), crtcInfo->outputs[0]), XRRFreeOutputInfo };
                screens.emplace_back(X11ScreenCreateInfo{ .screenResources = screenResources.get(), .crtcInfo = crtcInfo.get(), .outputInfo = outputInfo.get(), .workAreaExtents = workAreaExtents });
            }
        }
    }

    XEvent X11Context::PeekNextEvent() const
    {
        XEvent event = { };
        XPeekEvent(display, &event);
        return event;
    }

    XEvent X11Context::WaitForEvent(int(Callback)(Display*, XEvent*, XPointer), void* userData) const
    {
        XEvent event = { };
        XIfEvent(display, &event, Callback, reinterpret_cast<XPointer>(userData));
        return event;
    }

    XEvent X11Context::RegisterWindowEvent(const XID window, const Atom type, const long a, const long b, const long c, const long d, const int eventMask) const
    {
        XEvent event = { };
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

    /* --- GETTER METHODS --- */

    X11Screen& X11Context::GetWindowScreen(const XID window)
    {
        return const_cast<X11Screen&>(const_cast<const X11Context*>(this)->GetWindowScreen(window));
    }

    const X11Screen& X11Context::GetWindowScreen(XID window) const
    {
        XID child;
        int xPosition, yPosition;
        XWindowAttributes windowAttributes;
        XTranslateCoordinates(display, window, rootWindow, 0, 0, &xPosition, &yPosition, &child);
        XGetWindowAttributes(display, window, &windowAttributes);

        for (const X11Screen& screen : screens)
        {
            const Vector2Int screenOrigin = screen.GetOrigin();
            const Vector2Int screenSize = { screen.GetWidth(), screen.GetHeight() };
            if (xPosition >= screenOrigin.x && xPosition <= screenOrigin.x + screenSize.x && yPosition >= screenOrigin.y && yPosition <= screenOrigin.y + screenSize.y)
            {
                return screen;
            }
        }

        return screens[0];
    }

    uint32 X11Context::GetWindowProperty(const XID window, const Atom property, const Atom type, uchar** outValues) const noexcept
    {
        // Arbitrary values, needed to call XGetWindowProperty()
        Atom actualType;
        int actualFormat;
        ulong itemCount, bytesAfter;

        // Get the requested property
        XGetWindowProperty(display, window, property, 0, std::numeric_limits<long>::max(), False, type, &actualType, &actualFormat, &itemCount, &bytesAfter, outValues);
        return static_cast<uint32>(itemCount);
    }

    /* --- PRIVATE METHODS --- */

    void X11Context::Update()
    {
        while (QLength(display) > 0)
        {
            XEvent event = { };
            XNextEvent(display, &event);

            if (XFilterEvent(&event, None) || event.xany.window == rootWindow)
            {
                continue;
            }

            if (event.type == xrandrExtension.GetEventBase() + RRNotify)
            {
                XRRUpdateConfiguration(&event);
                ReloadScreens();
                continue;
            }

            for (const WindowEntry& windowEntry : windowMap)
            {
                windowEntry.Callback(event, windowEntry.window, windowEntry.userData);
            }
        }
    }

    /* --- DESTRUCTOR --- */

    X11Context::~X11Context() noexcept
    {
        XCloseDisplay(display);
    }

}