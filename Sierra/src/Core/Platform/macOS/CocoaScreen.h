//
// Created by Nikolay Kanchevski on 4.11.23.
//

#pragma once

#if !SR_PLATFORM_macOS
    #error "Including the CocoaScreen.h file is only allowed in macOS builds!"
#endif

#include "../../Screen.h"

#if !defined(__OBJC__)
    namespace Sierra
    {
        using NSScreen = void;
    }
#else
    #include <Cocoa/Cocoa.h>
#endif

namespace Sierra
{

    struct CocoaScreenCreateInfo
    {
        const NSScreen* nsScreen;
    };

    class SIERRA_API CocoaScreen final : public Screen
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit CocoaScreen(const CocoaScreenCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline std::string_view GetName() const override { return name; };
        [[nodiscard]] inline Vector2Int GetOrigin() const override { return origin; };
        [[nodiscard]] inline Vector2UInt GetSize() const override { return size; };
        [[nodiscard]] inline Vector2Int GetWorkAreaOrigin() const override { return workAreaOrigin; };
        [[nodiscard]] inline Vector2UInt GetWorkAreaSize() const override { return workAreaSize; };
        [[nodiscard]] inline uint32 GetRefreshRate() const override { return refreshRate; };
        [[nodiscard]] inline ScreenOrientation GetOrientation() const override { return size.x >= size.y ? ScreenOrientation::LandscapeNormal : ScreenOrientation::PortraitNormal; }

        /* --- MOVE SEMANTICS --- */
        CocoaScreen(CocoaScreen&& other);

    private:
        std::string name;
        Vector2Int origin = { 0, 0 };
        Vector2UInt size = { 0, 0 };
        Vector2Int workAreaOrigin = { 0, 0 };
        Vector2UInt workAreaSize = { 0, 0 };
        uint32 refreshRate = 0;

    };

}
