//
// Created by Nikolay Kanchevski on 7.11.23.
//

#pragma once

#include "Window.h"
#include "PlatformInstance.h"

namespace Sierra
{

    struct WindowManagerCreateInfo
    {
        const UniquePtr<PlatformInstance> &platformInstance;
    };

    class SIERRA_API WindowManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        static UniquePtr<WindowManager> Create(const WindowManagerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        UniquePtr<Window> CreateWindow(const WindowCreateInfo &createInfo) const;

        /* --- OPERATORS --- */
        WindowManager(const WindowManager&) = delete;
        WindowManager& operator=(const WindowManager&) = delete;

    private:
        const UniquePtr<PlatformInstance> &platformInstance;
        explicit WindowManager(const WindowManagerCreateInfo &createInfo);

    };

}
