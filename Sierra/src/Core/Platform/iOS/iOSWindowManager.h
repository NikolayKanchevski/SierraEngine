//
// Created by Nikolay Kanchevski on 3.07.24.
//

#pragma once

#include "../../WindowManager.h"

#include "UIKitContext.h"

namespace Sierra
{

    class SIERRA_API iOSWindowManager final : public WindowManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit iOSWindowManager(const WindowManagerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Window> CreateWindow(const WindowCreateInfo &createInfo) const override;

        /* --- DESTRUCTOR --- */
        ~iOSWindowManager() override = default;

    private:
        UIKitContext &uiKitContext;

    };

}
