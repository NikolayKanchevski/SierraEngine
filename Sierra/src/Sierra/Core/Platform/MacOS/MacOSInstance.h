//
// Created by Nikolay Kanchevski on 3.09.23.
//

#pragma once

#if !SR_PLATFORM_MACOS
    #error "Including the MacOSInstance.h file is only allowed in macOS builds!"
#endif

#if !defined(__OBJC__)
    namespace Sierra
    {
        typedef void NSApplication;
        typedef void CocoaApplicationDelegate;
    }
#else
    #include <Cocoa/Cocoa.h>
    @class CocoaApplicationDelegate;
#endif

#include "../../PlatformInstance.h"

namespace Sierra
{

    class SIERRA_API MacOSInstance final : public PlatformInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MacOSInstance(const PlatformInstanceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] NSApplication* GetApplication() const;
        [[nodiscard]] uint32 GetMenuBarHeight() const;

        [[nodiscard]] inline PlatformType GetType() const override { return PlatformType::MacOS; }

        /* --- DESTRUCTOR --- */
        ~MacOSInstance() override;

    private:
        NSApplication* application;
        CocoaApplicationDelegate* applicationDelegate;

    };

}