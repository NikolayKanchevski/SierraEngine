//
// Created by Nikolay Kanchevski on 10.29.23.
//

#include "X11WindowManager.h"

#include <unistd.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "X11Window.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    X11WindowManager::X11WindowManager(const X11WindowManagerCreateInfo& createInfo)
        : x11Context(createInfo.x11Context)
    {

    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Window> X11WindowManager::CreateWindow(const WindowCreateInfo& createInfo) const noexcept
    {
        // NOTE: Windows are managed by the window manager (no override redirect), which is what provides decorations, focus handling, and the ability to move, resize, maximize and minimize them
        XSetWindowAttributes setAttributes = { 0 };
        setAttributes.background_pixel = WhitePixel(x11Context.GetDisplay(), x11Context.GetScreen());
        setAttributes.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | FocusChangeMask | VisibilityChangeMask | EnterWindowMask | LeaveWindowMask | PropertyChangeMask;

        // Window is created with its physical size, which is its logical size scaled by the scale of the desktop
        const Vector2UInt physicalSize = x11Context.GetPhysicalSize({ createInfo.width, createInfo.height });

        // Create window
        const XID window = XCreateWindow(
            x11Context.GetDisplay(), x11Context.GetRootWindow(), 0, 0,
            physicalSize.x, physicalSize.y, 0, DefaultDepth(x11Context.GetDisplay(), x11Context.GetScreen()),
            InputOutput, DefaultVisual(x11Context.GetDisplay(), x11Context.GetScreen()), CWBackPixel | CWBorderPixel | CWEventMask, &setAttributes
        );

        // Set window manager protocols
        std::array<Atom, 2> protocols = { x11Context.GetAtom(AtomType::WM_DELETE_WINDOW), x11Context.GetAtom(AtomType::NET_WM_PING) };
        XSetWMProtocols(x11Context.GetDisplay(), window, protocols.data(), protocols.size());

        // Set the process ID
        pid_t processID = getpid();
        XChangeProperty(x11Context.GetDisplay(), window, x11Context.GetAtom(AtomType::NET_WM_PID), XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<uchar*>(&processID), 1);

        // If window supports typing and the normal type, assign it
        if (x11Context.GetAtom(AtomType::NET_WM_WINDOW_TYPE) && x11Context.GetAtom(AtomType::NET_WM_WINDOW_TYPE_NORMAL))
        {
            Atom type = x11Context.GetAtom(AtomType::NET_WM_WINDOW_TYPE_NORMAL);
            XChangeProperty(x11Context.GetDisplay(), window, x11Context.GetAtom(AtomType::NET_WM_WINDOW_TYPE), XA_ATOM, 32, PropModeReplace, reinterpret_cast<uchar*>(&type), 1);
        }

        // Set window manager hints
        {
            const std::unique_ptr<XWMHints, std::function<void(void*)>> windowManagerHints = { XAllocWMHints(), XFree };
            windowManagerHints->flags = StateHint;
            windowManagerHints->initial_state = NormalState;
            XSetWMHints(x11Context.GetDisplay(), window, windowManagerHints.get());
        }

        // Set class hint
        {
            const std::unique_ptr<XClassHint, std::function<void(void*)>> classHint = { XAllocClassHint(), XFree };
            char instanceName[12 + 1] = "SierraEngine";
            classHint->res_name = instanceName;
            XSetClassHint(x11Context.GetDisplay(), window, classHint.get());
        }

        XFlush(x11Context.GetDisplay());

        // NOTE: Size hints, title and the state the window is shown in are applied by the window itself, before it gets mapped
        return std::make_unique<X11Window>(x11Context, window, createInfo);
    }

    void X11WindowManager::EnumerateScreens(const ScreenEnumerationPredicate& Predicate) const noexcept
    {
        for (const X11Screen& screen : x11Context.GetScreens())
        {
            Predicate(screen);
        }
    }

    /* --- GETTER METHODS --- */

    const Screen& X11WindowManager::GetWindowScreen(const Window& window) const
    {
        SR_THROW_IF(window.GetBackendType() != WindowingBackendType::X11, UnexpectedTypeError(SR_FORMAT("Cannot get screen of window [{0}], as its windowing backend differs from [WindowingBackendType::X11]", window.GetTitle())));
        const X11Window& x11Window = static_cast<const X11Window&>(window);

        return x11Context.GetWindowScreen(x11Window.GetX11Window());
    }

}
