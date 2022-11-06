//
// Created by Nikolay Kanchevski on 27.09.22.
//

#include <utility>
#include "Window.h"
#include "../Debugger.h"
#include "../../Engine/Classes/Stopwatch.h"
#include "../../Engine/Classes/Cursor.h"
#include "../../Engine/Classes/Input.h"

using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering
{

    void Window::Update()
    {
        glfwPollEvents();

        if (resizeSet)
        {
            resized = true;
            resizeSet = false;
            return;
        }

        resized = false;
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

    void Window::SetResizeCallback(std::function<void()> givenCallback)
    {
        this->resizeCallbackSet = true;
        this->resizeCallback = givenCallback;
    }

    Window::Window(std::string givenTitle, const bool setMaximized, const bool setResizable, const bool setFocusRequirement)
            : title(std::move(givenTitle)), maximized(setMaximized), REQUIRE_FOCUS(setFocusRequirement), RESIZABLE(setResizable)
    {
        ASSERT_ERROR_IF(!glfwInit(), "GLFW could not be started");

        ASSERT_ERROR_IF(!glfwVulkanSupported(), "Vulkan not supported on this system");

        if (maximized && !setResizable)
        {
            ASSERT_WARNING("A maximized window cannot be created unless resizing is allowed");
            maximized = false;
        }

        #ifdef DEBUG
            Sierra::Engine::Classes::Stopwatch stopwatch;
        #endif

        this->width = 800;
        this->height = 600;

        InitWindow();

        #ifdef DEBUG
            ASSERT_INFO(
                "Window [" + this->title + "] successfully created! Initialization took: " + std::to_string(stopwatch.GetElapsedMilliseconds()) + "ms"
            );
        #endif
    }

    void Window::InitWindow()
    {
        glfwWindowHint(GLFW_RESIZABLE, RESIZABLE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_VISIBLE, 0);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        glfwWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        if (maximized)
        {
            glfwMaximizeWindow(glfwWindow);
            glfwGetWindowSize(glfwWindow, &width, &height);
        }

        Vulkan::VulkanCore::SetWindow(this);

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

        glfwSetCharCallback(glfwWindow, Input::Input::KeyboardCharacterCallback);

        glfwSetKeyCallback(glfwWindow, Input::KeyboardKeyCallback);

        glfwSetMouseButtonCallback(glfwWindow, Input::MouseButtonCallback);

        glfwSetScrollCallback(glfwWindow, Input::MouseScrollCallback);

        glfwSetJoystickCallback(Input::JoystickCallback);

        double xCursorPosition, yCursorPosition;
        glfwGetCursorPos(glfwWindow, &xCursorPosition, &yCursorPosition);
        glfwSetCursorPosCallback(glfwWindow, Cursor::CursorPositionCallback);
        Cursor::SetCursorPosition({ xCursorPosition, yCursorPosition });
    }

    void Window::RetrieveMonitorData()
    {
        auto defaultMonitor = glfwGetPrimaryMonitor();
        glfwGetMonitorWorkarea(defaultMonitor, &monitor.xPosition, &monitor.yPosition, &monitor.width, &monitor.height);
    }

    /* --- CALLBACKS --- */
    void Window::GlfwErrorCallback(int errorCode, const char *description)
    {
        ASSERT_WARNING("GLFW Error: " + std::string(description) + " (" + std::to_string(errorCode) + ")");
    }

    void Window::WindowResizeCallback(GLFWwindow *windowPtr, int newWidth, int newHeight)
    {
        auto windowObject = GetGlfwWindowParentClass(windowPtr);
        windowObject->width = newWidth;
        windowObject->height = newHeight;
        windowObject->resized = true;
        windowObject->resizeSet = true;

        if (windowObject->resizeCallbackSet) windowObject->resizeCallback();

        Cursor::ResetCursorOffset();
    }

    void Window::WindowFocusCallback(GLFWwindow *windowPtr, int focused)
    {
        auto windowObject = GetGlfwWindowParentClass(windowPtr);

        windowObject->focused = focused;

        Cursor::ResetCursorOffset();
    }

    void Window::WindowMinimizeCallback(GLFWwindow *windowPtr, int minimized)
    {
        auto windowObject = GetGlfwWindowParentClass(windowPtr);

        windowObject->minimized = !windowObject->focused;

        Cursor::ResetCursorOffset();
    }

    void Window::WindowMaximizeCallback(GLFWwindow *windowPtr, int maximized)
    {
        auto windowObject = GetGlfwWindowParentClass(windowPtr);

        windowObject->maximized = !windowObject->minimized;

        Cursor::ResetCursorOffset();
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
