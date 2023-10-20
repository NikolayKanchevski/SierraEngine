//
// Created by Nikolay Kanchevski on 9.1.23.
//

#pragma once

#if !SR_PLATFORMLINUX
    #error "Including the LinuxInstance.h file is only allowed in Linux builds!"
#endif

#include <X11/Xlib.h>
#include "../../PlatformInstance.h"

namespace Sierra
{

    class SIERRA_API LinuxInstance final : public PlatformInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit LinuxInstance(const PlatformInstanceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Display* GetDisplay() { return display; }
        [[nodiscard]] inline int GetScreen() const { return DefaultScreen(display); }
        [[nodiscard]] inline XID GetRootWindow() const  { return DefaultRootWindow(display); }
        [[nodiscard]] inline auto& GetAtoms() { return atoms; }
        [[nodiscard]] inline PlatformType GetType() const override { return PlatformType::Linux; }

        /* --- DESTRUCTOR --- */
        ~LinuxInstance() override;

    private:
        Display* display = nullptr;

        struct AtomCollection
        {
            Atom NET_SUPPORTED;
            Atom NET_SUPPORTING_WM_CHECK;
            Atom WM_PROTOCOLS;
            Atom WM_STATE;
            Atom WM_DELETE_WINDOW;
            Atom NET_WM_NAME;
            Atom NET_WM_ICON;
            Atom NET_WM_PING;
            Atom NET_WM_PID;
            Atom NET_WM_ICON_NAME;
            Atom NET_WM_BYPASS_COMPOSITOR;
            Atom NET_WM_WINDOW_OPACITY;
            Atom MOTIF_WM_HINTS;

            Atom NET_WM_WINDOW_TYPE;
            Atom NET_WM_WINDOW_TYPE_NORMAL;
            Atom NET_WM_STATE;
            Atom NET_WM_STATE_ABOVE;
            Atom NET_WM_STATE_FULLSCREEN;
            Atom NET_WM_STATE_MAXIMIZED_VERT;
            Atom NET_WM_STATE_MAXIMIZED_HORZ;
            Atom NET_WM_STATE_DEMANDS_ATTENTION;
            Atom NET_WM_FULLSCREEN_MONITORS;
            Atom NET_WM_CM_Sx;
            Atom NET_WORKAREA;
            Atom NET_CURRENT_DESKTOP;
            Atom NET_ACTIVE_WINDOW;
            Atom NET_FRAME_EXTENTS;
            Atom NET_REQUEST_FRAME_EXTENTS;
        };

        AtomCollection atoms;
        Atom GetAtom(const Atom* supportedAtoms, ulong atomCount, const char* atomName);

    };

}
