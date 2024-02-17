//
// Created by Nikolay Kanchevski on 10.20.2023.
//

#include "Win32CursorManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Win32CursorManager::Win32CursorManager(HWND window, const CursorManagerCreateInfo &createInfo)
        : CursorManager(createInfo), window(window)
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

    /* --- SETTER METHODS --- */

    void Win32CursorManager::SetCursorPosition(const Vector2 &position)
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

    void Win32CursorManager::ShowCursor()
    {
        // Show cursor
        ::ShowCursor(true);
        cursorHidden = false;
    }

    void Win32CursorManager::HideCursor()
    {
        ::ShowCursor(false);
        cursorHidden = true;
        justHidCursor = true;
    }

    /* --- PRIVATE METHODS  --- */

    void Win32CursorManager::Update()
    {
        lastCursorPosition = cursorPosition;
    }

    void Win32CursorManager::UpdateEnd()
    {
        if (!cursorHidden) return;

        // Get window dimensions
        RECT rect = { };
        GetClientRect(window, &rect);

        // Manually re-center cursor after all window events have been polled (so none more would be handled and SetWindowCursorPosition() produces one)
        const Vector2Int win32Center = Vector2Int(rect.left + rect.right, rect.top + rect.bottom) / 2;
        if (static_cast<Vector2Int>(cursorPosition) != win32Center)
        {
            // Move cursor to center
            SetCursorPosition(win32Center);

            // Update mouse position
            lastCursorPosition = cursorPosition;
            cursorPosition = { win32Center.x, win32Center.y };

            // Reset mouse delta when re-centering for the first time
            if (justHidCursor)
            {
                lastCursorPosition = cursorPosition;
                justHidCursor = false;
            }
        }
    }

    /* --- GETTER METHODS --- */

    bool Win32CursorManager::IsCursorHidden() const
    {
        return cursorHidden;
    }

    Vector2 Win32CursorManager::GetCursorPosition() const
    {
        return cursorPosition;
    }

    float32 Win32CursorManager::GetHorizontalDelta() const
    {
        float32 delta = cursorPosition.x - lastCursorPosition.x;
        if (cursorHidden) delta *= -1;
        return delta;
    }

    float32 Win32CursorManager::GetVerticalDelta() const
    {
        float32 delta = cursorPosition.y - lastCursorPosition.y;
        if (cursorHidden) delta *= -1;
        return delta;
    }

    /* --- EVENTS --- */

    void Win32CursorManager::MouseMoveMessage(const UINT, const WPARAM, const LPARAM lParam)
    {
        // Get window's dimensions
        RECT rect = { };
        GetClientRect(window, &rect);

        // Get and save position within the window
        cursorPosition = { LOWORD(lParam), rect.bottom - rect.top - HIWORD(lParam) };

        // Dispatch events
        if (!cursorHidden && cursorPosition != lastCursorPosition) GetCursorMoveDispatcher().DispatchEvent(cursorPosition);
    }

}