//
// Created by Nikolay Kanchevski on 26.09.23.
//

#include "iOSInstance.h"
#include "UIKitWindow.h"

namespace Sierra
{

    // There really is no better way to do this - we need to set the run info data before starting the application and then
    // retrieve inside the application delegate (no data can be passed to the application directly, meaning we need to globally
    // store such data and then retrieve it when needed through the UIKit delegate callbacks).
    class SIERRA_API UIKitApplicationRunInfoStorage
    {
    public:
        /* --- SETTER METHODS --- */
        static void SetCurrentRunInfo(const PlatformApplicationRunInfo &runInfo) { currentApplicationRunInfo = runInfo; }
        
        /* --- GETTER METHODS --- */
        static PlatformApplicationRunInfo MoveCurrentRunInfo() { return std::move(currentApplicationRunInfo); }
        
    private:
        static inline PlatformApplicationRunInfo currentApplicationRunInfo;
        
    };

}

@interface UIKitApplicationRunLoopConnector : NSObject

@end

@implementation UIKitApplicationRunLoopConnector

    /* --- MEMBERS ---- */
    {
        CADisplayLink* runLoopDisplayLink;
    }

    /* --- CONSTRUCTORS --- */

    - (instancetype) initWithRunInfo: (const Sierra::PlatformApplicationRunInfo&) initRunInfo
    {
        self = [super init];
        
        // Save current run info
        Sierra::UIKitApplicationRunInfoStorage::SetCurrentRunInfo(initRunInfo);
        
        // Set applicationShouldUpdate to be called every frame
        runLoopDisplayLink = [CADisplayLink displayLinkWithTarget: self selector: @selector(applicationShouldUpdate)];
        [runLoopDisplayLink addToRunLoop: [NSRunLoop mainRunLoop] forMode: NSDefaultRunLoopMode];

        return self;
    }

    /* --- POLLING METHODS --- */

    - (void) applicationShouldUpdate
    {
        const UIKitApplicationDelegate* applicationDelegate = reinterpret_cast<UIKitApplicationDelegate*>([[UIApplication sharedApplication] delegate]);
        
        // Check if application has been fully initialized already
        if ([applicationDelegate GetActiveWindowScene] != nil)
        {
            // Check if run loop exited, and if so, invalidate the link, causing it to not get run again
            if ([applicationDelegate applicationShouldUpdate])
            {
                [runLoopDisplayLink invalidate];
                runLoopDisplayLink = nil;
            }
        }
    }

    /* --- DESTRUCTOR --- */
    
    - (void) dealloc
    {
        [runLoopDisplayLink invalidate];
        runLoopDisplayLink = nil;

        [super dealloc];
    }
    

@end

@implementation UIKitApplicationDelegate

    /* --- MEMBERS --- */
    {
        UIWindowScene* activeWindowScene;
        Sierra::PlatformApplicationRunInfo runInfo;
    }

    /* --- POLLING METHODS --- */

    - (BOOL) application: (UIApplication*) application didFinishLaunchingWithOptions: (NSDictionary*) launchOptions
    {
        // Move and store application run info
        runInfo = Sierra::UIKitApplicationRunInfoStorage::MoveCurrentRunInfo();
        
        return YES;
    }

    - (void) sceneDidBecomeActive: (UIScene*) scene
    {
        // If this is the first scene to be activated
        if (activeWindowScene == nil)
        {
            // Save scene
            activeWindowScene = reinterpret_cast<UIWindowScene*>(scene);

            // This is the actual entrypoint of the engine (not the UIApplication), because we have to wait until a scene has been created automatically,
            // and we cannot wait for that, due to the single-threaded design of UIKit and its callback-based protocols
            runInfo.OnStart();
        }
    }

    - (BOOL) applicationShouldUpdate
    {
        return runInfo.OnUpdate();
    }

    - (void) applicationWillTerminate: (UIApplication*) application
    {
        // Perform any final cleanup or data saving operations here
        for (UIWindow* window in [activeWindowScene windows])
        {
            // Register all windows as closing
            [reinterpret_cast<UIKitWindowViewController*>([window rootViewController]) applicationWillTerminate];
        }
        
        // Run finalization code
        runInfo.OnEnd();
    }

    /* --- GETTER METHODS --- */

    - (UIWindowScene*) GetActiveWindowScene
    {
        return activeWindowScene;
    }

@end

@interface UIKitSceneDelegate : UIResponder<UISceneDelegate>

@end

@implementation UIKitSceneDelegate

    /* --- MEMBERS --- */
    {
        UIWindow* window;
    }

    /* --- POLLING METHODS --- */

    - (void) sceneDidBecomeActive: (UIScene*) scene
    {
        [reinterpret_cast<UIKitApplicationDelegate*>([[UIApplication sharedApplication] delegate]) sceneDidBecomeActive: scene];
    }

@end

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    iOSInstance::iOSInstance(const Sierra::PlatformInstanceCreateInfo &createInfo)
        : PlatformInstance(createInfo)
    {
        
    }

    /* --- POLLING METHODS --- */

    void iOSInstance::RunApplication(const PlatformApplicationRunInfo &runInfo)
    {
        @autoreleasepool 
        {
            // Initialize application connector
            applicationRunLoopConnector = [[UIKitApplicationRunLoopConnector alloc] initWithRunInfo: runInfo];
        }
        
        // Run application
        char** argv = nil; // Gets rid of compiler warning
        UIApplicationMain(0, argv, nil, NSStringFromClass([UIKitApplicationDelegate class]));
    }

    /* --- DESTRUCTOR --- */

    iOSInstance::~iOSInstance()
    {
        [applicationRunLoopConnector release];
        applicationRunLoopConnector = nil;
    }

}
