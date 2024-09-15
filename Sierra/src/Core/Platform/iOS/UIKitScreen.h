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
        using UIInterfaceOrientationMask = std::uintptr_t;
    }
#endif
#include "../../Screen.h"

namespace Sierra
{

    struct UIKitScreenCreatInfo
    {
        const UIScreen* screen;
    };

    class SIERRA_API UIKitScreen final : public Screen
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UIKitScreen(const UIKitScreenCreatInfo& creatInfo);

        /* --- POLLING METHODS --- */
        void RegisterScreenReorient(ScreenOrientation orientation) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const override { return name; }
        [[nodiscard]] uint16 GetRefreshRate() const override { return refreshRate; }
        [[nodiscard]] ScreenOrientation GetOrientation() const override { return orientation; }

        [[nodiscard]] Vector2Int GetOrigin() const override { return origin; }
        [[nodiscard]] uint32 GetWidth() const override { return size.x; }
        [[nodiscard]] uint32 GetHeight() const override { return size.y; }

        [[nodiscard]] Vector2Int GetWorkAreaOrigin() const override { return workAreaOrigin; }
        [[nodiscard]] uint32 GetWorkAreaWidth() const override { return workAreaSize.x; }
        [[nodiscard]] uint32 GetWorkAreaHeight() const override { return workAreaSize.y; }

        /* --- DESTRUCTOR --- */
        ~UIKitScreen() override;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static UIInterfaceOrientationMask ScreenOrientationToUIInterfaceOrientationMask(ScreenOrientation orientation);

    private:
        void* /* UIKitScreenEventWrapper* */ eventWrapper = nullptr;

        std::string name;
        uint16 refreshRate = 0;
        ScreenOrientation orientation = ScreenOrientation::Unknown;

        Vector2Int origin = { 0, 0 };
        Vector2UInt size = { 0, 0 };
        Vector2Int workAreaOrigin = { 0, 0 };
        Vector2UInt workAreaSize = { 0, 0 };

    };

}
