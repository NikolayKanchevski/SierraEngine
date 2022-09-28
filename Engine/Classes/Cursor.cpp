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
    bool Cursor::cursorShown;

    bool Cursor::cursorPositionSet;

    /* --- POLLING METHODS --- */
    void Cursor::Update()
    {
        if (cursorPositionSet)
        {
            cursorPositionSet = false;
            return;
        }

        ResetCursorOffset();
    }

    /* --- SETTER METHODS --- */
    void Cursor::SetCursorPosition(const glm::vec2 newPosition)
    {
        glfwSetCursorPos(VulkanCore::glfwWindow, newPosition.x, newPosition.y);
        CursorPositionCallback(VulkanCore::glfwWindow, newPosition.x, newPosition.y);

        ResetCursorOffset();
    }

    void Cursor::SetCursorPositionNormalized(const glm::vec2 newPosition)
    {
        glm::vec2 nonNormalizedPosition = { newPosition.x * (float) VulkanCore::window->GetWidth(), newPosition.y * (float) VulkanCore::window->GetHeight() };
        SetCursorPosition(nonNormalizedPosition);
    }

    void Cursor::CenterCursor()
    {
        SetCursorPosition({ VulkanCore::window->GetWidth() / 2, VulkanCore::window->GetHeight() / 2 });
    }

    void Cursor::ShowCursor(const bool centerCursor)
    {
        cursorShown = true;
        glfwSetInputMode(VulkanCore::glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        if (centerCursor) CenterCursor();

        ResetCursorOffset();
    }

    void Cursor::HideCursor(const bool centerCursor)
    {
        cursorShown = false;
        glfwSetInputMode(VulkanCore::glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (centerCursor) CenterCursor();

        ResetCursorOffset();
    }

    void Cursor::SetCursorVisibility(const bool showCursor)
    {
        showCursor ? ShowCursor() : HideCursor();
    }

    /* --- CALLBACKS --- */
    void Cursor::CursorPositionCallback(GLFWwindow *windowPtr, double xPosition, double yPosition)
    {
        lastCursorPosition = cursorPosition;

        yPosition = -(yPosition - VulkanCore::window->GetHeight());
        cursorPosition = glm::vec2(xPosition, yPosition);

        cursorOffset = glm::vec2(lastCursorPosition.x - cursorPosition.x, lastCursorPosition.y - cursorPosition.y);
        cursorPositionSet = true;
    }

    /* --- PRIVATE METHODS --- */
    void Cursor::ResetCursorOffset()
    {
        lastCursorPosition = cursorPosition;
        cursorOffset = glm::vec2(0);
    }

    glm::vec2 Cursor::GetGlfwCursorPosition()
    {
        double xPosition, yPosition;
        glfwGetCursorPos(VulkanCore::glfwWindow, &xPosition, &yPosition);
        return { xPosition, yPosition };
    }
}