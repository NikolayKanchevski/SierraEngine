//
// Created by Nikolay Kanchevski on 19.11.26.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the Win32WindowManager.h file is only allowed in Windows builds!"
#endif

#include "../WindowManager.h"

#include "Win32Screen.h"
#include "../../Platform/Windows/Win32Context.h"

namespace Sierra
{

    struct Win32WindowManagerCreateInfo
    {
        Win32Context& win32Context;
    };

    class SIERRA_API Win32WindowManager final : public WindowManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Win32WindowManager(const Win32WindowManagerCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Window> CreateWindow(const WindowCreateInfo& createInfo) const noexcept override;
        void EnumerateScreens(const ScreenEnumerationPredicate& Predicate) const noexcept override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Screen& GetPrimaryScreen() const noexcept override { return win32Context.GetPrimaryScreen(); }
        [[nodiscard]] const Screen& GetWindowScreen(const Window& window) const override;

        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override { return WindowingBackendType::Win32; }

        /* --- COPY SEMANTICS --- */
        Win32WindowManager(const Win32WindowManager&) = delete;
        Win32WindowManager& operator=(const Win32WindowManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        Win32WindowManager(Win32WindowManager&&) noexcept = delete;
        Win32WindowManager& operator=(Win32WindowManager&&) noexcept = delete;

        /* --- DESTRUCTOR --- */
        ~Win32WindowManager() noexcept override = default;

    private:
        Win32Context& win32Context;

    };

}
