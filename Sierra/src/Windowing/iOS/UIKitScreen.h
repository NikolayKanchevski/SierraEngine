//
// Created by Nikolay Kanchevski on 10.11.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the UIKitScreen.h file is only allowed in iOS builds!"
#endif

#if defined(__OBJC__)
    #include <UIKit/UIKit.h>
#else
    namespace Sierra
    {
        #define nil nullptr
        using UIScreen = void;
        using UIInterfaceOrientationMask = ulong;
    }
#endif
#include "../Screen.h"

namespace Sierra
{

    struct UIKitScreenCreateInfo
    {
        const UIScreen* screen;
    };

    class SIERRA_API UIKitScreen final : public Screen
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UIKitScreen(const UIKitScreenCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void RegisterScreenReorient(ScreenOrientation orientation) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }
        [[nodiscard]] uint16 GetRefreshRate() const noexcept override { return refreshRate; }
        [[nodiscard]] ScreenOrientation GetOrientation() const noexcept override { return orientation; }

        [[nodiscard]] Vector2Int GetOrigin() const noexcept override { return origin; }
        [[nodiscard]] uint32 GetWidth() const noexcept override { return size.x; }
        [[nodiscard]] uint32 GetHeight() const noexcept override { return size.y; }

        [[nodiscard]] Vector2Int GetWorkAreaOrigin() const noexcept override { return workAreaOrigin; }
        [[nodiscard]] uint32 GetWorkAreaWidth() const noexcept override { return workAreaSize.x; }
        [[nodiscard]] uint32 GetWorkAreaHeight() const noexcept override { return workAreaSize.y; }

        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override { return WindowingBackendType::UIKit; }

        /* --- COPY SEMANTICS --- */
        UIKitScreen(const UIKitScreen&) = delete;
        UIKitScreen& operator=(const UIKitScreen&) = delete;

        /* --- MOVE SEMANTICS --- */
        UIKitScreen(UIKitScreen&&) = default;
        UIKitScreen& operator=(UIKitScreen&&) = default;

        /* --- DESTRUCTOR --- */
        ~UIKitScreen() noexcept override;

    private:
        const void* /* UIKitScreenEventWrapper* */ eventWrapper = nullptr;

        std::string name;
        uint16 refreshRate = 0;
        ScreenOrientation orientation = ScreenOrientation::Unknown;

        Vector2Int origin = { 0, 0 };
        Vector2UInt size = { 0, 0 };
        Vector2Int workAreaOrigin = { 0, 0 };
        Vector2UInt workAreaSize = { 0, 0 };

    };

}
