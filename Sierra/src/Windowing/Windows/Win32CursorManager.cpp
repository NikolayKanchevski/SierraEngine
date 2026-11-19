//
// Created by Nikolay Kanchevski on 10.20.2023.
//

#include "Win32CursorManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Win32CursorManager::Win32CursorManager(HWND window, const Vector2UInt windowSize)
        : window(window), windowSize(windowSize)
    {
        POINT cursorPoint = { };
        if (GetCursorPos(&cursorPoint) && ScreenToClient(window, &cursorPoint))
        {
            // Cursor's Y coordinate is measured upwards from the bottom of the window
            cursorPosition = { cursorPoint.x, static_cast<int32>(windowSize.y) - cursorPoint.y };
        }

        lastCursorPosition = cursorPosition;
    }

    /* --- POLLING METHODS --- */

    void Win32CursorManager::RegisterCursorMove(const Vector2Int position)
    {
        cursorPosition = position;
        if (cursorShown && cursorPosition != lastCursorPosition) GetCursorMoveDispatcher().DispatchEvent(GetCursorPosition());
    }

    /* --- SETTER METHODS --- */

    void Win32CursorManager::SetCursorVisibility(const bool visible)
    {
        if (visible == cursorShown) return;
        cursorShown = visible;

        // NOTE: ShowCursor() is not used, as it hides the cursor everywhere (and keeps a count of calls, rather than a state), while it is only to be hidden over this window, which is what WM_SETCURSOR keeps applying
        ApplyCursor();

        // Motion, which happened while switching modes, is not a motion of the cursor
        lastCursorPosition = cursorPosition;
    }

    void Win32CursorManager::SetCursorPosition(const Vector2Int position)
    {
        // NOTE: Cursor positions are in physical pixels (those of the framebuffer), as on the other platforms
        WarpCursor(position);
    }

    /* --- GETTER METHODS --- */

    bool Win32CursorManager::IsCursorVisible() const noexcept
    {
        return cursorShown;
    }

    Vector2Int Win32CursorManager::GetCursorPosition() const noexcept
    {
        // NOTE: Windows reports positions in physical pixels (as the process is DPI-aware), which is what cursor positions are measured in on every platform
        return cursorPosition;
    }

    Vector2 Win32CursorManager::GetCursorDelta() const noexcept
    {
        Vector2 delta = { cursorPosition.x - lastCursorPosition.x, cursorPosition.y - lastCursorPosition.y };

        // Hidden cursor is kept in place, so its motion is inverted, as on the other platforms
        if (!cursorShown) delta *= -1.0f;
        return delta;
    }

    WindowingBackendType Win32CursorManager::GetBackendType() const noexcept
    {
        return WindowingBackendType::Win32;
    }

    /* --- PRIVATE METHODS --- */

    void Win32CursorManager::Update(const bool focused)
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

    void Win32CursorManager::SetWindowSize(const Vector2UInt size)
    {
        windowSize = size;
    }

    void Win32CursorManager::WarpCursor(const Vector2Int physicalPosition)
    {
        // Position is measured upwards from the bottom of the window, while Windows measures it downwards from the top of the screen
        POINT point = { physicalPosition.x, static_cast<int32>(windowSize.y) - physicalPosition.y };
        if (ClientToScreen(window, &point)) SetCursorPos(point.x, point.y);

        lastCursorPosition = physicalPosition;
        cursorPosition = physicalPosition;
    }

    void Win32CursorManager::ApplyCursor() const
    {
        // Cursor's image only changes right away, if it is over the window, and is otherwise set by WM_SETCURSOR, once it gets there
        if (!IsCursorOverClientArea()) return;
        SetCursor(cursorShown ? LoadCursorW(nullptr, MAKEINTRESOURCEW(32512)) : nullptr); // IDC_ARROW
    }

    bool Win32CursorManager::IsCursorOverClientArea() const
    {
        POINT point = { };
        if (!GetCursorPos(&point) || WindowFromPoint(point) != window) return false;

        RECT clientRect = { };
        if (!GetClientRect(window, &clientRect) || !ScreenToClient(window, &point)) return false;

        return PtInRect(&clientRect, point);
    }

}
