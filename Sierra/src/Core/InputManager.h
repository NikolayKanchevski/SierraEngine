//
// Created by Nikolay Kanchevski on 9.16.2023.
//

#pragma once

#include "EventDispatcher.hpp"

namespace Sierra
{

    enum class Key : uint8
    {
        Unknown,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Number0,
        Number1,
        Number2,
        Number3,
        Number4,
        Number5,
        Number6,
        Number7,
        Number8,
        Number9,
        Space,
        Escape,
        Minus,
        Equals,
        Comma,
        Period,
        Apostrophe,
        Semicolon,
        Slash,
        Backslash,
        LeftBracket,
        RightBracket,
        Grave,
        World1,
        World2,
        Enter,
        Backspace,
        Delete,
        Tab,
        CapsLock,
        Insert,
        Section,
        RightArrow,
        LeftArrow,
        DownArrow,
        UpArrow,
        PageUp,
        PageDown,
        Home,
        End,
        ScrollLock,
        NumpadLock,
        PrintScreen,
        Pause,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        F25,
        KeypadNumber0,
        KeypadNumber1,
        KeypadNumber2,
        KeypadNumber3,
        KeypadNumber4,
        KeypadNumber5,
        KeypadNumber6,
        KeypadNumber7,
        KeypadNumber8,
        KeypadNumber9,
        KeypadDecimal,
        KeypadDivide,
        KeypadMultiply,
        KeypadSubtract,
        KeypadAdd,
        KeypadEnter,
        KeypadEquals,
        LeftShift,
        RightShift,
        LeftControl,
        RightControl,
        Function,
        Menu,
        LeftAlt,
        RightAlt,
        LeftSystem,
        RightSystem,
        // Windows Keys:
        LeftWindows = LeftSystem,
        RightWindows = RightSystem,
        // macOS Keys
        LeftCommand = LeftSystem,
        RightCommand = RightSystem,
        LeftOption = LeftAlt,
        RightOption = RightAlt
    };
    [[nodiscard]] std::string_view GetKeyName(Key key);
    [[nodiscard]] char GetKeyCharacter(Key key);

    enum class MouseButton : uint8
    {
        Unknown,
        Left,
        Right,
        Middle,
        Extra1,
        Extra2
    };
    [[nodiscard]] std::string_view GetMouseButtonName(MouseButton mouseButton);

    class SIERRA_API InputEvent : public Event { protected: InputEvent() = default; };
    template<typename T> concept InputEventType = std::is_base_of_v<InputEvent, T> && !std::is_same_v<InputEvent, std::decay_t<T>>;

    class SIERRA_API KeyEvent : public InputEvent
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] Key GetKey() const { return key; }

    protected:
        explicit KeyEvent(const Key key) : key(key) { }

    private:
        const Key key = Key::Unknown;

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
        const MouseButton mouseButton = MouseButton::Unknown;

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
        const Vector2 scroll;

    };

    class SIERRA_API InputManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<InputEventType EventType>
        using EventCallback = std::function<bool(const EventType&)>;

        /* --- POLLING METHODS --- */
        virtual void RegisterKeyPress(Key key) = 0;
        virtual void RegisterKeyRelease(Key key) = 0;

        virtual void RegisterMouseButtonPress(MouseButton mouseButton) = 0;
        virtual void RegisterMouseButtonRelease(MouseButton mouseButton) = 0;
        virtual void RegisterMouseScroll(Vector2 scroll) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual bool IsKeyPressed(Key key) const = 0;
        [[nodiscard]] virtual bool IsKeyHeld(Key key) const = 0;
        [[nodiscard]] virtual bool IsKeyReleased(Key key) const = 0;
        [[nodiscard]] virtual bool IsKeyResting(Key key) const = 0;

        template<typename... Args>
        [[nodiscard]] bool IsKeyCombinationPressed(const Key first, const Args... rest) const { return IsKeyCombinationPressedImplementation({ first, rest... }); }

        template<typename... Args>
        [[nodiscard]] bool IsKeyCombinationHeld(const Key first, const Args... rest) const { return IsKeyCombinationHeldImplementation({ first, rest... }); }

        [[nodiscard]] virtual bool IsMouseButtonPressed(MouseButton mouseButton) const = 0;
        [[nodiscard]] virtual bool IsMouseButtonHeld(MouseButton mouseButton) const = 0;
        [[nodiscard]] virtual bool IsMouseButtonReleased(MouseButton mouseButton) const = 0;
        [[nodiscard]] virtual bool IsMouseButtonResting(MouseButton mouseButton) const = 0;
        [[nodiscard]] virtual Vector2 GetMouseScroll() const = 0;

        template<typename... Args>
        [[nodiscard]] bool IsMouseButtonCombinationPressed(const MouseButton first, const Args... rest) const { return IsMouseButtonCombinationPressedImplementation({ first, rest... }); }
        template<typename... Args>
        [[nodiscard]] bool IsMouseButtonCombinationHeld(const MouseButton first, const Args... rest) const { return IsMouseButtonCombinationHeldImplementation({ first, rest... }); }

        /* --- EVENTS --- */
        template<InputEventType EventType>
        EventSubscriptionID AddEventListener(const EventCallback<EventType>&);

        template<InputEventType EventType>
        bool RemoveEventListener(EventSubscriptionID);

        /* --- COPY SEMANTICS --- */
        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        InputManager(InputManager&&) = delete;
        InputManager& operator=(InputManager&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~InputManager() = default;

    protected:
        InputManager() = default;

        enum class InputAction : bool
        {
            Release = false,
            Press = true
        };

        constexpr static uint32 KEY_COUNT = static_cast<uint32>(Key::RightSystem) + 1;
        [[nodiscard]] constexpr static uint32 GetKeyIndex(const Key key) { return static_cast<uint32>(key); }

        constexpr static uint32 MOUSE_BUTTON_COUNT = static_cast<uint32>(MouseButton::Extra2) + 1;
        [[nodiscard]] constexpr static uint32 GetMouseButtonIndex(const MouseButton mouseButton) { return static_cast<uint32>(mouseButton); }

        [[nodiscard]] EventDispatcher<KeyPressEvent>& GetKeyPressDispatcher() { return keyPressDispatcher; }
        [[nodiscard]] EventDispatcher<KeyReleaseEvent>& GetKeyReleaseDispatcher() { return keyReleaseDispatcher; }

        [[nodiscard]] EventDispatcher<MouseButtonPressEvent>& GetMouseButtonPressDispatcher() { return mouseButtonPressDispatcher; }
        [[nodiscard]] EventDispatcher<MouseButtonReleaseEvent>& GetMouseButtonReleaseDispatcher() { return mouseButtonReleaseDispatcher; }
        [[nodiscard]] EventDispatcher<MouseScrollEvent>& GetMouseScrollDispatcher() { return mouseScrollDispatcher; }

    private:
        EventDispatcher<KeyPressEvent> keyPressDispatcher;
        EventDispatcher<KeyReleaseEvent> keyReleaseDispatcher;

        EventDispatcher<MouseButtonPressEvent> mouseButtonPressDispatcher;
        EventDispatcher<MouseButtonReleaseEvent> mouseButtonReleaseDispatcher;
        EventDispatcher<MouseScrollEvent> mouseScrollDispatcher;

        [[nodiscard]] bool IsKeyCombinationPressedImplementation(const std::initializer_list<Key>& keys) const;
        [[nodiscard]] bool IsKeyCombinationHeldImplementation(const std::initializer_list<Key>& keys) const;

        [[nodiscard]] bool IsMouseButtonCombinationPressedImplementation(const std::initializer_list<MouseButton>& mouseButtons) const;
        [[nodiscard]] bool IsMouseButtonCombinationHeldImplementation(const std::initializer_list<MouseButton>& mouseButtons) const;

    };

    template<> inline EventSubscriptionID InputManager::AddEventListener<KeyPressEvent>(const EventCallback<KeyPressEvent>& Callback) { return keyPressDispatcher.Subscribe(Callback); }
    template<> inline bool InputManager::RemoveEventListener<KeyPressEvent>(const EventSubscriptionID ID) { return keyPressDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID InputManager::AddEventListener<KeyReleaseEvent>(const EventCallback<KeyReleaseEvent>& Callback) { return keyReleaseDispatcher.Subscribe(Callback); }
    template<> inline bool InputManager::RemoveEventListener<KeyReleaseEvent>(const EventSubscriptionID ID) { return keyReleaseDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID InputManager::AddEventListener<MouseButtonPressEvent>(const EventCallback<MouseButtonPressEvent>& Callback) { return mouseButtonPressDispatcher.Subscribe(Callback); }
    template<> inline bool InputManager::RemoveEventListener<MouseButtonPressEvent>(const EventSubscriptionID ID) { return mouseButtonPressDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID InputManager::AddEventListener<MouseButtonReleaseEvent>(const EventCallback<MouseButtonReleaseEvent>& Callback) { return mouseButtonReleaseDispatcher.Subscribe(Callback); }
    template<> inline bool InputManager::RemoveEventListener<MouseButtonReleaseEvent>(const EventSubscriptionID ID) { return mouseButtonReleaseDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID InputManager::AddEventListener<MouseScrollEvent>(const EventCallback<MouseScrollEvent>& Callback) { return mouseScrollDispatcher.Subscribe(Callback); }
    template<> inline bool InputManager::RemoveEventListener<MouseScrollEvent>(const EventSubscriptionID ID) { return mouseScrollDispatcher.Unsubscribe(ID); }


}
