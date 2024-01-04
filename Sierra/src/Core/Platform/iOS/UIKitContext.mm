//
// Created by Nikolay Kanchevski on 9.11.23.
//

#include "UIKitContext.h"

#include "UIKitWindow.h"
#include "UIKitTemporaryCreateInfoStorage.h"

@interface UIKitApplicationDelegate : UIResponder<UIApplicationDelegate>

@end

@implementation UIKitApplicationDelegate

    /* --- MEMBERS --- */
    {
        CADisplayLink* runLoopDisplayLink;

        Sierra::PlatformApplicationRunInfo runInfo;
        UIScene* scene;
    }

    /* --- POLLING METHODS --- */

    - (BOOL) application: (UIApplication*) application didFinishLaunchingWithOptions: (NSDictionary*) launchOptions
    {
        // Retrieve application create info
        auto createInfo = Sierra::UIKitTemporaryCreateInfoStorage::MoveFront();
        runInfo = std::move(createInfo.runInfo);

        return YES;
    }

    - (void) sceneDidBecomeActive: (UIScene*) activeScene
    {
        // If this is the first scene to be activated
        if (scene == nil)
        {
            // This is the actual entrypoint of the engine (not the UIApplication), as we have to wait until a scene has been created automatically,
            // and we cannot wait for that, due to the single-threaded design of UIKit and its callback-based protocols
            runInfo.OnStart();
            
            // Connect custom run loop to application
            runLoopDisplayLink = [CADisplayLink displayLinkWithTarget: self selector: @selector(applicationShouldUpdate)];
            [runLoopDisplayLink addToRunLoop: [NSRunLoop mainRunLoop] forMode: NSDefaultRunLoopMode];
        }
        
        // Update scene
        scene = activeScene;
    }

    - (void) applicationShouldUpdate
    {
        if (runInfo.OnUpdate())
        {
            [runLoopDisplayLink invalidate];
            runLoopDisplayLink = nil;
        }
    }

    - (void) applicationWillTerminate: (UIApplication*) application
    {
        // Run finalization code
        runInfo.OnEnd();
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
        applicationDidFinishLaunchingBridge = UIKitSelectorBridge([NSNotificationCenter defaultCenter], UIApplicationDidFinishLaunchingNotification, [this]{ ApplicationDidFinishLaunching(); });
        applicationWillTerminateBridge = UIKitSelectorBridge([NSNotificationCenter defaultCenter], UIApplicationWillTerminateNotification, [this]{ ApplicationWillTerminate(); });
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
        application = reinterpret_cast<UIKitApplication*>([UIApplication sharedApplication]);
    }

    void UIKitContext::ApplicationWillTerminate()
    {

    }

    /* --- DESTRUCTOR --- */

    UIKitContext::~UIKitContext()
    {
        applicationDidFinishLaunchingBridge.Invalidate();
        applicationWillTerminateBridge.Invalidate();
    }

}
