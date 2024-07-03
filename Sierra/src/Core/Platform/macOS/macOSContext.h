//
// Created by Nikolay Kanchevski on 3.09.23.
//

#pragma once

#if !SR_PLATFORM_macOS
    #error "Including the macOSContext.h file is only allowed in macOS builds!"
#endif

#include "../../PlatformContext.h"
#include "CocoaContext.h"

namespace Sierra
{

    class SIERRA_API macOSContext final : public PlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        macOSContext();

        /* --- GETTER METHODS --- */
        [[nodiscard]] PlatformType GetType() const override { return PlatformType::macOS; }
        [[nodiscard]] CocoaContext& GetCocoaContext() { return cocoaContext; }

    private:
        CocoaContext cocoaContext;

    };

}