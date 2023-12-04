//
// Created by Nikolay Kanchevski on 26.09.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the iOSInstance.h file is only allowed in iOS builds!"
#endif

#include "../../PlatformInstance.h"
#include "UIKitContext.h"

#if defined(__OBJC__)
    #include <UIKit/UIKit.h>
#endif

namespace Sierra
{

    class SIERRA_API iOSInstance final : public PlatformInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit iOSInstance(const PlatformInstanceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const UIKitContext& GetUIKitContext() const { return uiKitContext; }
        [[nodiscard]] inline PlatformType GetType() const override { return PlatformType::iOS; }

    private:
        UIKitContext uiKitContext;
        void RunApplication(const PlatformApplicationRunInfo &runInfo) override;

    };

}
