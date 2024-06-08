//
// Created by Nikolay Kanchevski on 9.16.2023.
//

#include "InputManager.h"

namespace Sierra
{

    /* --- POLLING METHODS --- */

    void InputManager::RegisterKeyPress(const Key)
    {

    }

    void InputManager::RegisterKeyRelease(const Key)
    {

    }

    void InputManager::RegisterMouseButtonPress(const MouseButton)
    {

    }

    void InputManager::RegisterMouseButtonRelease(const MouseButton)
    {

    }

    void InputManager::RegisterMouseScroll(const Vector2)
    {

    }

    /* --- GETTER METHODS --- */

    bool InputManager::IsKeyPressed(const Key) const
    {
        return false;
    }

    bool InputManager::IsKeyHeld(const Key) const
    {
        return false;
    }

    bool InputManager::IsKeyReleased(const Key) const
    {
        return false;
    }

    bool InputManager::IsKeyResting(const Key) const
    {
        return false;
    }

    bool InputManager::IsMouseButtonPressed(const MouseButton) const
    {
        return false;
    }

    bool InputManager::IsMouseButtonHeld(const MouseButton) const
    {
        return false;
    }

    bool InputManager::IsMouseButtonReleased(const MouseButton) const
    {
        return false;
    }

    bool InputManager::IsMouseButtonResting(const MouseButton) const
    {
        return false;
    }

    Vector2 InputManager::GetMouseScroll() const
    {
        return { 0, 0 };
    }

    /* --- PRIVATE METHODS --- */

    bool InputManager::IsKeyCombinationPressedImplementation(const std::initializer_list<Key> &keys) const
    {
        bool atLeastOneKeyPressed = false;
        for (const Key key : keys)
        {
            if (IsKeyHeld(key)) continue;

            if (IsKeyPressed(key))
            {
                atLeastOneKeyPressed = true;
                continue;
            }

            return false;
        }

        return atLeastOneKeyPressed;
    }

    bool InputManager::IsKeyCombinationHeldImplementation(const std::initializer_list<Key> &keys) const
    {
        return std::ranges::all_of(keys, [this](const Key key) -> bool { return IsKeyHeld(key); });
    }

    bool InputManager::IsMouseButtonCombinationPressedImplementation(const std::initializer_list<MouseButton> &mouseButtons) const
    {
        bool atLeastOneMouseButtonPressed = false;
        for (const MouseButton mouseButton : mouseButtons)
        {
            if (IsMouseButtonHeld(mouseButton)) continue;

            if (IsMouseButtonPressed(mouseButton))
            {
                atLeastOneMouseButtonPressed = true;
                continue;
            }

            return false;
        }

        return atLeastOneMouseButtonPressed;
    }

    bool InputManager::IsMouseButtonCombinationHeldImplementation(const std::initializer_list<MouseButton> &mouseButtons) const
    {
        return std::ranges::all_of(mouseButtons, [this](const MouseButton mouseButton) -> bool { return IsMouseButtonHeld(mouseButton); });
    }

}