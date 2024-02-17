//
// Created by Nikolay Kanchevski on 9.11.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the UIKitContext.h file is only allowed in iOS builds!"
#endif

#include "UIKitScreen.h"
#include "UIKitSelectorBridge.h"

#if !defined(__OBJC__)
    namespace Sierra
    {
        typedef void UIKitApplication;
        typedef void UIKitApplicationDelegate;
        typedef void UIWindow;
        typedef void UIWindowScene;
        #define nil nullptr
    }
#else
    #include <UIKit/UIKit.h>
    typedef UIApplication UIKitApplication;
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
        [[nodiscard]] inline const UIKitApplication* GetApplication() const { return application; }
        [[nodiscard]] inline const UIKitScreen& GetPrimaryScreen() const { return primaryScreen; }

        /* --- DESTRUCTOR --- */
        ~UIKitContext();

    private:
        friend class iOSContext;
        explicit UIKitContext(const UIKitContextCreateInfo &createInfo);

        UIKitApplication* application = nil;
        UIKitScreen primaryScreen;

        UIKitSelectorBridge applicationDidFinishLaunchingBridge;
        UIKitSelectorBridge applicationWillTerminateBridge;

        /* --- EVENTS --- */
        void ApplicationDidFinishLaunching();
        void ApplicationWillTerminate();

    };

}
