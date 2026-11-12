//
// Created by Nikolay Kanchevski on 9.1.23.
//

#include "LinuxContext.h"

#include <cstdlib>

#include "../../Windowing/Linux/Wayland/WaylandContext.h"
#include "../../Windowing/Linux/Wayland/WaylandWindowManager.h"
#include "../../Windowing/Linux/X11/X11Context.h"
#include "../../Windowing/Linux/X11/X11WindowManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    LinuxContext::LinuxContext()
    {
        // Wayland is preferred, when the session runs on it, and X11 is used otherwise (Wayland sessions also provide it through XWayland)
        if (std::getenv("WAYLAND_DISPLAY") != nullptr)
        {
            if (wl_display* waylandDisplay = wl_display_connect(nullptr); waylandDisplay != nullptr)
            {
                // NOTE: Context takes ownership of the display, so it gets disconnected along with it, even when it turns out to be unusable
                waylandContext = std::make_unique<WaylandContext>(WaylandContextCreateInfo { .display = waylandDisplay });
                if (waylandContext->IsValid()) // TODO: Migrate to a design where optionals are used instead of validity
                {
                    windowManager = std::make_unique<WaylandWindowManager>(WaylandWindowManagerCreateInfo { .waylandContext = *waylandContext });
                    windowingBackendType = WindowingBackendType::Wayland;
                    return;
                }

                waylandContext.reset();
            }
        }

        // Xlib has to be made thread-aware before any of its other functions is called, so this is done ahead of opening the display
        XInitThreads();
        Display* const x11Display = XOpenDisplay(nullptr);
        SR_THROW_IF(x11Display == nullptr, UnsupportedFeatureError("Cannot create Linux platform context, as neither a Wayland compositor nor an X11 server could be connected to"));

        x11Context = std::make_unique<X11Context>(X11ContextCreateInfo { .display = x11Display });
        windowManager = std::make_unique<X11WindowManager>(X11WindowManagerCreateInfo { .x11Context = *x11Context });
        windowingBackendType = WindowingBackendType::X11;
    }

    /* --- POLLING METHODS --- */

    void LinuxContext::Update()
    {
        if (waylandContext != nullptr) waylandContext->Update();
        else x11Context->Update();
    }

}