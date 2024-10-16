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

#include "../../Windowing/macOS/CocoaScreen.h"

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
        void ReloadScreens();

        [[nodiscard]] NSWindow* CreateWindow(std::string_view title, uint32 width, uint32 height) const;
        void DestroyWindow(NSWindow* window) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const NSApplication* GetNSApplication() const noexcept { return application; }

        [[nodiscard]] CocoaScreen& GetPrimaryScreen() noexcept { return screens[0]; }
        [[nodiscard]] CocoaScreen& GetWindowScreen(const NSWindow* window) noexcept;
        [[nodiscard]] std::span<CocoaScreen> GetScreens() noexcept { return screens; }

        /* --- COPY SEMANTICS --- */
        CocoaContext(const CocoaContext&) = delete;
        CocoaContext& operator=(const CocoaContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        CocoaContext(CocoaContext&&) noexcept = default;
        CocoaContext& operator=(CocoaContext&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~CocoaContext() noexcept;

    private:
        const NSApplication* application = nil;
        std::vector<CocoaScreen> screens = { };

        friend class macOSContext;
        void Update();

    };

}
