//
// Created by Nikolay Kanchevski on 10.20.2023.
//

#include "Win32CursorManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Win32CursorManager::Win32CursorManager(const CursorManagerCreateInfo &createInfo)
        : CursorManager(createInfo)
    {

    }

    /* --- POLLING METHODS  --- */

    void Win32CursorManager::OnUpdate()
    {
        lastCursorPosition = cursorPosition;
    }

    /* --- SETTER METHODS --- */

    void Win32CursorManager::SetCursorPosition(const Vector2 &position)
    {
        // Get window's dimensions
        RECT rect;
        GetWindowRect(GetForegroundWindow(), &rect);

        // Set mouse position
        SetCursorPos(rect.left + static_cast<LONG>(position.x), rect.bottom - static_cast<LONG>(position.y));
    }

    void Win32CursorManager::ShowCursor()
    {
        cursorShown = true;
        ::ShowCursor(true);

        // Allow cursor to move
        ClipCursor(nullptr);
    }

    void Win32CursorManager::HideCursor()
    {
        cursorShown = false;
        ::ShowCursor(false);

        // Get window dimensions
        RECT constraint;
        GetWindowRect(GetForegroundWindow(), &constraint);

        // Calculate center X
        constraint.left = (constraint.left + constraint.right) / 2;
        constraint.right = constraint.left;

        // Calculate center Y
        constraint.bottom = (constraint.top + constraint.bottom) / 2;
        constraint.top = constraint.bottom;

        // Stick cursor to center
        ClipCursor(&constraint);
    }

    /* --- GETTER METHODS --- */

    Vector2 Win32CursorManager::GetCursorPosition()
    {
        return cursorPosition;
    }

    bool Win32CursorManager::IsCursorShown()
    {
        return cursorShown;
    }

    bool Win32CursorManager::IsCursorHidden()
    {
        return !cursorShown;
    }

    float32 Win32CursorManager::GetHorizontalDelta()
    {
        return -(lastCursorPosition.x - cursorPosition.x);
    }

    float32 Win32CursorManager::GetVerticalDelta()
    {
        return -(lastCursorPosition.y - cursorPosition.y);
    }

    /* --- EVENTS --- */

    void Win32CursorManager::MouseMoveMessage(const UINT, const WPARAM, const LPARAM lParam)
    {
        // Get window's dimensions
        RECT framebufferRect;
        GetClientRect(GetForegroundWindow(), &framebufferRect);

        // Get and save position within the window and flip to +Y coordinates
        cursorPosition = { LOWORD(lParam), framebufferRect.bottom - framebufferRect.top - HIWORD(lParam) };

        // Mouse messages with same position are still sent when cursor is clipped
        if (cursorPosition == lastCursorPosition) return;

        // Dispatch events
        GetCursorMoveDispatcher().DispatchEvent(cursorPosition);
    }
}