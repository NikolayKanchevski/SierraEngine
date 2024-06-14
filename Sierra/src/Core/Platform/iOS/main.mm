//
// Created by Nikolay Kanchevski on 3.06.24.
//

#include <UIKit/UIKit.h>

#include "../../Application.h"

namespace
{
    int __argc;
    char** __argv;
}

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
        timer = [NSTimer scheduledTimerWithTimeInterval: 0 target: self selector: @selector(applicationShouldUpdate) userInfo: nil repeats: true];
        [[NSRunLoop mainRunLoop] addTimer: timer forMode: NSDefaultRunLoopMode];

        // Create application
        application = Sierra::CreateApplication(__argc, __argv);
        if (application == nullptr)
        {
            APP_ERROR("Created application returned from Sierra::CreateApplication() must not be a null pointer!");
            return FALSE;
        }
    }

    - (void) applicationShouldUpdate
    {
        // Update application
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
   __argc = argc;
   __argv = argv;
    UIApplicationMain(argc, argv, nil, NSStringFromClass([UIKitEntryPointDelegate class]));
}
