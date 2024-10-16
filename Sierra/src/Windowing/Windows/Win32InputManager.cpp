//
// Created by Nikolay Kanchevski on 10.17.2023.
//

#include "Win32InputManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Win32InputManager::Win32InputManager()
        : InputManager()
    {

    }

    /* --- POLLING METHODS --- */

    void Win32InputManager::RegisterKeyPress(const Key key)
    {
        if (key == Key::Unknown) return;
        keyStates[GetKeyIndex(key)] = InputAction::Press;
        GetKeyPressDispatcher().DispatchEvent(key);
    }

    void Win32InputManager::RegisterKeyRelease(const Key key)
    {
        if (key == Key::Unknown) return;
        keyStates[GetKeyIndex(key)] = InputAction::Release;
        GetKeyReleaseDispatcher().DispatchEvent(key);
    }

    void Win32InputManager::RegisterMouseButtonPress(const MouseButton mouseButton)
    {
        if (mouseButton == MouseButton::Unknown) return;
        mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
        GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
    }

    void Win32InputManager::RegisterMouseButtonRelease(const MouseButton mouseButton)
    {
        if (mouseButton == MouseButton::Unknown) return;
        mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
        GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
    }

    void Win32InputManager::RegisterMouseScroll(const Vector2 scroll)
    {
        mouseScroll = scroll;
    }

    /* --- GETTER METHODS --- */

    bool Win32InputManager::IsKeyPressed(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool Win32InputManager::IsKeyHeld(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool Win32InputManager::IsKeyReleased(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool Win32InputManager::IsKeyResting(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool Win32InputManager::IsMouseButtonPressed(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool Win32InputManager::IsMouseButtonHeld(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool Win32InputManager::IsMouseButtonReleased(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    bool Win32InputManager::IsMouseButtonResting(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    Vector2 Win32InputManager::GetMouseScroll() const
    {
        return mouseScroll;
    }

    /* --- CONVERSIONS --- */

    Key Win32InputManager::VirtualKeyCodeToKey(const UINT keyCode)
    {
        if (keyCode >= KEY_TABLE.size()) return Key::Unknown;
        return KEY_TABLE[keyCode];
    }

    MouseButton Win32InputManager::VirtualKeyCodeToMouseButton(const UINT keyCode)
    {
        if (keyCode >= MOUSE_BUTTON_TABLE.size()) return MouseButton::Unknown;
        return MOUSE_BUTTON_TABLE[keyCode];
    }

    /* --- PRIVATE METHODS --- */

    void Win32InputManager::Update()
    {
        // Swap out current key/mouse states and move them to the array for the last frame
        std::copy(keyStates.begin(), keyStates.end(), lastKeyStates.begin());
        std::copy(mouseButtonStates.begin(), mouseButtonStates.end(), lastMouseButtonStates.begin());
        mouseScroll = { 0, 0 };
    }

}