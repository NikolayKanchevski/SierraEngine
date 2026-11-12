//
// Created by Nikolay Kanchevski on 10.29.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the X11WindowManager.h file is only allowed in Linux builds!"
#endif

#include "../../WindowManager.h"

#include "X11Context.h"
#include "X11Screen.h"

namespace Sierra
{

    struct X11WindowManagerCreateInfo
    {
        X11Context& x11Context;
    };

    class SIERRA_API X11WindowManager final : public WindowManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit X11WindowManager(const X11WindowManagerCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Window> CreateWindow(const WindowCreateInfo& createInfo) const noexcept override;
        void EnumerateScreens(const ScreenEnumerationPredicate& Predicate) const noexcept override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Screen& GetPrimaryScreen() const noexcept override { return x11Context.GetScreens()[0]; } // NOTE: Screens are never empty, as the display is always treated as at least one screen
        [[nodiscard]] const Screen& GetWindowScreen(const Window& window) const override;

        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override { return WindowingBackendType::X11; }

        /* --- COPY SEMANTICS --- */
        X11WindowManager(const X11WindowManager&) = delete;
        X11WindowManager& operator=(const X11WindowManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        X11WindowManager(X11WindowManager&&) noexcept = delete;
        X11WindowManager& operator=(X11WindowManager&&) noexcept = delete;

        /* --- DESTRUCTOR --- */
        ~X11WindowManager() noexcept override = default;

    private:
        X11Context& x11Context;

    };

}
