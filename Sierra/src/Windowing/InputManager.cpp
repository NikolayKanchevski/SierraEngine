//
// Created by Nikolay Kanchevski on 9.16.2023.
//

#include "InputManager.h"

namespace Sierra
{

    /* --- GETTER METHODS --- */

    bool InputManager::IsKeyCombinationPressed(const std::span<const Key> keys) const
    {
        bool atLeastOneKeyPressed = false;
        for (const Key key : keys)
        {
            const Key alternativeKey = KeyToAlternativeKey(key);

            if (IsKeyHeld(key) || (alternativeKey != Key::Unknown && IsKeyHeld(alternativeKey))) continue;

            if (IsKeyPressed(key) || (alternativeKey != Key::Unknown && IsKeyPressed(alternativeKey)))
            {
                atLeastOneKeyPressed = true;
                continue;
            }

            return false;
        }

        return atLeastOneKeyPressed;
    }

    bool InputManager::IsKeyCombinationHeld(const std::span<const Key> keys) const
    {
        return std::ranges::all_of(keys, [this](const Key key) -> bool
        {
            const Key alternativeKey = KeyToAlternativeKey(key);
            return IsKeyHeld(key) || (alternativeKey != Key::Unknown && IsKeyHeld(alternativeKey));
        });
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