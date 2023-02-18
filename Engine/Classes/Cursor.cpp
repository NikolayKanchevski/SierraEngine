//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "Cursor.h"

namespace Sierra::Engine::Classes
{
    Vector2 Cursor::lastCursorPosition;
    Vector2 Cursor::cursorPosition;
    Vector2 Cursor::cursorOffset;
    bool Cursor::cursorShown = true;

    bool Cursor::cursorPositionSet;

    /* --- POLLING METHODS --- */
    void Cursor::Start()
    {
        ResetCursorOffset();
    }

    void Cursor::Update()
    {
        if (cursorPositionSet) cursorPositionSet = false;
        else
        {
            ResetCursorOffset();
            return;
        }
    }

    /* --- SETTER METHODS --- */
    void Cursor::SetCursorPosition(const Vector2 newPosition)
    {
        glfwSetCursorPos(Window::GetCurrentlyFocusedWindow()->GetCoreWindow(), newPosition.x, newPosition.y);
        CursorPositionCallback(Window::GetCurrentlyFocusedWindow()->GetCoreWindow(), newPosition.x, newPosition.y);

        ResetCursorOffset();
    }

    void Cursor::SetCursorPositionNormalized(const Vector2 newPosition)
    {
        Vector2 nonNormalizedPosition = { newPosition.x * static_cast<float>(Window::GetCurrentlyFocusedWindow()->GetWidth()), newPosition.y * static_cast<float>(Window::GetCurrentlyFocusedWindow()->GetHeight()) };
        SetCursorPosition(nonNormalizedPosition);
    }

    void Cursor::CenterCursor()
    {
        SetCursorPosition({ Window::GetCurrentlyFocusedWindow()->GetWidth() / 2, Window::GetCurrentlyFocusedWindow()->GetHeight() / 2 });
    }

    void Cursor::ShowCursor(const bool centerCursor)
    {
        cursorShown = true;
        glfwSetInputMode(Window::GetCurrentlyFocusedWindow()->GetCoreWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        if (centerCursor) CenterCursor();

        ResetCursorOffset();
    }

    void Cursor::HideCursor(const bool centerCursor)
    {
        cursorShown = false;
        glfwSetInputMode(Window::GetCurrentlyFocusedWindow()->GetCoreWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (centerCursor) CenterCursor();

        ResetCursorOffset();
    }

    void Cursor::SetCursorVisibility(const bool showCursor)
    {
        showCursor ? ShowCursor() : HideCursor();
    }

    void Cursor::ResetCursorOffset()
    {
        cursorOffset = { 0, 0 };
        lastCursorPosition = cursorPosition;
        cursorPositionSet = true;
    }

    /* --- CALLBACKS --- */
    void Cursor::CursorPositionCallback(GLFWwindow *windowPtr, double xPosition, double yPosition)
    {
        lastCursorPosition = cursorPosition;

        if (Window::IsFocusedWindowPresent()) yPosition = -(yPosition - Window::GetCurrentlyFocusedWindow()->GetHeight());
        cursorPosition = Vector2(xPosition, yPosition);

        cursorOffset = Vector2(-(lastCursorPosition.x - cursorPosition.x), -(lastCursorPosition.y - cursorPosition.y));
        cursorPositionSet = true;
    }

    /* --- PRIVATE METHODS --- */

    Vector2 Cursor::GetGlfwCursorPosition()
    {
        if (!Window::IsFocusedWindowPresent())
        {
            ASSERT_WARNING("Trying to get the GLFW cursor position within a non-focused window. Since this is not possible and a value of { 0, 0 } has been returned");
            return { 0, 0 };
        }

        double xPosition, yPosition;
        glfwGetCursorPos(Window::GetCurrentlyFocusedWindow()->GetCoreWindow(), &xPosition, &yPosition);
        return { xPosition, yPosition };
    }
}