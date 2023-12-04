//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the WindowsInstance.h file is only allowed in Windows builds!"
#endif

#include "../../PlatformInstance.h"
#include "Win32Context.h"

namespace Sierra
{

    class SIERRA_API WindowsInstance final : public PlatformInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit WindowsInstance(const PlatformInstanceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const Win32Context& GetWin32Context() const { return win32Context; }
        [[nodiscard]] inline PlatformType GetType() const override { return PlatformType::Windows; }

    private:
        Win32Context win32Context;


    };

}
