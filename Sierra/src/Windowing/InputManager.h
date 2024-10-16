//
// Created by Nikolay Kanchevski on 9.16.2023.
//

#pragma once

#include "WindowingBackendType.h"

#include "InputEvents.h"
#include "../Events/EventDispatcher.hpp"

namespace Sierra
{

    template<typename T>
    concept InputEventType = std::is_base_of_v<InputEvent, T> && !std::is_same_v<InputEvent, std::decay_t<T>>;

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
        [[nodiscard]] virtual bool IsKeyPressed(Key key) const noexcept = 0;
        [[nodiscard]] virtual bool IsKeyHeld(Key key) const noexcept = 0;
        [[nodiscard]] virtual bool IsKeyReleased(Key key) const noexcept = 0;
        [[nodiscard]] virtual bool IsKeyResting(Key key) const noexcept = 0;

        template<typename... Args>
        [[nodiscard]] bool IsKeyCombinationPressed(const Key first, const Args... rest) const noexcept { return IsKeyCombinationPressedImplementation({ first, rest... }); }

        template<typename... Args>
        [[nodiscard]] bool IsKeyCombinationHeld(const Key first, const Args... rest) const noexcept { return IsKeyCombinationHeldImplementation({ first, rest... }); }

        [[nodiscard]] virtual bool IsMouseButtonPressed(MouseButton mouseButton) const noexcept = 0;
        [[nodiscard]] virtual bool IsMouseButtonHeld(MouseButton mouseButton) const noexcept = 0;
        [[nodiscard]] virtual bool IsMouseButtonReleased(MouseButton mouseButton) const noexcept = 0;
        [[nodiscard]] virtual bool IsMouseButtonResting(MouseButton mouseButton) const noexcept = 0;
        [[nodiscard]] virtual Vector2 GetMouseScroll() const noexcept = 0;

        template<typename... Args>
        [[nodiscard]] bool IsMouseButtonCombinationPressed(const MouseButton first, const Args... rest) const noexcept { return IsMouseButtonCombinationPressedImplementation({ first, rest... }); }
        template<typename... Args>
        [[nodiscard]] bool IsMouseButtonCombinationHeld(const MouseButton first, const Args... rest) const noexcept { return IsMouseButtonCombinationHeldImplementation({ first, rest... }); }

        [[nodiscard]] virtual WindowingBackendType GetBackendType() const noexcept = 0;

        /* --- EVENTS --- */
        template<InputEventType EventType>
        EventSubscriptionID AddEventListener(const EventCallback<EventType>&);

        template<InputEventType EventType>
        bool RemoveEventListener(EventSubscriptionID) noexcept;

        /* --- COPY SEMANTICS --- */
        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        InputManager(InputManager&&) = delete;
        InputManager& operator=(InputManager&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~InputManager() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        InputManager() noexcept = default;

        enum class InputAction : bool
        {
            Release = false,
            Press = true
        };

        /* --- CONSTANTS --- */
        constexpr static uint32 KEY_COUNT = static_cast<uint32>(Key::RightSystem) + 1;
        constexpr static uint32 MOUSE_BUTTON_COUNT = static_cast<uint32>(MouseButton::Extra2) + 1;

        /* --- GETTER METHODS --- */
        [[nodiscard]] constexpr static uint32 GetKeyIndex(const Key key) { return static_cast<uint32>(key); }
        [[nodiscard]] constexpr static uint32 GetMouseButtonIndex(const MouseButton mouseButton) { return static_cast<uint32>(mouseButton); }

        [[nodiscard]] EventDispatcher<KeyPressEvent>& GetKeyPressDispatcher() noexcept { return keyPressDispatcher; }
        [[nodiscard]] EventDispatcher<KeyReleaseEvent>& GetKeyReleaseDispatcher() noexcept { return keyReleaseDispatcher; }

        [[nodiscard]] EventDispatcher<MouseButtonPressEvent>& GetMouseButtonPressDispatcher() noexcept { return mouseButtonPressDispatcher; }
        [[nodiscard]] EventDispatcher<MouseButtonReleaseEvent>& GetMouseButtonReleaseDispatcher() noexcept { return mouseButtonReleaseDispatcher; }
        [[nodiscard]] EventDispatcher<MouseScrollEvent>& GetMouseScrollDispatcher() noexcept { return mouseScrollDispatcher; }

    private:
        EventDispatcher<KeyPressEvent> keyPressDispatcher = { };
        EventDispatcher<KeyReleaseEvent> keyReleaseDispatcher = { };

        EventDispatcher<MouseButtonPressEvent> mouseButtonPressDispatcher = { };
        EventDispatcher<MouseButtonReleaseEvent> mouseButtonReleaseDispatcher = { };
        EventDispatcher<MouseScrollEvent> mouseScrollDispatcher = { };

        [[nodiscard]] bool IsKeyCombinationPressedImplementation(const std::initializer_list<Key>& keys) const;
        [[nodiscard]] bool IsKeyCombinationHeldImplementation(const std::initializer_list<Key>& keys) const;

        [[nodiscard]] bool IsMouseButtonCombinationPressedImplementation(const std::initializer_list<MouseButton>& mouseButtons) const;
        [[nodiscard]] bool IsMouseButtonCombinationHeldImplementation(const std::initializer_list<MouseButton>& mouseButtons) const;

    };

    template<> inline EventSubscriptionID InputManager::AddEventListener<KeyPressEvent>(const EventCallback<KeyPressEvent>& Callback) { return keyPressDispatcher.Subscribe(Callback); }
    template<> inline bool InputManager::RemoveEventListener<KeyPressEvent>(const EventSubscriptionID ID) noexcept { return keyPressDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID InputManager::AddEventListener<KeyReleaseEvent>(const EventCallback<KeyReleaseEvent>& Callback) { return keyReleaseDispatcher.Subscribe(Callback); }
    template<> inline bool InputManager::RemoveEventListener<KeyReleaseEvent>(const EventSubscriptionID ID) noexcept { return keyReleaseDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID InputManager::AddEventListener<MouseButtonPressEvent>(const EventCallback<MouseButtonPressEvent>& Callback) { return mouseButtonPressDispatcher.Subscribe(Callback); }
    template<> inline bool InputManager::RemoveEventListener<MouseButtonPressEvent>(const EventSubscriptionID ID) noexcept { return mouseButtonPressDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID InputManager::AddEventListener<MouseButtonReleaseEvent>(const EventCallback<MouseButtonReleaseEvent>& Callback) { return mouseButtonReleaseDispatcher.Subscribe(Callback); }
    template<> inline bool InputManager::RemoveEventListener<MouseButtonReleaseEvent>(const EventSubscriptionID ID) noexcept { return mouseButtonReleaseDispatcher.Unsubscribe(ID); }

    template<> inline EventSubscriptionID InputManager::AddEventListener<MouseScrollEvent>(const EventCallback<MouseScrollEvent>& Callback) { return mouseScrollDispatcher.Subscribe(Callback); }
    template<> inline bool InputManager::RemoveEventListener<MouseScrollEvent>(const EventSubscriptionID ID) noexcept { return mouseScrollDispatcher.Unsubscribe(ID); }

}
