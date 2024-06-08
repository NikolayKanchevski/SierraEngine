//
// Created by Nikolay Kanchevski on 15.08.23.
//

#pragma once

#include "CursorManager.h"
#include "InputManager.h"
#include "Screen.h"
#include "TouchManager.h"

namespace Sierra
{

    class SIERRA_API WindowEvent : public Event { };
    template<typename T> concept WindowEventType = std::is_base_of_v<WindowEvent, T> && !std::is_same_v<WindowEvent, std::decay_t<T>>;

    class SIERRA_API WindowResizeEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit WindowResizeEvent(const uint32 width, const uint32 height) : width(width), height(height) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetWidth() const { return width; }
        [[nodiscard]] uint32 GetHeight() const { return height; }

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
        explicit WindowMoveEvent(const Vector2Int &position) : position(position) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] Vector2Int GetPosition() const { return position; }

    private:
        const Vector2Int position;

    };

    class SIERRA_API WindowFocusEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit WindowFocusEvent(const bool focused) : focused(focused) { }

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
        std::string_view title = "Window";
        uint32 width = 1280;
        uint32 height = 780;

        bool resizable = false;
        ScreenOrientation allowedOrientations = ScreenOrientation::Unknown;

        bool maximize = false;
        bool hide = false;
    };

    class SIERRA_API Window
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<WindowEventType EventType>
        using EventCallback = std::function<bool(const EventType&)>;

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

        [[nodiscard]] virtual Screen& GetScreen() const = 0;
        [[nodiscard]] virtual InputManager& GetInputManager();
        [[nodiscard]] virtual CursorManager& GetCursorManager();
        [[nodiscard]] virtual TouchManager& GetTouchManager();
        [[nodiscard]] virtual PlatformAPI GetAPI() const = 0;

        /* --- EVENTS --- */
        template<WindowEventType EventType>
        void OnEvent(const EventCallback<EventType>&) { }
        
        /* --- OPERATORS --- */
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Window() = default;

    protected:
        explicit Window(const WindowCreateInfo &createInfo);

        [[nodiscard]] EventDispatcher<WindowMoveEvent>& GetWindowMoveDispatcher() { return windowMoveDispatcher; }
        [[nodiscard]] EventDispatcher<WindowResizeEvent>& GetWindowResizeDispatcher() { return windowResizeDispatcher; }
        [[nodiscard]] EventDispatcher<WindowFocusEvent>& GetWindowFocusDispatcher() { return windowFocusDispatcher; }
        [[nodiscard]] EventDispatcher<WindowMinimizeEvent>& GetWindowMinimizeDispatcher() { return windowMinimizeDispatcher; }
        [[nodiscard]] EventDispatcher<WindowMaximizeEvent>& GetWindowMaximizeDispatcher() { return windowMaximizeDispatcher; }
        [[nodiscard]] EventDispatcher<WindowCloseEvent>& GetWindowCloseDispatcher() { return windowCloseDispatcher; }

    private:
        EventDispatcher<WindowMoveEvent> windowMoveDispatcher;
        EventDispatcher<WindowResizeEvent> windowResizeDispatcher;
        EventDispatcher<WindowFocusEvent> windowFocusDispatcher;
        EventDispatcher<WindowMinimizeEvent> windowMinimizeDispatcher;
        EventDispatcher<WindowMaximizeEvent> windowMaximizeDispatcher;
        EventDispatcher<WindowCloseEvent> windowCloseDispatcher;

    };

    template<> inline void Window::OnEvent<WindowMoveEvent>(const EventCallback<WindowMoveEvent> &Callback) { windowMoveDispatcher.Subscribe(Callback); }
    template<> inline void Window::OnEvent<WindowResizeEvent>(const EventCallback<WindowResizeEvent> &Callback) { windowResizeDispatcher.Subscribe(Callback); }
    template<> inline void Window::OnEvent<WindowFocusEvent>(const EventCallback<WindowFocusEvent> &Callback) { windowFocusDispatcher.Subscribe(Callback); }
    template<> inline void Window::OnEvent<WindowMinimizeEvent>(const EventCallback<WindowMinimizeEvent> &Callback) { windowMinimizeDispatcher.Subscribe(Callback); }
    template<> inline void Window::OnEvent<WindowMaximizeEvent>(const EventCallback<WindowMaximizeEvent> &Callback) { windowMaximizeDispatcher.Subscribe(Callback); }
    template<> inline void Window::OnEvent<WindowCloseEvent>(const EventCallback<WindowCloseEvent> &Callback) { windowCloseDispatcher.Subscribe(Callback); }

}
