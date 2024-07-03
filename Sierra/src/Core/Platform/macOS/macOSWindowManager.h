//
// Created by Nikolay Kanchevski on 3.07.24.
//

#pragma once

#include "../../WindowManager.h"

#include "CocoaContext.h"

namespace Sierra
{

    class SIERRA_API macOSWindowManager final : public WindowManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit macOSWindowManager(const WindowManagerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Window> CreateWindow(const WindowCreateInfo &createInfo) const override;

        /* --- DESTRUCTOR --- */
        ~macOSWindowManager() override = default;

    private:
        CocoaContext &cocoaContext;

    };

}
