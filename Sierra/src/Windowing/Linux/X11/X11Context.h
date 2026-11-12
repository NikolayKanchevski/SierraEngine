//
// Created by Nikolay Kanchevski on 10.29.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the X11Context.h file is only allowed in Linux builds!"
#endif

#include <X11/Xlib.h>

#include "X11Extensions.h"
#include "X11Screen.h"

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
        /* --- CONSTRUCTORS --- */
        explicit X11Context(const X11ContextCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        // Reads all events, that the server has sent, and dispatches them to their windows. Never blocks
        void Update() noexcept;

        [[nodiscard]] XEvent PeekNextEvent() const;

        // Sends a client message about the window to the window manager, which is what it expects for requests such as activation
        void RegisterWindowEvent(XID window, Atom type, long a = 0, long b = 0, long c = 0, long d = 0, int eventMask = SubstructureNotifyMask | SubstructureRedirectMask) const;

        // Sends a client message, received from the window manager, back to it as a reply (such as the answer to a ping)
        void ReplyToRootEvent(const XEvent& event) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] Display* GetDisplay() const noexcept { return display; }
        [[nodiscard]] XContext GetContext() const noexcept { return context; }

        [[nodiscard]] int GetScreen() const noexcept { return screen; }
        [[nodiscard]] XID GetRootWindow() const noexcept { return rootWindow; }

        [[nodiscard]] XID GetInvisibleCursor() const noexcept { return invisibleCursor; }

        // Returns the number of items of the property, and sets the data of the property, which must be released using XFree() if it is not null
        [[nodiscard]] uint32 GetWindowProperty(XID window, Atom property, Atom type, uchar** outValues) const noexcept;
        [[nodiscard]] Atom GetAtom(AtomType atomType) const noexcept;

        [[nodiscard]] const X11Screen& GetWindowScreen(XID window) const noexcept;
        [[nodiscard]] std::span<const X11Screen> GetScreens() const noexcept { return screens; }

        [[nodiscard]] const XkbExtension& GetXkbExtension() const noexcept { return xkbExtension; }
        [[nodiscard]] const XrandrExtension& GetXrandrExtension() const noexcept { return xrandrExtension; }

        // Scale of the desktop, which windows are laid out in logical units of. Physical units are those of the framebuffer and of the server
        [[nodiscard]] float32 GetScaling() const noexcept { return scaling; }
        [[nodiscard]] Vector2UInt GetPhysicalSize(const Vector2UInt logicalSize) const noexcept { return { static_cast<uint32>(glm::round(logicalSize.x * scaling)), static_cast<uint32>(glm::round(logicalSize.y * scaling)) }; }
        [[nodiscard]] Vector2UInt GetLogicalSize(const Vector2UInt physicalSize) const noexcept { return { static_cast<uint32>(glm::round(physicalSize.x / scaling)), static_cast<uint32>(glm::round(physicalSize.y / scaling)) }; }
        [[nodiscard]] Vector2Int GetPhysicalPosition(const Vector2Int logicalPosition) const noexcept { return { static_cast<int32>(glm::round(logicalPosition.x * scaling)), static_cast<int32>(glm::round(logicalPosition.y * scaling)) }; }
        [[nodiscard]] Vector2Int GetLogicalPosition(const Vector2Int physicalPosition) const noexcept { return { static_cast<int32>(glm::round(physicalPosition.x / scaling)), static_cast<int32>(glm::round(physicalPosition.y / scaling)) }; }

        /* --- COPY SEMANTICS --- */
        X11Context(const X11Context&) = delete;
        X11Context& operator=(const X11Context&) = delete;

        /* --- MOVE SEMANTICS --- */
        X11Context(X11Context&&) noexcept = default;
        X11Context& operator=(X11Context&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~X11Context() noexcept;

    private:
        Display* display = nullptr;
        XContext context = { };

        int screen = 0;
        float32 scaling = 1.0f;
        XID rootWindow = 0;

        XID invisibleCursor = 0;
        std::array<Atom, static_cast<uint32>(AtomType::NET_REQUEST_FRAME_EXTENTS) + 1> atomTable = { };

        XkbExtension xkbExtension;
        XrandrExtension xrandrExtension;

        std::vector<X11Screen> screens = {  };
        void ReloadScreens() noexcept;
    };

}
