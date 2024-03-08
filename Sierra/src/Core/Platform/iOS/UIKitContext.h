//
// Created by Nikolay Kanchevski on 9.11.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the UIKitContext.h file is only allowed in iOS builds!"
#endif

#include "UIKitScreen.h"

#if !defined(__OBJC__)
    namespace Sierra
    {
        using UIApplication = void;
        using UIWindow = void;
        #define nil nullptr
    }
#else
    #include <UIKit/UIKit.h>
#endif

namespace Sierra
{

    struct UIKitContextCreateInfo
    {

    };

    class SIERRA_API UIKitContext final
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] UIWindow* CreateWindow() const;
        void DestroyWindow(UIWindow* window) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const UIApplication* GetApplication() const { return application; }
        [[nodiscard]] inline const UIKitScreen& GetPrimaryScreen() const { return primaryScreen; }

        /* --- DESTRUCTOR --- */
        ~UIKitContext() = default;

    private:
        friend class iOSContext;
        explicit UIKitContext(const UIKitContextCreateInfo &createInfo);

        UIApplication* application = nil;
        UIKitScreen primaryScreen;

        #if defined(__OBJC__) && defined(UIKIT_CONTEXT_IMPLEMENTATION)
            public:
            /* --- EVENTS --- */
            void ApplicationDidFinishLaunching();
        #endif

    };

}
