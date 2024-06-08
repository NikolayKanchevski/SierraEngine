//
// Created by Nikolay Kanchevski on 19.09.23.
//

#include "CocoaInputManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaInputManager::CocoaInputManager()
        : InputManager()
    {

    }

    /* --- POLLING METHODS --- */

    void CocoaInputManager::RegisterKeyPress(const Key key)
    {
        if (key == Key::Unknown) return;
        keyStates[GetKeyIndex(key)] = InputAction::Press;
        GetKeyPressDispatcher().DispatchEvent(key);
    }

    void CocoaInputManager::RegisterKeyRelease(const Key key)
    {
        if (key == Key::Unknown) return;
        keyStates[GetKeyIndex(key)] = InputAction::Release;
        GetKeyReleaseDispatcher().DispatchEvent(key);
    }

    void CocoaInputManager::RegisterMouseButtonPress(const MouseButton mouseButton)
    {
        if (mouseButton == MouseButton::Unknown) return;
        mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
        GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
    }

    void CocoaInputManager::RegisterMouseButtonRelease(const MouseButton mouseButton)
    {
        if (mouseButton == MouseButton::Unknown) return;
        mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
        GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
    }

    void CocoaInputManager::RegisterMouseScroll(const Vector2 scroll)
    {
        mouseScroll = scroll;
    }

    /* --- GETTER METHODS --- */

    bool CocoaInputManager::IsKeyPressed(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool CocoaInputManager::IsKeyHeld(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool CocoaInputManager::IsKeyReleased(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool CocoaInputManager::IsKeyResting(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool CocoaInputManager::IsMouseButtonPressed(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool CocoaInputManager::IsMouseButtonHeld(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool CocoaInputManager::IsMouseButtonReleased(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    bool CocoaInputManager::IsMouseButtonResting(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    Vector2 CocoaInputManager::GetMouseScroll() const
    {
        return mouseScroll;
    }

    /* --- CONVERSIONS --- */

    Key CocoaInputManager::KeyCodeToKey(const uint32 keyCode)
    {
        if (keyCode >= KEY_TABLE.size()) return Key::Unknown;
        return KEY_TABLE[keyCode];
    }

    MouseButton CocoaInputManager::ButtonNumberToMouseButton(const uint32 buttonNumber)
    {
        if (buttonNumber >= static_cast<uint32>(buttonNumber)) return MouseButton::Unknown;
        return static_cast<MouseButton>(buttonNumber + 1);
    }

    /* --- PRIVATE METHODS --- */

    void CocoaInputManager::Update()
    {
        std::copy(keyStates.begin(), keyStates.end(), lastKeyStates.begin());
        std::copy(mouseButtonStates.begin(), mouseButtonStates.end(), lastMouseButtonStates.begin());
        mouseScroll = { 0, 0 };
    }

}