//
// Created by Nikolay Kanchevski on 10.29.23.
//

#include <X11/Xatom.h>
#include <X11/Xcursor/Xcursor.h>

#include "X11Window.h"

namespace Sierra
{

    namespace
    {
        [[nodiscard]] constexpr uint8 GetAtomTypeIndex(const AtomType atomType)
        {
            return static_cast<uint32>(atomType);
        }

        Atom TryRetrieveAtom(Display* display, const Atom* supportedAtoms, const ulong atomCount, const char* atomName)
        {
            // Retrieve atom
            const Atom atom = XInternAtom(display, atomName, False);

            // Check if it is supported and return it
            for (size i = atomCount; i--;)
            {
                if (supportedAtoms[i] == atom) return atom;
            }

            return None;
        }

        // Longest property, that is read in a single request, measured in 32-bit units
        constexpr long MAX_PROPERTY_LENGTH = 1 << 20;
    }

    namespace
    {
        // Scale of the desktop is announced through the dpi, which is 96 on an unscaled desktop. Fractional scales are snapped to 1/120th, as they are on Wayland
        float32 ReadDesktopScaling(Display* display) noexcept
        {
            constexpr float32 BASE_DPI = 96.0f;
            constexpr float32 SCALE_STEPS = 120.0f;

            const char* dpiValue = XGetDefault(display, "Xft", "dpi");
            if (dpiValue == nullptr) return 1.0f;

            const float32 dpi = std::strtof(dpiValue, nullptr);
            if (dpi <= 0.0f) return 1.0f;

            // Scales below 1 are not used, as swapchains never report them
            return glm::clamp(glm::round(dpi / BASE_DPI * SCALE_STEPS) / SCALE_STEPS, 1.0f, 4.0f);
        }
    }

    /* --- CONSTRUCTORS --- */

    X11Context::X11Context(const X11ContextCreateInfo& createInfo)
        : display(createInfo.display), screen(XDefaultScreen(display)), rootWindow(XRootWindow(display, screen)), xkbExtension({ .display = display }), xrandrExtension({ .display = display })
    {
        XrmInitialize();

        // Scale is read before screens are, as they are laid out in logical units
        scaling = ReadDesktopScaling(display);

        // Xlib terminates the application on any error, for which no handler is set, so the handler is always set. Errors are only logged in logging builds
        XSetErrorHandler([](Display* faultyDisplay, XErrorEvent* errorEvent) -> int
        {
            #if SR_ENABLE_LOGGING
                std::array<char, 256> error = { };
                XGetErrorText(faultyDisplay, errorEvent->error_code, error.data(), error.size());
                SR_ERROR("X11 - {0}", std::string_view(error.data(), error.size()));
            #else
                static_cast<void>(faultyDisplay);
                static_cast<void>(errorEvent);
            #endif
            return 0;
        });

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
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_CM_Sx)]                        = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_WM_CM_S0");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_WINDOW_TYPE)]                  = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_WM_WINDOW_TYPE");
        atomTable[GetAtomTypeIndex(AtomType::NET_WM_WINDOW_TYPE_NORMAL)]           = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_WM_WINDOW_TYPE_NORMAL");
        atomTable[GetAtomTypeIndex(AtomType::NET_WORKAREA)]                        = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_WORKAREA");
        atomTable[GetAtomTypeIndex(AtomType::NET_CURRENT_DESKTOP)]                 = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_CURRENT_DESKTOP");
        atomTable[GetAtomTypeIndex(AtomType::NET_ACTIVE_WINDOW)]                   = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_ACTIVE_WINDOW");
        atomTable[GetAtomTypeIndex(AtomType::NET_FRAME_EXTENTS)]                   = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_FRAME_EXTENTS");
        atomTable[GetAtomTypeIndex(AtomType::NET_REQUEST_FRAME_EXTENTS)]           = TryRetrieveAtom(display, supportedAtoms, supportedAtomCount, "_NET_REQUEST_FRAME_EXTENTS");

        // Deallocate atom data
        if (supportedAtomCount > 0) XFree(supportedAtoms);

        // Create invisible cursor
        XcursorImage* const emptyCursorImage = XcursorImageCreate(1, 1);
        invisibleCursor = XcursorImageLoadCursor(display, emptyCursorImage);
        XcursorImageDestroy(emptyCursorImage);

        // Work area changes are reported as property changes of the root window, and monitor changes as RandR events of it
        XSelectInput(display, rootWindow, PropertyChangeMask);
        if (xrandrExtension.IsSupported()) XRRSelectInput(display, rootWindow, RRScreenChangeNotifyMask | RRCrtcChangeNotifyMask | RROutputChangeNotifyMask);

        ReloadScreens();
    }

    /* --- POLLING METHODS --- */

    void X11Context::Update() noexcept
    {
        // XPending() reads everything the server has sent, so events are never left unread, and it does not block
        while (XPending(display) > 0)
        {
            XEvent event = { };
            XNextEvent(display, &event);

            if (XFilterEvent(&event, None)) continue;

            // Monitors being connected, disconnected or reconfigured are reported by RandR
            if (xrandrExtension.IsSupported())
            {
                if (event.type == xrandrExtension.GetEventBase() + RRScreenChangeNotify)
                {
                    XRRUpdateConfiguration(&event);
                    ReloadScreens();
                    continue;
                }

                if (event.type == xrandrExtension.GetEventBase() + RRNotify)
                {
                    ReloadScreens();
                    continue;
                }
            }

            // Root window is not a window of the engine, but its work area changes (such as panels appearing) are of interest
            if (event.xany.window == rootWindow)
            {
                if (event.type == PropertyNotify && event.xproperty.atom == GetAtom(AtomType::NET_WORKAREA)) ReloadScreens();
                continue;
            }

            X11Window* window = nullptr;
            if (XFindContext(display, event.xany.window, context, reinterpret_cast<XPointer*>(&window)) == 0)
            {
                window->HandleX11Event(event);
            }
        }
    }

    XEvent X11Context::PeekNextEvent() const
    {
        XEvent event = { };
        XPeekEvent(display, &event);
        return event;
    }

    void X11Context::RegisterWindowEvent(const XID window, const Atom type, const long a, const long b, const long c, const long d, const int eventMask) const
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

        // Requests of this kind are only honored when sent to the root window, where the window manager is listening for them
        XSendEvent(display, rootWindow, False, eventMask, &event);
        XFlush(display);
    }

    void X11Context::ReplyToRootEvent(const XEvent& event) const
    {
        XEvent reply = event;
        reply.xclient.window = rootWindow;

        XSendEvent(display, rootWindow, False, SubstructureNotifyMask | SubstructureRedirectMask, &reply);
        XFlush(display);
    }

    void X11Context::ReloadScreens() noexcept
    {
        screens.clear();

        // Get work area extents
        Vector4UInt workAreaExtents = { 0, 0, 0, 0 };
        if (GetAtom(AtomType::NET_WORKAREA))
        {
            uchar* workAreaExtentData = nullptr;
            if (GetWindowProperty(GetRootWindow(), GetAtom(AtomType::NET_WORKAREA), XA_CARDINAL, &workAreaExtentData) >= 4)
            {
                const ulong* const values = reinterpret_cast<const ulong*>(workAreaExtentData);
                workAreaExtents = { static_cast<uint32>(values[0]), static_cast<uint32>(values[1]), static_cast<uint32>(values[2]), static_cast<uint32>(values[3]) };
            }
            if (workAreaExtentData != nullptr) XFree(workAreaExtentData);
        }

        // Load screens, one for each connected output
        if (xrandrExtension.IsSupported())
        {
            std::unique_ptr<XRRScreenResources, std::function<void(XRRScreenResources*)>> screenResources = { XRRGetScreenResources(GetDisplay(), GetRootWindow()), XRRFreeScreenResources };
            if (screenResources != nullptr)
            {
                for (int i = 0; i < screenResources->ncrtc; i++)
                {
                    std::unique_ptr<XRRCrtcInfo, std::function<void(XRRCrtcInfo*)>> crtcInfo = { XRRGetCrtcInfo(GetDisplay(), screenResources.get(), screenResources->crtcs[i]), XRRFreeCrtcInfo };
                    if (crtcInfo == nullptr || crtcInfo->noutput == 0) continue;

                    std::unique_ptr<XRROutputInfo, std::function<void(XRROutputInfo*)>> outputInfo = { XRRGetOutputInfo(GetDisplay(), screenResources.get(), crtcInfo->outputs[0]), XRRFreeOutputInfo };
                    if (outputInfo == nullptr) continue;

                    screens.emplace_back(X11ScreenCreateInfo{ .screenResources = screenResources.get(), .crtcInfo = crtcInfo.get(), .outputInfo = outputInfo.get(), .workAreaExtents = workAreaExtents, .scaling = scaling });
                }
            }
        }

        // Without RandR (or without any connected output), the whole display is treated as a single screen, as there is always at least one
        if (screens.empty())
        {
            static char fallbackName[] = "Display";

            XRRScreenResources emptyResources = { };
            XRRCrtcInfo crtcInfo = { };
            crtcInfo.width = static_cast<unsigned int>(DisplayWidth(display, screen));
            crtcInfo.height = static_cast<unsigned int>(DisplayHeight(display, screen));
            crtcInfo.rotation = RR_Rotate_0;

            XRROutputInfo outputInfo = { };
            outputInfo.name = fallbackName;
            outputInfo.nameLen = static_cast<int>(sizeof(fallbackName) - 1);

            screens.emplace_back(X11ScreenCreateInfo{ .screenResources = &emptyResources, .crtcInfo = &crtcInfo, .outputInfo = &outputInfo, .workAreaExtents = workAreaExtents, .scaling = scaling });
        }
    }

    /* --- GETTER METHODS --- */

    uint32 X11Context::GetWindowProperty(const XID window, const Atom property, const Atom type, uchar** outValues) const noexcept
    {
        *outValues = nullptr;

        Atom actualType = None;
        int actualFormat = 0;
        ulong itemCount = 0;
        ulong bytesAfter = 0;
        uchar* data = nullptr;

        // Property may be missing, or be of a different type than requested, in which case there is nothing to return
        const int status = XGetWindowProperty(display, window, property, 0, MAX_PROPERTY_LENGTH, False, type, &actualType, &actualFormat, &itemCount, &bytesAfter, &data);
        if (status != Success || actualType != type || data == nullptr || itemCount == 0)
        {
            if (data != nullptr) XFree(data);
            return 0;
        }

        *outValues = data;
        return static_cast<uint32>(itemCount);
    }

    const X11Screen& X11Context::GetWindowScreen(const XID window) const noexcept
    {
        XID child = None;
        int xPosition = 0;
        int yPosition = 0;
        XTranslateCoordinates(display, window, rootWindow, 0, 0, &xPosition, &yPosition, &child);

        // Screens are laid out in logical units, so the position of the window is converted to them
        const Vector2Int position = GetLogicalPosition({ xPosition, yPosition });

        for (const X11Screen& screen : screens)
        {
            const Vector2Int screenOrigin = screen.GetOrigin();
            const Vector2Int screenSize = { screen.GetWidth(), screen.GetHeight() };
            if (position.x >= screenOrigin.x && position.x <= screenOrigin.x + screenSize.x && position.y >= screenOrigin.y && position.y <= screenOrigin.y + screenSize.y)
            {
                return screen;
            }
        }

        // Screens are never empty, as ReloadScreens() always provides at least one
        return screens[0];
    }

    Atom X11Context::GetAtom(const AtomType atomType) const noexcept
    {
        return atomTable[GetAtomTypeIndex(atomType)];
    }

    /* --- DESTRUCTOR --- */

    X11Context::~X11Context() noexcept
    {
        if (invisibleCursor != None) XFreeCursor(display, invisibleCursor);
        XCloseDisplay(display);
    }

}
