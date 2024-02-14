//
// Created by Nikolay Kanchevski on 3.10.23.
//

#define COCOA_CURSOR_MANAGER_IMPLEMENTATION
#include "CocoaCursorManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaCursorManager::CocoaCursorManager(const NSWindow* window, const CursorManagerCreateInfo &createInfo)
        : CursorManager(createInfo), window(window), cursorPosition({ [window mouseLocationOutsideOfEventStream].x, [window mouseLocationOutsideOfEventStream].y }), lastCursorPosition(cursorPosition)
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

    bool CocoaCursorManager::IsCursorHidden() const
    {
        return cursorHidden;
    }

    Vector2 CocoaCursorManager::GetCursorPosition() const
    {
        return cursorPosition;
    }

    float32 CocoaCursorManager::GetHorizontalDelta() const
    {
        return lastCursorPosition.x - cursorPosition.x;
    }

    float32 CocoaCursorManager::GetVerticalDelta() const
    {
        return lastCursorPosition.y - cursorPosition.y;
    }

    /* --- PRIVATE METHODS --- */

    void CocoaCursorManager::Update()
    {
        lastCursorPosition = cursorPosition;
    }

    void CocoaCursorManager::UpdateEnd()
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

    #if defined(__OBJC__) && (defined(COCOA_CURSOR_MANAGER_IMPLEMENTATION) || defined(COCOA_WINDOW_IMPLEMENTATION))
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