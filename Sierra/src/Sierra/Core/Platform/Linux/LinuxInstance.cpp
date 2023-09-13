//
// Created by Nikolay Kanchevski on 9.1.23.
//

#include <X11/Xmd.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "LinuxInstance.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    LinuxInstance::LinuxInstance(const PlatformInstanceCreateInfo &createInfo)
        : PlatformInstance(createInfo)
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
        }

        // Get atoms
        {
            // Retrieve supported atoms
            atoms.NET_SUPPORTED = XInternAtom(display, "_NET_SUPPORTED", False);

            // Declare supported atoms
            ulong supportedAtomCount = 0;
            Atom* supportedAtoms = nullptr;

            // Arbitrary values, so the method below can be called
            Atom actualType;
            int actualFormat;
            ulong bytesAfter;
            XGetWindowProperty(display, GetRootWindow(), atoms.NET_SUPPORTED, 0, LONG_MAX, False, XA_ATOM, &actualType, &actualFormat, &supportedAtomCount, &bytesAfter, reinterpret_cast<uchar**>(&supportedAtoms));

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
    }

    /* --- PRIVATE METHODS --- */

    Atom LinuxInstance::GetAtom(const Atom *supportedAtoms, const ulong atomCount, const char *atomName)
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

    LinuxInstance::~LinuxInstance()
    {
        XCloseDisplay(display);
        display = nullptr;
    }

}