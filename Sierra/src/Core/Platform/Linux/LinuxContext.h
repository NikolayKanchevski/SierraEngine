//
// Created by Nikolay Kanchevski on 9.1.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the LinuxContext.h file is only allowed in Linux builds!"
#endif

#include "../../PlatformContext.h"
#include "X11Context.h"

namespace Sierra
{

    class SIERRA_API LinuxContext final : public PlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit LinuxContext(const PlatformContextCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] const X11Context& GetX11Context() const { return x11Context; }
        [[nodiscard]] PlatformType GetType() const override { return PlatformType::Linux; }

    private:
        X11Context x11Context;

    };

}
