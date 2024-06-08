//
// Created by Nikolay Kanchevski on 4.11.23.
//

#pragma once

#if !SR_PLATFORM_macOS
    #error "Including the CocoaScreen.h file is only allowed in macOS builds!"
#endif

#if defined(__OBJC__)
    #include <Cocoa/Cocoa.h>
#else
    namespace Sierra
    {
        #define nil nullptr
        using NSScreen = void;
    }
#endif
#include "../../Screen.h"

namespace Sierra
{

    struct CocoaScreenCreateInfo
    {
        const NSScreen* screen;
    };

    class SIERRA_API CocoaScreen final : public Screen
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit CocoaScreen(const CocoaScreenCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const override { return name; }
        [[nodiscard]] ScreenOrientation GetOrientation() const override { return size.x >= size.y ? ScreenOrientation::Landscape : ScreenOrientation::Portrait; }
        [[nodiscard]] uint32 GetRefreshRate() const override { return refreshRate; }

        [[nodiscard]] Vector2Int GetOrigin() const override { return origin; }
        [[nodiscard]] uint32 GetWidth() const override { return size.x; }
        [[nodiscard]] uint32 GetHeight() const override { return size.y; }

        [[nodiscard]] Vector2Int GetWorkAreaOrigin() const override { return workAreaOrigin; }
        [[nodiscard]] uint32 GetWorkAreaWidth() const override { return workAreaSize.x; }
        [[nodiscard]] uint32 GetWorkAreaHeight() const override { return workAreaSize.y; }

        [[nodiscard]] const NSScreen* GetNSScreen() const { return screen; }

        /* --- DESTRUCTOR --- */
        ~CocoaScreen() override = default;

        /* --- MOVE SEMANTICS --- */
        CocoaScreen(CocoaScreen&& other);

    private:
        const NSScreen* screen = nil;

        std::string name;
        uint32 refreshRate = 0;

        Vector2Int origin = { 0, 0 };
        Vector2UInt size = { 0, 0 };
        Vector2Int workAreaOrigin = { 0, 0 };
        Vector2UInt workAreaSize = { 0, 0 };

    };

}
