//
// Created by Nikolay Kanchevski on 27.09.22.
//

#include "Window.h"

#include "../../Engine/Classes/Input.h"
#include "../../Engine/Classes/Cursor.h"
#include "Vulkan/VK.h"

namespace Sierra::Rendering
{

    UniquePtr<Window> Window::Create(const WindowCreateInfo &createInfo)
    {
        return std::make_unique<Window>(createInfo);
    }

    Window::Window(const WindowCreateInfo &createInfo)
        : title(createInfo.title), maximized(createInfo.startMaximized), requireFocus(createInfo.isFocusRequired), resizable(createInfo.isResizable)
    {
        PROFILE_FUNCTION();

        ASSERT_ERROR_IF(!glfwInit(), "GLFW could not be started");
        ASSERT_ERROR_IF(!glfwVulkanSupported(), "Vulkan not supported on this system");

        // Check if provided settings are legal
        if (maximized && !createInfo.isResizable)
        {
            ASSERT_WARNING("A maximized window cannot be created unless resizing is allowed. Setting was automatically disabled");
            maximized = false;
        }

        // Set window creation settings
        glfwWindowHint(GLFW_RESIZABLE, resizable);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_VISIBLE, 0);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        // Create window
        glfwWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(glfwWindow, this);

        // If maximized state requested, maximize
        if (maximized)
        {
            glfwMaximizeWindow(glfwWindow);
            glfwGetWindowSize(glfwWindow, &width, &height);
        }

        // Set focused window
        currentlyFocusedWindow = this;

        // Create window surface
        glfwCreateWindowSurface(VK::GetInstance(), glfwWindow, nullptr, &surface);

        // Set callbacks
        #if DEBUG
            glfwSetErrorCallback(GlfwErrorCallback);
        #endif

        glfwSetWindowSizeCallback(glfwWindow, WindowResizeCallback);
        glfwSetWindowFocusCallback(glfwWindow, WindowFocusCallback);
        glfwSetWindowRefreshCallback(glfwWindow, [](GLFWwindow* windowPtr) { WindowFocusCallback(windowPtr, true); });
        glfwSetWindowIconifyCallback(glfwWindow, WindowMinimizeCallback);
        glfwSetWindowMaximizeCallback(glfwWindow, WindowMaximizeCallback);

        glfwSetCharCallback(glfwWindow, Engine::Input::KeyboardCharacterCallback);
        glfwSetKeyCallback(glfwWindow, Engine::Input::KeyboardKeyCallback);
        glfwSetMouseButtonCallback(glfwWindow, Engine::Input::MouseButtonCallback);
        glfwSetScrollCallback(glfwWindow, Engine::Input::MouseScrollCallback);
        glfwSetJoystickCallback(Engine::Input::JoystickCallback);

        // Set initial cursor position
        double xCursorPosition, yCursorPosition;
        glfwGetCursorPos(glfwWindow, &xCursorPosition, &yCursorPosition);
        glfwSetCursorPosCallback(glfwWindow, Engine::Cursor::CursorPositionCallback);
        Engine::Cursor::SetCursorPosition({ xCursorPosition, yCursorPosition });
    }

    /* --- POLLING METHODS --- */

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

    /* --- SETTER METHODS --- */

    void Window::SetTitle(const String &givenTitle)
    {
        title = givenTitle;
        glfwSetWindowTitle(glfwWindow, givenTitle.c_str());
    }

    void Window::Show()
    {
        hidden = false;
        glfwShowWindow(glfwWindow);
    }

    void Window::Hide()
    {
       hidden = true;
        glfwHideWindow(glfwWindow);
    }

    void Window::SetOpacity(const float givenOpacity)
    {
        opacity = givenOpacity;
        glfwSetWindowOpacity(glfwWindow, givenOpacity);
    }

    /* --- CALLBACKS --- */

    void Window::GlfwErrorCallback(int errorCode, const char* description)
    {
        ASSERT_WARNING_FORMATTED("GLFW Error: {0}! Error code: {1}.", description, errorCode);
    }

    void Window::WindowResizeCallback(GLFWwindow *windowPtr, int newWidth, int newHeight)
    {
        auto windowObject = GetGlfwWindowParentClass(windowPtr);
        windowObject->width = newWidth;
        windowObject->height = newHeight;
        windowObject->resized = true;
        windowObject->resizeSet = true;

        Engine::Cursor::ResetCursorOffset();
    }

    void Window::WindowFocusCallback(GLFWwindow *windowPtr, int focused)
    {
        auto windowObject = GetGlfwWindowParentClass(windowPtr);

        currentlyFocusedWindow = focused ? windowObject : nullptr;

        windowObject->focused = focused;

        Engine::Cursor::ResetCursorOffset();
    }

    void Window::WindowMinimizeCallback(GLFWwindow *windowPtr, int minimized)
    {
        auto windowObject = GetGlfwWindowParentClass(windowPtr);

        windowObject->minimized = !windowObject->focused;

        Engine::Cursor::ResetCursorOffset();
    }

    void Window::WindowMaximizeCallback(GLFWwindow *windowPtr, int maximized)
    {
        auto windowObject = GetGlfwWindowParentClass(windowPtr);

        windowObject->maximized = !windowObject->minimized;

        Engine::Cursor::ResetCursorOffset();
    }

    Window *Window::GetGlfwWindowParentClass(GLFWwindow* windowPtr)
    {
        auto* windowObject = static_cast<Window*>(glfwGetWindowUserPointer(windowPtr));
        return windowObject;
    }


    void Window::Destroy()
    {
        vkDestroySurfaceKHR(VK::GetInstance(), surface, nullptr);
        glfwDestroyWindow(glfwWindow);
    }
}
