//
// Created by Nikolay Kanchevski on 3.06.24.
//

#include <UIKit/UIKit.h>

#include "../../Application.h"

@interface UIKitEntryPointDelegate : UIResponder<UIApplicationDelegate>

@end

@implementation UIKitEntryPointDelegate

    /* --- MEMBERS --- */
    {
        NSTimer* timer;
        CADisplayLink* displayLink;
        Sierra::Application* application;
    }

    /* --- EVENTS --- */

    - (BOOL) application: (UIApplication*) application didFinishLaunchingWithOptions: (NSDictionary*) launchOptions
    {
        return YES;
    }

    - (void) sceneDidBecomeActive: (UIScene*) activeScene
    {
        // Only consider first scene load as entry point
        if (displayLink != nil) return;

        // Connect run loop
        displayLink = [CADisplayLink displayLinkWithTarget: self selector: @selector(applicationShouldUpdate)];
        // [displayLink addToRunLoop: [NSRunLoop mainRunLoop] forMode: NSDefaultRunLoopMode];
        timer = [NSTimer scheduledTimerWithTimeInterval: 0 target: self selector: @selector(applicationShouldUpdate) userInfo: nil repeats: true];
        [[NSRunLoop mainRunLoop] addTimer: timer forMode: NSDefaultRunLoopMode];

        // Process arguments
        const char* argv[[NSProcessInfo processInfo].arguments.count];
        for (uint32 i = 0; i < [NSProcessInfo processInfo].arguments.count; i++)
        {
            argv[i] = [[NSProcessInfo processInfo].arguments[i] UTF8String];
        }

        // Create and run application
        application = Sierra::CreateApplication(static_cast<int>([NSProcessInfo processInfo].arguments.count), argv);
    }

    - (void) applicationShouldUpdate
    {
        if (application->Update())
        {
            [displayLink invalidate];
            [displayLink release];
            
            [timer invalidate];
            [timer release];
            
            delete(application);
        }
    }

    - (void) applicationWillTerminate: (UIApplication*) application
    {
        self->application->~Application();  // Some genius at Apple decided application should do exit() right afterwards, which does not call destructors, so manually calling destructor is mandatory
    }

@end

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedClassInspection"

// While this type may seem unused, it is actually referenced in the plist.in of the application, so it is up to UIKit to create an instance of it
@interface UIKitSceneDelegate : UIResponder<UISceneDelegate>

@end

@implementation UIKitSceneDelegate

    /* --- POLLING METHODS --- */

    - (void) sceneDidBecomeActive: (UIScene*) scene
    {
        [reinterpret_cast<UIKitEntryPointDelegate*>([[UIApplication sharedApplication] delegate]) sceneDidBecomeActive: scene];
    }

@end

#pragma clang diagnostic pop

int main(const int argc, char* argv[])
{
    UIApplicationMain(argc, argv, nil, NSStringFromClass([UIKitEntryPointDelegate class]));
}
