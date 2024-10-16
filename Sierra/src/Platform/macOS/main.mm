//
// Created by Nikolay Kanchevski on 3.06.24.
//

#include <Cocoa/Cocoa.h>

namespace
{
    int _argc;
    char** _argv;
}

@interface CocoaEntryPointDelegate : NSObject<NSApplicationDelegate>

@end

@implementation CocoaEntryPointDelegate

    /* --- MEMBERS --- */
    {
        NSTimer* timer;
        Sierra::Application* application;
    }

    /* --- EVENTS --- */

    - (BOOL) applicationSupportsSecureRestorableState: (NSApplication*) application
    {
        return YES;
    }

    - (void) applicationWillFinishLaunching: (NSNotification*) notification
    {
        // Get process name
        id appName = [[NSProcessInfo processInfo] processName];

        // Create default application menu bar
        NSMenu* const bar = [[NSMenu alloc] init];
        const NSMenuItem* const appMenuItem = [bar addItemWithTitle: @"" action: nil keyEquivalent: @""];
        NSMenu* const appMenu = [[NSMenu alloc] init];
        [appMenu addItemWithTitle: [NSString stringWithFormat: @"About %@", appName] action: @selector(orderFrontStandardAboutPanel:) keyEquivalent: @""];
        [appMenu addItem: [NSMenuItem separatorItem]];

        NSMenu* const servicesMenu = [[NSMenu alloc] init];
        [[appMenu addItemWithTitle: @"Services" action:nil keyEquivalent: @""] setSubmenu:servicesMenu];
        [appMenu addItem: [NSMenuItem separatorItem]];
        [appMenu addItemWithTitle: [NSString stringWithFormat: @"Hide %@", appName] action: @selector(hide:) keyEquivalent: @"h"];
        [[appMenu addItemWithTitle: @"Hide Others" action: @selector(hideOtherApplications:) keyEquivalent: @"h"] setKeyEquivalentModifierMask:NSEventModifierFlagOption | NSEventModifierFlagCommand];
        [appMenu addItemWithTitle: @"Show All" action: @selector(unhideAllApplications:) keyEquivalent: @""];
        [appMenu addItem: [NSMenuItem separatorItem]];
        [appMenu addItemWithTitle: [NSString stringWithFormat: @"Quit %@", appName] action: @selector(terminate:) keyEquivalent: @"q"];
        [appMenuItem setSubmenu: appMenu];
        [[NSApplication sharedApplication] setServicesMenu: servicesMenu];
        [servicesMenu release];

        const NSMenuItem* const windowMenuItem = [bar addItemWithTitle: @"" action:nil keyEquivalent: @""];
        NSMenu* const windowMenu = [[NSMenu alloc] initWithTitle: @"Window"];
        [windowMenu addItemWithTitle: @"Minimize" action: @selector(performMiniaturize:) keyEquivalent: @"m"];
        [windowMenu addItemWithTitle: @"Zoom" action: @selector(performZoom:) keyEquivalent: @""];
        [windowMenu addItem: [NSMenuItem separatorItem]];
        [windowMenu addItemWithTitle: @"Bring All to Front" action: @selector(arrangeInFront:) keyEquivalent: @""];
        [windowMenu addItem: [NSMenuItem separatorItem]];
        [[windowMenu addItemWithTitle: @"Enter Full Screen" action: @selector(toggleFullScreen:) keyEquivalent: @"f"] setKeyEquivalentModifierMask:NSEventModifierFlagControl | NSEventModifierFlagCommand];
        [[NSApplication sharedApplication] setWindowsMenu: windowMenu];
        [windowMenuItem setSubmenu: windowMenu];
        [windowMenu release];

        [[NSApplication sharedApplication] setMainMenu: bar];
        [bar release];

        // This is required for macOS versions prior to Snow Leopard
        SEL setAppleMenuSelector = NSSelectorFromString(@"setAppleMenu:");
        [[NSApplication sharedApplication] performSelector: setAppleMenuSelector withObject: appMenu];
        [appMenu release];
    }

    - (void) applicationDidFinishLaunching: (NSNotification*) notification
    {
        // Create application
        application = Sierra::CreateApplication(_argc, _argv);
        if (application == nullptr)
        {
            APP_ERROR("Created application returned from Sierra::CreateApplication() must not be a null pointer!");
        }

        // Create run loop
        timer = [NSTimer scheduledTimerWithTimeInterval: 0 target: self selector: @selector(applicationShouldUpdate) userInfo: nil repeats: true];
        [[NSRunLoop mainRunLoop] addTimer: timer forMode: NSDefaultRunLoopMode];
    }

    - (void) applicationShouldUpdate
    {
        if (application->Update())
        {
            [timer invalidate];
            [timer release];

            [[NSApplication sharedApplication] terminate: nil];
        }
    }

    - (NSApplicationTerminateReply) applicationShouldTerminate: (NSApplication*) sender
    {
        NSArray<NSWindow*>* windows = [[NSApplication sharedApplication] windows];
        for (NSWindow* window in windows) [window performClose: nil];

        delete(application);
        return NSTerminateCancel;
    }

@end

int main(const int argc, char* argv[])
{
    _argc = argc;
    _argv = argv;

    // NOTE: This delegate is released in CocoaContext.mm, when overridden by a new one
    [[NSApplication sharedApplication] setDelegate: [[CocoaEntryPointDelegate alloc] init]];
    NSApplicationMain(argc, const_cast<const char**>(argv));
}