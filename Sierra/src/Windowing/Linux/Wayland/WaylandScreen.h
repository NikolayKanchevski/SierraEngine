//
// Created by Nikolay Kanchevski on 11.12.26.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the WaylandScreen.h file is only allowed in Linux builds!"
#endif

#include "../../Screen.h"

#include <wayland-client.h>

namespace Sierra
{

    struct WaylandScreenCreateInfo
    {
        wl_output* output = nullptr;
        uint32 registryName = 0;
    };

    class SIERRA_API WaylandScreen final : public Screen
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit WaylandScreen(const WaylandScreenCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }
        [[nodiscard]] ScreenOrientation GetOrientation() const noexcept override { return GetWidth() >= GetHeight() ? ScreenOrientation::Landscape : ScreenOrientation::Portrait; }
        [[nodiscard]] uint16 GetRefreshRate() const noexcept override { return refreshRate; }

        [[nodiscard]] Vector2Int GetOrigin() const noexcept override { return origin; }
        [[nodiscard]] uint32 GetWidth() const noexcept override { return rotated ? size.y : size.x; }
        [[nodiscard]] uint32 GetHeight() const noexcept override { return rotated ? size.x : size.y; }

        // NOTE: Wayland does not expose what parts of a screen are reserved by the compositor (panels, docks, etc.), so the work area always spans the whole screen
        [[nodiscard]] Vector2Int GetWorkAreaOrigin() const noexcept override { return GetOrigin(); }
        [[nodiscard]] uint32 GetWorkAreaWidth() const noexcept override { return GetWidth(); }
        [[nodiscard]] uint32 GetWorkAreaHeight() const noexcept override { return GetHeight(); }

        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override { return WindowingBackendType::Wayland; }

        [[nodiscard]] wl_output* GetOutput() const noexcept { return output; }
        [[nodiscard]] uint32 GetRegistryName() const noexcept { return registryName; }
        [[nodiscard]] int32 GetScale() const noexcept { return scale; }

        /* --- COPY SEMANTICS --- */
        WaylandScreen(const WaylandScreen&) = delete;
        WaylandScreen& operator=(const WaylandScreen&) = delete;

        /* --- MOVE SEMANTICS --- */
        WaylandScreen(WaylandScreen&&) = delete;
        WaylandScreen& operator=(WaylandScreen&&) = delete;

        /* --- DESTRUCTOR --- */
        ~WaylandScreen() noexcept override;

    private:
        wl_output* output = nullptr;
        uint32 registryName = 0;

        std::string name = { };
        Vector2Int origin = { 0, 0 };
        Vector2UInt size = { 0, 0 };
        uint16 refreshRate = 0;

        int32 scale = 1;
        bool rotated = false;

        // The compositor describes the screen over time through events, which are handled by the context
        friend class WaylandContext;

    };

}
