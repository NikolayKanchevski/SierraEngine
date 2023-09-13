//
// Created by Nikolay Kanchevski on 3.09.23.
//

#include "MacOSInstance.h"

@interface CocoaApplication : NSApplication

@end

@implementation CocoaApplication

@end

@interface CocoaApplicationDelegate : NSObject<NSApplicationDelegate>

@end

@implementation CocoaApplicationDelegate

    /* --- MEMBERS --- */
    {
        CocoaApplication* application;
    }

    /* --- CONSTRUCTORS --- */

    - (instancetype) initWithApplication: (CocoaApplication*) initApplication
    {
        self = [super init];
        if (self != nil)
        {
            application = initApplication;
        }
        return self;
    }

    // Application closed:
    - (NSApplicationTerminateReply) applicationShouldTerminate: (NSApplication*) sender
    {
        // Close all windows
        NSArray<NSWindow*>* windows = [application windows];
        for (uint32 i = [windows count]; i--;)
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
            [[NSBundle mainBundle] loadNibNamed: @"MainMenu" owner: application topLevelObjects: nil];
        }
        else
        {
            // Get process name
            id appName = [[NSProcessInfo processInfo] processName];

            // Create default application menu bar
            @autoreleasepool
            {
                NSMenu* bar = [[NSMenu alloc] init];
                NSMenuItem* appMenuItem = [bar addItemWithTitle: @"" action:nil keyEquivalent: @""];
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
                [application setServicesMenu: servicesMenu];
                [servicesMenu release];

                NSMenuItem* windowMenuItem = [bar addItemWithTitle: @"" action:nil keyEquivalent: @""];
                NSMenu* windowMenu = [[NSMenu alloc] initWithTitle: @"Window"];
                [windowMenu addItemWithTitle: @"Minimize" action: @selector(performMiniaturize:) keyEquivalent: @"m"];
                [windowMenu addItemWithTitle: @"Zoom" action: @selector(performZoom:) keyEquivalent: @""];
                [windowMenu addItem: [NSMenuItem separatorItem]];
                [windowMenu addItemWithTitle: @"Bring All to Front" action: @selector(arrangeInFront:) keyEquivalent: @""];
                [windowMenu addItem: [NSMenuItem separatorItem]];
                [[windowMenu addItemWithTitle: @"Enter Full Screen" action: @selector(toggleFullScreen:) keyEquivalent: @"f"] setKeyEquivalentModifierMask:NSEventModifierFlagControl | NSEventModifierFlagCommand];
                [application setWindowsMenu: windowMenu];
                [windowMenuItem setSubmenu: windowMenu];

                [application setMainMenu:bar];
                [bar release];

                // This is required for macOS versions prior to Snow Leopard
                SEL setAppleMenuSelector = NSSelectorFromString(@"setAppleMenu:");
                [application performSelector: setAppleMenuSelector withObject: appMenu];
            }
        }
    }

    // Application already setup:
    - (void) applicationDidFinishLaunching: (NSNotification*) notification
    {
        NSEvent* event = [NSEvent otherEventWithType: NSEventTypeApplicationDefined location: NSMakePoint(0, 0) modifierFlags: 0 timestamp: 0 windowNumber: 0 context: nil subtype: 0 data1: 0 data2: 0];
        [application postEvent:event atStart: YES];
        [application stop: nil];
    }

@end



namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MacOSInstance::MacOSInstance(const PlatformInstanceCreateInfo &createInfo)
        : PlatformInstance(createInfo)
    {
        @autoreleasepool
        {
            // Initialize application
            application = [CocoaApplication sharedApplication];

            // Create delegate
            applicationDelegate = [[CocoaApplicationDelegate alloc] initWithApplication:application];
            SR_ERROR_IF(applicationDelegate == nil, "Could not create MacOS application delegate!");
        }

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
    }

    /* --- GETTER METHODS --- */

    CocoaApplication* MacOSInstance::GetApplication() const
    {
        return application;
    }

    uint32 MacOSInstance::GetMenuBarHeight() const
    {
        return NSHeight([NSScreen mainScreen].frame) - NSHeight([NSScreen mainScreen].visibleFrame);
    }

    /* --- DESTRUCTOR --- */

    MacOSInstance::~MacOSInstance()
    {
        [application setDelegate: nil];
        application = nil;

        @autoreleasepool
        {
            [applicationDelegate release];
            applicationDelegate = nil;
        }
    }

}