//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include <glm/common.hpp>
#include "Cursor.h"

namespace Sierra::Engine::Classes
{
    glm::vec2 Cursor::lastCursorPosition;
    glm::vec2 Cursor::cursorPosition;
    glm::vec2 Cursor::cursorOffset;
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
    void Cursor::SetCursorPosition(const glm::vec2 newPosition)
    {
        glfwSetCursorPos(VulkanCore::GetCoreWindow(), newPosition.x, newPosition.y);
        CursorPositionCallback(VulkanCore::GetCoreWindow(), newPosition.x, newPosition.y);

        ResetCursorOffset();
    }

    void Cursor::SetCursorPositionNormalized(const glm::vec2 newPosition)
    {
        glm::vec2 nonNormalizedPosition = { newPosition.x * (float) VulkanCore::GetWindow()->GetWidth(), newPosition.y * (float) VulkanCore::GetWindow()->GetHeight() };
        SetCursorPosition(nonNormalizedPosition);
    }

    void Cursor::CenterCursor()
    {
        SetCursorPosition({ VulkanCore::GetWindow()->GetWidth() / 2, VulkanCore::GetWindow()->GetHeight() / 2 });
    }

    void Cursor::ShowCursor(const bool centerCursor)
    {
        cursorShown = true;
        glfwSetInputMode(VulkanCore::GetCoreWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        if (centerCursor) CenterCursor();

        ResetCursorOffset();
    }

    void Cursor::HideCursor(const bool centerCursor)
    {
        cursorShown = false;
        glfwSetInputMode(VulkanCore::GetCoreWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (centerCursor) CenterCursor();

        ResetCursorOffset();
    }

    void Cursor::SetCursorVisibility(const bool showCursor)
    {
        showCursor ? ShowCursor() : HideCursor();
    }

    void Cursor::ResetCursorOffset()
    {
        lastCursorPosition = cursorPosition;
        cursorOffset = { 0, 0 };
    }

    /* --- CALLBACKS --- */
    void Cursor::CursorPositionCallback(GLFWwindow *windowPtr, double xPosition, double yPosition)
    {
        lastCursorPosition = cursorPosition;

        yPosition = -(yPosition - VulkanCore::GetWindow()->GetHeight());
        cursorPosition = glm::vec2(xPosition, yPosition);

        cursorOffset = glm::vec2(lastCursorPosition.x - cursorPosition.x, lastCursorPosition.y - cursorPosition.y);
        cursorPositionSet = true;
    }

    /* --- PRIVATE METHODS --- */

    glm::vec2 Cursor::GetGlfwCursorPosition()
    {
        double xPosition, yPosition;
        glfwGetCursorPos(VulkanCore::GetCoreWindow(), &xPosition, &yPosition);
        return { xPosition, yPosition };
    }
}