//
// Created by Nikolay Kanchevski on 9.16.2023.
//

#include "InputManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    InputManager::InputManager(const InputManagerCreateInfo &createInfo)
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
        for (const Key key : keys)
        {
            if (!IsKeyHeld(key))
            {
                return false;
            }
        }
        return true;
    }

}