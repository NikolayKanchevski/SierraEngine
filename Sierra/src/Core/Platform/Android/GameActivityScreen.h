//
// Created by Nikolay Kanchevski on 12/14/23.
//

#pragma once

#if !SR_PLATFORM_ANDROID
    #error "Including the GameActivityScreen.h file is only allowed in Android builds!"
#endif

#include "../../Screen.h"

#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace Sierra
{

    struct ConfigurationScreenCreateInfo
    {
        GameActivity* gameActivity = nullptr;
        AConfiguration* configuration = nullptr;
    };

    class SIERRA_API GameActivityScreen final : public Screen
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit GameActivityScreen(const ConfigurationScreenCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const override { return name; };
        [[nodiscard]] uint16 GetRefreshRate() const override { return refreshRate; };
        [[nodiscard]] ScreenOrientation GetOrientation() const override;

        [[nodiscard]] Vector2Int GetOrigin() const override { return origin; };
        [[nodiscard]] uint32 GetWidth() const override { return size.x; }
        [[nodiscard]] uint32 GetHeight() const override { return size.y; }

        [[nodiscard]] Vector2Int GetWorkAreaOrigin() const override { return workAreaOrigin; };
        [[nodiscard]] uint32 GetWorkAreaWidth() const override { return workAreaSize.x; }
        [[nodiscard]] uint32 GetWorkAreaHeight() const override { return workAreaSize.y; }

    private:
        AConfiguration* configuration;

        std::string name;
        Vector2Int origin = { 0, 0 };
        Vector2UInt size = { 0, 0 };
        Vector2Int workAreaOrigin = { 0, 0 };
        Vector2UInt workAreaSize = { 0, 0 };
        uint16 refreshRate = 0;

    };

}
