//
// Created by Nikolay Kanchevski on 9.1.23.
//

#include "LinuxContext.h"

#include "../../Windowing/Linux/X11Window.h"
#include "../../Windowing/Linux/X11Screen.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    LinuxContext::LinuxContext()
        : x11Context({ .display = XOpenDisplay(nullptr) })
    {

    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Window> LinuxContext::CreateWindow(const WindowCreateInfo& createInfo) const
    {
        return std::make_unique<X11Window>(x11Context, createInfo);
    }

    /* --- GETTER METHODS --- */

    Screen& LinuxContext::GetWindowScreen(const Window& window)
    {
        SR_THROW_IF(window.GetBackendType() != WindowingBackendType::X11, UnexpectedTypeError(SR_FORMAT("Cannot get screen of window [{0}], as its windowing backend differs from [WindowingBackendType::X11]", window.GetTitle())));
        const X11Window& x11Window = static_cast<const X11Window&>(window);

        return x11Context.GetWindowScreen(x11Window.GetX11Window());
    }

    void LinuxContext::EnumerateScreens(const ScreenEnumerationPredicate& Predicate)
    {
        for (X11Screen& screen : x11Context.GetScreens())
        {
            Predicate(screen);
        }
    }

    /* --- PRIVATE METHODS --- */

    void LinuxContext::Update()
    {
        x11Context.Update();
    }

}