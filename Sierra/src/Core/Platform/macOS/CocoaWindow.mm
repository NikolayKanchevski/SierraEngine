//
// Created by Nikolay Kanchevski on 17.08.23.
//

#define COCOA_WINDOW_IMPLEMENTATION
#include "CocoaWindow.h"

#include <QuartzCore/QuartzCore.h>

@interface CocoaWindowDelegate : NSObject<NSWindowDelegate>

@end

@implementation CocoaWindowDelegate

    /* --- MEMBERS --- */
    {
        Sierra::CocoaWindow* window;
    }

    /* --- CONSTRUCTORS --- */

    - (instancetype) initWithWindow: (Sierra::CocoaWindow*) initWindow;
    {
        self = [super init];
        window = initWindow;
        return self;
    }

    /* --- EVENTS --- */

    - (BOOL) windowShouldClose: (id) sender
    {
        window->WindowShouldClose();
        return YES;
    }

    - (void) windowDidResize: (NSNotification*) notification
    {
        window->WindowDidResize(notification);
    }

    - (void) windowDidMove: (NSNotification*) notification
    {
        window->WindowDidMove(notification);
    }

    - (void) windowDidMiniaturize: (NSNotification*) notification
    {
        window->WindowDidMiniaturize(notification);
    }

    - (void) windowDidBecomeKey: (NSNotification*) notification
    {
        window->WindowDidBecomeKey(notification);
    }

    - (void) windowDidResignKey: (NSNotification*) notification
    {
        window->WindowDidResignKey(notification);
    }

    /* --- DESTRUCTOR --- */

    - (void) dealloc
    {
        [super dealloc];
    }

@end

@interface CocoaWindowView : NSView<NSTextInputClient>

@end

@implementation CocoaWindowView

    /* --- MEMBERS --- */
    {
        Sierra::CocoaWindow* window;
    }

    /* --- CONSTRUCTORS --- */

    - (instancetype) initWithWindow: (Sierra::CocoaWindow*) initWindow;
    {
        self = [super init];
        window = initWindow;

        // Modifiers prevent keyUp from being called, so we intercept such events and handle them manually
        [NSEvent addLocalMonitorForEventsMatchingMask: (NSEventMaskKeyUp) handler: ^(NSEvent* event)
        {
            [self keyUp: event];
            event = nil;

            return event;
        }];

        return self;
    }

    /* --- EVENTS --- */

    - (void) keyDown: (NSEvent*) event
    {
        const Sierra::Key key = Sierra::CocoaInputManager::KeyCodeToKey([event keyCode]);
        static_cast<Sierra::CocoaInputManager&>(*window->GetInputManager()).RegisterKeyPress(key);

        [self interpretKeyEvents: @[event]];
    }

    - (void) flagsChanged: (NSEvent*) event
    {
        const Sierra::Key key = Sierra::CocoaInputManager::KeyCodeToKey([event keyCode]);

        // Modifier keys need to be registered as pressed manually, so we get modifier flags of the modifier key... fun stuff!
        NSEventModifierFlags keyModifierFlag = 0;
        switch (key)
        {
            case Sierra::Key::LeftShift:
            case Sierra::Key::RightShift:
            {
                keyModifierFlag = NSEventModifierFlagShift;
                break;
            }
            case Sierra::Key::LeftControl:
            case Sierra::Key::RightControl:
            {
                keyModifierFlag = NSEventModifierFlagControl;
                break;
            }
            case Sierra::Key::LeftOption:
            case Sierra::Key::RightOption:
            {
                keyModifierFlag = NSEventModifierFlagOption;
                break;
            }
            case Sierra::Key::LeftCommand:
            case Sierra::Key::RightCommand:
            {
                keyModifierFlag = NSEventModifierFlagCommand;
                break;
            }
            case Sierra::Key::CapsLock:
            {
                keyModifierFlag = NSEventModifierFlagCapsLock;
                break;
            }
            default:
            {
                break;
            }
        }

        Sierra::CocoaInputManager& inputManager = static_cast<Sierra::CocoaInputManager&>(*window->GetInputManager());
        if(!inputManager.IsKeyPressed(key) && keyModifierFlag & [event modifierFlags] & NSEventModifierFlagDeviceIndependentFlagsMask)
        {
            inputManager.RegisterKeyPress(key);
        }
        else
        {
            inputManager.RegisterKeyRelease(key);
        }
    }

    - (void) keyUp: (NSEvent*) event
    {
        const Sierra::Key key = Sierra::CocoaInputManager::KeyCodeToKey([event keyCode]);
        static_cast<Sierra::CocoaInputManager&>(*window->GetInputManager()).RegisterKeyRelease(key);
    }

    - (void) mouseDown: (NSEvent*) event
    {
        static_cast<Sierra::CocoaInputManager&>(*window->GetInputManager()).RegisterMouseButtonPress(Sierra::MouseButton::Left);
    }

    - (void) rightMouseDown: (NSEvent*) event
    {
        static_cast<Sierra::CocoaInputManager&>(*window->GetInputManager()).RegisterMouseButtonPress(Sierra::MouseButton::Right);
    }

    - (void) otherMouseDown: (NSEvent*) event
    {
        const Sierra::MouseButton mouseButton = Sierra::CocoaInputManager::ButtonNumberToMouseButton([event buttonNumber]);
        static_cast<Sierra::CocoaInputManager&>(*window->GetInputManager()).RegisterMouseButtonPress(mouseButton);
    }

    - (void) mouseUp: (NSEvent*) event
    {
        static_cast<Sierra::CocoaInputManager&>(*window->GetInputManager()).RegisterMouseButtonRelease(Sierra::MouseButton::Left);
    }

    - (void) rightMouseUp: (NSEvent*) event
    {
        static_cast<Sierra::CocoaInputManager&>(*window->GetInputManager()).RegisterMouseButtonRelease(Sierra::MouseButton::Right);
    }

    - (void) otherMouseUp: (NSEvent*) event
    {
        const Sierra::MouseButton mouseButton = Sierra::CocoaInputManager::ButtonNumberToMouseButton([event buttonNumber]);
        static_cast<Sierra::CocoaInputManager&>(*window->GetInputManager()).RegisterMouseButtonRelease(mouseButton);
    }

    - (void) scrollWheel: (NSEvent*) event
    {
        static_cast<Sierra::CocoaInputManager&>(*window->GetInputManager()).RegisterMouseScroll({ [event deltaX], [event scrollingDeltaY] });
    }

    - (void) mouseDragged: (NSEvent*) event
    {
        [self mouseMoved: event];
    }

    - (void) mouseMoved: (NSEvent*) event
    {
        const NSPoint point = [event locationInWindow];
        static_cast<Sierra::CocoaCursorManager&>(*window->GetCursorManager()).RegisterCursorMove({ point.x, point.y });
    }

    - (BOOL) canBecomeKeyView
    {
        return YES;
    }

    - (BOOL) acceptsFirstResponder
    {
        return YES;
    }

    - (BOOL) wantsUpdateLayer
    {
        return YES;
    }

    - (BOOL) acceptsFirstMouse: (NSEvent*) event
    {
        return YES;
    }

    - (BOOL) hasMarkedText
    {
        return NO;
    }

    - (NSArray*) validAttributesForMarkedText
    {
        return [NSArray array];
    }
    - (NSRange) markedRange
    {
        return NSRange { NSNotFound, 0 };
    }

    - (NSRange) selectedRange
    {
        return { NSNotFound, 0 };
    }

    - (NSUInteger) characterIndexForPoint: (NSPoint) point
    {
        return 0;
    }

    - (NSRect) firstRectForCharacterRange: (NSRange) range actualRange: (NSRangePointer) actualRange
    {
        const NSRect frame = [self frame];
        return NSMakeRect(frame.origin.x, frame.origin.y, 0.0, 0.0);
    }

    - (NSAttributedString*) attributedSubstringForProposedRange: (NSRange) range actualRange: (NSRangePointer) actualRange
    {
        return nil;
    }

    /* --- DESTRUCTOR --- */

    - (void) dealloc
    {
        [self setLayer: nil];
        [self setWantsLayer: NO];
        [super dealloc];
    }

    - (void) insertText: (nonnull id) string replacementRange: (NSRange) replacementRange
    {

    }

    - (void) unmarkText
    {

    }

    - (void) setMarkedText:(nonnull id) string selectedRange: (NSRange) selectedRange replacementRange: (NSRange) replacementRange
    {

    }

@end

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaWindow::CocoaWindow(CocoaContext& cocoaContext, const WindowCreateInfo& createInfo)
        : Window(createInfo), cocoaContext(cocoaContext),
            view([[CocoaWindowView alloc] initWithWindow: this]),
            delegate([[CocoaWindowDelegate alloc] initWithWindow: this]),
            window(cocoaContext.CreateWindow(createInfo.title, createInfo.width, createInfo.height)),
            inputManager(), cursorManager(window),
            title(createInfo.title)
    {
        // Assign Metal layer
        [view setLayer: [CAMetalLayer layer]];
        [view setWantsLayer: YES];

        // Configure layer
        [view.layer setFrame: view.bounds];
        [view.layer setContentsScale: window.backingScaleFactor];

        // Maximize window manually, or through Cocoa if resizable
        if (createInfo.maximize)
        {
            if (createInfo.resizable)
            {
                [window performZoom: nil];
            }
            else
            {
                const CocoaScreen& screen = cocoaContext.GetWindowScreen(window);
                const NSRect newFrame = NSMakeRect(screen.GetOrigin().x, screen.GetOrigin().y, screen.GetWorkAreaWidth(), screen.GetWorkAreaHeight());
                [window setFrame: newFrame display: YES animate: YES];
            }
        }

        // Disable resizing
        if (!createInfo.resizable)
        {
            [window setCollectionBehavior: NSWindowCollectionBehaviorFullScreenNone];
            [window setStyleMask: [window styleMask] & ~NSWindowStyleMaskResizable];
        }

        // Assign window handlers
        [window setDelegate: reinterpret_cast<CocoaWindowDelegate*>(delegate)];
        [window setContentView: view];
        [window makeFirstResponder: view];

        if (!createInfo.hide)
        {
            // Show and focus window
            [cocoaContext.GetNSApplication() activateIgnoringOtherApps: YES];
            [window orderFrontRegardless];
            [window makeKeyWindow];
        }

        if (!createInfo.maximize)
        {
            // Adjust window size to account for title bar, because the rect passed in init is the size of the framebuffer
            NSRect contentRect = [window contentRectForFrameRect: [window frame]];
            contentRect.size = NSMakeSize([view frame].size.width, [view frame].size.height - GetTitleBarHeight());
            [window setFrame: [window frameRectForContentRect: contentRect] display: YES];
        }
    }

    /* --- POLLING METHODS --- */

    void CocoaWindow::Update()
    {
        inputManager.Update();
        cursorManager.Update();
        cocoaContext.PollNextEvent();

        cursorManager.PostUpdate();
    }

    void CocoaWindow::Minimize()
    {
        if (![window isMiniaturized])
        {
            [window performMiniaturize: nil];
        }
    }

    void CocoaWindow::Maximize()
    {
        if (![window isZoomed])
        {
            if ([window isMiniaturized]) [window deminiaturize: nil];
            [window performZoom: nil];
        }
    }

    void CocoaWindow::Show()
    {
        [cocoaContext.GetNSApplication() activateIgnoringOtherApps: YES];
        [window orderFrontRegardless];
        [window makeKeyWindow];
    }

    void CocoaWindow::Hide()
    {
        [window orderOut: nil];
    }

    void CocoaWindow::Focus()
    {
        [cocoaContext.GetNSApplication() activateIgnoringOtherApps: YES];
        [window orderFrontRegardless];
        [window makeKeyWindow];
    }

    void CocoaWindow::Close()
    {
        if (closed) return;
        closed = true;

        GetWindowCloseDispatcher().DispatchEvent();

        [view release];
        [reinterpret_cast<CocoaWindowDelegate*>(delegate) release];

        cocoaContext.DestroyWindow(window);
        window = nil;
    }

    /* --- SETTER METHODS --- */

    void CocoaWindow::SetTitle(const std::string_view newTitle)
    {
        title = newTitle;
        [window setTitle: @(newTitle.data())];
    }

    void CocoaWindow::SetPosition(const Vector2Int& position)
    {
        [window setFrameOrigin: [window frameRectForContentRect: NSMakeRect(position.x, position.y - GetHeight(), 0, 0)].origin];
    }

    void CocoaWindow::SetSize(const Vector2UInt& size)
    {
        // Set new size and move window up, so that it retains its top Y position
        NSRect contentRect = [window contentRectForFrameRect: [window frame]];
        contentRect.size = NSMakeSize(size.x, static_cast<float32>(size.y - GetTitleBarHeight()));
        [window setFrame: [window frameRectForContentRect: contentRect] display: YES];
    }

    void CocoaWindow::SetOpacity(float32 opacity)
    {
        [window setAlphaValue: opacity];
    }

    /* --- GETTER METHODS --- */

    std::string_view CocoaWindow::GetTitle() const
    {
        return title;
    }

    Vector2Int CocoaWindow::GetPosition() const
    {
        const NSRect contentRect = [window frame];
        return { contentRect.origin.x, contentRect.origin.y + contentRect.size.height };
    }

    uint32 CocoaWindow::GetWidth() const
    {
        return static_cast<uint32>([window frame].size.width);
    }

    uint32 CocoaWindow::GetHeight() const
    {
        return static_cast<uint32>([window frame].size.height) + GetTitleBarHeight();
    }

    uint32 CocoaWindow::GetFramebufferWidth() const
    {
        const NSRect contentRect = [window convertRectToBacking: view.frame];
        return static_cast<uint32>(contentRect.size.width);
    }

    uint32 CocoaWindow::GetFramebufferHeight() const
    {
        const NSRect contentRect = [window convertRectToBacking: view.frame];
        return static_cast<uint32>(contentRect.size.height);
    }

    float32 CocoaWindow::GetOpacity() const
    {
        return static_cast<float32>([window alphaValue]);
    }

    bool CocoaWindow::IsClosed() const
    {
        return closed;
    }

    bool CocoaWindow::IsMinimized() const
    {
        return [window isMiniaturized];
    }

    bool CocoaWindow::IsMaximized() const
    {
        return maximized;
    }

    bool CocoaWindow::IsFocused() const
    {
        return [window isKeyWindow];
    }

    bool CocoaWindow::IsHidden() const
    {
        return ![window isVisible];
    }

    Screen& CocoaWindow::GetScreen() const
    {
        return cocoaContext.GetWindowScreen(window);
    }

    InputManager* CocoaWindow::GetInputManager()
    {
        return &inputManager;
    }

    CursorManager* CocoaWindow::GetCursorManager()
    {
        return &cursorManager;
    }

    TouchManager* CocoaWindow::GetTouchManager()
    {
        return nullptr;
    }

    PlatformAPI CocoaWindow::GetAPI() const
    {
        return PlatformAPI::Cocoa;
    }

    /* --- EVENTS --- */

    #if defined(__OBJC__) && defined(COCOA_WINDOW_IMPLEMENTATION)
        void CocoaWindow::WindowShouldClose()
        {
            Close();
        }

        void CocoaWindow::WindowDidResize(const NSNotification* notification)
        {
            // Check if window has been maximized
            const bool nowMaximized = [window isZoomed];
            if (maximized != nowMaximized)
            {
                maximized = nowMaximized;
                if (maximized)
                {
                    GetWindowMaximizeDispatcher().DispatchEvent();
                }
            }

            // Otherwise handle event like a normal resize
            GetWindowResizeDispatcher().DispatchEvent(GetWidth(), GetHeight());
        }

        void CocoaWindow::WindowDidMove(const NSNotification* notification)
        {
            GetWindowMoveDispatcher().DispatchEvent(GetPosition());
        }

        void CocoaWindow::WindowDidMiniaturize(const NSNotification* notification)
        {
            GetWindowMinimizeDispatcher().DispatchEvent();
        }

        void CocoaWindow::WindowDidBecomeKey(const NSNotification* notification)
        {
            // macOS automatically shows cursor when window is unfocused, so we need to manually hide it again when focusing the window
            if (!cursorManager.IsCursorVisible()) cursorManager.SetCursorVisibility(false);
            GetWindowFocusDispatcher().DispatchEvent(true);
        }

        void CocoaWindow::WindowDidResignKey(const NSNotification* notification)
        {
            if (closed) return;
            GetWindowFocusDispatcher().DispatchEvent(false);
        }
    #endif

    /* --- PRIVATE METHODS --- */

    uint32 CocoaWindow::GetTitleBarHeight() const
    {
        return static_cast<uint32>(window.frame.size.height - [window contentRectForFrameRect: window.frame].size.height);
    }

    /* --- DESTRUCTOR --- */

    CocoaWindow::~CocoaWindow()
    {
        if (closed) return;

        [view release];
        [reinterpret_cast<CocoaWindowDelegate*>(delegate) release];

        cocoaContext.DestroyWindow(window);
        window = nil;
    }

}
