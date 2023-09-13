//
// Created by Nikolay Kanchevski on 9.1.23.
//

#include <X11/Xmd.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "X11Instance.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    X11Instance::X11Instance()
    {
        // Set error handler and initialize X11
        #if SR_DEBUG
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

        // Create objects
        {
            // Create display
            display = XOpenDisplay(nullptr);
            SR_ERROR_IF(display == nullptr, "Could not open X11 display!");

            // Create rest of required objects
            screen = DefaultScreen(display);
            rootWindow = RootWindow(display, screen);
            defaultVisual = DefaultVisual(display, screen);

        }

        // Get atoms
        {
            // Retrieve supported atoms
            atoms.NET_SUPPORTED = XInternAtom(display, "_NET_SUPPORTED", False);
            Atom *supportedAtoms = NULL;
            ulong supportedAtomCount = GetWindowProperty(rootWindow, atoms.NET_SUPPORTED, XA_ATOM, reinterpret_cast<uchar **>(&supportedAtoms));

            // Query support for each atom, that the application needs
            atoms.NET_SUPPORTING_WM_CHECK             = XInternAtom(display, "_NET_SUPPORTING_WM_CHECK", False);
            atoms.WM_PROTOCOLS                        = XInternAtom(display, "WM_PROTOCOLS", False);
            atoms.WM_STATE                            = XInternAtom(display, "WM_STATE", False);
            atoms.WM_DELETE_WINDOW                    = XInternAtom(display, "WM_DELETE_WINDOW", False);
            atoms.NET_WM_NAME                         = XInternAtom(display, "_NET_WM_NAME", False);
            atoms.NET_WM_ICON                         = XInternAtom(display, "_NET_WM_ICON", False);
            atoms.NET_WM_PING                         = XInternAtom(display, "_NET_WM_PING", False);
            atoms.NET_WM_PID                          = XInternAtom(display, "_NET_WM_PID", False);
            atoms.NET_WM_ICON_NAME                    = XInternAtom(display, "_NET_WM_ICON_NAME", False);
            atoms.NET_WM_BYPASS_COMPOSITOR            = XInternAtom(display, "_NET_WM_BYPASS_COMPOSITOR", False);
            atoms.NET_WM_WINDOW_OPACITY               = XInternAtom(display, "_NET_WM_WINDOW_OPACITY", False);
            atoms.MOTIF_WM_HINTS                      = XInternAtom(display, "_MOTIF_WM_HINTS", False);

            atoms.NET_WM_STATE                        = GetAtom(supportedAtoms, supportedAtomCount, "_NET_WM_STATE");
            atoms.NET_WM_STATE_ABOVE                  = GetAtom(supportedAtoms, supportedAtomCount, "_NET_WM_STATE_ABOVE");
            atoms.NET_WM_STATE_FULLSCREEN             = GetAtom(supportedAtoms, supportedAtomCount, "_NET_WM_STATE_FULLSCREEN");
            atoms.NET_WM_STATE_MAXIMIZED_VERT         = GetAtom(supportedAtoms, supportedAtomCount, "_NET_WM_STATE_MAXIMIZED_VERT");
            atoms.NET_WM_STATE_MAXIMIZED_HORZ         = GetAtom(supportedAtoms, supportedAtomCount, "_NET_WM_STATE_MAXIMIZED_HORZ");
            atoms.NET_WM_STATE_DEMANDS_ATTENTION      = GetAtom(supportedAtoms, supportedAtomCount, "_NET_WM_STATE_DEMANDS_ATTENTION");
            atoms.NET_WM_FULLSCREEN_MONITORS          = GetAtom(supportedAtoms, supportedAtomCount, "_NET_WM_FULLSCREEN_MONITORS");
            atoms.NET_WM_WINDOW_TYPE                  = GetAtom(supportedAtoms, supportedAtomCount, "_NET_WM_WINDOW_TYPE");
            atoms.NET_WM_WINDOW_TYPE_NORMAL           = GetAtom(supportedAtoms, supportedAtomCount, "_NET_WM_WINDOW_TYPE_NORMAL");
            atoms.NET_WORKAREA                        = GetAtom(supportedAtoms, supportedAtomCount, "_NET_WORKAREA");
            atoms.NET_CURRENT_DESKTOP                 = GetAtom(supportedAtoms, supportedAtomCount, "_NET_CURRENT_DESKTOP");
            atoms.NET_ACTIVE_WINDOW                   = GetAtom(supportedAtoms, supportedAtomCount, "_NET_ACTIVE_WINDOW");
            atoms.NET_FRAME_EXTENTS                   = GetAtom(supportedAtoms, supportedAtomCount, "_NET_FRAME_EXTENTS");
            atoms.NET_REQUEST_FRAME_EXTENTS           = GetAtom(supportedAtoms, supportedAtomCount, "_NET_REQUEST_FRAME_EXTENTS");

            // Deallocate atom data
            if (supportedAtomCount != 0) XFree(supportedAtoms);
        }

        // Create a temporary XID to extract frame extents (if supported)
        {
            if (atoms.NET_FRAME_EXTENTS)
            {
                // Create temporary window
                XID temporaryWindow = CreateWindow("%", 0, 0, 1, 1, false, false, PropertyChangeMask);

                // Show temporary window
                XMapWindow(display, temporaryWindow);

                // Register extent request
                XEvent event;
                RegisterWindowEvent(temporaryWindow, atoms.NET_REQUEST_FRAME_EXTENTS, 0, 0, 0, 0, 0);

                // Create lambda to check wether a given event is of type NET_FRAME_EXTENTS
                static const Atom NET_FRAME_EXTENTS_REFERENCE = atoms.NET_FRAME_EXTENTS;
                auto IsFrameExtentEventLambda = [](Display* display, XEvent* event, XPointer windowPtr) -> Bool
                {
                    return static_cast<Bool>(event->type == PropertyNotify && event->xproperty.state == PropertyNewValue && event->xproperty.window == *reinterpret_cast<Window*>(windowPtr) && event->xproperty.atom == NET_FRAME_EXTENTS_REFERENCE);
                };

                // Wait until request was processed
                while (!XCheckIfEvent(display, &event, IsFrameExtentEventLambda, reinterpret_cast<XPointer>(&temporaryWindow)))
                {
                    continue;
                }

                // Save extent data
                long* extentData = nullptr;
                if (GetWindowProperty(temporaryWindow, atoms.NET_FRAME_EXTENTS, XA_CARDINAL, reinterpret_cast<uchar**>(&extentData)) == 4)
                {
                    windowExtents.x = extentData[0];
                    windowExtents.y = extentData[1];
                    windowExtents.z = extentData[2];
                    windowExtents.w = extentData[3];
                    XFree(extentData);
                }

                // Hide & destroy temporary window
                CloseWindow(temporaryWindow);
            }
        }
    }

    UniquePtr<X11Instance> X11Instance::Create()
    {
        return std::make_unique<X11Instance>();
    }

    /* --- SETTER METHODS --- */

    XID X11Instance::CreateWindow(const char* title, const int xPosition, const int yPosition, const uint width, const uint height, const bool resizable, const bool maximize, const ulong eventMask)
    {
        // Assign attributes
        XSetWindowAttributes setAttributes = { 0 };
        setAttributes.override_redirect = true;
        setAttributes.background_pixel = WhitePixel(display, screen);
        setAttributes.event_mask = eventMask;

        // Create window
        XID window = XCreateWindow(
            display, rootWindow, xPosition, yPosition,
            width, height, 0, DefaultDepth(display, screen),
            InputOutput, defaultVisual, CWBackPixel | (eventMask != 0 ? CWEventMask : 0), &setAttributes
        );
        SR_ERROR_IF(window == 0, "Could not create X11 window!");

        // Set window state to be lastMaximized
        if (atoms.NET_WM_STATE)
        {
            Atom states[3];
            int stateCount = 0;

            if (maximize)
            {
                if (atoms.NET_WM_STATE_MAXIMIZED_VERT && atoms.NET_WM_STATE_MAXIMIZED_HORZ)
                {
                    states[stateCount++] = atoms.NET_WM_STATE_MAXIMIZED_VERT;
                    states[stateCount++] = atoms.NET_WM_STATE_MAXIMIZED_HORZ;
                }
                else
                {
                    SR_WARNING("Could not request a maximize operation on an X11 window, that does not support it!");
                }
            }

            if (stateCount != 0) XChangeProperty(display, window, atoms.NET_WM_STATE, XA_ATOM, 32, PropModeReplace, reinterpret_cast<uchar*>(states), stateCount);
        }

        // Set window manager protocols
        Atom protocols[] = { atoms.WM_DELETE_WINDOW, atoms.NET_WM_PING };
        XSetWMProtocols(display, window, protocols, sizeof(protocols) / sizeof(Atom));

        // Set the process ID
        int64 processID = getpid();
        XChangeProperty(display, window, atoms.NET_WM_PID, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<uchar*>(&processID), 1);

        // If window supports typing and the normal type, assign it
        if (atoms.NET_WM_WINDOW_TYPE && atoms.NET_WM_WINDOW_TYPE_NORMAL)
        {
            Atom type = atoms.NET_WM_WINDOW_TYPE_NORMAL;
            XChangeProperty(display, window, atoms.NET_WM_WINDOW_TYPE, XA_ATOM, 32, PropModeReplace, reinterpret_cast<uchar*>(&type), 1);
        }

        // Handle OS' window manager hints
        {
            XWMHints* wmHints = XAllocWMHints();
            SR_ERROR_IF(wmHints == nullptr, "Could not allocate X11 window manager hints!");

            wmHints->flags = StateHint;
            wmHints->initial_state = NormalState;

            XSetWMHints(display, window, wmHints);
            XFree(wmHints);
        }

        // Handle lastSize hints
        {
            XSizeHints* sizeHints = XAllocSizeHints();
            SR_ERROR_IF(sizeHints == nullptr, "Could not allocate X11 lastSize hints");

            if (!resizable)
            {
                sizeHints->flags |= (PMinSize | PMaxSize);
                sizeHints->min_width = sizeHints->max_width = width;
                sizeHints->min_height = sizeHints->max_height = height;
            }

            sizeHints->flags |= PWinGravity;
            sizeHints->win_gravity = StaticGravity;

            XSetWMNormalHints(display, window, sizeHints);
            XFree(sizeHints);
        }

        // Get instance name
        char instanceName[12 + 1];
        std::snprintf(instanceName, sizeof(instanceName), "%s", "SierraEngine");

        // Handle class hints
        {
            XClassHint* classHint = XAllocClassHint();
            classHint->res_name = instanceName;

            XSetClassHint(display, window, classHint);
            XFree(classHint);
        }

        // Set window title
        XStoreName(display, window, title);
        XFlush(display);

        return window;
    }

    void X11Instance::CloseWindow(const XID window)
    {
        // Register XID closing
        RegisterWindowEvent(window, atoms.WM_PROTOCOLS, atoms.WM_DELETE_WINDOW, CurrentTime, 0, 0, NoEventMask);

        // Hide and destroy window
        XUnmapWindow(display, window);
        XDestroyWindow(display, window);

        // Remove XID from unhandled event map
        auto iterator = unhandledEvents.find(window);
        if (iterator != unhandledEvents.end())
        {
            unhandledEvents.erase(iterator);
        }

        // Dispatch all queried X11 events
        XFlush(display);
    }

    void X11Instance::RegisterWindowEvent(const XID window, const Atom type, const long a, const long b, const long c, const long d, const int eventMask)
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
    }

    void X11Instance::RegisterUnhandledWindowEvent(const XEvent &event)
    {
        unhandledEvents[event.xclient.window].push_back(event);
    }

    /* --- GETTER METHODS --- */

    int32 X11Instance::GetWindowProperty(const XID window, const Atom property, const Atom type, uchar** value)
    {
        // Arbitrary values, so the method below can be called
        Atom actualType;
        int actualFormat;
        ulong itemCount;
        ulong bytesAfter;

        // Get the requested property
        XGetWindowProperty(display, window, property, 0, LONG_MAX, False, type, &actualType, &actualFormat, &itemCount, &bytesAfter, value);
        return itemCount;
    }

    std::vector<XEvent> X11Instance::GetUnhandledEventsForWindow(const XID window)
    {
        std::vector<XEvent> events = { };

        auto iterator = unhandledEvents.find(window);
        if (iterator != unhandledEvents.end())
        {
            events = iterator->second;
            iterator->second.clear();
        }

        return events;
    }

    /* --- PRIVATE METHODS --- */

    Atom X11Instance::GetAtom(const Atom *supportedAtoms, const ulong atomCount, const char *atomName)
    {
        // Retrieve atom
        Atom atom = XInternAtom(display, atomName, False);

        // Check if it is supported and return it
        for (ulong i = atomCount; i--;)
        {
            if (supportedAtoms[i] == atom) return atom;
        }

        return None;
    }

    /* --- DESTRUCTOR --- */

    X11Instance::~X11Instance()
    {
        XCloseDisplay(display);
        display = nullptr;
    }

}