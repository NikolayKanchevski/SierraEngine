//
// Created by Nikolay Kanchevski on 17.08.23.
//

#define COCOA_WINDOW_IMPLEMENTATION
#include "CocoaWindow.h"

#include <QuartzCore/QuartzCore.h>
#include <IOKit/hidsystem/IOLLEvent.h>

@interface CocoaWindowDelegate : NSObject<NSWindowDelegate>

@end

@implementation CocoaWindowDelegate

    /* --- MEMBERS --- */
    {
        Sierra::CocoaWindow* window;
    }

    /* --- CONSTRUCTORS --- */

    - (instancetype) initWithWindow: (Sierra::CocoaWindow*) initWindow
    {
        self = [super init];
        window = initWindow;
        return self;
    }

    /* --- EVENTS --- */

    - (BOOL) windowShouldClose: (id) sender
    {
        // The Sierra window closes itself, so AppKit must not close it as well
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

    - (void) windowDidChangeBackingProperties: (NSNotification*) notification
    {
        window->WindowDidChangeBackingProperties();
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
        id keyUpMonitor;
    }

    /* --- CONSTRUCTORS --- */

    - (instancetype) initWithWindow: (Sierra::CocoaWindow*) initWindow
    {
        self = [super init];
        window = initWindow;

        // Modifiers prevent keyUp from being called, so we intercept such events and handle them manually
        keyUpMonitor = [NSEvent addLocalMonitorForEventsMatchingMask: NSEventMaskKeyUp handler: ^NSEvent* (NSEvent* event)
        {
            if ([event window] != [self window])
            {
                return event;
            }

            [self keyUp: event];
            return nil;
        }];

        return self;
    }

    /* --- EVENTS --- */

    - (void) keyDown: (NSEvent*) event
    {
        const Sierra::Key key = Sierra::KeyCodeToKey([event keyCode]);
        static_cast<Sierra::CocoaInputManager&>(*window->GetInputManager()).RegisterKeyPress(key);
    }

    - (void) flagsChanged: (NSEvent*) event
    {
        const Sierra::Key key = Sierra::KeyCodeToKey([event keyCode]);

        // Modifier keys need to be registered as pressed manually, so we get modifier flags of the modifier key... fun stuff!
        NSEventModifierFlags keyModifierFlag = 0;
        switch (key)
        {
            case Sierra::Key::LeftShift:    { keyModifierFlag = NX_DEVICELSHIFTKEYMASK; break; }
            case Sierra::Key::RightShift:   { keyModifierFlag = NX_DEVICERSHIFTKEYMASK; break; }
            case Sierra::Key::LeftControl:  { keyModifierFlag = NX_DEVICELCTLKEYMASK; break; }
            case Sierra::Key::RightControl: { keyModifierFlag = NX_DEVICERCTLKEYMASK; break; }
            case Sierra::Key::LeftOption:   { keyModifierFlag = NX_DEVICELALTKEYMASK; break; }
            case Sierra::Key::RightOption:  { keyModifierFlag = NX_DEVICERALTKEYMASK; break; }
            case Sierra::Key::LeftCommand:  { keyModifierFlag = NX_DEVICELCMDKEYMASK; break; }
            case Sierra::Key::RightCommand: { keyModifierFlag = NX_DEVICERCMDKEYMASK; break; }
            case Sierra::Key::CapsLock:     { keyModifierFlag = NSEventModifierFlagCapsLock; break; }
            case Sierra::Key::Function:     { keyModifierFlag = NSEventModifierFlagFunction; break; }
            default:                        { break; }
        }

        // Only register changes, as the modifier flags of one key also reflect the state of the other key with the same modifier
        Sierra::CocoaInputManager& inputManager = static_cast<Sierra::CocoaInputManager&>(*window->GetInputManager());
        const bool pressed = ([event modifierFlags] & keyModifierFlag) != 0;
        const bool wasPressed = inputManager.IsKeyPressed(key) || inputManager.IsKeyHeld(key);

        if (pressed && !wasPressed) inputManager.RegisterKeyPress(key);
        else if (!pressed && wasPressed) inputManager.RegisterKeyRelease(key);
    }

    - (void) keyUp: (NSEvent*) event
    {
        const Sierra::Key key = Sierra::KeyCodeToKey([event keyCode]);
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
        const Sierra::MouseButton mouseButton = Sierra::ButtonNumberToMouseButton([event buttonNumber]);
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
        const Sierra::MouseButton mouseButton = Sierra::ButtonNumberToMouseButton([event buttonNumber]);
        static_cast<Sierra::CocoaInputManager&>(*window->GetInputManager()).RegisterMouseButtonRelease(mouseButton);
    }

    - (void) scrollWheel: (NSEvent*) event
    {
        static_cast<Sierra::CocoaInputManager&>(*window->GetInputManager()).RegisterMouseScroll({ [event deltaX], [event scrollingDeltaY] });
    }

    - (void) mouseMoved: (NSEvent*) event
    {
        const NSPoint point = [self convertPointToBacking: [self convertPoint: [event locationInWindow] fromView: nil]];
        static_cast<Sierra::CocoaCursorManager&>(*window->GetCursorManager()).RegisterCursorMove({ static_cast<int32>(point.x), static_cast<int32>(point.y) });
    }

    - (void) mouseDragged: (NSEvent*) event
    {
        [self mouseMoved: event];
    }

    - (void) rightMouseDragged: (NSEvent*) event
    {
        [self mouseMoved: event];
    }

    - (void) otherMouseDragged: (NSEvent*) event
    {
        [self mouseMoved: event];
    }

    /* --- GETTER METHODS --- */

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
        [NSEvent removeMonitor: keyUpMonitor];

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

    CocoaWindow::CocoaWindow(const CocoaContext& cocoaContext, const WindowCreateInfo& createInfo)
        : Window(createInfo), cocoaContext(&cocoaContext),
            delegate([[CocoaWindowDelegate alloc] initWithWindow: this]),
            window(cocoaContext.CreateWindow(createInfo.title, createInfo.width, createInfo.height)),
            view([[CocoaWindowView alloc] initWithWindow: this]),
            cursorManager(window),
            title(createInfo.title)
    {
        // Assign Metal layer
        [view setLayer: [CAMetalLayer layer]];
        [view setWantsLayer: YES];

        // Configure layer
        [view.layer setFrame: view.bounds];
        [view.layer setContentsScale: window.backingScaleFactor];

        // Disable resizing (done before maximizing, as maximizing depends on whether the window is resizable)
        if (!createInfo.resizable)
        {
            [window setCollectionBehavior: NSWindowCollectionBehaviorFullScreenNone];
            [window setStyleMask: [window styleMask] & ~NSWindowStyleMaskResizable];
        }

        // Maximize window
        if (createInfo.maximize) Maximize();

        // Assign window handlers
        [window setDelegate: reinterpret_cast<CocoaWindowDelegate*>(delegate)];
        [window setContentView: view];
        [window makeFirstResponder: view];

        // Show and focus window
        [cocoaContext.GetNSApplication() activateIgnoringOtherApps: YES];
        [window orderFrontRegardless];
        [window makeKeyWindow];

        if (!createInfo.maximize)
        {
            // Adjust window size to account for title bar, because the rect passed in init is the size of the framebuffer
            NSRect contentRect = [window contentRectForFrameRect: [window frame]];
            contentRect.size = NSMakeSize([view frame].size.width, [view frame].size.height - GetTitleBarHeight());
            [window setFrame: [window frameRectForContentRect: contentRect] display: YES];
        }

        maximized = [window isZoomed];
    }

    /* --- POLLING METHODS --- */

    void CocoaWindow::Update()
    {
        inputManager.Update();
        cursorManager.Update();
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
        if ([window isMiniaturized]) [window deminiaturize: nil];
        if ([window isZoomed]) return;

        if ([window styleMask] & NSWindowStyleMaskResizable)
        {
            [window performZoom: nil];
        }
        else
        {
            // Non-resizable windows cannot be zoomed, so they are expanded to the work area of their screen manually
            const CocoaScreen& screen = cocoaContext->GetWindowScreen(window);
            const NSRect newFrame = NSMakeRect(screen.GetWorkAreaOrigin().x, screen.GetWorkAreaOrigin().y, screen.GetWorkAreaWidth(), screen.GetWorkAreaHeight());
            [window setFrame: newFrame display: YES animate: YES];
        }
    }

    void CocoaWindow::Show()
    {
        [cocoaContext->GetNSApplication() activateIgnoringOtherApps: YES];
        [window orderFrontRegardless];
        [window makeKeyWindow];
    }

    void CocoaWindow::Hide()
    {
        [window orderOut: nil];
    }

    void CocoaWindow::Focus()
    {
        [cocoaContext->GetNSApplication() activateIgnoringOtherApps: YES];
        [window orderFrontRegardless];
        [window makeKeyWindow];
    }

    void CocoaWindow::Close()
    {
        if (closed) return;
        closed = true;

        GetWindowCloseDispatcher().DispatchEvent();

        // The window is only hidden here, and is released in the destructor, as AppKit may still reference it while it is closing
        [window close];
    }

    /* --- SETTER METHODS --- */

    void CocoaWindow::SetTitle(const std::string_view newTitle)
    {
        title = newTitle;
        [window setTitle: [NSString stringWithCString: title.data() encoding: NSUTF8StringEncoding]];
    }

    void CocoaWindow::SetPosition(const Vector2Int position)
    {
        // Position is the top left corner of the window, while the frame origin is the bottom left corner, so the height is subtracted (as a signed value, which may be negative)
        const NSPoint origin = [window frameRectForContentRect: NSMakeRect(position.x, static_cast<float32>(position.y) - static_cast<float32>(GetHeight()), 0, 0)].origin;
        [window setFrameOrigin: origin];
    }

    void CocoaWindow::SetSize(const Vector2UInt size)
    {
        // The frame grows downwards in Cocoa coordinates, so its origin is moved to keep the top edge of the window in place
        NSRect frame = [window frame];
        frame.origin.y += frame.size.height - static_cast<float32>(size.y);
        frame.size = NSMakeSize(static_cast<float32>(size.x), static_cast<float32>(size.y));

        [window setFrame: frame display: YES];
    }

    void CocoaWindow::SetOpacity(float32 opacity)
    {
        [window setAlphaValue: opacity];
    }

    /* --- GETTER METHODS --- */

    std::string_view CocoaWindow::GetTitle() const noexcept
    {
        return title;
    }

    Vector2Int CocoaWindow::GetPosition() const noexcept
    {
        const NSRect contentRect = [window frame];
        return { static_cast<int32>(contentRect.origin.x), static_cast<int32>(contentRect.origin.y + contentRect.size.height) };
    }

    uint32 CocoaWindow::GetWidth() const noexcept
    {
        return static_cast<uint32>([window frame].size.width);
    }

    uint32 CocoaWindow::GetHeight() const noexcept
    {
        // Frame already includes the title bar
        return static_cast<uint32>([window frame].size.height);
    }

    uint32 CocoaWindow::GetFramebufferWidth() const noexcept
    {
        return static_cast<uint32>([view convertRectToBacking: [view bounds]].size.width);
    }

    uint32 CocoaWindow::GetFramebufferHeight() const noexcept
    {
        return static_cast<uint32>([view convertRectToBacking: [view bounds]].size.height);
    }

    float32 CocoaWindow::GetOpacity() const noexcept
    {
        return static_cast<float32>([window alphaValue]);
    }

    bool CocoaWindow::IsClosed() const noexcept
    {
        return closed;
    }

    bool CocoaWindow::IsMinimized() const noexcept
    {
        return [window isMiniaturized];
    }

    bool CocoaWindow::IsMaximized() const noexcept
    {
        return [window isZoomed];
    }

    bool CocoaWindow::IsFocused() const noexcept
    {
        return [window isKeyWindow];
    }

    bool CocoaWindow::IsHidden() const noexcept
    {
        return ![window isVisible];
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

        void CocoaWindow::WindowDidChangeBackingProperties()
        {
            // Window may have moved to a screen with a different scale factor, which changes the size of its framebuffer in pixels
            [view.layer setContentsScale: [window backingScaleFactor]];
            GetWindowResizeDispatcher().DispatchEvent(GetWidth(), GetHeight());
        }
    #endif

    /* --- GETTER METHODS --- */

    uint32 CocoaWindow::GetTitleBarHeight() const
    {
        return static_cast<uint32>(window.frame.size.height - [window contentRectForFrameRect: window.frame].size.height);
    }

    /* --- DESTRUCTOR --- */

    CocoaWindow::~CocoaWindow() noexcept
    {
        [window setDelegate: nil];
        [window close];

        [view release];
        [window release];
        [reinterpret_cast<CocoaWindowDelegate*>(delegate) release];
    }

}
