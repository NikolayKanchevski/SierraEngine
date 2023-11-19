//
// Created by Nikolay Kanchevski on 3.09.23.
//

#pragma once

#if !SR_PLATFORM_MACOS
    #error "Including the MacOSInstance.h file is only allowed in macOS builds!"
#endif

#include "../../PlatformInstance.h"
#include "CocoaContext.h"

namespace Sierra
{

    class SIERRA_API MacOSInstance final : public PlatformInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MacOSInstance(const PlatformInstanceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const CocoaContext& GetCocoaContext() const { return cocoaContext; }
        [[nodiscard]] inline PlatformType GetType() const override { return PlatformType::MacOS; }

    private:
        CocoaContext cocoaContext;

    };

}