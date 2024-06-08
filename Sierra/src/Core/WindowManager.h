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
        PlatformContext &platformContext;
    };

    class SIERRA_API WindowManager final
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Window> CreateWindow(const WindowCreateInfo &createInfo) const;

        /* --- OPERATORS --- */
        WindowManager(const WindowManager&) = delete;
        WindowManager& operator=(const WindowManager&) = delete;

    private:
        PlatformContext &platformContext;
        explicit WindowManager(const WindowManagerCreateInfo &createInfo);

        friend class Application;
        [[nodiscard]] static std::unique_ptr<WindowManager> Create(const WindowManagerCreateInfo &createInfo);

    };

}
