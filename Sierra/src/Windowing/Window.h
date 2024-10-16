//
// Created by Nikolay Kanchevski on 15.08.23.
//

#pragma once

#include "Screen.h"
#include "WindowEvents.h"

#include "CursorManager.h"
#include "InputManager.h"
#include "TouchManager.h"

namespace Sierra
{

    struct WindowCreateInfo
    {
        std::string_view title = "Window";
        uint32 width = 1280;
        uint32 height = 780;

        bool resizable = false;
        ScreenOrientation allowedOrientations = ScreenOrientation::Any;

        bool maximize = false;
        bool hide = false;
    };

    template<typename T>
    concept WindowEventType = std::is_base_of_v<WindowEvent, T> && !std::is_same_v<WindowEvent, std::decay_t<T>>;

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
        virtual void SetPosition(Vector2Int position) = 0;
        virtual void SetSize(Vector2UInt size) = 0;
        virtual void SetOpacity(float32 opacity) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual std::string_view GetTitle() const noexcept = 0;
        [[nodiscard]] virtual Vector2Int GetPosition() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetWidth() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetHeight() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetFramebufferWidth() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetFramebufferHeight() const noexcept = 0;
        [[nodiscard]] virtual float32 GetOpacity() const noexcept = 0;

        [[nodiscard]] virtual bool IsClosed() const noexcept = 0;
        [[nodiscard]] virtual bool IsMinimized() const noexcept = 0;
        [[nodiscard]] virtual bool IsMaximized() const noexcept = 0;
        [[nodiscard]] virtual bool IsFocused() const noexcept = 0;
        [[nodiscard]] virtual bool IsHidden() const noexcept = 0;

        [[nodiscard]] virtual InputManager* GetInputManager() noexcept = 0;
        [[nodiscard]] virtual CursorManager* GetCursorManager() noexcept = 0;
        [[nodiscard]] virtual TouchManager* GetTouchManager() noexcept = 0;
        [[nodiscard]] virtual WindowingBackendType GetBackendType() const noexcept = 0;

        /* --- EVENTS --- */
        template<WindowEventType EventType>
        EventSubscriptionID AddEventListener(const EventCallback<EventType>&);

        template<WindowEventType EventType>
        bool RemoveEventListener(EventSubscriptionID) noexcept;

        /* --- MOVE SEMANTICS --- */
        Window(Window&&) noexcept = delete;
        Window& operator=(Window&&) noexcept = delete;

        /* --- COPY SEMANTICS --- */
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Window() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit Window(const WindowCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] EventDispatcher<WindowMoveEvent>& GetWindowMoveDispatcher() noexcept { return windowMoveDispatcher; }
        [[nodiscard]] EventDispatcher<WindowResizeEvent>& GetWindowResizeDispatcher() noexcept { return windowResizeDispatcher; }
        [[nodiscard]] EventDispatcher<WindowFocusEvent>& GetWindowFocusDispatcher() noexcept { return windowFocusDispatcher; }
        [[nodiscard]] EventDispatcher<WindowMinimizeEvent>& GetWindowMinimizeDispatcher() noexcept { return windowMinimizeDispatcher; }
        [[nodiscard]] EventDispatcher<WindowMaximizeEvent>& GetWindowMaximizeDispatcher() noexcept { return windowMaximizeDispatcher; }
        [[nodiscard]] EventDispatcher<WindowCloseEvent>& GetWindowCloseDispatcher() noexcept { return windowCloseDispatcher; }

    private:
        EventDispatcher<WindowMoveEvent> windowMoveDispatcher = { };
        EventDispatcher<WindowResizeEvent> windowResizeDispatcher = { };
        EventDispatcher<WindowFocusEvent> windowFocusDispatcher = { };
        EventDispatcher<WindowMinimizeEvent> windowMinimizeDispatcher = { };
        EventDispatcher<WindowMaximizeEvent> windowMaximizeDispatcher = { };
        EventDispatcher<WindowCloseEvent> windowCloseDispatcher = { };

    };

    template<> inline EventSubscriptionID Window::AddEventListener<WindowMoveEvent>(const EventCallback<WindowMoveEvent>& Callback) { return windowMoveDispatcher.Subscribe(Callback); }
    template<> inline bool Window::RemoveEventListener<WindowMoveEvent>(const EventSubscriptionID ID) noexcept { return windowMoveDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID Window::AddEventListener<WindowResizeEvent>(const EventCallback<WindowResizeEvent>& Callback) { return windowResizeDispatcher.Subscribe(Callback); }
    template<> inline bool Window::RemoveEventListener<WindowResizeEvent>(const EventSubscriptionID ID) noexcept { return windowResizeDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID Window::AddEventListener<WindowFocusEvent>(const EventCallback<WindowFocusEvent>& Callback) { return windowFocusDispatcher.Subscribe(Callback); }
    template<> inline bool Window::RemoveEventListener<WindowFocusEvent>(const EventSubscriptionID ID) noexcept { return windowFocusDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID Window::AddEventListener<WindowMinimizeEvent>(const EventCallback<WindowMinimizeEvent>& Callback) { return windowMinimizeDispatcher.Subscribe(Callback); }
    template<> inline bool Window::RemoveEventListener<WindowMinimizeEvent>(const EventSubscriptionID ID) noexcept { return windowMinimizeDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID Window::AddEventListener<WindowMaximizeEvent>(const EventCallback<WindowMaximizeEvent>& Callback) { return windowMaximizeDispatcher.Subscribe(Callback); }
    template<> inline bool Window::RemoveEventListener<WindowMaximizeEvent>(const EventSubscriptionID ID) noexcept { return windowMaximizeDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID Window::AddEventListener<WindowCloseEvent>(const EventCallback<WindowCloseEvent>& Callback) { return windowCloseDispatcher.Subscribe(Callback); }
    template<> inline bool Window::RemoveEventListener<WindowCloseEvent>(const EventSubscriptionID ID) noexcept { return windowCloseDispatcher.Unsubscribe(ID); }


}
