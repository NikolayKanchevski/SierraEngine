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
        explicit GameActivityScreen(const ConfigurationScreenCreateInfo &createInfo);

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
        AConfiguration* configuration;

        std::string name;
        Vector2Int origin = { 0, 0 };
        Vector2UInt size = { 0, 0 };
        Vector2Int workAreaOrigin = { 0, 0 };
        Vector2UInt workAreaSize = { 0, 0 };
        uint32 refreshRate = 0;

    };

}
