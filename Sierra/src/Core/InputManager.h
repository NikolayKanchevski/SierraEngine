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
        [[nodiscard]] inline Key GetKey() const { return key; }

    protected:
        inline explicit KeyEvent(const Key key) : key(key) { }

    private:
        Key key = Key::Unknown;

    };

    class SIERRA_API KeyPressEvent final : public KeyEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit KeyPressEvent(const Key pressedKey) : KeyEvent(pressedKey) { }

    };

    class SIERRA_API KeyReleaseEvent final : public KeyEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit KeyReleaseEvent(const Key releasedKey) : KeyEvent(releasedKey) { }

    };

    class SIERRA_API MouseButtonEvent : public InputEvent
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline MouseButton GetMouseButton() const { return mouseButton; }

    protected:
        inline explicit MouseButtonEvent(const MouseButton mouseButton) : mouseButton(mouseButton) { }

    private:
        MouseButton mouseButton = MouseButton::Unknown;

    };

    class SIERRA_API MouseButtonPressEvent final : public MouseButtonEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit MouseButtonPressEvent(const MouseButton mouseButton) : MouseButtonEvent(mouseButton) { }

    };

    class SIERRA_API MouseButtonReleaseEvent final : public MouseButtonEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit MouseButtonReleaseEvent(const MouseButton mouseButton) : MouseButtonEvent(mouseButton) { }

    };

    class SIERRA_API MouseScrollEvent final : public InputEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit MouseScrollEvent(const Vector2 scroll) : scroll(scroll) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline float32 GetHorizontalScroll() const { return scroll.x; }
        [[nodiscard]] inline float32 GetVerticalScroll() const { return scroll.y; }

    private:
        Vector2 scroll;

    };

    struct InputManagerCreateInfo
    {

    };

    class SIERRA_API InputManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<InputEventType EventType>
        using EventCallback = std::function<bool(const EventType&)>;

        /* --- CONSTRUCTORS --- */
        explicit InputManager(const InputManagerCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual bool IsKeyPressed(Key key) const;
        [[nodiscard]] virtual bool IsKeyHeld(Key key) const;
        [[nodiscard]] virtual bool IsKeyReleased(Key key) const;
        [[nodiscard]] virtual bool IsKeyResting(Key key) const;

        template<typename... Args>
        [[nodiscard]] inline bool IsKeyCombinationPressed(const Key first, const Args... rest) const { return IsKeyCombinationPressedImplementation({ first, rest... }); };
        template<typename... Args>
        [[nodiscard]] inline bool IsKeyCombinationHeld(const Key first, const Args... rest) const { return IsKeyCombinationHeldImplementation({ first, rest... }); };

        [[nodiscard]] virtual bool IsMouseButtonPressed(MouseButton key) const;
        [[nodiscard]] virtual bool IsMouseButtonHeld(MouseButton key) const;
        [[nodiscard]] virtual bool IsMouseButtonReleased(MouseButton key) const;
        [[nodiscard]] virtual bool IsMouseButtonResting(MouseButton key) const;
        [[nodiscard]] virtual Vector2 GetMouseScroll() const;

        /* --- EVENTS --- */
        template<InputEventType EventType>
        void OnEvent(const EventCallback<EventType> &Callback) {  }

        /* --- OPERATORS --- */
        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~InputManager() = default;

    protected:
        enum class InputAction : bool
        {
            Release = 0,
            Press = 1
        };

        constexpr static uint32 KEY_COUNT = static_cast<uint32>(Key::RightSystem) + 1;
        constexpr static uint32 MOUSE_BUTTON_COUNT = static_cast<uint32>(MouseButton::Extra2) + 1;

        [[nodiscard]] inline constexpr static uint32 GetKeyIndex(const Key key) { return static_cast<uint32>(key); }
        [[nodiscard]] inline constexpr static uint32 GetMouseButtonIndex(const MouseButton mouseButton) { return static_cast<uint32>(mouseButton); }

        [[nodiscard]] inline EventDispatcher<KeyPressEvent>& GetKeyPressDispatcher() { return keyPressDispatcher; }
        [[nodiscard]] inline EventDispatcher<KeyReleaseEvent>& GetKeyReleaseDispatcher() { return keyReleaseDispatcher; }

        [[nodiscard]] inline EventDispatcher<MouseButtonPressEvent>& GetMouseButtonPressDispatcher() { return mouseButtonPressDispatcher; }
        [[nodiscard]] inline EventDispatcher<MouseButtonReleaseEvent>& GetMouseButtonReleaseDispatcher() { return mouseButtonReleaseDispatcher; }
        [[nodiscard]] inline EventDispatcher<MouseScrollEvent>& GetMouseScrollDispatcher() { return mouseScrollDispatcher; }

    private:
        EventDispatcher<KeyPressEvent> keyPressDispatcher;
        EventDispatcher<KeyReleaseEvent> keyReleaseDispatcher;

        EventDispatcher<MouseButtonPressEvent> mouseButtonPressDispatcher;
        EventDispatcher<MouseButtonReleaseEvent> mouseButtonReleaseDispatcher;
        EventDispatcher<MouseScrollEvent> mouseScrollDispatcher;

       [[nodiscard]] bool IsKeyCombinationPressedImplementation(const std::initializer_list<Key> &keys) const;
       [[nodiscard]] bool IsKeyCombinationHeldImplementation(const std::initializer_list<Key> &keys) const;

        std::queue<char> enteredCharacters;
    };

    template<> inline void InputManager::OnEvent<KeyPressEvent>(const EventCallback<KeyPressEvent> &Callback) { keyPressDispatcher.Subscribe(Callback); }
    template<> inline void InputManager::OnEvent<KeyReleaseEvent>(const EventCallback<KeyReleaseEvent> &Callback) { keyReleaseDispatcher.Subscribe(Callback); }

    template<> inline void InputManager::OnEvent<MouseButtonPressEvent>(const EventCallback<MouseButtonPressEvent> &Callback) { mouseButtonPressDispatcher.Subscribe(Callback); }
    template<> inline void InputManager::OnEvent<MouseButtonReleaseEvent>(const EventCallback<MouseButtonReleaseEvent> &Callback) { mouseButtonReleaseDispatcher.Subscribe(Callback); }
    template<> inline void InputManager::OnEvent<MouseScrollEvent>(const EventCallback<MouseScrollEvent> &Callback) { mouseScrollDispatcher.Subscribe(Callback); }

}
