//
// Created by Nikolay Kanchevski on 9.1.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the LinuxInstance.h file is only allowed in Linux builds!"
#endif

#include "../../PlatformInstance.h"
#include "X11Context.h"

namespace Sierra
{

    class SIERRA_API LinuxInstance final : public PlatformInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit LinuxInstance(const PlatformInstanceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const X11Context& GetX11Context() const { return x11Context; }

        PlatformType GetType() const override { return PlatformType::Linux; }

    private:
        X11Context x11Context;

    };

}
