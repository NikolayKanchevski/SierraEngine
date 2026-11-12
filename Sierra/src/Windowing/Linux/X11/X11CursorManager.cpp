//
// Created by Nikolay Kanchevski on 10.28.23.
//

#include "X11CursorManager.h"

#include <X11/Xlib.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    X11CursorManager::X11CursorManager(const X11Context& x11Context, const XID window, const Vector2UInt windowSize)
        : x11Context(x11Context), window(window), windowSize(windowSize)
    {
        XID rootWindow = None;
        XID childWindow = None;
        int xRootCursorPosition = 0;
        int yRootCursorPosition = 0;
        int xChildCursorPosition = 0;
        int yChildCursorPosition = 0;
        uint mask = 0;

        if (XQueryPointer(x11Context.GetDisplay(), window, &rootWindow, &childWindow, &xRootCursorPosition, &yRootCursorPosition, &xChildCursorPosition, &yChildCursorPosition, &mask))
        {
            // Cursor's Y coordinate is measured upwards from the bottom of the window
            cursorPosition = { xChildCursorPosition, static_cast<int32>(windowSize.y) - yChildCursorPosition };
        }

        lastCursorPosition = cursorPosition;
    }

    /* --- POLLING METHODS --- */

    void X11CursorManager::RegisterCursorMove(const Vector2Int position)
    {
        cursorPosition = position;
        if (cursorShown && cursorPosition != lastCursorPosition) GetCursorMoveDispatcher().DispatchEvent(GetCursorPosition());
    }

    /* --- SETTER METHODS --- */

    void X11CursorManager::SetCursorVisibility(const bool visible)
    {
        if (visible == cursorShown) return;

        // Hidden cursor is an empty image, which replaces the default one
        if (visible) XUndefineCursor(x11Context.GetDisplay(), window);
        else XDefineCursor(x11Context.GetDisplay(), window, x11Context.GetInvisibleCursor());

        cursorShown = visible;

        // Motion, which happened while switching modes, is not a motion of the cursor
        lastCursorPosition = cursorPosition;
        XFlush(x11Context.GetDisplay());
    }

    void X11CursorManager::SetCursorPosition(const Vector2Int position)
    {
        WarpCursor(x11Context.GetPhysicalPosition(position));
    }

    /* --- GETTER METHODS --- */

    bool X11CursorManager::IsCursorVisible() const noexcept
    {
        return cursorShown;
    }

    Vector2Int X11CursorManager::GetCursorPosition() const noexcept
    {
        return x11Context.GetLogicalPosition(cursorPosition);
    }

    Vector2 X11CursorManager::GetCursorDelta() const noexcept
    {
        Vector2 delta = { cursorPosition.x - lastCursorPosition.x, cursorPosition.y - lastCursorPosition.y };

        // Motion is measured in physical pixels, so that small motions are not lost to rounding, and is then converted to logical units
        delta /= x11Context.GetScaling();

        // Hidden cursor is kept in place, so its motion is inverted, as on the other platforms
        if (!cursorShown) delta *= -1.0f;
        return delta;
    }

    WindowingBackendType X11CursorManager::GetBackendType() const noexcept
    {
        return WindowingBackendType::X11;
    }

    /* --- PRIVATE METHODS --- */

    void X11CursorManager::WarpCursor(const Vector2Int physicalPosition)
    {
        // Position is measured upwards from the bottom of the window, while X11 measures it downwards from the top
        XWarpPointer(x11Context.GetDisplay(), None, window, 0, 0, 0, 0, physicalPosition.x, static_cast<int32>(windowSize.y) - physicalPosition.y);
        XFlush(x11Context.GetDisplay());

        lastCursorPosition = physicalPosition;
        cursorPosition = physicalPosition;
    }

    void X11CursorManager::Update(const bool focused)
    {
        if (cursorShown || !focused)
        {
            lastCursorPosition = cursorPosition;
            return;
        }

        // Hidden cursor is kept at the center of the focused window, so that its motion is always measured from there, and it never reaches an edge
        const Vector2Int center = { static_cast<int32>(windowSize.x) / 2, static_cast<int32>(windowSize.y) / 2 };
        if (cursorPosition != center) WarpCursor(center);

        lastCursorPosition = center;
    }

    void X11CursorManager::SetWindowSize(const Vector2UInt size)
    {
        windowSize = size;
    }

}
