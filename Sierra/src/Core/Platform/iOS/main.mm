//
// Created by Nikolay Kanchevski on 3.06.24.
//

#include <UIKit/UIKit.h>

namespace
{
    int _argc;
    char** _argv;
}

@interface UIKitEntryPointDelegate : UIResponder<UIApplicationDelegate>

@end

@implementation UIKitEntryPointDelegate

    /* --- MEMBERS --- */
    {
        NSTimer* timer;
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
        if (timer != nil) return;

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

    /* --- EVENTS --- */

    - (void) sceneDidBecomeActive: (UIScene*) scene
    {
        [reinterpret_cast<UIKitEntryPointDelegate*>([[UIApplication sharedApplication] delegate]) sceneDidBecomeActive: scene];
    }

@end

#pragma clang diagnostic pop

int main(const int argc, char* argv[])
{
   _argc = argc;
   _argv = argv;
    UIApplicationMain(argc, argv, nil, NSStringFromClass([UIKitEntryPointDelegate class]));
}
