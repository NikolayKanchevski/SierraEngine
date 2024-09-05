//
// Created by Nikolay Kanchevski on 7.11.23.
//

#pragma once

#include "Window.h"
#include "PlatformContext.h"

namespace Sierra
{

    struct WindowManagerCreateInfo
    {
        PlatformContext& platformContext;
    };

    class SIERRA_API WindowManager
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::unique_ptr<Window> CreateWindow(const WindowCreateInfo& createInfo) const = 0;

        /* --- COPY SEMANTICS --- */
        WindowManager(const WindowManager&) = delete;
        WindowManager& operator=(const WindowManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        WindowManager(WindowManager&&) = delete;
        WindowManager& operator=(WindowManager&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~WindowManager() = default;

    protected:
        explicit WindowManager(const WindowManagerCreateInfo& createInfo);

    };

}
