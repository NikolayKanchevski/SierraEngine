//
// Created by Nikolay Kanchevski on 3.10.23.
//

#include "CocoaCursorManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaCursorManager::CocoaCursorManager(const CocoaCursorManagerCreateInfo &createInfo)
        : CursorManager(createInfo), window(createInfo.window), cursorPosition({ [window mouseLocationOutsideOfEventStream].x, [window mouseLocationOutsideOfEventStream].y }), lastCursorPosition(cursorPosition)
    {

    }

    /* --- SETTER METHODS --- */

    void CocoaCursorManager::ShowCursor()
    {
        cursorHidden = false;
        [NSCursor unhide];
    }

    void CocoaCursorManager::HideCursor()
    {
        cursorHidden = true;
        justHidCursor = true;
        [NSCursor hide];
    }

    void CocoaCursorManager::SetCursorPosition(const Vector2 &position)
    {
        // Set mouse position (method takes -Y screen coordinates)
        CGWarpMouseCursorPosition(CGPointMake([window frame].origin.x + position.x, [[window screen] frame].size.height - [window frame].origin.y - position.y));
    }

    /* --- GETTER METHODS --- */

    bool CocoaCursorManager::IsCursorHidden()
    {
        return cursorHidden;
    }

    Vector2 CocoaCursorManager::GetCursorPosition()
    {
        return cursorPosition;
    }

    float32 CocoaCursorManager::GetHorizontalDelta()
    {
        return lastCursorPosition.x - cursorPosition.x;
    }

    float32 CocoaCursorManager::GetVerticalDelta()
    {
        return lastCursorPosition.y - cursorPosition.y;
    }

    /* --- PRIVATE METHODS --- */

    void CocoaCursorManager::OnUpdate()
    {
        lastCursorPosition = cursorPosition;
    }

    void CocoaCursorManager::OnUpdateEnd()
    {
        if (!cursorHidden || ![window isKeyWindow]) return;

        const Vector2 cocoaCenter = Vector2([window frame].size.width, [window frame].size.height) / 2.0f;
        if (cursorPosition != cocoaCenter)
        {
            // Move cursor to center
            SetCursorPosition(cocoaCenter);

            // Update mouse position
            lastCursorPosition = cursorPosition;
            cursorPosition = cocoaCenter;

            // Reset mouse delta when re-centering for the first time
            if (justHidCursor)
            {
                lastCursorPosition = cursorPosition;
                justHidCursor = false;
            }
        }
    }

    /* --- EVENTS --- */

    #if defined(__OBJC__)
        void CocoaCursorManager::MouseMoved(const NSEvent* event)
        {
            // Get and save position within the window
            const NSPoint position = [event locationInWindow];
            cursorPosition = { position.x, position.y };

            // Dispatch events
            if (!cursorHidden) GetCursorMoveDispatcher().DispatchEvent(cursorPosition);
        }
    #endif

}