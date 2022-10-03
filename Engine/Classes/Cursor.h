//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include <glm/vec2.hpp>
#include "../../Core/Rendering/Vulkan/VulkanCore.h"

using namespace Sierra::Core::Rendering::Vulkan;

namespace Sierra::Engine::Classes
{
    /// \brief A class to interface with the cursor.
    class Cursor
    {
    public:
        /* --- POLLING METHODS --- */
        static void Update();

        /* --- SETTER METHODS --- */

        /// \brief Moves the cursor to a given position.
        /// @param newPosition Where exactly to put the cursor.
        static void SetCursorPosition(glm::vec2 newPosition);

        /// \brief Moves the cursor to a given normalized position (a vec2 where both X and Y are between 0.0 and 1.0).
        /// @param newPosition Where exactly to put the cursor (as a normalized vector).
        static void SetCursorPositionNormalized(glm::vec2 newPosition);

        /// \brief Puts the cursor in the center of the window.
        static void CenterCursor();

        /// \brief Shows the cursor if hidden.
        /// @param centerCursor Whether to also center the cursor when showing it. True by default.
        static void ShowCursor(bool centerCursor = true);

        /// \brief Hides the cursor if shown.
        /// @param centerCursor Whether to also center the cursor when hiding it. True by default.
        static void HideCursor(bool centerCursor = true);

        /// \brief Shows or hides the cursor depending on the passed bool.
        /// @param showCursor Whether to hide or show the cursor.
        static void SetCursorVisibility(bool showCursor);

        /// \brief Resets the cursor position offset. Should only be called from the Window class.
        static void ResetCursorOffset();

        /* --- GETTER METHODS --- */
        /// \brief Returns the position on the cursor.
        [[nodiscard]] inline static glm::vec2 GetCursorPosition() { return cursorPosition; }

        /// \brief Returns the normalized position of the cursor where both X and Y is a value between 0 and 1.
        [[nodiscard]] inline static glm::vec2 GetCursorPositionNormalized() { return {cursorPosition.x / (float) VulkanCore::window->GetWidth(), cursorPosition.y / (float) VulkanCore::window->GetHeight()}; }

        /// \brief Returns how much the mouse has been moved horizontally since last frame.
        [[nodiscard]] inline static float GetHorizontalCursorOffset() { return cursorOffset.x; }

        /// \brief Returns how much the mouse has been moved vertically since last frame.
        [[nodiscard]] inline static float  GetVerticalCursorOffset() { return cursorOffset.x; }

        /// \brief Tells whether the cursor is visible on the screen.
        [[nodiscard]] inline static bool IsCursorShown() { return cursorShown; }

        /// \brief Gets the raw (unformatted) cursor position returned by GLFW. Only used for ImGui.
        [[nodiscard]] static glm::vec2 GetGlfwCursorPosition();

        /* --- CALLBACKS --- */
        static void CursorPositionCallback(GLFWwindow* windowPtr, double xPosition, double yPosition);

    private:
        static glm::vec2 lastCursorPosition;
        static glm::vec2 cursorPosition;
        static glm::vec2 cursorOffset;
        static bool cursorShown;

        static bool cursorPositionSet;
    };

}