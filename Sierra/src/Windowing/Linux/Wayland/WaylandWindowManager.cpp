//
// Created by Nikolay Kanchevski on 11.12.26.
//

#include "WaylandWindowManager.h"

#include "WaylandWindow.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    WaylandWindowManager::WaylandWindowManager(const WaylandWindowManagerCreateInfo& createInfo)
        : waylandContext(createInfo.waylandContext)
    {

    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Window> WaylandWindowManager::CreateWindow(const WindowCreateInfo& createInfo) const noexcept
    {
        return std::make_unique<WaylandWindow>(waylandContext, createInfo);
    }

    void WaylandWindowManager::EnumerateScreens(const ScreenEnumerationPredicate& Predicate) const noexcept
    {
        for (const std::unique_ptr<WaylandScreen>& screen : waylandContext.GetScreens())
        {
            Predicate(*screen);
        }
    }

    /* --- GETTER METHODS --- */

    const Screen& WaylandWindowManager::GetWindowScreen(const Window& window) const
    {
        SR_THROW_IF(window.GetBackendType() != WindowingBackendType::Wayland, UnexpectedTypeError(SR_FORMAT("Cannot get screen of window [{0}], as its windowing backend differs from [WindowingBackendType::Wayland]", window.GetTitle())));
        const WaylandWindow& waylandWindow = static_cast<const WaylandWindow&>(window);

        // NOTE: Window is not on any screen, until compositor has presented it for the first time
        const WaylandScreen* const screen = waylandWindow.GetScreen();
        return screen != nullptr ? *screen : GetPrimaryScreen();
    }

}
