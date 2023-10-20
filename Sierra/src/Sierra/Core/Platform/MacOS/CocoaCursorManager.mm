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
        cursorShown = true;
        [NSCursor unhide];
    }

    void CocoaCursorManager::HideCursor()
    {
        cursorShown = false;
        [NSCursor hide];
    }

    /* --- GETTER METHODS --- */

    Vector2 CocoaCursorManager::GetCursorPosition()
    {
        return cursorPosition;
    }

    bool CocoaCursorManager::IsCursorShown()
    {
        return cursorShown;
    }

    bool CocoaCursorManager::IsCursorHidden()
    {
        return !cursorShown;
    }

    float32 CocoaCursorManager::GetHorizontalDelta()
    {
        return -(lastCursorPosition.x - cursorPosition.x);
    }

    float32 CocoaCursorManager::GetVerticalDelta()
    {
        return -(lastCursorPosition.y - cursorPosition.y);
    }

    /* --- EVENTS --- */

    #if defined(__OBJC__)
        void CocoaCursorManager::MouseMoved(const NSEvent* event)
        {
            // Get and save position within the window
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