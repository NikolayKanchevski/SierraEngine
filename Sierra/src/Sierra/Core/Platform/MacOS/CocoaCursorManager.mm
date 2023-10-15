//
// Created by Nikolay Kanchevski on 3.10.23.
//

#include "CocoaCursorManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaCursorManager::CocoaCursorManager(const CursorManagerCreateInfo &createInfo)
        : CursorManager(createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    void CocoaCursorManager::OnUpdate()
    {
        lastCursorPosition = cursorPosition;
    }

    /* --- SETTER METHODS --- */

    void CocoaCursorManager::SetCursorPosition(const Vector2 &position)
    {
        // Get window position
        const NSRect windowRect = [[[NSApplication sharedApplication] keyWindow] frame];

        // Create point in -Y coordinate system
        const CGPoint point = CGPointMake(windowRect.origin.x + position.x, InvertWindowPositionY(windowRect.origin.y + position.y));

        // Set mouse position
        CGWarpMouseCursorPosition(point);
    }

    void CocoaCursorManager::ShowCursor()
    {
        [NSCursor unhide];
        cursorHidden = false;
    }

    void CocoaCursorManager::HideCursor()
    {
        [NSCursor hide];
        cursorHidden = true;
    }

    /* --- GETTER METHODS --- */

    Vector2 CocoaCursorManager::GetCursorPosition()
    {
        return cursorPosition;
    }

    bool CocoaCursorManager::IsCursorShown()
    {
        return !cursorHidden;
    }

    bool CocoaCursorManager::IsCursorHidden()
    {
        return cursorHidden;
    }

    float CocoaCursorManager::GetHorizontalDelta()
    {
        return -(lastCursorPosition.x - cursorPosition.x);
    }

    float CocoaCursorManager::GetVerticalDelta()
    {
        return -(lastCursorPosition.y - cursorPosition.y);
    }

    /* --- EVENTS --- */

    #if defined(__OBJC__)
        void CocoaCursorManager::MouseMoved(const NSEvent* event)
        {
            // Get position within the window and save new position
            const NSPoint position = [event locationInWindow];
            cursorPosition = { position.x, position.y };

            // Dispatch events
            GetCursorMoveDispatcher().DispatchEvent(cursorPosition);
        }
    #endif

    /* --- PRIVATE METHODS --- */

    double CocoaCursorManager::InvertWindowPositionY(const double yPosition)
    {
        return CGDisplayBounds(CGMainDisplayID()).size.height - yPosition;
    }

}