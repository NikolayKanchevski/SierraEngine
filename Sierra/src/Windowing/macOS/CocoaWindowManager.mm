//
// Created by Nikolay Kanchevski on 15.11.26.
//

#include "CocoaWindowManager.h"

#include "CocoaWindow.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaWindowManager::CocoaWindowManager(const CocoaWindowManagerCreateInfo& createInfo)
        : cocoaContext({ .application = [NSApplication sharedApplication] })
    {

    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Window> CocoaWindowManager::CreateWindow(const WindowCreateInfo& createInfo) const noexcept
    {
        return std::make_unique<CocoaWindow>(cocoaContext, createInfo);
    }

    void CocoaWindowManager::EnumerateScreens(const ScreenEnumerationPredicate& Predicate) const noexcept
    {
        for (const CocoaScreen& screen: cocoaContext.GetScreens())
        {
            Predicate(screen);
        }
    }

    /* --- SETTER METHODS --- */

    const Screen& CocoaWindowManager::GetWindowScreen(const Window& window) const
    {
        SR_THROW_IF(window.GetBackendType() != WindowingBackendType::Cocoa, UnexpectedTypeError(SR_FORMAT("Cannot get screen of window [{0}], as its windowing backend differs from [WindowingBackendType::Cocoa]", window.GetTitle())));
        const CocoaWindow& cocoaWindow = static_cast<const CocoaWindow&>(window);

        for (const CocoaScreen& screen : cocoaContext.GetScreens())
        {
            if (screen.GetNSScreen() == [cocoaWindow.GetNSWindow() screen])
            {
                return screen;
            }
        }

        return GetPrimaryScreen();
    }

}