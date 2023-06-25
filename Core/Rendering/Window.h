//
// Created by Nikolay Kanchevski on 27.09.22.
//

#pragma once

#include "../Version.h"

using namespace Sierra::Engine;

namespace Sierra::Core::Rendering
{
    /// @brief Window manager class. Wraps around a "core" GLFW window and extends its capabilities.
    class Window
    {
    public:

        /// @param givenTitle What the title / tag of the window should be.
        /// @param startMaximized A bool indicating whether the window should use all the space on your screen upon start.
        /// @param isResizable Whether the window is going to be setResizable or not.
        /// @param isFocusRequired Whether the window requires to be focused in order to draw and handle events.
        struct WindowCreateInfo
        {
            String givenTitle = FORMAT_STRING("v{0}.{1}.{2} {3}", Version::MAJOR, Version::MINOR, Version::PATCH, Version::RELEASE_TYPE);
            bool startMaximized = true;
            bool isResizable = true;
            bool isFocusRequired = true;
        };

        /* --- CONSTRUCTORS --- */

        static UniquePtr<Window> Create(const WindowCreateInfo &createInfo);
        explicit Window(const WindowCreateInfo &createInfo);

        /* --- POLLING METHODS --- */

        /// @brief Does drawing and required GLFW updates. Only gets executed if the window is not minimised and is focused if required to be.
        void Update();

        /* --- SETTER METHODS --- */

        /// @brief Sets the title (tag) of the window
        void SetTitle(const String& givenTitle);

        /// @brief Shows the window after startup, or manually hiding it. @see Hide()
        void Show();

        /// @brief Hides the window completely from the user. Removes it from the task bar and is not visible.
        void Hide();

        /// @brief Sets the transparency (opacity) of the window
        void SetOpacity(float givenOpacity);

        /* --- GETTER METHODS --- */

        /// @brief A pointer to the core GLFW window.
        [[nodiscard]] inline GLFWwindow* GetCoreWindow()
        { return glfwWindow; };

        /// @brief Gets the width of the window.
        [[nodiscard]] inline int GetWidth() const
        { return width; };

        /// @brief Gets the height of the window.
        [[nodiscard]] inline int GetHeight() const
        { return height; };

        /// @brief Returns the title displayed at the top of the window.
        [[nodiscard]] inline String GetTitle() const
        { return title; };

        /// @brief Returns the corresponding window surface. Should only be used for inside the core functionalities.
        [[nodiscard]] inline VkSurfaceKHR GetSurface() const
        { return surface; }

        /// @brief Checks whether the window is closed.
        [[nodiscard]] inline bool IsClosed() const
        { return glfwWindowShouldClose(glfwWindow); };

        /// @brief Checks whether the window is minimized and is not shown.
        [[nodiscard]] inline bool IsMinimized() const
        { return minimized; };

        /// @brief Checks whether the window is maximised (uses the whole screen).
        [[nodiscard]] inline bool IsMaximized() const
        { return maximized; };

        /// @brief Checks whether the window is focused (is the one handling input currently).
        [[nodiscard]] inline bool IsFocused() const
        { return focused; };

        /// @brief Checks whether the window is hidden from the user.
        [[nodiscard]] inline bool IsHidden() const
        { return hidden; };

        /// @brief Checks if the window requires focus for it to be updated.
        [[nodiscard]] inline bool IsFocusRequired() const
        { return requireFocus; }

        /// @brief Checks if the window has been resized. Only true for one frame after every resize.
        [[nodiscard]] inline bool IsResized()
        {
            bool result = false;
            if (resized)
            {
                result = true;
                resized = false;
            }

            return result;
        }

        /// @brief Returns the current opacity of the window.
        [[nodiscard]] inline float GetOpacity() const
        { return opacity; };

        /// @brief Returns a bool indicating whether there currently is a focused window.
        [[nodiscard]] static inline bool IsFocusedWindowPresent() { return currentlyFocusedWindow != nullptr; }

        /// @brief Returns the currently focused window. Make sure to always check if there is one by calling IsFocusedWindowPresent() before calling this method!
        [[nodiscard]] static inline Window* GetCurrentlyFocusedWindow() { ASSERT_WARNING_IF(currentlyFocusedWindow == nullptr, "No windows are currently focused. Make sure to first check if return value is not null"); return currentlyFocusedWindow; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Window);

    private:
        GLFWwindow *glfwWindow;
        VkSurfaceKHR surface;

        uint xPosition;
        uint yPosition;

        String title;
        int width = 1300, height = 800;

        float opacity = 0.0f;
        bool closed = false, minimized = false, maximized = false, focused = false, hidden = false, resized = false, resizeSet = false, resizable = false, requireFocus = false;

        /* --- SETTER METHODS --- */
        void Initialize();
        void CreateSurface();
        void SetCallbacks();

        /* --- CALLBACKS --- */
        static void GlfwErrorCallback(int errorCode, const char* description);
        static void WindowResizeCallback(GLFWwindow* windowPtr, int newWidth, int newHeight);
        static void WindowFocusCallback(GLFWwindow* windowPtr, int focused);
        static void WindowMinimizeCallback(GLFWwindow* windowPtr, int minimized);
        static void WindowMaximizeCallback(GLFWwindow* windowPtr, int maximized);
        static Window* GetGlfwWindowParentClass(GLFWwindow* windowPtr);

        static inline Window* currentlyFocusedWindow;
    };
}
