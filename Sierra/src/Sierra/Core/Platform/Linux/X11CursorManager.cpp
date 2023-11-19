//
// Created by Nikolay Kanchevski on 10.28.23.
//

#include "X11CursorManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    X11CursorManager::X11CursorManager(const X11CursorManagerCreateInfo &createInfo)
        : CursorManager(createInfo), window(createInfo.window), x11Context(createInfo.x11Context)
    {

    }

    void X11CursorManager::OnWindowInitialize()
    {
        // Retrieve initial cursor position (not done in constructor, as X11 is async and window may not be initialized there)
        cursorPosition = x11Context.GetWindowCursorPosition(window);
        cursorPosition.y = static_cast<int32>(x11Context.GetWindowSize(window).y) - cursorPosition.y;
        lastCursorPosition = cursorPosition;
    }

    /* --- POLLING METHODS --- */

    void X11CursorManager::OnUpdate()
    {
        lastCursorPosition = cursorPosition;
    }

    void X11CursorManager::OnUpdateEnd()
    {
        if (!cursorHidden || !x11Context.IsWindowFocused(window)) return;

        // Manually re-center cursor after all window events have been polled (so none more would be handled and SetWindowCursorPosition() produces one)
        const Vector2Int x11Center = static_cast<Vector2Int>(x11Context.GetWindowSize(window)) / 2;
        if (cursorPosition != x11Center)
        {
            // Move cursor to center
            x11Context.SetWindowCursorPosition(window, x11Center);

            // Update mouse position
            lastCursorPosition = cursorPosition;
            cursorPosition = x11Center;

            // Reset mouse delta when re-centering for the first time
            if (justHidCursor)
            {
                lastCursorPosition = cursorPosition;
                justHidCursor = false;
            }
        }
    }

    /* --- SETTER METHODS --- */

    void X11CursorManager::SetCursorPosition(const Vector2 &position)
    {
        // Get X11-suited position and move cursor
        const Vector2Int x11Position = { position.x, static_cast<int32>(x11Context.GetWindowSize(window).y) - static_cast<int32>(position.y) };
        x11Context.SetWindowCursorPosition(window, x11Position);

        // Reset mouse delta
        lastCursorPosition = cursorPosition;
        cursorPosition = position;
    }

    void X11CursorManager::ShowCursor()
    {
        cursorHidden = false;
        x11Context.ShowWindowCursor(window);
    }

    void X11CursorManager::HideCursor()
    {
        cursorHidden = true;
        justHidCursor = true;
        x11Context.HideWindowCursor(window);
    }

    /* --- GETTER METHODS --- */

    Vector2 X11CursorManager::GetCursorPosition()
    {
        return cursorPosition;
    }

    float32 X11CursorManager::GetHorizontalDelta()
    {
        float32 delta = static_cast<float32>(cursorPosition.x) - static_cast<float32>(lastCursorPosition.x);
        if (cursorHidden) delta *= -1;
        return delta;
    }

    float32 X11CursorManager::GetVerticalDelta()
    {
        float32 delta = static_cast<float32>(cursorPosition.y) - static_cast<float32>(lastCursorPosition.y);
        if (cursorHidden) delta *= -1;
        return delta;
    }

    bool X11CursorManager::IsCursorHidden()
    {
        return cursorHidden;
    }

    /* --- EVENTS --- */

    void X11CursorManager::MotionNotifyEvent(const XEvent &event)
    {
        const Vector2Int newPosition = { event.xmotion.x, x11Context.GetWindowSize(window).y - event.xmotion.y };
        if (cursorPosition == newPosition) return;

        cursorPosition = newPosition;
        if (!cursorHidden) GetCursorMoveDispatcher().DispatchEvent(cursorPosition);
    }

}