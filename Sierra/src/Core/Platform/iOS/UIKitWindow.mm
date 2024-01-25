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

    - (void) resizeDrawable: (CGFloat) scaleFactor
    {
        // Set new size
        CGSize newSize = self.bounds.size;
        newSize.width *= scaleFactor;
        newSize.height *= scaleFactor;
        if (newSize.width <= 0 || newSize.height <= 0) return;
        
        // Assign new size if it differs
        CAMetalLayer* metalLayer = reinterpret_cast<CAMetalLayer*>(self.layer);
        if (newSize.width == metalLayer.drawableSize.width && newSize.height == metalLayer.drawableSize.height) return;
        metalLayer.drawableSize = newSize;
    }

    - (void) didMoveToWindow
    {
        [super didMoveToWindow];
        [self resizeDrawable: self.window.screen.nativeScale];
    }

    - (void) setContentScaleFactor: (CGFloat) contentScaleFactor
    {
        [super setContentScaleFactor: contentScaleFactor];
        [self resizeDrawable: self.window.screen.nativeScale];
    }

    - (void) layoutSubviews
    {
        [super layoutSubviews];
        [self resizeDrawable: self.window.screen.nativeScale];
    }

    - (void) setFrame: (CGRect) frame
    {
        [super setFrame:frame];
        [self resizeDrawable: self.window.screen.nativeScale];
    }

    - (void) setBounds: (CGRect) bounds
    {
        [super setBounds:bounds];
        [self resizeDrawable: self.window.screen.nativeScale];
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
        : Window(createInfo), title(createInfo.title), uiKitContext(uiKitContext), touchManager(UIKitTouchManager({ }))
    {
        // Create window
        window = uiKitContext.CreateWindow();

        // Create view controller
        viewController = [[UIKitWindowViewController alloc] initWithWindow: this];

        // Set up view
        view = [viewController view];
        [view setContentScaleFactor: window.screen.nativeScale];

        // Assign window handlers and set background color
        [window setRootViewController: viewController];
        [window setBackgroundColor: [UIColor whiteColor]];

        // Observe UIKit events
        deviceOrientationDidChangeBridge = UIKitSelectorBridge([NSNotificationCenter defaultCenter], UIDeviceOrientationDidChangeNotification, [this] { DeviceOrientationDidChange(); });
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

    void UIKitWindow::SetTitle(const std::string &newTitle)
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

    const std::string& UIKitWindow::GetTitle() const
    {
        return title;
    }

    Vector2Int UIKitWindow::GetPosition() const
    {
        return { 0, 0 };
    }

    Vector2UInt UIKitWindow::GetSize() const
    {
        return uiKitContext.GetPrimaryScreen().GetOrientation() & ScreenOrientation::Portrait ? uiKitContext.GetPrimaryScreen().GetSize() : Vector2UInt(uiKitContext.GetPrimaryScreen().GetSize().y, uiKitContext.GetPrimaryScreen().GetSize().x);
    }

    Vector2UInt UIKitWindow::GetFramebufferSize() const
    {
        return GetSize();
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

        GetWindowResizeDispatcher().DispatchEvent(GetSize());
        lastOrientation = uiKitContext.GetPrimaryScreen().GetOrientation();
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

        [viewController release];
        viewController = nil;

        [window release];
        window = nil;
    }

}
