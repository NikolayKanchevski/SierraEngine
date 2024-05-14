//
// Created by Nikolay Kanchevski on 26.09.23.
//

#define UIKIT_WINDOW_IMPLEMENTATION
#include "UIKitWindow.h"

#include "iOSContext.h"

@interface UIKitWindowView : UIView<CALayerDelegate>

@end

@implementation UIKitWindowView

    /* --- POLLING METHODS --- */

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

    /* --- POLLING METHODS --- */

    - (UIInterfaceOrientationMask) supportedInterfaceOrientations
    {
        return window->AllowsOrientationChange() ? UIInterfaceOrientationMaskAll : UIInterfaceOrientationMaskPortrait;
    }

    - (void) touchesBegan: (NSSet<UITouch*>*) touches withEvent: (UIEvent*) event
    {
        [super touchesBegan: touches withEvent: event];

        // No casting error checks are done, since the touch manager type within a UIKitWindow is guaranteed to be UIKitTouchManager
        static_cast<Sierra::UIKitTouchManager*>(&window->GetTouchManager())->TouchesBegan(touches, event);
    }

    - (void) touchesMoved: (NSSet<UITouch*>*) touches withEvent: (UIEvent*) event
    {
        [super touchesMoved: touches withEvent: event];

        // No casting error checks are done, since the touch manager type within a UIKitWindow is guaranteed to be UIKitTouchManager
        static_cast<Sierra::UIKitTouchManager*>(&window->GetTouchManager())->TouchesMoved(touches, event);
    }

    - (void) touchesEnded: (NSSet<UITouch*>*) touches withEvent: (UIEvent*) event
    {
        [super touchesEnded: touches withEvent: event];

        // No casting error checks are done, since the touch manager type within a UIKitWindow is guaranteed to be UIKitTouchManager
        static_cast<Sierra::UIKitTouchManager*>(&window->GetTouchManager())->TouchesEnded(touches, event);
    }

    - (void) touchesCancelled: (NSSet<UITouch*>*) touches withEvent: (UIEvent*) event
    {
        [super touchesCancelled: touches withEvent: event];

        // No casting error checks are done, since the touch manager type within a UIKitWindow is guaranteed to be UIKitTouchManager
        static_cast<Sierra::UIKitTouchManager*>(&window->GetTouchManager())->TouchesCancelled(touches, event);
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

    UIKitWindow::UIKitWindow(const UIKitContext &uiKitContext, const WindowCreateInfo &createInfo)
        : Window(createInfo),
            title(createInfo.title),
            allowsOrientationChange(createInfo.resizable),
            uiKitContext(uiKitContext),
            touchManager(UIKitTouchManager({ }))
    {
        // Create window
        window = uiKitContext.CreateWindow();

        // Create view controller
        viewController = [[UIKitWindowViewController alloc] initWithWindow: this];

        // Set up view
        view = viewController.view;

        // Configure layer
        [view.layer setFrame: view.bounds];
        [view.layer setContentsScale: window.contentScaleFactor];

        // Assign window handlers
        [window setRootViewController: viewController];

        // Observe UIKit events
        deviceOrientationDidChangeBridge = UIKitSelectorBridge([NSNotificationCenter defaultCenter], UIDeviceOrientationDidChangeNotification, [this] { DeviceOrientationDidChange(); });
        applicationDidEnterBackgroundBridge = UIKitSelectorBridge([NSNotificationCenter defaultCenter], UIApplicationDidEnterBackgroundNotification, [this] { ApplicationDidEnterBackground(); });
        applicationWillEnterForegroundBridge = UIKitSelectorBridge([NSNotificationCenter defaultCenter], UIApplicationWillEnterForegroundNotification, [this] { ApplicationWillEnterForeground(); });
        applicationWillTerminateBridge = UIKitSelectorBridge([NSNotificationCenter defaultCenter], UISceneDidDisconnectNotification, [this] { ApplicationWillTerminate(); });

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

        applicationDidEnterBackgroundBridge.Invalidate();
        applicationWillEnterForegroundBridge.Invalidate();
        applicationWillTerminateBridge.Invalidate();

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

    void UIKitWindow::SetPosition(const Vector2Int &position)
    {
        // Not applicable
    }

    void UIKitWindow::SetSize(const Vector2UInt &size)
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
        return uiKitContext.GetPrimaryScreen().GetWidth();
    }

    uint32 UIKitWindow::GetHeight() const
    {
        return uiKitContext.GetPrimaryScreen().GetHeight();
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

    const Screen& UIKitWindow::GetScreen() const
    {
        return uiKitContext.GetPrimaryScreen();
    }

    TouchManager& UIKitWindow::GetTouchManager()
    {
        return touchManager;
    }

    PlatformAPI UIKitWindow::GetAPI() const
    {
        return PlatformAPI::UIKit;
    }

    /* --- EVENTS --- */

    void UIKitWindow::DeviceOrientationDidChange()
    {
        // Check if orientation has actually changed
        if ((lastOrientation & ScreenOrientation::Landscape && uiKitContext.GetPrimaryScreen().GetOrientation() & ScreenOrientation::Landscape) || (lastOrientation & ScreenOrientation::Portrait && uiKitContext.GetPrimaryScreen().GetOrientation() & ScreenOrientation::Portrait)) return;

        lastOrientation = uiKitContext.GetPrimaryScreen().GetOrientation();
        GetWindowResizeDispatcher().DispatchEvent(GetWidth(), GetHeight());
    }

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

    void UIKitWindow::ApplicationWillTerminate()
    {
        Close();
    }

    /* --- DESTRUCTOR --- */

    UIKitWindow::~UIKitWindow()
    {
        if (closed) return;
        closed = true;

        deviceOrientationDidChangeBridge.Invalidate();
        applicationDidEnterBackgroundBridge.Invalidate();
        applicationWillEnterForegroundBridge.Invalidate();
        applicationWillTerminateBridge.Invalidate();

        GetWindowCloseDispatcher().DispatchEvent();
        uiKitContext.DestroyWindow(window);

        [window release];
        [viewController release];
        // NOTE: We do not have a custom view implementation, so it is managed by view controller
    }

}
