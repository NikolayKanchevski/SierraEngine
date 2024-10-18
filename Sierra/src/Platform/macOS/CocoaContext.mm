//
// Created by Nikolay Kanchevski on 5.11.23.
//

#include "CocoaContext.h"

@interface CocoaApplicationDelegate : NSObject<NSApplicationDelegate>

@end

@implementation CocoaApplicationDelegate

    /* --- MEMBERS --- */
    {
        Sierra::CocoaContext* cocoaContext;
    }

    /* --- CONSTRUCTORS --- */

    - (instancetype) initWithContext: (Sierra::CocoaContext*) initContext
    {
        self = [super init];
        cocoaContext = initContext;
        return self;
    }

    /* --- EVENTS --- */

    - (void) applicationDidChangeScreenParameters: (NSNotification*) notification
    {
        cocoaContext->ReloadScreens();
    }

@end

@interface CocoaWindow : NSWindow

@end

@implementation CocoaWindow

    /* --- CONSTRUCTORS --- */

    - (instancetype) initWithTitle: (const std::string_view) title width: (const uint32) width height: (const uint32) height
    {
        self = [super initWithContentRect: NSMakeRect(0.0f, 0.0f, static_cast<float32>(width), static_cast<float32>(height)) styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable backing: NSBackingStoreBuffered defer: NO];
        [self setTitle: @(title.data())];
        return self;
    }

    /* --- GETTER METHODS --- */

    - (BOOL) canBecomeKeyWindow
    {
        return YES;
    }

    - (BOOL) canBecomeMainWindow
    {
        return YES;
    }

    /* --- DESTRUCTOR --- */

    - (void) dealloc
    {
        [super dealloc];
    }

@end

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaContext::CocoaContext(const CocoaContextCreateInfo& createInfo)
        : application(createInfo.application)
    {
        SR_THROW_IF(createInfo.application == nil, InvalidValueError("Cannot create Cocoa context, as specified application must not be nil"));

        // Entry point assigns a temporary delegate, so we free and override that
        [[application delegate] release];
        [application setDelegate: [[CocoaApplicationDelegate alloc] initWithContext: this]];

        NSEvent* (^const block)(NSEvent*) = ^NSEvent* (NSEvent* event)
        {
            if ([event modifierFlags] & NSEventModifierFlagCommand) [[application keyWindow] sendEvent: event];
            return event;
        };

        NSDictionary* const defaults = @{@"ApplePressAndHoldEnabled": @NO};
        [[NSUserDefaults standardUserDefaults] registerDefaults: defaults];

        // Retrieve monitors
        ReloadScreens();
    }

    void CocoaContext::ReloadScreens()
    {
        screens.clear();

        screens.reserve([[NSScreen screens] count]);
        for (const NSScreen* screen in [NSScreen screens])
        {
            screens.emplace_back(CocoaScreen({ .screen = screen }));
        }
    }

    NSWindow* CocoaContext::CreateWindow(const std::string_view title, const uint32 width, const uint32 height) const
    {
        NSWindow* const window = [[CocoaWindow alloc] initWithTitle: title width: width height: height];
        [window center];
        [window setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary | NSWindowCollectionBehaviorManaged];
        [window setAcceptsMouseMovedEvents: YES];
        [window setRestorable: NO];
        #if MAC_OS_X_VERSION_MAX_ALLOWED >= 101200
            if ([window respondsToSelector: @selector(setTabbingMode:)]) [window setTabbingMode: NSWindowTabbingModeDisallowed];
        #endif
        return window;
    }

    void CocoaContext::DestroyWindow(NSWindow* window) const
    {
        [window performClose: nil];
        [window setDelegate: nil];
        [window setContentView: nil];
        [window release];
    }

    /* --- GETTER METHODS --- */

    CocoaScreen& CocoaContext::GetWindowScreen(const NSWindow* window)
    {
        return const_cast<CocoaScreen&>(const_cast<const CocoaContext*>(this)->GetWindowScreen(window));
    }

    const CocoaScreen& CocoaContext::GetWindowScreen(const NSWindow* window) const
    {
        return *std::find_if(screens.begin(), screens.end(), [window](const CocoaScreen& cocoaScreen) -> bool { return cocoaScreen.GetNSScreen() == [window screen]; });
    }

    /* --- PRIVATE METHODS --- */

    void CocoaContext::Update()
    {
        NSEvent* event = [application nextEventMatchingMask: NSEventMaskAny untilDate: [NSDate distantPast] inMode: NSDefaultRunLoopMode dequeue: YES];
        while (event != nil)
        {
            [application sendEvent: event];
            event = [application nextEventMatchingMask: NSEventMaskAny untilDate: [NSDate distantPast] inMode: NSDefaultRunLoopMode dequeue: YES];
        }
    }

    /* --- DESTRUCTOR --- */

    CocoaContext::~CocoaContext() noexcept
    {
        [[application delegate] release];
        [application setDelegate: nil];
    }

}