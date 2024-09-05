//
// Created by Nikolay Kanchevski on 26.09.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the iOSContext.h file is only allowed in iOS builds!"
#endif

#include "../../PlatformContext.h"

#include "UIKitContext.h"

namespace Sierra
{

    class SIERRA_API iOSContext final : public PlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit iOSContext(const PlatformContextCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] PlatformType GetType() const override { return PlatformType::iOS; }
        [[nodiscard]] UIKitContext& GetUIKitContext() { return uiKitContext; }

    private:
        UIKitContext uiKitContext;

    };

}
