//
// Created by Nikolay Kanchevski on 14.11.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the UIKitSelectorBridge.h file is only allowed in iOS builds!"
#endif

#if !defined(__OBJC__)
    namespace Sierra
    {
        using SelectorDispatcher = void;
        using NSNotificationCenter = void;
        using NSNotificationName = void*;
    }
#else
    #include <UIKit/UIKit.h>
    @class SelectorDispatcher;
#endif

namespace Sierra
{

    class SIERRA_API UIKitSelectorBridge final
    {
    public:
        /* --- CONSTRUCTORS --- */
        UIKitSelectorBridge() = default;
        explicit UIKitSelectorBridge(const NSNotificationCenter* notificationCenter, NSNotificationName notificationName, const std::function<void()> &Callback);

        /* --- POLLING METHODS --- */
        void Invalidate() const;

    private:
        const SelectorDispatcher* selectorDispatcher = nullptr;

    };

}
