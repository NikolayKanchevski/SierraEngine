//
// Created by Nikolay Kanchevski on 3.09.23.
//

#include "macOSContext.h"

#include "../../Windowing/macOS/CocoaWindow.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    macOSContext::macOSContext()
        : cocoaContext({ .application = [NSApplication sharedApplication] }), fileManager({ .fileManager = [NSFileManager defaultManager] })
    {

    }

    std::unique_ptr<Window> macOSContext::CreateWindow(const WindowCreateInfo& createInfo) const
    {
        return std::make_unique<CocoaWindow>(cocoaContext, createInfo);
    }

    /* --- GETTER METHODS --- */

    Screen& macOSContext::GetWindowScreen(const Window& window)
    {
        SR_THROW_IF(window.GetBackendType() != WindowingBackendType::Cocoa, UnexpectedTypeError(SR_FORMAT("Cannot get screen of window [{0}], as its windowing backend differs from [WindowingBackendType::Cocoa]", window.GetTitle())));
        const CocoaWindow& cocoaWindow = static_cast<const CocoaWindow&>(window);

        return cocoaContext.GetWindowScreen(cocoaWindow.GetNSWindow());
    }

    void macOSContext::EnumerateScreens(const ScreenEnumerationPredicate& Predicate)
    {
        for (CocoaScreen& screen : cocoaContext.GetScreens())
        {
            Predicate(screen);
        }
    }

    /* --- PRIVATE METHODS --- */

    void macOSContext::Update()
    {
        cocoaContext.Update();
    }

}