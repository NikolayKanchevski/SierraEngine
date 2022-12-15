//
// Created by Nikolay Kanchevski on 27.09.22.
//

#pragma once

#define GLFW_INCLUDE_VULKAN

#include <string>
#include <GLFW/glfw3.h>

namespace Sierra::Core::Rendering
{
    /// @brief Window manager class. Wraps around a "core" GLFW window and extends its capabilities.
    class Window
    {
    public:

        /* --- CONSTRUCTORS --- */

        /// @brief Creates a new window without the need of setting its size. It will automatically be 800x600 or,
        /// if setMaximized, as big as it can be on your display.
        /// @param givenTitle What the givenTitle / tag of the window should be.
        /// @param setMaximized A bool indicating whether the window should use all the space on your screen and start setMaximized.
        /// @param setResizable Whether the window is going to be setResizable or not.
        /// @param setFocusRequirement Whether the window requires to be focused in order to draw and handle events.
        Window(std::string  givenTitle, bool setMaximized, bool setResizable, bool setFocusRequirement);

        /* --- POLLING METHODS --- */

        /// @brief Does drawing and required GLFW updates. Only gets executed if the window is not minimised and is focused if required to be.
        void Update();

        /* --- SETTER METHODS --- */

        /// @brief Sets the title (tag) of the window
        void SetTitle(const std::string& givenTitle);

        /// @brief Shows the window after startup, or manually hiding it. @see Hide()
        void Show();

        /// @brief Hides the window completely from the user. Removes it from the task bar and is not visible.
        void Hide();

        /// @brief Sets the transparency (opacity) of the window
        void SetOpacity(float givenOpacity);

        // void SetIcon():

        /* --- GETTER METHODS --- */
        /// @brief A pointer to the core GLFW window.
        [[nodiscard]] inline GLFWwindow* GetCoreWindow()
        { return this->glfwWindow; };

        /// @brief Gets the width of the window.
        [[nodiscard]] inline int GetWidth() const
        { return this->width; };

        /// @brief Gets the height of the window.
        [[nodiscard]] inline int GetHeight() const
        { return this->height; };

        /// @brief Returns the title displayed at the top of the window.
        [[nodiscard]] inline std::string GetTitle() const
        { return this->title; };

        /// @brief Checks whether the window is closed.
        [[nodiscard]] inline bool IsClosed() const
        { return glfwWindowShouldClose(glfwWindow); };

        /// @brief Checks whether the window is minimized and is not shown.
        [[nodiscard]] inline bool IsMinimized() const
        { return this->minimized; };

        /// @brief Checks whether the window is maximised (uses the whole screen).
        [[nodiscard]] inline bool IsMaximized() const
        { return this->maximized; };

        /// @brief Checks whether the window is focused (is the one handling input currently).
        [[nodiscard]] inline bool IsFocused() const
        { return this->focused; };

        /// @brief Checks whether the window is hidden from the user.
        [[nodiscard]] inline bool IsHidden() const
        { return this->hidden; };

        /// @brief Checks if the window requires focus for it to be updated.
        [[nodiscard]] inline bool IsFocusRequired() const
        { return this->REQUIRE_FOCUS; }

        /// @brief Checks if the window has been resized. Only true for one frame after every resize.
        [[nodiscard]] inline bool IsResized()
        {
            bool result = false;
            if (this->resized)
            {
                result = true;
                this->resized = false;
            }

            return result;
        }

        /// @brief Returns the current opacity of the window.
        [[nodiscard]] inline float GetOpacity() const
        { return this->opacity; };

        /* --- DESTRUCTOR --- */
        ~Window();
        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;
    private:
        GLFWwindow *glfwWindow;
        uint32_t xPosition;
        uint32_t yPosition;

        int width, height;
        std::string title;

        bool closed = false, minimized = false, maximized = false, focused = false, hidden = false, resized = false, resizeSet = false;
        float opacity = 0.0f;

        const bool RESIZABLE = false;
        const bool REQUIRE_FOCUS = false;

        struct {
            int xPosition; int yPosition; int width; int height;
        } monitor;

        void InitWindow();

        void SetCallbacks();

        void RetrieveMonitorData();

        /* --- CALLBACKS --- */
        static void GlfwErrorCallback(int errorCode, const char* description);

        static void WindowResizeCallback(GLFWwindow* windowPtr, int newWidth, int newHeight);

        static void WindowFocusCallback(GLFWwindow* windowPtr, int focused);

        static void WindowMinimizeCallback(GLFWwindow* windowPtr, int minimized);

        static void WindowMaximizeCallback(GLFWwindow* windowPtr, int maximized);

        static Window* GetGlfwWindowParentClass(GLFWwindow* windowPtr);
    };
}
