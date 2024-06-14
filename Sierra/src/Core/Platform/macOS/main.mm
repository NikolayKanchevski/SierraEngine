//
// Created by Nikolay Kanchevski on 3.06.24.
//

#include <Cocoa/Cocoa.h>

@interface CocoaEntryPointDelegate : NSObject<NSApplicationDelegate>

@end

@implementation CocoaEntryPointDelegate

    /* --- EVENTS --- */

    - (BOOL) applicationSupportsSecureRestorableState: (NSApplication*) application
    {
        return YES;
    }

    - (void) applicationWillFinishLaunching: (NSNotification*) notification
    {
        if ([[NSBundle mainBundle] pathForResource: @"MainMenu" ofType: @"nib"])
        {
            [[NSBundle mainBundle] loadNibNamed: @"MainMenu" owner: [NSApplication sharedApplication] topLevelObjects: nil];
        }
        else
        {
            // Get process name
            id appName = [[NSProcessInfo processInfo] processName];

            // Create default application menu bar
            NSMenu* bar = [[NSMenu alloc] init];
            NSMenuItem* appMenuItem = [bar addItemWithTitle: @"" action: nil keyEquivalent: @""];
            NSMenu* appMenu = [[NSMenu alloc] init];
            [appMenu addItemWithTitle: [NSString stringWithFormat: @"About %@", appName] action: @selector(orderFrontStandardAboutPanel:) keyEquivalent: @""];
            [appMenu addItem: [NSMenuItem separatorItem]];

            NSMenu* servicesMenu = [[NSMenu alloc] init];
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

            NSMenuItem* windowMenuItem = [bar addItemWithTitle: @"" action:nil keyEquivalent: @""];
            NSMenu* windowMenu = [[NSMenu alloc] initWithTitle: @"Window"];
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
    }

    - (void) applicationDidFinishLaunching: (NSNotification*) notification
    {
        // Process arguments
        const char* argv[[NSProcessInfo processInfo].arguments.count];
        for (uint32 i = 0; i < [NSProcessInfo processInfo].arguments.count; i++)
        {
            argv[i] = [[NSProcessInfo processInfo].arguments[i] UTF8String];
        }

        // Create and application
        const std::unique_ptr<Sierra::Application> application = std::unique_ptr<Sierra::Application>(Sierra::CreateApplication(static_cast<int>([NSProcessInfo processInfo].arguments.count), argv));
        if (application == nullptr)
        {
            APP_ERROR("Created application returned from Sierra::CreateApplication() must not be a null pointer!");
            return FALSE;
        }

        // Run application
        while (!application->Update());
    }

    - (NSApplicationTerminateReply) applicationShouldTerminate: (NSApplication*) sender
    {
        NSArray<NSWindow*>* windows = [[NSApplication sharedApplication] windows];
        for (NSWindow* window in windows) [window performClose: nil];
        return NSTerminateCancel;
    }

@end

int main(const int argc, char* argv[])
{
    // NOTE: This delegate is released in CocoaContext.mm, when overridden by a new one
    [[NSApplication sharedApplication] setDelegate: [[CocoaEntryPointDelegate alloc] init]];
    NSApplicationMain(argc, argv);
}