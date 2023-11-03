//
// Created by Nikolay Kanchevski on 9.16.2023.
//

#pragma once

#include "Key.h"
#include "MouseButton.h"
#include "../Events/InputEvent.h"

namespace Sierra
{

    struct InputManagerCreateInfo
    {

    };

    class SIERRA_API InputManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<typename T>
        using InputEventCallback = std::function<bool(const T&)>;

        /* --- CONSTRUCTORS --- */
        explicit InputManager(const InputManagerCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual bool IsKeyPressed(Key key) const;
        [[nodiscard]] virtual bool IsKeyHeld(Key key) const;
        [[nodiscard]] virtual bool IsKeyReleased(Key key) const;
        [[nodiscard]] virtual bool IsKeyResting(Key key) const;

        template<typename ...Args>
        [[nodiscard]] inline bool IsKeyCombinationPressed(const Key first, const Key second, const Args... rest) const { return IsKeyCombinationPressedImplementation({ first, second, rest... }); };
        template<typename ...Args>
        [[nodiscard]] inline bool IsKeyCombinationHeld(const Key first, const Key second, const Args... rest) const { return IsKeyCombinationHeldImplementation({ first, second, rest... }); };

        [[nodiscard]] virtual bool IsMouseButtonPressed(MouseButton key) const;
        [[nodiscard]] virtual bool IsMouseButtonHeld(MouseButton key) const;
        [[nodiscard]] virtual bool IsMouseButtonReleased(MouseButton key) const;
        [[nodiscard]] virtual bool IsMouseButtonResting(MouseButton key) const;
        [[nodiscard]] virtual Vector2 GetMouseScroll() const;

        /* --- EVENTS --- */
        template<typename T> void OnEvent(InputEventCallback<T>) { static_assert(std::is_base_of_v<InputEvent, T> && !std::is_same_v<InputEvent, T>, "Template function accepts derived input events only!"); }

        /* --- DESTRUCTORS --- */
        virtual ~InputManager() = default;

        /* --- OPERATORS --- */
        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;

    protected:
        enum class InputAction : bool
        {
            Release = 0,
            Press = 1
        };

        constexpr static auto KEY_COUNT = static_cast<std::underlying_type<Key>::type>(Key::RightSystem) + 1;
        constexpr static auto MOUSE_BUTTON_COUNT = static_cast<std::underlying_type<MouseButton>::type>(MouseButton::Extra2) + 1;

        [[nodiscard]] inline constexpr auto GetKeyIndex(const Key key) const { return static_cast<std::underlying_type<Key>::type>(key); }
        [[nodiscard]] inline constexpr auto GetMouseButtonIndex(const MouseButton mouseButton) const { return static_cast<std::underlying_type<MouseButton>::type>(mouseButton); }

        [[nodiscard]] inline const EventDispatcher<KeyPressEvent>& GetKeyPressDispatcher() const { return keyPressDispatcher; }
        [[nodiscard]] inline const EventDispatcher<KeyReleaseEvent>& GetKeyReleaseDispatcher() const { return keyReleaseDispatcher; }

        [[nodiscard]] inline const EventDispatcher<MouseButtonPressEvent>& GetMouseButtonPressDispatcher() const { return mouseButtonPressDispatcher; }
        [[nodiscard]] inline const EventDispatcher<MouseButtonReleaseEvent>& GetMouseButtonReleaseDispatcher() const { return mouseButtonReleaseDispatcher; }
        [[nodiscard]] inline const EventDispatcher<MouseScrollEvent>& GetMouseScrollDispatcher() const { return mouseScrollDispatcher; }

    private:
        EventDispatcher<KeyPressEvent> keyPressDispatcher;
        EventDispatcher<KeyReleaseEvent> keyReleaseDispatcher;

        EventDispatcher<MouseButtonPressEvent> mouseButtonPressDispatcher;
        EventDispatcher<MouseButtonReleaseEvent> mouseButtonReleaseDispatcher;
        EventDispatcher<MouseScrollEvent> mouseScrollDispatcher;

        bool IsKeyCombinationPressedImplementation(const std::initializer_list<Key> &keys) const;
        bool IsKeyCombinationHeldImplementation(const std::initializer_list<Key> &keys) const;

    };

    template<> inline void InputManager::OnEvent<KeyPressEvent>(InputEventCallback<KeyPressEvent> Callback) { keyPressDispatcher.Subscribe(Callback); }
    template<> inline void InputManager::OnEvent<KeyReleaseEvent>(InputEventCallback<KeyReleaseEvent> Callback) { keyReleaseDispatcher.Subscribe(Callback); }

    template<> inline void InputManager::OnEvent<MouseButtonPressEvent>(InputEventCallback<MouseButtonPressEvent> Callback) { mouseButtonPressDispatcher.Subscribe(Callback); }
    template<> inline void InputManager::OnEvent<MouseButtonReleaseEvent>(InputEventCallback<MouseButtonReleaseEvent> Callback) { mouseButtonReleaseDispatcher.Subscribe(Callback); }
    template<> inline void InputManager::OnEvent<MouseScrollEvent>(InputEventCallback<MouseScrollEvent> Callback) { mouseScrollDispatcher.Subscribe(Callback); }

}
