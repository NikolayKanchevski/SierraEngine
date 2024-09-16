//
// Created by Nikolay Kanchevski on 5.11.23.
//

#pragma once

#if !SR_PLATFORM_macOS
    #error "Including the CocoaContext.h file is only allowed in macOS builds!"
#endif

#if defined(__OBJC__)
    #include <Cocoa/Cocoa.h>
#else
    namespace Sierra
    {
        #define nil nullptr
        using NSApplication = void;
        using NSWindow = void;
        using NSEvent = void;
        using NSNotification = void;
    }
#endif

#include "CocoaScreen.h"

namespace Sierra
{

    struct CocoaContextCreateInfo
    {
        NSApplication* application = nil;
    };

    class SIERRA_API CocoaContext final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit CocoaContext(const CocoaContextCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] NSWindow* CreateWindow(std::string_view title, uint32 width, uint32 height) const;
        void DestroyWindow(NSWindow* window) const;

        NSEvent* PollNextEvent() const;
        void ReloadScreens();

        /* --- GETTER METHODS --- */
        [[nodiscard]] const NSApplication* GetNSApplication() const { return application; }

        [[nodiscard]] CocoaScreen& GetPrimaryScreen();
        [[nodiscard]] CocoaScreen& GetWindowScreen(const NSWindow* window);

        /* --- COPY SEMANTICS --- */
        CocoaContext(const CocoaContext&) = delete;
        CocoaContext& operator=(const CocoaContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        CocoaContext(CocoaContext&&) = default;
        CocoaContext& operator=(CocoaContext&&) = default;

        /* --- DESTRUCTOR --- */
        ~CocoaContext();

    private:
        NSApplication* application = nil;
        std::vector<CocoaScreen> screens;


    };

}
