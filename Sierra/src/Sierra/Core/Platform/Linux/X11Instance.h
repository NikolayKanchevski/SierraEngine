//
// Created by Nikolay Kanchevski on 9.1.23.
//

#pragma once

#include <X11/Xlib.h>

namespace Sierra
{

    class SIERRA_API X11Instance
    {
    public:
        /* --- CONSTRUCTORS --- */
        X11Instance();
        static UniquePtr<X11Instance> Create();

        /* --- POLLING METHODS --- */
        [[nodiscard]] XID CreateWindow(const char* title, int xPosition, int yPosition, uint width, uint height, bool resizable, bool maximize, ulong eventMask);
        void CloseWindow(XID window);

        void RegisterWindowEvent(XID window, Atom type, long a = 0, long b = 0, long c = 0, long d = 0, int eventMask = SubstructureNotifyMask | SubstructureRedirectMask);
        void RegisterUnhandledWindowEvent(const XEvent &event);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline int GetScreen() const { return screen; }
        [[nodiscard]] inline Display* GetDisplay() { return display; }
        [[nodiscard]] inline XID GetRootWindow() const  { return rootWindow; }
        [[nodiscard]] inline Vector4Int& GetWindowExtents() { return windowExtents; }
        [[nodiscard]] inline auto& GetAtoms() { return atoms; }

        [[nodiscard]] int32 GetWindowProperty(XID window, Atom property, Atom type, uchar** value);
        [[nodiscard]] std::vector<XEvent> GetUnhandledEventsForWindow(XID window);

        /* --- DESTRUCTOR --- */
        ~X11Instance();

    private:
        Display* display = nullptr;
        int screen = -1;
        XID rootWindow;
        Visual* defaultVisual = nullptr;

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

        std::unordered_map<XID, std::vector<XEvent>> unhandledEvents;
        Vector4Int windowExtents = { 0, 0, 0, 0 }; // [ Left | Right | Top | Bottom ]
        [[nodiscard]] Atom GetAtom(const Atom* supportedAtoms, ulong atomCount, const char* atomName);

    };

}
