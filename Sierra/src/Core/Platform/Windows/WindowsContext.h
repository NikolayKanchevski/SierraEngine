//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the WindowsContext.h file is only allowed in Windows builds!"
#endif

#include "../../PlatformContext.h"
#include "Win32Context.h"

namespace Sierra
{

    class SIERRA_API WindowsContext final : public PlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        WindowsContext();

        /* --- GETTER METHODS --- */
        [[nodiscard]] Win32Context& GetWin32Context() { return win32Context; }
        [[nodiscard]] PlatformType GetType() const override { return PlatformType::Windows; }

    private:
        Win32Context win32Context;


    };

}
