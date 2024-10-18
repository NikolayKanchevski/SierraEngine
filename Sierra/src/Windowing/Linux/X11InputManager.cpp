//
// Created by Nikolay Kanchevski on 10.23.23.
//

#include "X11InputManager.h"

#define Button8 (Button5 + 3)
#define Button9 (Button5 + 4)

namespace Sierra
{
    /* --- CONSTRUCTORS --- */

    X11InputManager::X11InputManager(const XkbExtension& xkbExtension)
        : xkbExtension(xkbExtension)
    {

    }

    /* --- GETTER METHODS --- */

    bool X11InputManager::IsKeyPressed(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool X11InputManager::IsKeyHeld(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool X11InputManager::IsKeyReleased(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool X11InputManager::IsKeyResting(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool X11InputManager::IsMouseButtonPressed(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool X11InputManager::IsMouseButtonHeld(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool X11InputManager::IsMouseButtonReleased(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    bool X11InputManager::IsMouseButtonResting(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    Vector2 X11InputManager::GetMouseScroll() const noexcept
    {
        return mouseScroll;
    }

    WindowingBackendType X11InputManager::GetBackendType() const noexcept
    {
        return WindowingBackendType::X11;
    }

    /* --- POLLING METHODS --- */

    void X11InputManager::RegisterKeyPress(const Key key)
    {
        if (key == Key::Unknown) return;
        keyStates[GetKeyIndex(key)] = InputAction::Press;
        GetKeyPressDispatcher().DispatchEvent(key);
    }

    void X11InputManager::RegisterKeyRelease(const Key key)
    {
        if (key == Key::Unknown) return;
        keyStates[GetKeyIndex(key)] = InputAction::Release;
        GetKeyReleaseDispatcher().DispatchEvent(key);
    }

    void X11InputManager::RegisterMouseButtonPress(const MouseButton mouseButton)
    {
        if (mouseButton == MouseButton::Unknown) return;
        mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
        GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
    }

    void X11InputManager::RegisterMouseButtonRelease(const MouseButton mouseButton)
    {
        if (mouseButton == MouseButton::Unknown) return;
        mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
        GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
    }

    void X11InputManager::RegisterMouseScroll(Vector2 scroll)
    {
        mouseScroll = scroll;
    }

    /* --- CONVERSIONS --- */

    Key X11InputManager::XKeyCodeToKey(const uint keyCode)
    {
        return xkbExtension.XKeyCodeToKey(keyCode);
    }

    MouseButton X11InputManager::XButtonToMouseButton(const uint button)
    {
        switch (button)
        {
            case Button1:       return MouseButton::Left;
            case Button2:       return MouseButton::Middle;
            case Button3:       return MouseButton::Right;
            case Button8:       return MouseButton::Extra1;
            case Button9:       return MouseButton::Extra2;
            default:            break;
        }

        return MouseButton::Unknown;
    }

    /* --- PRIVATE METHODS --- */

    void X11InputManager::Update()
    {
        std::copy(keyStates.begin(), keyStates.end(), lastKeyStates.begin());
        std::copy(mouseButtonStates.begin(), mouseButtonStates.end(), lastMouseButtonStates.begin());
        mouseScroll = { 0, 0 };
    }

}