//
// Created by Nikolay Kanchevski on 9.11.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the UIKitContext.h file is only allowed in iOS builds!"
#endif
#if defined(__OBJC__)
    #include <UIKit/UIKit.h>
#else
    namespace Sierra
    {
        #define nil nullptr
        using UIApplication = void;
        using UIWindow = void;
    }
#endif
#include "UIKitScreen.h"

namespace Sierra
{

    struct UIKitContextCreateInfo
    {
        UIApplication* application = nil;
    };

    class SIERRA_API UIKitContext final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UIKitContext(const UIKitContextCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] UIWindow* CreateWindow() const;
        void DestroyWindow(UIWindow* window) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const UIApplication* GetUIApplication() const { return application; }
        [[nodiscard]] UIKitScreen& GetScreen() { return screen; }

        /* --- DESTRUCTOR --- */
        ~UIKitContext() = default;

    private:
        UIApplication* application = nil;
        UIKitScreen screen;

    };

}
