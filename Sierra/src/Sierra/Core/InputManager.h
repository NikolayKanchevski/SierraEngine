//
// Created by Nikolay Kanchevski on 9.16.2023.
//

#pragma once

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

        /* --- POLLING METHODS --- */
        virtual void OnUpdate();

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual bool IsKeyPressed(Key key);
        [[nodiscard]] virtual bool IsKeyHeld(Key key);
        [[nodiscard]] virtual bool IsKeyReleased(Key key);
        [[nodiscard]] virtual bool IsKeyResting(Key key);

        template<typename ...Args>
        [[nodiscard]] inline bool IsKeyCombinationPressed(Key first, Key second, Args... rest) { return IsKeyCombinationPressedImplementation({ first, second, rest... }); };
        template<typename ...Args>
        [[nodiscard]] inline bool IsKeyCombinationHeld(Key first, Key second, Args... rest) { return IsKeyCombinationHeldImplementation({ first, second, rest... }); };

        [[nodiscard]] virtual bool IsMouseButtonPressed(MouseButton key);
        [[nodiscard]] virtual bool IsMouseButtonHeld(MouseButton key);
        [[nodiscard]] virtual bool IsMouseButtonReleased(MouseButton key);
        [[nodiscard]] virtual bool IsMouseButtonResting(MouseButton key);
        [[nodiscard]] virtual Vector2 GetMouseScroll();

        /* --- EVENTS --- */
        template<typename T> void OnEvent(InputEventCallback<T>) { static_assert(std::is_base_of_v<InputEvent, T> && !std::is_same_v<InputEvent, T>, "Template function accepts derived input events only!"); }

        /* --- DESTRUCTORS --- */
        virtual ~InputManager() = default;

        /* --- OPERATORS --- */
        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;

    protected:
        [[nodiscard]] inline const EventDispatcher<KeyPressEvent>& GetKeyPressDispatcher() const { return keyPressDispatcher; }
        [[nodiscard]] inline const EventDispatcher<KeyReleaseEvent>& GetKeyReleaseDispatcher() const { return keyReleaseDispatcher; }

        [[nodiscard]] inline const EventDispatcher<MouseButtonPressEvent>& GetMouseButtonPressDispatcher() const { return mouseButtonPressDispatcher; }
        [[nodiscard]] inline const EventDispatcher<MouseButtonReleaseEvent>& GetMouseButtonReleaseDispatcher() const { return mouseButtonReleaseDispatcher; }
        [[nodiscard]] inline const EventDispatcher<MouseScrollEvent>& GetMouseScrollDispatcher() const { return mouseScrollDispatcher; }

        constexpr static auto KEY_COUNT = static_cast<std::underlying_type<Key>::type>(Key::RightSystem) + 1;
        constexpr static auto MOUSE_BUTTON_COUNT = static_cast<std::underlying_type<MouseButton>::type>(MouseButton::Extra2) + 1;

        typedef enum
        {
            KEY_MODIFIER_FLAGS_NONE             = 0,
            KEY_MODIFIER_FLAGS_SHIFT            = 1 << 0,
            KEY_MODIFIER_FLAGS_CONTROL          = 1 << 1,
            KEY_MODIFIER_FLAGS_ALT              = 1 << 2,
            KEY_MODIFIER_FLAGS_CAPS_LOCK        = 1 << 3,
            KEY_MODIFIER_FLAGS_NUM_LOCK         = 1 << 4,
            KEY_MODIFIER_FLAGS_SYSTEM           = 1 << 5
        } KeyModifierFlagBits;
        typedef uint8 KeyModifierFlags;

        enum class InputAction : bool
        {
            Release = 0,
            Press = 1
        };

    private:
        EventDispatcher<KeyPressEvent> keyPressDispatcher;
        EventDispatcher<KeyReleaseEvent> keyReleaseDispatcher;

        EventDispatcher<MouseButtonPressEvent> mouseButtonPressDispatcher;
        EventDispatcher<MouseButtonReleaseEvent> mouseButtonReleaseDispatcher;
        EventDispatcher<MouseScrollEvent> mouseScrollDispatcher;

        bool IsKeyCombinationPressedImplementation(const std::initializer_list<Key> &keys);
        bool IsKeyCombinationHeldImplementation(const std::initializer_list<Key> &keys);

    };

    template<> inline void InputManager::OnEvent<KeyPressEvent>(InputEventCallback<KeyPressEvent> Callback) { keyPressDispatcher.Subscribe(Callback); }
    template<> inline void InputManager::OnEvent<KeyReleaseEvent>(InputEventCallback<KeyReleaseEvent> Callback) { keyReleaseDispatcher.Subscribe(Callback); }

    template<> inline void InputManager::OnEvent<MouseButtonPressEvent>(InputEventCallback<MouseButtonPressEvent> Callback) { mouseButtonPressDispatcher.Subscribe(Callback); }
    template<> inline void InputManager::OnEvent<MouseButtonReleaseEvent>(InputEventCallback<MouseButtonReleaseEvent> Callback) { mouseButtonReleaseDispatcher.Subscribe(Callback); }
    template<> inline void InputManager::OnEvent<MouseScrollEvent>(InputEventCallback<MouseScrollEvent> Callback) { mouseScrollDispatcher.Subscribe(Callback); }

}
