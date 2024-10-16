//
// Created by Nikolay Kanchevski on 9.16.2023.
//

#include "InputManager.h"

namespace Sierra
{



    /* --- CONVERSIONS --- */


    /* --- PRIVATE METHODS --- */

    bool InputManager::IsKeyCombinationPressedImplementation(const std::initializer_list<Key>& keys) const
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

    bool InputManager::IsKeyCombinationHeldImplementation(const std::initializer_list<Key>& keys) const
    {
        return std::ranges::all_of(keys, [this](const Key key) -> bool { return IsKeyHeld(key); });
    }

    bool InputManager::IsMouseButtonCombinationPressedImplementation(const std::initializer_list<MouseButton>& mouseButtons) const
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

    bool InputManager::IsMouseButtonCombinationHeldImplementation(const std::initializer_list<MouseButton>& mouseButtons) const
    {
        return std::ranges::all_of(mouseButtons, [this](const MouseButton mouseButton) -> bool { return IsMouseButtonHeld(mouseButton); });
    }

}