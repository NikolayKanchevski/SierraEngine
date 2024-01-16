//
// Created by Nikolay Kanchevski on 5.11.23.
//

#pragma once

#if !SR_PLATFORM_macOS
    #error "Including the CocoaContext.h file is only allowed in macOS builds!"
#endif

#include "CocoaScreen.h"

#if !defined(__OBJC__)
    namespace Sierra
    {
        typedef void CocoaApplication;
        typedef void CocoaApplicationDelegate;

        typedef void NSEvent;
        typedef void NSScreen;
        typedef void NSWindow;
    }
#else
    #include <Cocoa/Cocoa.h>
    typedef NSApplication CocoaApplication;
    @class CocoaApplicationDelegate;
#endif

namespace Sierra
{

    struct CocoaContextCreateInfo
    {

    };

    class CocoaContext
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] NSWindow* CreateWindow(const std::string &title, uint32 width, uint32 height) const;
        void DestroyWindow(NSWindow* window) const;

        NSEvent* PollNextEvent() const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const CocoaApplication* GetApplication() const { return application; }

        [[nodiscard]] const CocoaScreen& GetPrimaryScreen() const;
        [[nodiscard]] const CocoaScreen& GetWindowScreen(const NSWindow* window) const;

        /* --- DESTRUCTOR --- */
        ~CocoaContext();

    private:
        friend class macOSContext;
        explicit CocoaContext(const CocoaContextCreateInfo &createInfo);

        CocoaApplication* application;
        CocoaApplicationDelegate* applicationDelegate;

        struct CocoaScreenPair
        {
            const NSScreen* nsScreen;
            CocoaScreen cocoaScreen;
        };
        std::vector<CocoaScreenPair> screens;
        void ReloadScreens();

        #if defined(__OBJC__) && defined(COCOA_CONTEXT_IMPLEMENTATION)
            public:
                /* --- EVENTS --- */
                void ApplicationDidChangeScreenParameters(const NSNotification* notification);
        #endif

    };

}
