//
// Created by Nikolay Kanchevski on 5.11.23.
//

#define COCOA_CONTEXT_IMPLEMENTATION
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

    // Application closed:
    - (NSApplicationTerminateReply) applicationShouldTerminate: (NSApplication*) sender
    {
        // Close all windows
        NSArray<NSWindow*>* windows = [cocoaContext->GetApplication() windows];
        for (ulong i = [windows count]; i--;)
        {
            [windows[i] performClose: nil];
        }

        return NSTerminateCancel;
    }

    // Application being setup:
    - (void) applicationWillFinishLaunching: (NSNotification*) notification
    {
        if ([[NSBundle mainBundle] pathForResource: @"MainMenu" ofType: @"nib"])
        {
            [[NSBundle mainBundle] loadNibNamed: @"MainMenu" owner: cocoaContext->GetApplication() topLevelObjects: nil];
        }
        else
        {
            // Get process name
            id appName = [[NSProcessInfo processInfo] processName];

            // Create default application menu bar
            NSMenu* bar = [[NSMenu alloc] init];
            NSMenuItem* appMenuItem = [bar addItemWithTitle: @"" action: nil keyEquivalent: @""];
            NSMenu* appMenu = [[NSMenu alloc] init];
            [appMenu addItemWithTitle:[NSString stringWithFormat: @"About %@", appName] action: @selector(orderFrontStandardAboutPanel:) keyEquivalent: @""];
            [appMenu addItem:[NSMenuItem separatorItem]];

            NSMenu* servicesMenu = [[NSMenu alloc] init];
            [[appMenu addItemWithTitle: @"Services" action:nil keyEquivalent: @""] setSubmenu:servicesMenu];
            [appMenu addItem: [NSMenuItem separatorItem]];
            [appMenu addItemWithTitle: [NSString stringWithFormat: @"Hide %@", appName] action: @selector(hide:) keyEquivalent: @"h"];
            [[appMenu addItemWithTitle: @"Hide Others" action: @selector(hideOtherApplications:) keyEquivalent: @"h"] setKeyEquivalentModifierMask:NSEventModifierFlagOption | NSEventModifierFlagCommand];
            [appMenu addItemWithTitle: @"Show All" action: @selector(unhideAllApplications:) keyEquivalent: @""];
            [appMenu addItem: [NSMenuItem separatorItem]];
            [appMenu addItemWithTitle: [NSString stringWithFormat: @"Quit %@", appName] action: @selector(terminate:) keyEquivalent: @"q"];
            [appMenuItem setSubmenu: appMenu];
            [cocoaContext->GetApplication() setServicesMenu: servicesMenu];
            [servicesMenu release];

            NSMenuItem* windowMenuItem = [bar addItemWithTitle: @"" action:nil keyEquivalent: @""];
            NSMenu* windowMenu = [[NSMenu alloc] initWithTitle: @"Window"];
            [windowMenu addItemWithTitle: @"Minimize" action: @selector(performMiniaturize:) keyEquivalent: @"m"];
            [windowMenu addItemWithTitle: @"Zoom" action: @selector(performZoom:) keyEquivalent: @""];
            [windowMenu addItem: [NSMenuItem separatorItem]];
            [windowMenu addItemWithTitle: @"Bring All to Front" action: @selector(arrangeInFront:) keyEquivalent: @""];
            [windowMenu addItem: [NSMenuItem separatorItem]];
            [[windowMenu addItemWithTitle: @"Enter Full Screen" action: @selector(toggleFullScreen:) keyEquivalent: @"f"] setKeyEquivalentModifierMask:NSEventModifierFlagControl | NSEventModifierFlagCommand];
            [cocoaContext->GetApplication() setWindowsMenu: windowMenu];
            [windowMenuItem setSubmenu: windowMenu];
            [windowMenu release];

            [cocoaContext->GetApplication() setMainMenu: bar];
            [bar release];

            // This is required for macOS versions prior to Snow Leopard
            SEL setAppleMenuSelector = NSSelectorFromString(@"setAppleMenu:");
            [cocoaContext->GetApplication() performSelector: setAppleMenuSelector withObject: appMenu];
            [appMenu release];
        }
    }

    // Application already setup:
    - (void) applicationDidFinishLaunching: (NSNotification*) notification
    {
        NSEvent* event = [NSEvent otherEventWithType: NSEventTypeApplicationDefined location: NSMakePoint(0, 0) modifierFlags: 0 timestamp: 0 windowNumber: 0 context: nil subtype: 0 data1: 0 data2: 0];
        [cocoaContext->GetApplication() postEvent:event atStart: YES];
        [cocoaContext->GetApplication() stop: nil];
    }

    - (void) applicationDidChangeScreenParameters: (NSNotification*) notification
    {
        cocoaContext->ApplicationDidChangeScreenParameters(notification);
    }

@end

@interface CocoaWindow : NSWindow

@end

@implementation CocoaWindow

    /* --- CONSTRUCTORS --- */

    - (instancetype)initWithTitle: (const std::string&) title width: (const uint32) width height: (const uint32) height
    {
        self = [self initWithContentRect: NSMakeRect(0.0f, 0.0f, static_cast<float32>(width), static_cast<float32>(height)) styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable backing: NSBackingStoreBuffered defer: NO];
        [self setTitle: @(title.c_str())];
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

    CocoaContext::CocoaContext(const CocoaContextCreateInfo &createInfo)
    {
        // Get application instance and delegate
        application = [NSApplication sharedApplication];
        applicationDelegate = [[CocoaApplicationDelegate alloc] initWithContext: this];

        // Assign delegate and filter out specific events
        [application setDelegate: applicationDelegate];
        NSEvent* (^block)(NSEvent*) = ^ NSEvent* (NSEvent* event)
        {
            if ([event modifierFlags] & NSEventModifierFlagCommand) [[application keyWindow] sendEvent: event];
            return event;
        };

        NSDictionary* defaults = @{@"ApplePressAndHoldEnabled": @NO};
        [[NSUserDefaults standardUserDefaults] registerDefaults: defaults];

        // Run application
        if (![[NSRunningApplication currentApplication] isFinishedLaunching]) [application run];
        [application setActivationPolicy: NSApplicationActivationPolicyRegular];

        // Retrieve monitors
        ReloadScreens();
    }

    /* --- POLLING METHODS --- */

    NSWindow* CocoaContext::CreateWindow(const std::string &title, uint32 width, uint32 height) const
    {
        NSWindow* window = [[CocoaWindow alloc] initWithTitle: title width: width height: height];
        [window center];
        [window setTitle: @(title.c_str())];
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
    }

    NSEvent* CocoaContext::PollNextEvent() const
    {
        NSEvent* event = [application nextEventMatchingMask: NSEventMaskAny untilDate: [NSDate distantPast] inMode: NSDefaultRunLoopMode dequeue: YES];
        if (event != nil) [application sendEvent: event];
        return event;
    }

    /* --- GETTER METHODS --- */

    const CocoaScreen& CocoaContext::GetPrimaryScreen() const
    {
        return screens[0].cocoaScreen;
    }

    const CocoaScreen& CocoaContext::GetWindowScreen(const NSWindow* window) const
    {
        return std::find_if(screens.begin(), screens.end(), [window](const CocoaScreenPair &pair) { return pair.nsScreen == [window screen]; })->cocoaScreen;
    }

    /* --- EVENTS --- */

    void CocoaContext::ApplicationDidChangeScreenParameters(const NSNotification* notification)
    {
        ReloadScreens();
    }

    /* --- PRIVATE METHODS --- */

    void CocoaContext::ReloadScreens()
    {
        screens.clear();
        screens.reserve([[NSScreen screens] count]);
        for (const NSScreen* screen in [NSScreen screens])
        {
            screens.emplace_back(CocoaScreenPair{ screen, CocoaScreen({ .nsScreen = screen })});
        }
        screens.shrink_to_fit();
    }

    /* --- DESTRUCTOR --- */

    CocoaContext::~CocoaContext()
    {
        [application setDelegate: nil];
        application = nil;

        [applicationDelegate release];
        applicationDelegate = nil;
    }

}