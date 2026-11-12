//
// Created by Nikolay Kanchevski on 11.12.26.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the WaylandWindowManager.h file is only allowed in Linux builds!"
#endif

#include "../../WindowManager.h"

#include "WaylandScreen.h"
#include "WaylandContext.h"

namespace Sierra
{

    struct WaylandWindowManagerCreateInfo
    {
        WaylandContext& waylandContext;
    };

    class SIERRA_API WaylandWindowManager final : public WindowManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit WaylandWindowManager(const WaylandWindowManagerCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Window> CreateWindow(const WindowCreateInfo& createInfo) const noexcept override;
        void EnumerateScreens(const ScreenEnumerationPredicate& Predicate) const noexcept override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Screen& GetPrimaryScreen() const noexcept override { return *waylandContext.GetScreens()[0]; } // NOTE: Wayland has no notion of a primary screen, so the first one reported is used
        [[nodiscard]] const Screen& GetWindowScreen(const Window& window) const override;

        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override { return WindowingBackendType::Wayland; }

        /* --- COPY SEMANTICS --- */
        WaylandWindowManager(const WaylandWindowManager&) = delete;
        WaylandWindowManager& operator=(const WaylandWindowManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        WaylandWindowManager(WaylandWindowManager&&) noexcept = delete;
        WaylandWindowManager& operator=(WaylandWindowManager&&) noexcept = delete;

        /* --- DESTRUCTOR --- */
        ~WaylandWindowManager() noexcept override = default;

    private:
        WaylandContext& waylandContext;

    };

}
