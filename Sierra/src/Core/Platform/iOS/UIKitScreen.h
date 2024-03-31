//
// Created by Nikolay Kanchevski on 10.11.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the UIKitScreen.h file is only allowed in iOS builds!"
#endif

#include "../../Screen.h"

#if !defined(__OBJC__)
    namespace Sierra
    {
        using UIScreen = void;
    }
#else
    #include <UIKit/UIKit.h>
#endif

namespace Sierra
{

    struct UIKitScreenCreatInfo
    {
        const UIScreen* uiScreen;
    };

    class SIERRA_API UIKitScreen : public Screen
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UIKitScreen(const UIKitScreenCreatInfo &creatInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline std::string_view GetName() const override { return name; };
        [[nodiscard]] inline uint32 GetRefreshRate() const override { return refreshRate; };
        [[nodiscard]] ScreenOrientation GetOrientation() const override;

        [[nodiscard]] inline Vector2Int GetOrigin() const override { return origin; };
        [[nodiscard]] inline uint32 GetWidth() const override { return size.x; }
        [[nodiscard]] inline uint32 GetHeight() const override { return size.y; }

        [[nodiscard]] inline Vector2Int GetWorkAreaOrigin() const override { return workAreaOrigin; };
        [[nodiscard]] inline uint32 GetWorkAreaWidth() const override { return workAreaSize.x; }
        [[nodiscard]] inline uint32 GetWorkAreaHeight() const override { return workAreaSize.y; }

    private:
        std::string name;
        Vector2Int origin = { 0, 0 };
        Vector2UInt size = { 0, 0 };
        Vector2Int workAreaOrigin = { 0, 0 };
        Vector2UInt workAreaSize = { 0, 0 };
        uint32 refreshRate = 0;

    };

}
