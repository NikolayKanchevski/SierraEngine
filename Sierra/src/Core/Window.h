//
// Created by Nikolay Kanchevski on 15.08.23.
//

#pragma once

#include "PlatformInstance.h"
#include "../Events/WindowEvent.h"

#include "Screen.h"
#include "InputManager.h"
#include "CursorManager.h"
#include "TouchManager.h"

namespace Sierra
{

    BETTER_ENUM(
        WindowAPI, uint8,
        Win32,
        X11,
        Cocoa,
        UIKit
    );

    struct WindowCreateInfo
    {
        const std::string &title = "Sierra Application";
        uint32 width = 1280;
        uint32 height = 780;
        bool resizable = true;
        bool maximize = false;
        bool hide = false;
    };

    class SIERRA_API Window
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<typename T>
        using WindowEventCallback = std::function<bool(const T&)>;

        /* --- POLLING METHODS --- */
        virtual void OnUpdate() = 0;
        virtual void Minimize() = 0;
        virtual void Maximize() = 0;
        virtual void Show() = 0;
        virtual void Hide() = 0;
        virtual void Focus() = 0;
        virtual void Close() = 0;

        /* --- SETTER METHODS --- */
        virtual void SetTitle(const std::string &title) = 0;
        virtual void SetPosition(const Vector2Int &position) = 0;
        virtual void SetSize(const Vector2UInt &size) = 0;
        virtual void SetOpacity(float32 opacity) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const std::string& GetTitle() const = 0;
        [[nodiscard]] virtual Vector2Int GetPosition() const = 0;
        [[nodiscard]] virtual Vector2UInt GetSize() const = 0;
        [[nodiscard]] virtual Vector2UInt GetFramebufferSize() const = 0;
        [[nodiscard]] virtual float32 GetOpacity() const = 0;
        [[nodiscard]] virtual bool IsClosed() const = 0;
        [[nodiscard]] virtual bool IsMinimized() const = 0;
        [[nodiscard]] virtual bool IsMaximized() const = 0;
        [[nodiscard]] virtual bool IsFocused() const = 0;
        [[nodiscard]] virtual bool IsHidden() const = 0;

        [[nodiscard]] virtual const Screen& GetScreen() const = 0;
        [[nodiscard]] virtual InputManager& GetInputManager();
        [[nodiscard]] virtual CursorManager& GetCursorManager();
        [[nodiscard]] virtual TouchManager& GetTouchManager();
        [[nodiscard]] virtual WindowAPI GetAPI() const = 0;

        /* --- EVENTS --- */
        template<typename T> void OnEvent(WindowEventCallback<T>) { static_assert(std::is_base_of_v<WindowEvent, T> && !std::is_same_v<WindowEvent, T>, "Template function accepts derived window events only!"); }

        /* --- DESTRUCTOR --- */
        virtual ~Window() = default;
        
        /* --- OPERATORS --- */
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

    protected:
        explicit Window(const WindowCreateInfo &createInfo);

        [[nodiscard]] inline EventDispatcher<WindowMoveEvent>& GetWindowMoveDispatcher() { return windowMoveDispatcher; };
        [[nodiscard]] inline EventDispatcher<WindowResizeEvent>& GetWindowResizeDispatcher() { return windowResizeDispatcher; };
        [[nodiscard]] inline EventDispatcher<WindowFocusEvent>& GetWindowFocusDispatcher() { return windowFocusDispatcher; };
        [[nodiscard]] inline EventDispatcher<WindowMinimizeEvent>& GetWindowMinimizeDispatcher() { return windowMinimizeDispatcher; };
        [[nodiscard]] inline EventDispatcher<WindowMaximizeEvent>& GetWindowMaximizeDispatcher() { return windowMaximizeDispatcher; };
        [[nodiscard]] inline EventDispatcher<WindowCloseEvent>& GetWindowCloseDispatcher() { return windowCloseDispatcher; };

    private:
        EventDispatcher<WindowMoveEvent> windowMoveDispatcher;
        EventDispatcher<WindowResizeEvent> windowResizeDispatcher;
        EventDispatcher<WindowFocusEvent> windowFocusDispatcher;
        EventDispatcher<WindowMinimizeEvent> windowMinimizeDispatcher;
        EventDispatcher<WindowMaximizeEvent> windowMaximizeDispatcher;
        EventDispatcher<WindowCloseEvent> windowCloseDispatcher;

    };

    template<> inline void Window::OnEvent<WindowMoveEvent>(WindowEventCallback<WindowMoveEvent> Callback) { windowMoveDispatcher.Subscribe(Callback); }
    template<> inline void Window::OnEvent<WindowResizeEvent>(WindowEventCallback<WindowResizeEvent> Callback) { windowResizeDispatcher.Subscribe(Callback); }
    template<> inline void Window::OnEvent<WindowFocusEvent>(WindowEventCallback<WindowFocusEvent> Callback) { windowFocusDispatcher.Subscribe(Callback); }
    template<> inline void Window::OnEvent<WindowMinimizeEvent>(WindowEventCallback<WindowMinimizeEvent> Callback) { windowMinimizeDispatcher.Subscribe(Callback); }
    template<> inline void Window::OnEvent<WindowMaximizeEvent>(WindowEventCallback<WindowMaximizeEvent> Callback) { windowMaximizeDispatcher.Subscribe(Callback); }
    template<> inline void Window::OnEvent<WindowCloseEvent>(WindowEventCallback<WindowCloseEvent> Callback) { windowCloseDispatcher.Subscribe(Callback); }

}
