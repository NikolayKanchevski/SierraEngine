//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include "../../Core/Rendering/Window.h"

namespace Sierra::Engine
{
    /// @brief A class to interface with the cursor.
    class Cursor
    {
    public:
        /* --- POLLING METHODS --- */
        static void Start();
        static void Update();

        /* --- SETTER METHODS --- */

        /// @brief Moves the cursor to a given position.
        /// @param newPosition Where exactly to put the cursor.
        static void SetCursorPosition(Vector2 newPosition);

        /// @brief Moves the cursor to a given normalized position (a vec2 where both X and Y are between 0.0 and 1.0).
        /// @param newPosition Where exactly to put the cursor (as a normalized vector).
        static void SetCursorPositionNormalized(Vector2 newPosition);

        /// @brief Puts the cursor in the center of the window.
        static void CenterCursor();

        /// @brief Shows the cursor if hidden.
        /// @param centerCursor Whether to also center the cursor when showing it. True by default.
        static void ShowCursor(bool centerCursor = true);

        /// @brief Hides the cursor if shown.
        /// @param centerCursor Whether to also center the cursor when hiding it. True by default.
        static void HideCursor(bool centerCursor = true);

        /// @brief Shows or hides the cursor depending on the passed bool.
        /// @param showCursor Whether to hide or show the cursor.
        static void SetCursorVisibility(bool showCursor);

        /// @brief Resets the cursor position offset. Should only be called from the Window class.
        static void ResetCursorOffset();

        /* --- GETTER METHODS --- */
        /// @brief Returns the position on the cursor.
        [[nodiscard]] inline static Vector2 GetCursorPosition() { return cursorPosition; }

        /// @brief Returns the normalized position of the cursor where both X and Y is a value between 0 and 1.
        [[nodiscard]] inline static Vector2 GetCursorPositionNormalized();

        /// @brief Returns how much the mouse has been moved horizontally since last frame.
        [[nodiscard]] inline static float GetHorizontalCursorOffset() { return cursorOffset.x; }

        /// @brief Returns how much the mouse has been moved vertically since last frame.
        [[nodiscard]] inline static float  GetVerticalCursorOffset() { return cursorOffset.y; }

        /// @brief Tells whether the cursor is visible on the screen.
        [[nodiscard]] inline static bool IsCursorShown() { return cursorShown; }

        /// @brief Gets the raw (unformatted) cursor position returned by GLFW. Only used for ImGui.
        [[nodiscard]] static Vector2 GetGlfwCursorPosition();

        /* --- CALLBACKS --- */
        static void CursorPositionCallback(GLFWwindow* windowPtr, double xPosition, double yPosition);

    private:
        static inline bool cursorPositionSet = false;
        static inline Vector2 lastCursorPosition = { 0.0f, 0.0f };
        static inline Vector2 cursorPosition = { 0.0f, 0.0f };

        static inline Vector2 cursorOffset = { 0.0f, 0.0f };
        static inline bool cursorShown = true;
    };

}