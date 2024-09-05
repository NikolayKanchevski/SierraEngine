//
// Created by Nikolay Kanchevski on 26.09.23.
//

#define UIKIT_WINDOW_IMPLEMENTATION
#include "UIKitWindow.h"

@interface UIKitWindowView : UIView<CALayerDelegate>

@end

@implementation UIKitWindowView

    /* --- PROPERTIES --- */

    + (Class) layerClass
    {
        return [CAMetalLayer class];
    }

@end

@interface UIKitWindowViewController : UIViewController

@end

@implementation UIKitWindowViewController

    /* --- MEMBERS --- */
    {
        Sierra::UIKitWindow* window;
    }

    /* --- CONSTRUCTORS --- */

    - (instancetype) initWithWindow: (Sierra::UIKitWindow*) initWindow
    {
        self = [super init];
        window = initWindow;
        self.view = [[UIKitWindowView alloc] initWithFrame: self.view.bounds];
        return self;
    }

    /* --- PROPERTIES --- */

    - (UIInterfaceOrientationMask) supportedInterfaceOrientations
    {
        return Sierra::UIKitScreen::ScreenOrientationToUIInterfaceOrientationMask(window->GetAllowedOrientations());
    }

    /* --- EVENTS --- */

    - (void) touchesBegan: (NSSet<UITouch*>*) touches withEvent: (UIEvent*) event
    {
        [super touchesBegan: touches withEvent: event];

        for (const UITouch* rawTouch in touches)
        {
            // Get position within the screen and flip Y coordinate
            const Vector2 position = { [rawTouch locationInView: rawTouch.window.rootViewController.view].x, [[UIScreen mainScreen] bounds].size.height - [rawTouch locationInView: rawTouch.window.rootViewController.view].y };

            // Create touch
            const Sierra::Touch touch = Sierra::Touch({
                .ID = std::bit_cast<uint64>([rawTouch timestamp]),
                .type = Sierra::TouchType::Press,
                .tapTime = Sierra::TimePoint::Now(),
                .force = static_cast<float32>([rawTouch force] / [rawTouch maximumPossibleForce]),
                .position = position,
                .lastPosition = position
            });

            static_cast<Sierra::UIKitTouchManager&>(*window->GetTouchManager()).RegisterTouchPress(touch);
        }
    }

    - (void) touchesMoved: (NSSet<UITouch*>*) touches withEvent: (UIEvent*) event
    {
        [super touchesMoved: touches withEvent: event];

        for (const UITouch* rawTouch in touches)
        {
            // Get position within the screen and flip Y coordinate
            const Vector2 position = { [rawTouch locationInView: rawTouch.window.rootViewController.view].x, [[UIScreen mainScreen] bounds].size.height - [rawTouch locationInView: rawTouch.window.rootViewController.view].y };
            static_cast<Sierra::UIKitTouchManager&>(*window->GetTouchManager()).RegisterTouchMove(std::bit_cast<uint64>([rawTouch timestamp]), position);
        }
    }

    - (void) touchesEnded: (NSSet<UITouch*>*) touches withEvent: (UIEvent*) event
    {
        [super touchesEnded: touches withEvent: event];

        for (const UITouch* rawTouch in touches)
        {
            static_cast<Sierra::UIKitTouchManager&>(*window->GetTouchManager()).RegisterTouchRelease(std::bit_cast<uint64>([rawTouch timestamp]));
        }
    }

    - (void) touchesCancelled: (NSSet<UITouch*>*) touches withEvent: (UIEvent*) event
    {
        [super touchesCancelled: touches withEvent: event];

        for (const UITouch* rawTouch in touches)
        {
            static_cast<Sierra::UIKitTouchManager&>(*window->GetTouchManager()).RegisterTouchRelease(std::bit_cast<uint64>([rawTouch timestamp]));
        }
    }

    - (void) applicationDidEnterBackground
    {
        window->ApplicationDidEnterBackground();
    }

    - (void) applicationWillEnterForeground
    {
        window->ApplicationWillEnterForeground();
    }

    - (void) sceneDidDisconnect
    {
        window->SceneDidDisconnect();
    }

    /* --- DESTRUCTOR --- */

    - (void) dealloc
    {
        [self.view release];
        [super dealloc];
    }

@end

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UIKitWindow::UIKitWindow(UIKitContext& uiKitContext, const WindowCreateInfo& createInfo)
        : Window(createInfo), uiKitContext(uiKitContext),
            title(createInfo.title),
            allowedOrientations(createInfo.allowedOrientations)
    {
        // Create window
        window = uiKitContext.CreateWindow();
        viewController = [[UIKitWindowViewController alloc] initWithWindow: this];
        view = viewController.view;

        // Configure window
        [view.layer setFrame: view.bounds];
        [view.layer setContentsScale: window.contentScaleFactor];
        [window setRootViewController: viewController];

        // Observe UIKit events
        [[NSNotificationCenter defaultCenter] addObserver: viewController selector: @selector(applicationDidEnterBackground) name: UIApplicationDidEnterBackgroundNotification object: nil];
        [[NSNotificationCenter defaultCenter] addObserver: viewController selector: @selector(applicationWillEnterForeground) name: UIApplicationWillEnterForegroundNotification object: nil];
        [[NSNotificationCenter defaultCenter] addObserver: viewController selector: @selector(sceneDidDisconnect) name: UISceneDidDisconnectNotification object: nil];

        // Show and assign window
        [window makeKeyAndVisible];
    }

    /* --- POLLING METHODS --- */

    void UIKitWindow::Update()
    {
        touchManager.Update();
    }

    void UIKitWindow::Minimize()
    {
        // Though possible, method is unimplemented, because it needs to perform private API calls, which would not let an app submission to the App Store pass
    }

    void UIKitWindow::Maximize()
    {
        // Not applicable
    }

    void UIKitWindow::Show()
    {
        // Not applicable
    }

    void UIKitWindow::Hide()
    {
        // Though possible, method is unimplemented, because it needs to perform private API calls, which would not let an app submission to the App Store pass
    }

    void UIKitWindow::Focus()
    {
        // Not applicable
    }

    void UIKitWindow::Close()
    {
        if (closed) return;
        closed = true;

        GetWindowCloseDispatcher().DispatchEvent();
        uiKitContext.DestroyWindow(window);

        [viewController release];
        viewController = nil;

        [window release];
        window = nil;
    }

    /* --- SETTER METHODS --- */

    void UIKitWindow::SetTitle(const std::string_view newTitle)
    {
        title = newTitle;
    }

    void UIKitWindow::SetPosition(const Vector2Int& position)
    {
        // Not applicable
    }

    void UIKitWindow::SetSize(const Vector2UInt& size)
    {
        // Not applicable
    }

    void UIKitWindow::SetOpacity(float32 opacity)
    {
        // Not applicable
    }

    /* --- GETTER METHODS --- */

    std::string_view UIKitWindow::GetTitle() const
    {
        return title;
    }

    Vector2Int UIKitWindow::GetPosition() const
    {
        return { 0, 0 };
    }

    uint32 UIKitWindow::GetWidth() const
    {
        return uiKitContext.GetScreen().GetWidth();
    }

    uint32 UIKitWindow::GetHeight() const
    {
        return uiKitContext.GetScreen().GetHeight();
    }

    uint32 UIKitWindow::GetFramebufferWidth() const
    {
        return GetWidth() * static_cast<uint32>([window contentScaleFactor]);
    }

    uint32 UIKitWindow::GetFramebufferHeight() const
    {
        return GetHeight() * static_cast<uint32>([window contentScaleFactor]);
    }

    float32 UIKitWindow::GetOpacity() const
    {
        return 1.0f;
    }

    bool UIKitWindow::IsClosed() const
    {
        return closed;
    }

    bool UIKitWindow::IsMinimized() const
    {
        return minimized;
    }

    bool UIKitWindow::IsMaximized() const
    {
        return !minimized;
    }

    bool UIKitWindow::IsFocused() const
    {
        return !minimized;
    }

    bool UIKitWindow::IsHidden() const
    {
        return minimized;
    }

    Screen& UIKitWindow::GetScreen() const
    {
        return uiKitContext.GetScreen();
    }

    InputManager* UIKitWindow::GetInputManager()
    {
        return nullptr;
    }

    CursorManager* UIKitWindow::GetCursorManager()
    {
        return nullptr;
    }

    TouchManager* UIKitWindow::GetTouchManager()
    {
        return &touchManager;
    }

    PlatformAPI UIKitWindow::GetAPI() const
    {
        return PlatformAPI::UIKit;
    }

    /* --- EVENTS --- */

    #if defined(__OBJC__) && defined(UIKIT_WINDOW_IMPLEMENTATION)
        void UIKitWindow::ApplicationDidEnterBackground()
        {
            minimized = true;
            GetWindowFocusDispatcher().DispatchEvent(false);
            GetWindowMinimizeDispatcher().DispatchEvent();
        }

        void UIKitWindow::ApplicationWillEnterForeground()
        {
            minimized = false;
            GetWindowFocusDispatcher().DispatchEvent(true);
        }

        void UIKitWindow::SceneDidDisconnect()
        {
            Close();
        }
    #endif

    /* --- DESTRUCTOR --- */

    UIKitWindow::~UIKitWindow()
    {
        if (closed) return;
        closed = true;

        [[NSNotificationCenter defaultCenter] removeObserver: viewController];
        
        GetWindowCloseDispatcher().DispatchEvent();
        uiKitContext.DestroyWindow(window);

        [viewController release];
        [window release];
    }

}
