//
// Created by Nikolay Kanchevski on 26.09.23.
//

#include "UIKitWindow.h"
#include "iOSInstance.h"

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

    - (void) viewDidLoad
    {
        [super viewDidLoad];

        // Observe events
        [[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(applicationDidEnterBackground) name: UIApplicationDidEnterBackgroundNotification object: nil];
        [[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(applicationWillEnterForeground) name: UIApplicationWillEnterForegroundNotification object: nil];
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

    - (void) applicationDidEnterBackground
    {
        window->ApplicationDidEnterBackground();
    }

    - (void) applicationWillEnterForeground
    {
        window->ApplicationWillEnterForeground();
    }

    - (void) applicationWillTerminate
    {
        window->ApplicationWillTerminate();
    }

    - (void) dealloc
    {
        [[NSNotificationCenter defaultCenter] removeObserver: self];
        [super dealloc];
    }

@end

@interface UIKitWindowImplementation : UIWindow

@end

@implementation UIKitWindowImplementation

@end

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UIKitWindow::UIKitWindow(const Sierra::WindowCreateInfo &createInfo)
        : Window(createInfo), title(createInfo.title), iOSInstance(*static_cast<class iOSInstance*>(createInfo.platformInstance.get())), touchManager(UIKitTouchManager({ }))
    {
        SR_ERROR_IF(createInfo.platformInstance->GetType() !=+ PlatformType::iOS, "Cannot create UIKit window using a platform instance of type [{0}]!", createInfo.platformInstance->GetType()._to_string());
        
        @autoreleasepool
        {
            // Create window
            window = [[UIKitWindowImplementation alloc] initWithWindowScene: [reinterpret_cast<UIKitApplicationDelegate*>([[UIApplication sharedApplication] delegate]) GetActiveWindowScene]];
            SR_ERROR_IF(window == nil, "Could not create UIKit window!");

            // Create view controller
            viewController = [[UIKitWindowViewController alloc] initWithWindow: this];
            SR_ERROR_IF(viewController == nil, "Could not create Cocoa UIKit view controller!");

        }

        // Assign window handlers and set background color
        [window setRootViewController: viewController];
        [window setBackgroundColor: [UIColor whiteColor]];

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
        closed = true;
        GetWindowCloseDispatcher().DispatchEvent();
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
        CGRect bounds = [[UIScreen mainScreen] nativeBounds];
        return { CGRectGetWidth(bounds), CGRectGetHeight(bounds) };
    }

    Vector2UInt UIKitWindow::GetFramebufferSize() const
    {
        CGRect bounds = [[UIScreen mainScreen] nativeBounds];
        return { CGRectGetWidth(bounds), CGRectGetHeight(bounds) };
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

    TouchManager& UIKitWindow::GetTouchManager()
    {
        return touchManager;
    }

    WindowAPI UIKitWindow::GetAPI() const
    {
        return WindowAPI::UIKit;
    }

    /* --- EVENTS --- */

    #if defined(__OBJC__)
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
    #endif

    /* --- DESTRUCTOR --- */

    UIKitWindow::~UIKitWindow()
    {
        @autoreleasepool
        {
            [window setRootViewController: nil];
            [viewController release];
            viewController = nil;

            [window release];
            window = nil;
        }
    }

}
