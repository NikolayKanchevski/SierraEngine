//
// Created by Nikolay Kanchevski on 27.09.22.
//

#include <iostream>
#include <utility>
#include "Window.h"
#include "../../Engine/Classes/Stopwatch.h"
#include "../../Engine/Classes/Cursor.h"
#include "Vulkan/VulkanDebugger.h"
#include "Vulkan/VulkanCore.h"

using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering {
    void Window::Update()
    {
        glfwPollEvents();

        closed = glfwWindowShouldClose(glfwWindow);

        if (REQUIRE_FOCUS && !focused) return;

        if (minimized || hidden) return;

//        vulkanRenderer?.Update();
    }

    void Window::SetTitle(const std::string& givenTitle)
    {
        this->title = givenTitle;
        glfwSetWindowTitle(glfwWindow, givenTitle.c_str());
    }

    void Window::Show()
    {
        this->hidden = false;
        glfwShowWindow(glfwWindow);
    }

    void Window::Hide()
    {
       this->hidden = true;
        glfwHideWindow(glfwWindow);
    }

    void Window::SetOpacity(const float givenOpacity)
    {
        this->opacity = givenOpacity;
        glfwSetWindowOpacity(glfwWindow, givenOpacity);
    }

    Window::Window(const std::string& givenTitle, const bool setMaximized, const bool setResizable, const bool setFocusRequirement)
            : REQUIRE_FOCUS(setFocusRequirement), RESIZABLE(setResizable)
    {

        glfwInit();

        #ifdef DEBUG
                Sierra::Engine::Classes::Stopwatch stopwatch;
        #endif

        RetrieveMonitorData();

        this->title = givenTitle;
        this->maximized = setMaximized;

        if (setMaximized)
        {
            this->width = monitor.width;
            this->height = monitor.height;
        }
        else
        {
            this->width = 800;
            this->height = 600;
            this->position = glm::vec2((monitor.width - width) / 2, (monitor.height - height) / 2);
        }

        this->position.x += (float) monitor.xPosition;

        InitWindow();

        #ifdef DEBUG
            Sierra::Core::Rendering::Vulkan::VulkanDebugger::DisplayInfo(
                "Window [" + this->title + "] successfully created! Initialization took: " + std::to_string(stopwatch.GetElapsedMilliseconds()) + "ms"
            );
        #endif
    }

    void Window::InitWindow()
    {
        glfwWindowHint(GLFW_RESIZABLE, RESIZABLE);
        glfwWindowHint(GLFW_CLIENT_API, 0);
        glfwWindowHint(GLFW_MAXIMIZED, maximized);
        glfwWindowHint(GLFW_VISIBLE, 0);

        glfwWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        glfwSetWindowPos(glfwWindow, (int) position.x, (int) position.y);

        Vulkan::VulkanCore::window = this;
        Vulkan::VulkanCore::glfwWindow = glfwWindow;

        glfwSetWindowUserPointer(glfwWindow, this);

        SetCallbacks();
    }

    void Window::SetCallbacks()
    {
        #if DEBUG
            glfwSetErrorCallback(GlfwErrorCallback);
        #endif

        glfwSetWindowSizeCallback(glfwWindow, WindowResizeCallback);

        glfwSetWindowFocusCallback(glfwWindow, WindowFocusCallback);

        glfwSetWindowRefreshCallback(glfwWindow, [](GLFWwindow* windowPtr) { WindowFocusCallback(windowPtr, true); });

        glfwSetWindowIconifyCallback(glfwWindow, WindowMinimizeCallback);

        glfwSetWindowMaximizeCallback(glfwWindow, WindowMaximizeCallback);

//        glfwSetKeyCallback(glfwWindow, Input.KeyboardKeyCallback);

        double xCursorPosition, yCursorPosition;
        glfwGetCursorPos(glfwWindow, &xCursorPosition, &yCursorPosition);
        glfwSetCursorPosCallback(glfwWindow, Cursor::CursorPositionCallback);
        Cursor::SetCursorPosition({ xCursorPosition, yCursorPosition });

//        glfwSetMouseButtonCallback(glfwWindow, Input.MouseButtonCallback);

//        glfwSetScrollCallback(glfwWindow, Input.MouseScrollCallback);
//
//        glfwSetJoystickCallback(Input.JoystickCallback);
    }

    void Window::RetrieveMonitorData()
    {
        auto defaultMonitor = glfwGetPrimaryMonitor();
        glfwGetMonitorWorkarea(defaultMonitor, &monitor.xPosition, &monitor.yPosition, &monitor.width, &monitor.height);
    }

    /* --- CALLBACKS --- */
    void Window::GlfwErrorCallback(int errorCode, const char *description)
    {
        Vulkan::VulkanDebugger::ThrowError("GLFW Error: " + std::string(description) + " (" + std::to_string(errorCode) + ")");
    }

    void Window::WindowResizeCallback(GLFWwindow *windowPtr, int newWidth, int newHeight)
    {
        auto windowObject = GetGlfwWindowParentClass(windowPtr);
        windowObject->width = newWidth;
        windowObject->height = newHeight;

//        if (windowObject.vulkanRenderer == null)
//        {
//            return;
//        }
//
//        windowObject.vulkanRenderer.frameBufferResized = true;
//        windowObject.vulkanRenderer.Update();
//        windowObject.vulkanRenderer.Update();

//        SierraEngine.Engine.Classes.Cursor.ResetCursorOffset();
    }

    void Window::WindowFocusCallback(GLFWwindow *windowPtr, int focused)
    {
        auto windowObject = GetGlfwWindowParentClass(windowPtr);

        windowObject->focused = focused;
        windowObject->minimized = false;

//        SierraEngine.Engine.Classes.Cursor.ResetCursorOffset();
    }

    void Window::WindowMinimizeCallback(GLFWwindow *windowPtr, int minimized)
    {
        auto windowObject = GetGlfwWindowParentClass(windowPtr);

        windowObject->minimized = false;

//        SierraEngine.Engine.Classes.Cursor.ResetCursorOffset();
    }

    void Window::WindowMaximizeCallback(GLFWwindow *windowPtr, int maximized)
    {
        auto windowObject = GetGlfwWindowParentClass(windowPtr);

        windowObject->minimized = !maximized;
        windowObject->maximized = maximized;

//        SierraEngine.Engine.Classes.Cursor.ResetCursorOffset();
    }

    Window *Window::GetGlfwWindowParentClass(GLFWwindow* windowPtr)
    {
        auto* windowObject = static_cast<Window*>(glfwGetWindowUserPointer(windowPtr));
        return windowObject;
    }


    Window::~Window()
    {
        glfwDestroyWindow(glfwWindow);
        glfwTerminate();
    }
}
