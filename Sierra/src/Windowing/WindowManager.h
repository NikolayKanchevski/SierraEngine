//
// Created by Nikolay Kanchevski on 10.2.26.
//

#pragma once

#include "Window.h"
#include "WindowingBackendType.h"

namespace Sierra
{

    class SIERRA_API WindowManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using ScreenEnumerationPredicate = std::function<void(const Screen&)>;

        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::unique_ptr<Window> CreateWindow(const WindowCreateInfo& createInfo) const noexcept = 0;

        [[nodiscard]] virtual const Screen& GetPrimaryScreen() const noexcept = 0;
        [[nodiscard]] virtual const Screen& GetWindowScreen(const Window& window) const = 0;
        virtual void EnumerateScreens(const ScreenEnumerationPredicate& Predicate) const noexcept = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual WindowingBackendType GetBackendType() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        WindowManager(const WindowManager&) = delete;
        WindowManager& operator=(const WindowManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        WindowManager(WindowManager&&) = delete;
        WindowManager& operator=(WindowManager&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~WindowManager() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        WindowManager() noexcept = default;

    };

}