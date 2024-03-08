//
// Created by Nikolay Kanchevski on 9.11.23.
//

#define UIKIT_CONTEXT_IMPLEMENTATION
#include "UIKitContext.h"

#include "../../PlatformContext.h"

namespace Sierra
{
    extern PlatformApplicationRunInfo iOSApplicationRunInfo;
    extern UIKitContext* iOSUIKitContext;
}

@interface UIKitApplicationDelegate : UIResponder<UIApplicationDelegate>

@end

@implementation UIKitApplicationDelegate

    /* --- MEMBERS --- */
    {
        Sierra::UIKitContext* uiKitContext;
        CADisplayLink* displayLink;
    }

    /* --- POLLING METHODS --- */

    - (BOOL) application: (UIApplication*) application didFinishLaunchingWithOptions: (NSDictionary*) launchOptions
    {
        uiKitContext = Sierra::iOSUIKitContext;
        uiKitContext->ApplicationDidFinishLaunching();
        return YES;
    }

    - (UIInterfaceOrientationMask) application: (UIApplication*) application supportedInterfaceOrientationsForWindow: (UIWindow*) window
    {
        return window.rootViewController.supportedInterfaceOrientations;
    }

    - (void) sceneDidBecomeActive: (UIScene*) activeScene
    {
        if (displayLink != nil) return;

        // Call Start() of application
        Sierra::iOSApplicationRunInfo.OnStart();

        // Connect run loop
        displayLink = [CADisplayLink displayLinkWithTarget: self selector: @selector(applicationShouldUpdate)];
        [displayLink addToRunLoop: [NSRunLoop mainRunLoop] forMode: NSDefaultRunLoopMode];
    }

    - (void) applicationShouldUpdate
    {
        if (Sierra::iOSApplicationRunInfo.OnUpdate())
        {
            [displayLink invalidate];
        }
    }

    - (void) applicationWillTerminate: (UIApplication*) application
    {
        Sierra::iOSApplicationRunInfo.OnEnd();
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
        [reinterpret_cast<UIKitApplicationDelegate*>([[UIApplication sharedApplication] delegate]) sceneDidBecomeActive: scene];
    }

@end

#pragma clang diagnostic pop

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UIKitContext::UIKitContext(const Sierra::UIKitContextCreateInfo &createInfo)
        : primaryScreen({ .uiScreen = [UIScreen mainScreen] })
    {

    }

    /* --- POLLING METHODS --- */

    UIWindow* UIKitContext::CreateWindow() const
    {
        return [[UIWindow alloc] initWithWindowScene: reinterpret_cast<UIWindowScene*>([[UIApplication sharedApplication] connectedScenes].allObjects.firstObject)];
    }

    void UIKitContext::DestroyWindow(UIWindow* window) const
    {
        [window setRootViewController: nil];
        [window removeFromSuperview];
    }

    /* --- EVENTS --- */

    void UIKitContext::ApplicationDidFinishLaunching()
    {
        application = [UIApplication sharedApplication];
    }

}
