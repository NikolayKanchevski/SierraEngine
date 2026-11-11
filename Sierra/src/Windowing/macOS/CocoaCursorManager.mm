//
// Created by Nikolay Kanchevski on 3.10.23.
//

#include "CocoaCursorManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaCursorManager::CocoaCursorManager(const NSWindow* window)
        : window(window)
    {
        // Mouse location is in points, so we convert to pixels
        const NSPoint mouseLocation = [window convertPointToBacking: [window mouseLocationOutsideOfEventStream]];
        cursorPosition = { static_cast<int32>(mouseLocation.x), static_cast<int32>(mouseLocation.y) };
        lastCursorPosition = cursorPosition;
    }

    /* --- POLLING METHODS --- */

    void CocoaCursorManager::RegisterCursorMove(const Vector2Int position)
    {
        cursorPosition = position;
        if (cursorShown) GetCursorMoveDispatcher().DispatchEvent(position);
    }

    /* --- SETTER METHODS --- */

    void CocoaCursorManager::SetCursorVisibility(const bool visible)
    {
        if (visible)
        {
            [NSCursor unhide];
            cursorShown = true;
        }
        else
        {
            [NSCursor hide];
            cursorShown = false;
            justHidCursor = true;
        }
    }

    void CocoaCursorManager::SetCursorPosition(const Vector2Int position)
    {
        // Convert position from backing pixels to points, relative to the bottom left corner of the window
        const NSPoint windowPosition = [window convertPointFromBacking: NSMakePoint(position.x, position.y)];

        // Set mouse position (CoreGraphics takes Y coordinates from the top of the primary screen, which is not necessarily the screen of this window)
        const CGFloat primaryScreenHeight = CGDisplayBounds(CGMainDisplayID()).size.height;
        CGWarpMouseCursorPosition(CGPointMake(window.frame.origin.x + windowPosition.x, primaryScreenHeight - window.frame.origin.y - windowPosition.y));
    }

    /* --- GETTER METHODS --- */

    bool CocoaCursorManager::IsCursorVisible() const noexcept
    {
        return cursorShown;
    }

    Vector2Int CocoaCursorManager::GetCursorPosition() const noexcept
    {
        return cursorPosition;
    }

    Vector2 CocoaCursorManager::GetCursorDelta() const noexcept
    {
        return -Vector2(lastCursorPosition.x - cursorPosition.x, lastCursorPosition.y - cursorPosition.y);
    }

    WindowingBackendType CocoaCursorManager::GetBackendType() const noexcept
    {
        return WindowingBackendType::Cocoa;
    }

    /* --- POLLING METHODS --- */

    void CocoaCursorManager::Update()
    {
        if (![window isKeyWindow]) return;
        lastCursorPosition = cursorPosition;

        // Center of the framebuffer, in backing pixels
        const NSSize contentSize = [window contentRectForFrameRect: [window frame]].size;
        const CGFloat scale = [window backingScaleFactor];
        const Vector2Int center = Vector2Int(contentSize.width * scale, contentSize.height * scale) / 2;
        if (cursorShown || cursorPosition == center) return;

        // Move cursor to center
        SetCursorPosition(center);

        // Update mouse position
        lastCursorPosition = cursorPosition;
        cursorPosition = center;

        // Reset mouse delta when re-centering for the first time
        if (justHidCursor)
        {
            lastCursorPosition = cursorPosition;
            justHidCursor = false;
        }
    }

}