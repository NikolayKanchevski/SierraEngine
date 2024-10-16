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

#include "../Screen.h"

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
        explicit CocoaScreen(const CocoaScreenCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }
        [[nodiscard]] ScreenOrientation GetOrientation() const noexcept override { return size.x >= size.y ? ScreenOrientation::Landscape : ScreenOrientation::Portrait; }
        [[nodiscard]] uint16 GetRefreshRate() const noexcept override { return refreshRate; }

        [[nodiscard]] Vector2Int GetOrigin() const noexcept override { return origin; }
        [[nodiscard]] uint32 GetWidth() const noexcept override { return size.x; }
        [[nodiscard]] uint32 GetHeight() const noexcept override { return size.y; }

        [[nodiscard]] Vector2Int GetWorkAreaOrigin() const noexcept override { return workAreaOrigin; }
        [[nodiscard]] uint32 GetWorkAreaWidth() const noexcept override { return workAreaSize.x; }
        [[nodiscard]] uint32 GetWorkAreaHeight() const noexcept override { return workAreaSize.y; }
        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override { return WindowingBackendType::Cocoa; }

        [[nodiscard]] const NSScreen* GetNSScreen() const noexcept { return screen; }

        /* --- COPY SEMANTICS --- */
        CocoaScreen(const CocoaScreen&) = delete;
        CocoaScreen& operator=(const CocoaScreen&) = delete;

        /* --- MOVE SEMANTICS --- */
        CocoaScreen(CocoaScreen&&) noexcept = default;
        CocoaScreen& operator=(CocoaScreen&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~CocoaScreen() noexcept override = default;

    private:
        const NSScreen* screen = nil;
        std::string name;

        uint16 refreshRate = 0;
        Vector2Int origin = { 0, 0 };
        Vector2UInt size = { 0, 0 };
        Vector2Int workAreaOrigin = { 0, 0 };
        Vector2UInt workAreaSize = { 0, 0 };

    };

}
