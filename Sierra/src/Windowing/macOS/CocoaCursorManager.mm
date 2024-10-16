//
// Created by Nikolay Kanchevski on 3.10.23.
//

#include "CocoaCursorManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaCursorManager::CocoaCursorManager(const NSWindow* window)
        : window(window), cursorPosition({ [window mouseLocationOutsideOfEventStream].x, [window mouseLocationOutsideOfEventStream].y }),   lastCursorPosition(cursorPosition)
    {

    }

    /* --- POLLING METHODS --- */

    void CocoaCursorManager::RegisterCursorMove(const Vector2 position)
    {
        cursorPosition = position;
        GetCursorMoveDispatcher().DispatchEvent(position);
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

    void CocoaCursorManager::SetCursorPosition(const Vector2 position)
    {
        // Set mouse position (method takes -Y screen coordinates)
        CGWarpMouseCursorPosition(CGPointMake(window.frame.origin.x + position.x, window.screen.frame.size.height - window.frame.origin.y - position.y));
    }

    /* --- GETTER METHODS --- */

    bool CocoaCursorManager::IsCursorVisible() const noexcept
    {
        return cursorShown;
    }

    Vector2 CocoaCursorManager::GetCursorPosition() const noexcept
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

    /* --- PRIVATE METHODS --- */

    void CocoaCursorManager::Update()
    {
        if (![window isKeyWindow]) return;
        lastCursorPosition = cursorPosition;

        const Vector2 center = Vector2(window.frame.size.width, window.frame.size.height) / 2.0f;
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