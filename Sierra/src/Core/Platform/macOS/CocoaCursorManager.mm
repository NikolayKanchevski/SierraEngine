//
// Created by Nikolay Kanchevski on 3.10.23.
//

#include "CocoaCursorManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaCursorManager::CocoaCursorManager(const NSWindow* window)
        : CursorManager(), window(window),
            cursorPosition({ [window mouseLocationOutsideOfEventStream].x, [window mouseLocationOutsideOfEventStream].y }),
            lastCursorPosition(cursorPosition)
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
        CGWarpMouseCursorPosition(CGPointMake([window frame].origin.x + position.x, [[window screen] frame].size.height - [window frame].origin.y - position.y));
    }

    /* --- GETTER METHODS --- */

    bool CocoaCursorManager::IsCursorVisible() const
    {
        return cursorShown;
    }

    Vector2 CocoaCursorManager::GetCursorPosition() const
    {
        return cursorPosition;
    }

    Vector2 CocoaCursorManager::GetCursorDelta() const
    {
        return { lastCursorPosition.x - cursorPosition.x, lastCursorPosition.y - cursorPosition.y };
    }

    /* --- PRIVATE METHODS --- */

    void CocoaCursorManager::Update()
    {
        lastCursorPosition = cursorPosition;
    }

    void CocoaCursorManager::PostUpdate()
    {
        if (cursorShown || ![window isKeyWindow]) return;

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

}