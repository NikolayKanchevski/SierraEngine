//
// Created by Nikolay Kanchevski on 10.28.23.
//

#include "X11CursorManager.h"

#include <X11/Xlib.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    X11CursorManager::X11CursorManager(const X11Context& x11Context, const XID window)
        : x11Context(x11Context), window(window)
    {
        // Get window size
        XWindowAttributes windowAttributes = { };
        XGetWindowAttributes(x11Context.GetDisplay(), window, &windowAttributes);

        uint mask;
        XID rootWindow, childWindow;
        int xRootCursorPosition, yRootCursorPosition;

        // Retrieve current cursor position
        int xChildCursorPosition, yChildCursorPosition;
        XQueryPointer(x11Context.GetDisplay(), window, &rootWindow, &childWindow, &xRootCursorPosition, &yRootCursorPosition, &xChildCursorPosition, &yChildCursorPosition, &mask);

        // Save cursor position
        cursorPosition = { xChildCursorPosition, windowAttributes.height - yChildCursorPosition };
        lastCursorPosition = cursorPosition;
    }

    /* --- POLLING METHODS --- */

    void X11CursorManager::RegisterCursorMove(const Vector2Int position)
    {
        cursorPosition = position;
        if (cursorShown && cursorPosition != lastCursorPosition) GetCursorMoveDispatcher().DispatchEvent(cursorPosition);
    }

    /* --- SETTER METHODS --- */

    void X11CursorManager::SetCursorVisibility(const bool visible)
    {
        if (visible) XDefineCursor(x11Context.GetDisplay(), window, x11Context.GetInvisibleCursor());
        else XUndefineCursor(x11Context.GetDisplay(), window);

        cursorShown = visible;
    }

    void X11CursorManager::SetCursorPosition(const Vector2Int position)
    {
        // Get window size
        XWindowAttributes windowAttributes = { };
        XGetWindowAttributes(x11Context.GetDisplay(), window, &windowAttributes);

        // Invert Y coordinate
        const Vector2Int x11Position = { position.x, windowAttributes.height - position.y };

        // Update cursor position
        XWarpPointer(x11Context.GetDisplay(), None, window, 0, 0, 0, 0, position.x, position.y);
        XFlush(x11Context.GetDisplay());

        // Reset mouse delta
        lastCursorPosition = cursorPosition;
        cursorPosition = position;
    }

    /* --- GETTER METHODS --- */

    bool X11CursorManager::IsCursorVisible() const noexcept
    {
        return cursorShown;
    }

    Vector2Int X11CursorManager::GetCursorPosition() const noexcept
    {
        return cursorPosition;
    }

    Vector2 X11CursorManager::GetCursorDelta() const noexcept
    {
        Vector2 delta = { cursorPosition.x - lastCursorPosition.x, cursorPosition.y - lastCursorPosition.y };
        delta *= static_cast<float32>(static_cast<uint8>(!cursorShown) * -1);
        return delta;
    }

    WindowingBackendType X11CursorManager::GetBackendType() const noexcept
    {
        return WindowingBackendType::X11;
    }

    /* --- PRIVATE METHODS --- */

    void X11CursorManager::Update()
    {
        XID focusedWindow;
        int focusedWindowState;
        XGetInputFocus(x11Context.GetDisplay(), &focusedWindow, &focusedWindowState);

        if (focusedWindow != window) return;
        lastCursorPosition = cursorPosition;

        XWindowAttributes windowAttributes = { };
        XGetWindowAttributes(x11Context.GetDisplay(), window, &windowAttributes);

        const Vector2Int center = Vector2Int(windowAttributes.width, windowAttributes.height) / 2;
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