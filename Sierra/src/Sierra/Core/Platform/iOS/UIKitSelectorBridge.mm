//
// Created by Nikolay Kanchevski on 14.11.23.
//

#include "UIKitSelectorBridge.h"

struct SelectorDispatcherInitInfo
{
    const std::function<void()> &Callback;
    const NSNotificationCenter* notificationCenter;
    const NSNotificationName notificationName;
};

@interface SelectorDispatcher : NSObject

@end

@implementation SelectorDispatcher

    /* --- MEMBERS --- */
    {
        std::function<void()> Callback;
        const NSNotificationCenter* notificationCenter;
    }

    /* --- CONSTRUCTORS --- */

    - (instancetype) initWithInfo: (const SelectorDispatcherInitInfo&) initInfo
    {
        self = [super init];

        // Save members
        Callback = initInfo.Callback;
        notificationCenter = initInfo.notificationCenter;

        // Add observer
        [notificationCenter addObserver: self selector: @selector(OnNotification) name: initInfo.notificationName object: nil];

        return self;
    }

    /* --- EVENTS --- */

    - (void) OnNotification
    {
        Callback();
    }

    /* --- DESTRUCTOR --- */

    - (void) dealloc
    {
        [notificationCenter removeObserver: self];
        [super dealloc];
    }

@end

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UIKitSelectorBridge::UIKitSelectorBridge(const NSNotificationCenter* notificationCenter, const NSNotificationName notificationName, const std::function<void()> &Callback)
        : selectorDispatcher([[SelectorDispatcher alloc] initWithInfo: { .Callback = Callback, .notificationCenter = notificationCenter, .notificationName = notificationName }])
    {

    }

    /* --- POLLING METHODS --- */

    void UIKitSelectorBridge::Invalidate() const
    {
        [selectorDispatcher release];
    }

}
