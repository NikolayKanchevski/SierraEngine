//
// Created by Nikolay Kanchevski on 15.08.23.
//

#pragma once

#include "PlatformContext.h"

#include "Screen.h"
#include "InputManager.h"
#include "CursorManager.h"
#include "TouchManager.h"

namespace Sierra
{

    #pragma region Events
        class SIERRA_API WindowEvent : public Event { };

        class SIERRA_API WindowResizeEvent final : public WindowEvent
        {
        public:
            /* --- CONSTRUCTORS --- */
            inline explicit WindowResizeEvent(const uint32 width, const uint32 height) : width(width), height(height) { }

            /* --- GETTER METHODS --- */
            [[nodiscard]] inline uint32 GetWidth() const { return width; }
            [[nodiscard]] inline uint32 GetHeight() const { return height; }

        private:
            const uint32 width;
            const uint32 height;

        };

        class SIERRA_API WindowCloseEvent final : public WindowEvent
        {
        public:
            /* --- CONSTRUCTORS --- */
            WindowCloseEvent() = default;

        };

        class SIERRA_API WindowMoveEvent final : public WindowEvent
        {
        public:
            /* --- CONSTRUCTORS --- */
            inline explicit WindowMoveEvent(const Vector2Int &position) : position(position) { }

            /* --- GETTER METHODS --- */
            [[nodiscard]] Vector2Int GetPosition() const { return position; }

        private:
            const Vector2Int position;

        };

        class SIERRA_API WindowFocusEvent final : public WindowEvent
        {
        public:
            /* --- CONSTRUCTORS --- */
            inline explicit WindowFocusEvent(const bool focused) : focused(focused) { };

            /* --- GETTER METHODS --- */
            [[nodiscard]] bool IsFocused() const { return focused; }

        private:
            const bool focused;

        };

        class SIERRA_API WindowMinimizeEvent final : public WindowEvent
        {
        public:
            /* --- CONSTRUCTORS --- */
            WindowMinimizeEvent() = default;

        };

        class SIERRA_API WindowMaximizeEvent final : public WindowEvent
        {
        public:
            /* --- CONSTRUCTORS --- */
            WindowMaximizeEvent() = default;

        };
    #pragma endregion

    enum class PlatformAPI : uint8
    {
        Win32,
        X11,
        Cocoa,
        GameActivity,
        UIKit
    };

    struct WindowCreateInfo
    {
        std::string_view title = "Sierra Application";
        uint32 width = 1280;
        uint32 height = 780;
        bool resizable = false;
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
        virtual void Update() = 0;
        virtual void Minimize() = 0;
        virtual void Maximize() = 0;
        virtual void Show() = 0;
        virtual void Hide() = 0;
        virtual void Focus() = 0;
        virtual void Close() = 0;

        /* --- SETTER METHODS --- */
        virtual void SetTitle(std::string_view title) = 0;
        virtual void SetPosition(const Vector2Int &position) = 0;
        virtual void SetSize(const Vector2UInt &size) = 0;
        virtual void SetOpacity(float32 opacity) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual std::string_view GetTitle() const = 0;
        [[nodiscard]] virtual Vector2Int GetPosition() const = 0;
        [[nodiscard]] virtual uint32 GetWidth() const = 0;
        [[nodiscard]] virtual uint32 GetHeight() const = 0;
        [[nodiscard]] virtual uint32 GetFramebufferWidth() const = 0;
        [[nodiscard]] virtual uint32 GetFramebufferHeight() const = 0;
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
        [[nodiscard]] virtual PlatformAPI GetAPI() const = 0;

        /* --- EVENTS --- */
        template<typename T> requires (std::is_base_of_v<WindowEvent, T> && !std::is_same_v<WindowEvent, T>) void OnEvent(WindowEventCallback<T> Callback) { }
        
        /* --- OPERATORS --- */
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Window() = default;

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
