//
// Created by Nikolay Kanchevski on 12/14/23.
//

#pragma once

#if !SR_PLATFORM_ANDROID
    #error "Including the GameActivityScreen.h file is only allowed in Android builds!"
#endif

#include "../Screen.h"

#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace Sierra
{

    struct GameScreenCreateInfo
    {
        GameActivity* gameActivity = nullptr;
        AConfiguration* configuration = nullptr;
    };

    class SIERRA_API GameKitScreen final : public Screen
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit GameKitScreen(const GameScreenCreateInfo& createInfo);

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

        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override { return WindowingBackendType::GameKit; }

        /* --- COPY SEMANTICS --- */
        GameKitScreen(const GameKitScreen&) = delete;
        GameKitScreen& operator=(const GameKitScreen&) = delete;

        /* --- MOVE SEMANTICS --- */
        GameKitScreen(GameKitScreen&&) noexcept = default;
        GameKitScreen& operator=(GameKitScreen&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~GameKitScreen() noexcept override = default;

    private:
        AConfiguration* configuration;

        std::string name;
        uint16 refreshRate = 0;
        ScreenOrientation orientation = ScreenOrientation::Unknown;

        Vector2Int origin = { 0, 0 };
        Vector2UInt size = { 0, 0 };
        Vector2Int workAreaOrigin = { 0, 0 };
        Vector2UInt workAreaSize = { 0, 0 };

    };

}
