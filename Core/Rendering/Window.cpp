//
// Created by Nikolay Kanchevski on 27.09.22.
//

#include <imgui_impl_glfw.h>
#include "Window.h"

#include "../Debugger.h"
#include "../../Engine/Classes/Input.h"
#include "../../Engine/Classes/Cursor.h"
#include "../../Engine/Classes/Stopwatch.h"
#include "Vulkan/VK.h"

using Rendering::Vulkan::VK;
using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering
{

    std::unique_ptr<Window> Window::Create(WindowCreateInfo createInfo)
    {
        return std::make_unique<Window>(createInfo);
    }

    Window::Window(const WindowCreateInfo &createInfo)
        : title(std::move(createInfo.givenTitle)), maximized(createInfo.startMaximized), requireFocus(createInfo.isFocusRequired), resizable(createInfo.isResizable)
    {
        PROFILE_FUNCTION();

        ASSERT_ERROR_IF(!glfwInit(), "GLFW could not be started");

        ASSERT_ERROR_IF(!glfwVulkanSupported(), "Vulkan not supported on this system");

        if (maximized && !createInfo.isResizable)
        {
            ASSERT_WARNING("A maximized window cannot be created unless resizing is allowed. Setting was automatically disabled");
            maximized = false;
        }

        Initialize();
        CreateSurface();
        SetCallbacks();
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

    /* --- SETTER METHODS --- */

    void Window::Initialize()
    {
        glfwWindowHint(GLFW_RESIZABLE, resizable);
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

        glfwSetWindowUserPointer(glfwWindow, this);

//        imGuiContext = ImGui::CreateContext();
//        ImGui::SetCurrentContext(imGuiContext);
//        ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);

        currentlyFocusedWindow = this;
    }

    void Window::CreateSurface()
    {
        // Create window surface
        glfwCreateWindowSurface(VK::GetInstance(), glfwWindow, nullptr, &surface);
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

        glfwSetCharCallback(glfwWindow, Input::KeyboardCharacterCallback);

        glfwSetKeyCallback(glfwWindow, Input::KeyboardKeyCallback);

        glfwSetMouseButtonCallback(glfwWindow, Input::MouseButtonCallback);

        glfwSetScrollCallback(glfwWindow, Input::MouseScrollCallback);

        glfwSetJoystickCallback(Input::JoystickCallback);

        double xCursorPosition, yCursorPosition;
        glfwGetCursorPos(glfwWindow, &xCursorPosition, &yCursorPosition);
        glfwSetCursorPosCallback(glfwWindow, Cursor::CursorPositionCallback);
        Cursor::SetCursorPosition({ xCursorPosition, yCursorPosition });
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

        Cursor::ResetCursorOffset();
    }

    void Window::WindowFocusCallback(GLFWwindow *windowPtr, int focused)
    {
        auto windowObject = GetGlfwWindowParentClass(windowPtr);

        currentlyFocusedWindow = focused ? windowObject : nullptr;

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


    void Window::Destroy()
    {
        vkDestroySurfaceKHR(VK::GetInstance(), surface, nullptr);
        glfwDestroyWindow(glfwWindow);
    }
}
