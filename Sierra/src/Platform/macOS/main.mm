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

    - (void) applicationDidFinishLaunching: (NSNotification*) notification
    {
        // Create application
        application = Sierra::CreateApplication(_argc, _argv);
        APP_THROW_IF(application == nullptr, Sierra::InvalidValueError("Cannot create macOS application, as application returned from Sierra::CreateApplication() must not be null"));

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