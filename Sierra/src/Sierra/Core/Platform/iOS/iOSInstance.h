//
// Created by Nikolay Kanchevski on 26.09.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the iOSInstance.h file is only allowed in iOS builds!"
#endif

#include "../../PlatformInstance.h"

#if !defined(__OBJC__)
    namespace Sierra
    {
        typedef void UIKitApplicationRunLoopConnector;
    }
#else
    #include <UIKit/UIKit.h>
    @class UIKitApplicationRunLoopConnector;

    @interface UIKitApplicationDelegate : UIResponder<UIApplicationDelegate>

        /* --- POLLING METHODS --- */
        - (BOOL) applicationShouldUpdate;

        /* --- GETTER METHODS --- */
        - (UIWindowScene*) GetActiveWindowScene;

    @end

#endif

namespace Sierra
{

    class SIERRA_API iOSInstance final : public PlatformInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit iOSInstance(const PlatformInstanceCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void RunApplication(const PlatformApplicationRunInfo &runInfo) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline PlatformType GetType() const override { return PlatformType::iOS; }

        /* --- DESTRUCTOR --- */
        ~iOSInstance() override;

    private:
        UIKitApplicationRunLoopConnector* applicationRunLoopConnector;

    };

}
