//
// Created by Nikolay Kanchevski on 26.09.23.
//

#include "UIKitWindow.h"

#include "iOSInstance.h"

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
        return self;
    }

    /* --- POLLING METHODS --- */

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

@end

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UIKitWindow::UIKitWindow(const UIKitContext &uiKitContext, const WindowCreateInfo &createInfo)
        : Window(createInfo), title(createInfo.title), uiKitContext(uiKitContext), touchManager(UIKitTouchManager({ }))
    {
        // Create window
        window = uiKitContext.CreateWindow();

        // Create view controller
        viewController = [[UIKitWindowViewController alloc] initWithWindow: this];

        // Assign window handlers and set background color
        [window setRootViewController: viewController];
        [window setBackgroundColor: [UIColor whiteColor]];

        // Observe UIKit events
        applicationDidEnterBackgroundBridge = UIKitSelectorBridge([NSNotificationCenter defaultCenter], UIApplicationDidEnterBackgroundNotification, [this] { ApplicationDidEnterBackground(); });
        applicationWillEnterForegroundBridge = UIKitSelectorBridge([NSNotificationCenter defaultCenter], UIApplicationWillEnterForegroundNotification, [this] { ApplicationWillEnterForeground(); });
        applicationWillTerminateBridge = UIKitSelectorBridge([NSNotificationCenter defaultCenter], UISceneDidDisconnectNotification, [this] { ApplicationWillTerminate(); });

        // Show and assign window
        [window makeKeyAndVisible];
    }

    /* --- POLLING METHODS --- */

    void UIKitWindow::OnUpdate()
    {
        // Not applicable
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

    void UIKitWindow::SetTitle(const String &newTitle)
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

    String UIKitWindow::GetTitle() const
    {
        return title;
    }

    Vector2Int UIKitWindow::GetPosition() const
    {
        return { 0, 0 };
    }

    Vector2UInt UIKitWindow::GetSize() const
    {
        return uiKitContext.GetPrimaryScreen().GetSize();
    }

    Vector2UInt UIKitWindow::GetFramebufferSize() const
    {
        return uiKitContext.GetPrimaryScreen().GetSize();
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

    Screen& UIKitWindow::GetScreen()
    {
        return uiKitContext.GetPrimaryScreen();
    }

    TouchManager& UIKitWindow::GetTouchManager()
    {
        return touchManager;
    }

    WindowAPI UIKitWindow::GetAPI() const
    {
        return WindowAPI::UIKit;
    }

    /* --- EVENTS --- */

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
        
        applicationDidEnterBackgroundBridge.Invalidate();
        applicationWillEnterForegroundBridge.Invalidate();
        applicationWillTerminateBridge.Invalidate();

        GetWindowCloseDispatcher().DispatchEvent();
        uiKitContext.DestroyWindow(window);

        [viewController release];
        viewController = nil;

        [window release];
        window = nil;
    }

}
