//
// Created by Nikolay Kanchevski on 10.20.2023.
//

#include "Win32CursorManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Win32CursorManager::Win32CursorManager(HWND window)
        : CursorManager(), window(window)
    {
        // Get cursor position
        POINT cursorPoint = { };
        GetCursorPos(&cursorPoint);
        ScreenToClient(window, &cursorPoint);

        // Get window's dimensions
        RECT rect = { };
        GetClientRect(window, &rect);

        // Reset mouse delta
        cursorPosition = { cursorPoint.x, rect.bottom - rect.top - cursorPoint.y };
        lastCursorPosition = cursorPosition;
    }

    /* --- POLLING METHODS --- */

    void Win32CursorManager::RegisterCursorMove(const Vector2Int position)
    {
        cursorPosition = position;
        if (cursorShown && cursorPosition != lastCursorPosition) GetCursorMoveDispatcher().DispatchEvent(cursorPosition);
    }

    /* --- SETTER METHODS --- */

    void Win32CursorManager::SetCursorVisibility(const bool visible)
    {
        // Show cursor
        ::ShowCursor(visible);
        cursorShown = visible;
        justHidCursor = !visible;
    }

    void Win32CursorManager::SetCursorPosition(const Vector2Int position)
    {
        // Get window's dimensions
        RECT rect = { };
        GetClientRect(window, &rect);

        // Get window's position
        POINT windowPosition = { };
        ClientToScreen(window, &windowPosition);

        // Set mouse position
        SetCursorPos(windowPosition.x + static_cast<LONG>(position.x), windowPosition.y + (rect.bottom - rect.top - static_cast<LONG>(position.y)));

        // Reset mouse delta
        lastCursorPosition = cursorPosition;
        cursorPosition = position;
    }

    /* --- GETTER METHODS --- */

    bool Win32CursorManager::IsCursorVisible() const noexcept
    {
        return cursorShown;
    }

    Vector2Int Win32CursorManager::GetCursorPosition() const noexcept
    {
        return cursorPosition;
    }

    Vector2 Win32CursorManager::GetCursorDelta() const noexcept
    {
        Vector2 delta = { cursorPosition.x - lastCursorPosition.x, cursorPosition.y - lastCursorPosition.y };
        if (!cursorShown) delta *= -1;
        return delta;
    }

    WindowingBackendType Win32CursorManager::GetBackendType() const noexcept
    {
        return WindowingBackendType::Win32;
    }

    /* --- PRIVATE METHODS --- */

    void Win32CursorManager::Update()
    {
        if (GetForegroundWindow() != window) return;
        lastCursorPosition = cursorPosition;

        // Get window dimensions
        RECT rect = { };
        GetClientRect(window, &rect);

        const Vector2Int center = Vector2Int(rect.left + rect.right, rect.top + rect.bottom) / 2;
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