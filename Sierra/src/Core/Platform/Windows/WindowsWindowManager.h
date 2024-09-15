//
// Created by Nikolay Kanchevski on 8.7.2024.
//

#pragma once

#include "../../WindowManager.h"

#include "Win32Context.h"

namespace Sierra
{

    class SIERRA_API WindowsWindowManager final : public WindowManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit WindowsWindowManager(const WindowManagerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Window> CreateWindow(const WindowCreateInfo &createInfo) const override;

        /* --- DESTRUCTOR --- */
        ~WindowsWindowManager() override = default;

    private:
        Win32Context &win32Context;

    };

}
