//
// Created by Nikolay Kanchevski on 10.29.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the X11Context.h file is only allowed in Linux builds!"
#endif

#include <X11/Xlib.h>
#include "X11Screen.h"
#include "X11Extensions.h"

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

    };

    class SIERRA_API X11Context final
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] XID CreateWindow(std::string_view title, uint32 width, uint32 height) const;
        XEvent QueryWindowDestruction(XID window) const;
        void DestroyWindow(XID window) const;

        [[nodiscard]] bool IsEventQueueEmpty() const;
        XEvent PollNextEvent() const;
        XEvent PeekNextEvent() const;
        [[nodiscard]] bool IsEventFiltered(XEvent &event) const;
        XEvent WaitForEvent(void* eventData, int(EventCheck)(Display*, XEvent*, XPointer)) const;

        XEvent RegisterWindowEvent(XID window, Atom type, long a = 0, long b = 0, long c = 0, long d = 0, int eventMask = NoEventMask) const;
        XEvent RegisterRootWindowEvent(Atom type, long a = 0, long b = 0, long c = 0, long d = 0, int eventMask = NoEventMask) const;
        void Flush() const;

        void MinimizeWindow(XID window) const;
        void MaximizeWindow(XID window) const;
        void ShowWindow(XID window) const;
        void HideWindow(XID window) const;
        void FocusWindow(XID window) const;

        /* --- SETTER METHODS --- */
        void SetWindowTitle(XID window, std::string_view title) const;
        void SetWindowPosition(XID window, const Vector2Int &position) const;
        void SetWindowSize(XID window, const Vector2UInt &size) const;
        void SetWindowSizeLimits(XID window, const Vector2UInt &minSize, const Vector2UInt &maxSize) const;
        void SetWindowOpacity(XID window, float32 opacity) const;

        void ShowWindowCursor(XID window) const;
        void HideWindowCursor(XID window) const;
        void SetWindowCursorPosition(XID window, const Vector2Int &position) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] X11Screen& GetPrimaryScreen() const;
        [[nodiscard]] X11Screen& GetWindowScreen(XID window) const;
        [[nodiscard]] std::optional<XID> GetFocusedWindow() const;

        [[nodiscard]] std::string GetWindowTitle(XID window) const;
        [[nodiscard]] Vector2Int GetWindowPosition(XID window) const;
        [[nodiscard]] Vector2UInt GetWindowSize(XID window) const;
        [[nodiscard]] float32 GetWindowOpacity(XID window) const;
        [[nodiscard]] bool IsWindowMinimized(XID window) const;
        [[nodiscard]] bool IsWindowMaximized(XID window) const;
        [[nodiscard]] bool IsWindowFocused(XID window) const;
        [[nodiscard]] bool IsWindowHidden(XID window) const;

        [[nodiscard]] Vector2Int GetWindowCursorPosition(XID window) const;

        [[nodiscard]] Vector4UInt GetWindowExtents(XID window) const;
        [[nodiscard]] Atom GetAtom(const AtomType atomType) const { return atomTable[GetAtomTypeIndex(atomType)]; }

        [[nodiscard]] Display* GetDisplay() const { return display; }
        [[nodiscard]] const XkbExtension& GetXkbExtension() const { return xkbExtension; }
        [[nodiscard]] const XrandrExtension& GetXrandrExtension() const { return xrandrExtension; }

        /* --- DESTRUCTOR --- */
        ~X11Context();

    private:
        friend class LinuxContext;
        explicit X11Context(const X11ContextCreateInfo &createInfo);

        Display* display = nullptr;
        int screen = 0;
        XID rootWindow = 0;
        XID invisibleCursor = 0;

        XkbExtension xkbExtension;
        XrandrExtension xrandrExtension;

        std::array<Atom, static_cast<uint32>(AtomType::NET_REQUEST_FRAME_EXTENTS) + 1> atomTable { 0 };
        Atom TryRetrieveAtom(const Atom* supportedAtoms, const ulong atomCount, const char* atomName);
        [[nodiscard]] constexpr std::underlying_type<AtomType>::type GetAtomTypeIndex(const AtomType atomType) const { return static_cast<uint32>(atomType); }

        mutable std::vector<X11Screen> screens;
        void ReloadScreens(XEvent* notifyEvent = nullptr) const;
        [[nodiscard]] int32 GetWindowProperty(XID window, Atom property, Atom type, uchar** value) const;

    };

}
