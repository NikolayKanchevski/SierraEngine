//
// Created by Nikolay Kanchevski on 3.09.23.
//

#pragma once

#if !SR_PLATFORM_macOS
    #error "Including the macOSInstance.h file is only allowed in macOS builds!"
#endif

#include "../../PlatformInstance.h"
#include "CocoaContext.h"

namespace Sierra
{

    class SIERRA_API macOSInstance final : public PlatformInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit macOSInstance(const PlatformInstanceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const CocoaContext& GetCocoaContext() const { return cocoaContext; }
        [[nodiscard]] inline PlatformType GetType() const override { return PlatformType::macOS; }

    private:
        CocoaContext cocoaContext;

    };

}