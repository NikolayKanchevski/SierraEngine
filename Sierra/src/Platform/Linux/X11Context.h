//
// Created by Nikolay Kanchevski on 10.29.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the X11Context.h file is only allowed in Linux builds!"
#endif

#include <X11/Xlib.h>

#include "X11Extensions.h"
#include "../../Windowing/Linux/X11Screen.h"

namespace Sierra
{

    enum class AtomType : uint8
    {
        NET_SUPPORTED,
        NET_SUPPORTING_WM_CHECK,
        WM_PROTOCOLS,
        WM_STATE,
        WM_DELETE_WINDOW,
        NET_WM_NAME,
        NET_WM_ICON,
        NET_WM_PING,
        NET_WM_PID,
        NET_WM_ICON_NAME,
        NET_WM_BYPASS_COMPOSITOR,
        NET_WM_WINDOW_OPACITY,
        MOTIF_WM_HINTS,
        NET_WM_WINDOW_TYPE,
        NET_WM_WINDOW_TYPE_NORMAL,
        NET_WM_STATE,
        NET_WM_STATE_ABOVE,
        NET_WM_STATE_FULLSCREEN,
        NET_WM_STATE_MAXIMIZED_VERT,
        NET_WM_STATE_MAXIMIZED_HORZ,
        NET_WM_STATE_DEMANDS_ATTENTION,
        NET_WM_FULLSCREEN_MONITORS,
        NET_WM_CM_Sx,
        NET_WORKAREA,
        NET_CURRENT_DESKTOP,
        NET_ACTIVE_WINDOW,
        NET_FRAME_EXTENTS,
        NET_REQUEST_FRAME_EXTENTS
    };

    struct X11ContextCreateInfo
    {
        Display* display = nullptr;
    };

    class SIERRA_API X11Context final
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using WindowEventCallback = std::function<void(const XEvent&, XID, void*)>;
        using EventWaitCallback = std::function<int(Display*, XEvent*, XPointer)>;

        /* --- CONSTRUCTORS --- */
        explicit X11Context(const X11ContextCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] XID CreateWindow(std::string_view title, uint32 width, uint32 height, const WindowEventCallback& Callback, void* userData = nullptr) const;
        void DestroyWindow(XID window) const;

        void ReloadScreens();

        [[nodiscard]] XEvent PeekNextEvent() const;
        XEvent RegisterWindowEvent(XID window, Atom type, long a = 0, long b = 0, long c = 0, long d = 0, int eventMask = NoEventMask) const;
        XEvent WaitForEvent(int(Callback)(Display*, XEvent*, XPointer), void* userData = nullptr) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] X11Screen& GetPrimaryScreen() noexcept { return screens[0]; }
        [[nodiscard]] std::span<X11Screen> GetScreens() noexcept { return screens; }

        [[nodiscard]] X11Screen& GetWindowScreen(XID window);
        [[nodiscard]] const X11Screen& GetWindowScreen(XID window) const;

        [[nodiscard]] Display* GetDisplay() const noexcept { return display; }
        [[nodiscard]] int GetScreen() const noexcept { return screen; }
        [[nodiscard]] XID GetRootWindow() const noexcept { return rootWindow; }
        [[nodiscard]] XID GetInvisibleCursor() const noexcept { return invisibleCursor; }

        [[nodiscard]] Atom GetAtom(const AtomType atomType) const noexcept { return atomTable[GetAtomTypeIndex(atomType)]; }
        [[nodiscard]] uint32 GetWindowProperty(XID window, Atom property, Atom type, uchar** outValues) const noexcept;

        [[nodiscard]] const XkbExtension& GetXkbExtension() const noexcept { return xkbExtension; }
        [[nodiscard]] const XrandrExtension& GetXrandrExtension() const noexcept { return xrandrExtension; }

        /* --- COPY SEMANTICS --- */
        X11Context(const X11Context&) = delete;
        X11Context& operator=(const X11Context&) = delete;

        /* --- MOVE SEMANTICS --- */
        X11Context(X11Context&&) = default;
        X11Context& operator=(X11Context&&) = default;

        /* --- DESTRUCTOR --- */
        ~X11Context() noexcept;

    private:
        Display* display = nullptr;
        int screen = 0;
        XID rootWindow = 0;

        XID invisibleCursor = 0;
        XkbExtension xkbExtension;
        XrandrExtension xrandrExtension;

        std::vector<X11Screen> screens;
        std::array<Atom, static_cast<uint32>(AtomType::NET_REQUEST_FRAME_EXTENTS) + 1> atomTable = { 0 };
        [[nodiscard]] constexpr uint8 GetAtomTypeIndex(const AtomType atomType) const { return static_cast<uint32>(atomType); }

        friend class LinuxContext;
        void Update();

    };

}
