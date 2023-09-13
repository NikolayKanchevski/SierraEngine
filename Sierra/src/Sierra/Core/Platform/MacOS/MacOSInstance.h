//
// Created by Nikolay Kanchevski on 3.09.23.
//

#pragma once

#if !PLATFORM_MACOS
    #error "Including the MacOSInstance.h file is only allowed in macOS builds!"
#endif

#include "../../PlatformInstance.h"
#if !defined(__OBJC__)
    namespace Sierra
    {
        typedef void CocoaApplication;
        typedef void CocoaApplicationDelegate;
    }

#else
    #include <Cocoa/Cocoa.h>
    @class CocoaApplication;
    @class CocoaApplicationDelegate;

#endif

namespace Sierra
{

    class SIERRA_API MacOSInstance final : public PlatformInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MacOSInstance(const PlatformInstanceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] CocoaApplication* GetApplication() const;
        [[nodiscard]] uint32 GetMenuBarHeight() const;
        [[nodiscard]] inline PlatformType GetType() const override { return PlatformType::MacOS; }

        /* --- DESTRUCTOR --- */
        ~MacOSInstance();

    private:
        CocoaApplication* application;
        CocoaApplicationDelegate* applicationDelegate;

    };

}