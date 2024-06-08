//
// Created by Nikolay Kanchevski on 9.16.2023.
//

#pragma once

#include "Key.h"
#include "MouseButton.h"
#include "EventDispatcher.hpp"

namespace Sierra
{

    class SIERRA_API InputEvent : public Event { };
    template<typename T> concept InputEventType = std::is_base_of_v<InputEvent, T> && !std::is_same_v<InputEvent, std::decay_t<T>>;

    class SIERRA_API KeyEvent : public InputEvent
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] Key GetKey() const { return key; }

    protected:
        explicit KeyEvent(const Key key) : key(key) { }

    private:
        Key key = Key::Unknown;

    };

    class SIERRA_API KeyPressEvent final : public KeyEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit KeyPressEvent(const Key pressedKey) : KeyEvent(pressedKey) { }

    };

    class SIERRA_API KeyReleaseEvent final : public KeyEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit KeyReleaseEvent(const Key releasedKey) : KeyEvent(releasedKey) { }

    };

    class SIERRA_API MouseButtonEvent : public InputEvent
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] MouseButton GetMouseButton() const { return mouseButton; }

    protected:
        explicit MouseButtonEvent(const MouseButton mouseButton) : mouseButton(mouseButton) { }

    private:
        MouseButton mouseButton = MouseButton::Unknown;

    };

    class SIERRA_API MouseButtonPressEvent final : public MouseButtonEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MouseButtonPressEvent(const MouseButton mouseButton) : MouseButtonEvent(mouseButton) { }

    };

    class SIERRA_API MouseButtonReleaseEvent final : public MouseButtonEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MouseButtonReleaseEvent(const MouseButton mouseButton) : MouseButtonEvent(mouseButton) { }

    };

    class SIERRA_API MouseScrollEvent final : public InputEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MouseScrollEvent(const Vector2 scroll) : scroll(scroll) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] float32 GetHorizontalScroll() const { return scroll.x; }
        [[nodiscard]] float32 GetVerticalScroll() const { return scroll.y; }

    private:
        Vector2 scroll;

    };

    class SIERRA_API InputManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<InputEventType EventType>
        using EventCallback = std::function<bool(const EventType&)>;

        /* --- POLLING METHODS --- */
        virtual void RegisterKeyPress(Key key);
        virtual void RegisterKeyRelease(Key key);

        virtual void RegisterMouseButtonPress(MouseButton mouseButton);
        virtual void RegisterMouseButtonRelease(MouseButton mouseButton);
        virtual void RegisterMouseScroll(Vector2 scroll);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual bool IsKeyPressed(Key key) const;
        [[nodiscard]] virtual bool IsKeyHeld(Key key) const;
        [[nodiscard]] virtual bool IsKeyReleased(Key key) const;
        [[nodiscard]] virtual bool IsKeyResting(Key key) const;

        template<typename... Args>
        [[nodiscard]] bool IsKeyCombinationPressed(const Key first, const Args... rest) const { return IsKeyCombinationPressedImplementation({ first, rest... }); }
        template<typename... Args>
        [[nodiscard]] bool IsKeyCombinationHeld(const Key first, const Args... rest) const { return IsKeyCombinationHeldImplementation({ first, rest... }); }

        [[nodiscard]] virtual bool IsMouseButtonPressed(MouseButton mouseButton) const;
        [[nodiscard]] virtual bool IsMouseButtonHeld(MouseButton mouseButton) const;
        [[nodiscard]] virtual bool IsMouseButtonReleased(MouseButton mouseButton) const;
        [[nodiscard]] virtual bool IsMouseButtonResting(MouseButton mouseButton) const;
        [[nodiscard]] virtual Vector2 GetMouseScroll() const;

        template<typename... Args>
        [[nodiscard]] bool IsMouseButtonCombinationPressed(const MouseButton first, const Args... rest) const { return IsMouseButtonCombinationPressedImplementation({ first, rest... }); }
        template<typename... Args>
        [[nodiscard]] bool IsMouseButtonCombinationHeld(const MouseButton first, const Args... rest) const { return IsMouseButtonCombinationHeldImplementation({ first, rest... }); }

        /* --- EVENTS --- */
        template<InputEventType EventType>
        void OnEvent(const EventCallback<EventType>&) {  }

        /* --- OPERATORS --- */
        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~InputManager() = default;

    protected:
        explicit InputManager() = default;

        enum class InputAction : bool
        {
            Release = 0,
            Press = 1
        };

        constexpr static uint32 KEY_COUNT = static_cast<uint32>(Key::RightSystem) + 1;
        constexpr static uint32 MOUSE_BUTTON_COUNT = static_cast<uint32>(MouseButton::Extra2) + 1;

        [[nodiscard]] constexpr static uint32 GetKeyIndex(const Key key) { return static_cast<uint32>(key); }
        [[nodiscard]] EventDispatcher<KeyPressEvent>& GetKeyPressDispatcher() { return keyPressDispatcher; }
        [[nodiscard]] EventDispatcher<KeyReleaseEvent>& GetKeyReleaseDispatcher() { return keyReleaseDispatcher; }

        [[nodiscard]] constexpr static uint32 GetMouseButtonIndex(const MouseButton mouseButton) { return static_cast<uint32>(mouseButton); }
        [[nodiscard]] EventDispatcher<MouseButtonPressEvent>& GetMouseButtonPressDispatcher() { return mouseButtonPressDispatcher; }
        [[nodiscard]] EventDispatcher<MouseButtonReleaseEvent>& GetMouseButtonReleaseDispatcher() { return mouseButtonReleaseDispatcher; }
        [[nodiscard]] EventDispatcher<MouseScrollEvent>& GetMouseScrollDispatcher() { return mouseScrollDispatcher; }

    private:
        EventDispatcher<KeyPressEvent> keyPressDispatcher;
        EventDispatcher<KeyReleaseEvent> keyReleaseDispatcher;

        EventDispatcher<MouseButtonPressEvent> mouseButtonPressDispatcher;
        EventDispatcher<MouseButtonReleaseEvent> mouseButtonReleaseDispatcher;
        EventDispatcher<MouseScrollEvent> mouseScrollDispatcher;

        [[nodiscard]] bool IsKeyCombinationPressedImplementation(const std::initializer_list<Key> &keys) const;
        [[nodiscard]] bool IsKeyCombinationHeldImplementation(const std::initializer_list<Key> &keys) const;

        [[nodiscard]] bool IsMouseButtonCombinationPressedImplementation(const std::initializer_list<MouseButton> &mouseButtons) const;
        [[nodiscard]] bool IsMouseButtonCombinationHeldImplementation(const std::initializer_list<MouseButton> &mouseButtons) const;

    };

    template<> inline void InputManager::OnEvent<KeyPressEvent>(const EventCallback<KeyPressEvent> &Callback) { keyPressDispatcher.Subscribe(Callback); }
    template<> inline void InputManager::OnEvent<KeyReleaseEvent>(const EventCallback<KeyReleaseEvent> &Callback) { keyReleaseDispatcher.Subscribe(Callback); }

    template<> inline void InputManager::OnEvent<MouseButtonPressEvent>(const EventCallback<MouseButtonPressEvent> &Callback) { mouseButtonPressDispatcher.Subscribe(Callback); }
    template<> inline void InputManager::OnEvent<MouseButtonReleaseEvent>(const EventCallback<MouseButtonReleaseEvent> &Callback) { mouseButtonReleaseDispatcher.Subscribe(Callback); }
    template<> inline void InputManager::OnEvent<MouseScrollEvent>(const EventCallback<MouseScrollEvent> &Callback) { mouseScrollDispatcher.Subscribe(Callback); }

}
