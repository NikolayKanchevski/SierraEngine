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

    /* --- POLLING METHODS --- */

    void InputManager::OnUpdate()
    {

    }

    /* --- GETTER METHODS --- */

    bool InputManager::IsKeyPressed(const Key)
    {
        return false;
    }

    bool InputManager::IsKeyHeld(const Key)
    {
        return false;
    }

    bool InputManager::IsKeyReleased(const Key)
    {
        return false;
    }

    bool InputManager::IsKeyResting(const Key)
    {
        return false;
    }

    bool InputManager::IsMouseButtonPressed(const MouseButton)
    {
        return false;
    }

    bool InputManager::IsMouseButtonHeld(const MouseButton)
    {
        return false;
    }

    bool InputManager::IsMouseButtonReleased(const MouseButton)
    {
        return false;
    }

    bool InputManager::IsMouseButtonResting(const MouseButton)
    {
        return false;
    }

    Vector2 InputManager::GetMouseScroll()
    {
        return { 0, 0 };
    }

    /* --- PRIVATE METHODS --- */

    bool InputManager::IsKeyCombinationPressedImplementation(const std::initializer_list<Key> &keys)
    {
        bool atLeastOneKeyPressed = false;
        for (const auto key : keys)
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

    bool InputManager::IsKeyCombinationHeldImplementation(const std::initializer_list<Key> &keys)
    {
        return std::ranges::all_of(keys, [this](const auto key) {
            return IsKeyHeld(key);
        });
    }

}