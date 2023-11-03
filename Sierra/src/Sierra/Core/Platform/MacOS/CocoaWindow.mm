//
// Created by Nikolay Kanchevski on 17.08.23.
//

#include "CocoaWindow.h"

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

    /* --- POLLING METHODS --- */

    - (BOOL) windowShouldClose: (id) sender
    {
        window->WindowShouldClose();
        return NO;
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

@interface CocoaWindowContentView : NSView<NSTextInputClient>

@end

@implementation CocoaWindowContentView

    /* --- MEMBERS --- */
    {
        Sierra::CocoaWindow* window;
        NSTrackingArea* trackingArea;
        NSMutableAttributedString* markedText;
    }

    /* --- CONSTRUCTORS --- */

    - (instancetype) initWithWindow: (Sierra::CocoaWindow*) initWindow;
    {
        self = [super init];

        window = initWindow;
        trackingArea = nil;
        markedText = [[NSMutableAttributedString alloc] init];
        [self updateTrackingAreas];
        [self registerForDraggedTypes: @[NSPasteboardTypeURL]];

        return self;
    }

    /* --- POLLING METHODS --- */

    - (void) keyDown: (NSEvent*) event
    {
        // No casting error checks are done, since the input manager type within a CocoaWindow is guaranteed to be CocoaInputManager
        static_cast<Sierra::CocoaInputManager*>(&window->GetInputManager())->KeyDown(event);
        [self interpretKeyEvents: @[event]];
    }

    - (void) flagsChanged: (NSEvent*) event
    {
        // Unsafe casting, as the input manager type within a CocoaWindow is guaranteed to be CocoaInputManager
        static_cast<Sierra::CocoaInputManager*>(&window->GetInputManager())->FlagsChanged(event);
    }

    - (void) keyUp: (NSEvent*) event
    {
        // Unsafe casting, as the input manager type within a CocoaWindow is guaranteed to be CocoaInputManager
        static_cast<Sierra::CocoaInputManager*>(&window->GetInputManager())->KeyUp(event);
    }

    - (void) mouseDown: (NSEvent*) event
    {
        // Unsafe casting, as the input manager type within a CocoaWindow is guaranteed to be CocoaInputManager
        static_cast<Sierra::CocoaInputManager*>(&window->GetInputManager())->MouseDown(event);
    }

    - (void) rightMouseDown: (NSEvent*) event
    {
        // Unsafe casting, as the input manager type within a CocoaWindow is guaranteed to be CocoaInputManager
        static_cast<Sierra::CocoaInputManager*>(&window->GetInputManager())->RightMouseDown(event);
    }

    - (void) otherMouseDown: (NSEvent*) event
    {
        // Unsafe casting, as the input manager type within a CocoaWindow is guaranteed to be CocoaInputManager
        static_cast<Sierra::CocoaInputManager*>(&window->GetInputManager())->OtherMouseDown(event);
    }

    - (void) mouseUp: (NSEvent*) event
    {
        // Unsafe casting, as the input manager type within a CocoaWindow is guaranteed to be CocoaInputManager
        static_cast<Sierra::CocoaInputManager*>(&window->GetInputManager())->MouseUp(event);
    }

    - (void) rightMouseUp: (NSEvent*) event
    {
        // Unsafe casting, as the input manager type within a CocoaWindow is guaranteed to be CocoaInputManager
        static_cast<Sierra::CocoaInputManager*>(&window->GetInputManager())->RightMouseUp(event);
    }

    - (void) otherMouseUp: (NSEvent*) event
    {
        // Unsafe casting, as the input manager type within a CocoaWindow is guaranteed to be CocoaInputManager
        static_cast<Sierra::CocoaInputManager*>(&window->GetInputManager())->OtherMouseUp(event);
    }

    - (void) scrollWheel: (NSEvent*) event
    {
        // Unsafe casting, as the input manager type within a CocoaWindow is guaranteed to be CocoaInputManager
        static_cast<Sierra::CocoaInputManager*>(&window->GetInputManager())->ScrollWheel(event);
    }

    - (void) mouseMoved: (NSEvent*) event
    {
        // Unsafe casting, as the cursor manager type within a CocoaWindow is guaranteed to be CocoaCursorManager
        static_cast<Sierra::CocoaCursorManager*>(&window->GetCursorManager())->MouseMoved(event);
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
        return [markedText length] > 0;
    }

    - (void) setMarkedText: (id) string selectedRange: (NSRange) selectedRange replacementRange: (NSRange) replacementRange
    {
        [markedText release];
        if ([string isKindOfClass: [NSAttributedString class]]) markedText = [[NSMutableAttributedString alloc] initWithAttributedString:string];
        else markedText = [[NSMutableAttributedString alloc] initWithString:string];
    }

    - (void) unmarkText
    {
        [[markedText mutableString] setString: @""];
    }

    - (NSArray*) validAttributesForMarkedText
    {
        return [NSArray array];
    }
    - (NSRange) markedRange
    {
        return [markedText length] > 0 ? NSMakeRange(0, [markedText length] - 1) : NSRange { NSNotFound, 0 };
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

    - (void) insertText: (id) string replacementRange: (NSRange) replacementRange
    {

    }

    - (NSAttributedString*) attributedSubstringForProposedRange: (NSRange) range actualRange: (NSRangePointer) actualRange
    {
        return nil;
    }

    /* --- DESTRUCTOR --- */

    - (void) dealloc
    {
        [trackingArea release];
        [markedText release];
        [super dealloc];
    }

@end

@interface CocoaWindowImplementation : NSWindow

@end

@implementation CocoaWindowImplementation

    /* --- GETTER METHODS --- */

    - (BOOL) canBecomeKeyWindow
    {
        return YES;
    }

    - (BOOL) canBecomeMainWindow
    {
        return YES;
    }

    /* --- DESTRUCTOR --- */

    - (void) dealloc
    {
        [super dealloc];
    }

@end

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaWindow::CocoaWindow(const WindowCreateInfo &createInfo)
        : Window(createInfo), macOSInstance(*static_cast<MacOSInstance*>(createInfo.platformInstance.get())), inputManager(new CocoaInputManager({ })), cursorManager({ })
    {
        SR_ERROR_IF(createInfo.platformInstance->GetType() !=+ PlatformType::MacOS, "Cannot create Cocoa window using a platform instance of type [{0}]!", createInfo.platformInstance->GetType()._to_string());

        @autoreleasepool
        {
            // Create delegate
            delegate = [[CocoaWindowDelegate alloc] initWithWindow: this];
            SR_ERROR_IF(delegate == nil, "Could not create Cocoa window delegate!");

            // Create window
            window = [[CocoaWindowImplementation alloc] initWithContentRect: NSMakeRect(0.0f, 0.0f, static_cast<float32>(createInfo.width), static_cast<float32>(createInfo.height)) styleMask: NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | (createInfo.resizable ? NSWindowStyleMaskResizable : 0) backing: NSBackingStoreBuffered defer: NO];
            SR_ERROR_IF(window == nil, "Could not create Cocoa window!");

            // Create view
            view = [[CocoaWindowContentView alloc] initWithWindow: this];
            SR_ERROR_IF(view == nil, "Could not create Cocoa window view!");
        }

        // Maximize window manually, or through Cocoa if resizable
        if (createInfo.maximize)
        {
            if (createInfo.resizable)
            {
                [window performZoom: nil];
            }
            else
            {
                const NSScreen* screen = [window screen];
                NSRect newFrame = NSMakeRect(screen.frame.origin.x, screen.frame.origin.y, screen.frame.size.width, screen.frame.size.height - macOSInstance.GetMenuBarHeight());
                [window setFrame: newFrame display: YES animate: YES];
            }
        }

        // Set window properties
        [window center];
        [window setTitle: @(createInfo.title.c_str())];
        [window setCollectionBehavior: createInfo.resizable ? (NSWindowCollectionBehaviorFullScreenPrimary | NSWindowCollectionBehaviorManaged) : (NSWindowCollectionBehaviorFullScreenNone)];
        [window setAcceptsMouseMovedEvents: YES];
        [window setRestorable: NO];

        // Assign window handlers
        [window setContentView: view];
        [window makeFirstResponder: view];
        [window setDelegate: delegate];
        #if MAC_OS_X_VERSION_MAX_ALLOWED >= 101200
            if ([window respondsToSelector: @selector(setTabbingMode:)]) [window setTabbingMode: NSWindowTabbingModeDisallowed];
        #endif

        if (!createInfo.hide)
        {
            // Show and focus window
            [macOSInstance.GetApplication() activateIgnoringOtherApps: YES];
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

        // Poll creation events
        while (true)
        {
            NSEvent* event = [macOSInstance.GetApplication() nextEventMatchingMask: NSEventMaskAny untilDate: [NSDate distantPast] inMode: NSDefaultRunLoopMode dequeue: YES];
            if (event == nil) break;

            [macOSInstance.GetApplication() sendEvent: event];
        }
    }

    /* --- POLLING METHODS --- */

    void CocoaWindow::OnUpdate()
    {
        // TODO: TRY NO POINTER ON INPUT MANAGER
        inputManager->OnUpdate();
        cursorManager.OnUpdate();

        while (true)
        {
            NSEvent* event = [macOSInstance.GetApplication() nextEventMatchingMask: NSEventMaskAny untilDate: [NSDate distantPast] inMode: NSDefaultRunLoopMode dequeue: YES];
            if (event == nil) break;
            [macOSInstance.GetApplication() sendEvent: event];
        }
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
        [macOSInstance.GetApplication() activateIgnoringOtherApps: YES];
        [window orderFrontRegardless];
        [window makeKeyWindow];
    }

    void CocoaWindow::Hide()
    {
        [window orderOut: nil];
    }

    void CocoaWindow::Focus()
    {
        [macOSInstance.GetApplication() activateIgnoringOtherApps: YES];
        [window orderFrontRegardless];
        [window makeKeyWindow];
    }

    void CocoaWindow::Close()
    {
        if (closed) return;
        closed = true;

        GetWindowCloseDispatcher().DispatchEvent();
        [window performClose: nil];

        @autoreleasepool
        {
            [window setDelegate: nil];
            [delegate release];
            delegate = nil;

            [window setContentView: nil];
            [view release];
            view = nil;

            [window release];
            window = nil;
        }
    }

    /* --- SETTER METHODS --- */

    void CocoaWindow::SetTitle(const String &newTitle)
    {
        [window setTitle: @(newTitle.c_str())];
    }

    void CocoaWindow::SetPosition(const Vector2Int &position)
    {
        [window setFrameOrigin: [window frameRectForContentRect: NSMakeRect(position.x, position.y - GetSize().y, 0, 0)].origin];
    }

    void CocoaWindow::SetSize(const Vector2UInt &size)
    {
        // Set new size and move window up, so that it retains its top Y position
        NSRect contentRect = [window contentRectForFrameRect: [window frame]];
        contentRect.size = NSMakeSize(size.x, static_cast<float32>(size.y) - GetTitleBarHeight());
        [window setFrame: [window frameRectForContentRect: contentRect] display: YES];
    }

    void CocoaWindow::SetOpacity(float32 opacity)
    {
        [window setAlphaValue: opacity];
    }

    /* --- GETTER METHODS --- */

    String CocoaWindow::GetTitle() const
    {
        return window.title.UTF8String;
    }

    Vector2Int CocoaWindow::GetPosition() const
    {
        const NSRect contentRect = [window frame];
        return { contentRect.origin.x, contentRect.origin.y + contentRect.size.height };
    }

    Vector2UInt CocoaWindow::GetSize() const
    {
        return { [view frame].size.width, [view frame].size.height + GetTitleBarHeight() };
    }

    Vector2UInt CocoaWindow::GetFramebufferSize() const
    {
        const NSRect contentRect = [window contentRectForFrameRect: [[window contentView] frame]];
        return { contentRect.size.width, contentRect.size.height };
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

    InputManager& CocoaWindow::GetInputManager()
    {
        return *inputManager;
    }

    CursorManager& CocoaWindow::GetCursorManager()
    {
        return cursorManager;
    }

    WindowAPI CocoaWindow::GetAPI() const
    {
        return WindowAPI::Cocoa;
    }

    /* --- EVENTS --- */

    #if defined(__OBJC__)
        void CocoaWindow::WindowShouldClose()
        {
            Close();
        }

        void CocoaWindow::WindowDidResize(const NSNotification* notification)
        {
            // Get current window size
            const NSRect contentRect = [view frame];

            // Check if window has been maximized
            bool nowMaximized = [window isZoomed];
            if (maximized != nowMaximized)
            {
                maximized = nowMaximized;
                if (maximized)
                {
                    GetWindowMaximizeDispatcher().DispatchEvent();
                    return;
                }
            }

            // Otherwise handle event like a normal resize
            GetWindowResizeDispatcher().DispatchEvent(GetSize());
            GetWindowResizeDispatcher().DispatchEvent(GetFramebufferSize());
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
            // MacOS automatically shows cursor when window is unfocused, so we need to manually hide it again when focusing the window
            if (cursorManager.IsCursorHidden()) cursorManager.HideCursor();

            GetWindowFocusDispatcher().DispatchEvent(true);
        }

        void CocoaWindow::WindowDidResignKey(const NSNotification* notification)
        {
            if (closed) return;
            GetWindowFocusDispatcher().DispatchEvent(false);
        }
    #endif

    /* --- PRIVATE METHODS --- */

    float32 CocoaWindow::GetTitleBarHeight() const
    {
        return static_cast<float32>(window.frame.size.height - [window contentRectForFrameRect: window.frame].size.height);
    }

    /* --- DESTRUCTOR --- */

    CocoaWindow::~CocoaWindow()
    {
        if (closed) return;
        [window performClose: nil];
    }
}
