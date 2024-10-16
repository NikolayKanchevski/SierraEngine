//
// Created by Nikolay Kanchevski on 10.17.2023.
//

#include "Win32InputManager.h"

namespace Sierra
{

    namespace
    {
        /* === Reference: https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes === */
        constexpr std::array<Key, 223> KEY_TABLE
        {
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Backspace,
            Key::Tab,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Enter,
            Key::Unknown,
            Key::Unknown,
            Key::LeftShift,
            Key::LeftControl,
            Key::LeftAlt,
            Key::Pause,
            Key::CapsLock,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Escape,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Space,
            Key::PageUp,
            Key::PageDown,
            Key::End,
            Key::Home,
            Key::LeftArrow,
            Key::UpArrow,
            Key::RightArrow,
            Key::DownArrow,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::PrintScreen,
            Key::Insert,
            Key::Delete,
            Key::Unknown,
            Key::Number0,
            Key::Number1,
            Key::Number2,
            Key::Number3,
            Key::Number4,
            Key::Number5,
            Key::Number6,
            Key::Number7,
            Key::Number8,
            Key::Number9,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::A,
            Key::B,
            Key::C,
            Key::D,
            Key::E,
            Key::F,
            Key::G,
            Key::H,
            Key::I,
            Key::J,
            Key::K,
            Key::L,
            Key::M,
            Key::N,
            Key::O,
            Key::P,
            Key::Q,
            Key::R,
            Key::S,
            Key::T,
            Key::U,
            Key::V,
            Key::W,
            Key::X,
            Key::Y,
            Key::Z,
            Key::LeftWindows,
            Key::RightWindows,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::KeypadNumber0,
            Key::KeypadNumber1,
            Key::KeypadNumber2,
            Key::KeypadNumber3,
            Key::KeypadNumber4,
            Key::KeypadNumber5,
            Key::KeypadNumber6,
            Key::KeypadNumber7,
            Key::KeypadNumber8,
            Key::KeypadNumber9,
            Key::KeypadMultiply,
            Key::KeypadAdd,
            Key::Unknown,
            Key::KeypadSubtract,
            Key::KeypadDecimal,
            Key::KeypadDivide,
            Key::F1,
            Key::F2,
            Key::F3,
            Key::F4,
            Key::F5,
            Key::F6,
            Key::F7,
            Key::F8,
            Key::F9,
            Key::F10,
            Key::F11,
            Key::F12,
            Key::F13,
            Key::F14,
            Key::F15,
            Key::F16,
            Key::F17,
            Key::F18,
            Key::F19,
            Key::F20,
            Key::F21,
            Key::F22,
            Key::F23,
            Key::F24,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::NumpadLock,
            Key::ScrollLock,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::LeftShift,
            Key::RightShift,
            Key::LeftControl,
            Key::RightShift,
            Key::LeftAlt,
            Key::RightAlt,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Semicolon,
            Key::Equals,
            Key::Comma,
            Key::Minus,
            Key::Period,
            Key::Slash,
            Key::Grave,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::LeftBracket,
            Key::Backslash,
            Key::RightBracket,
            Key::Apostrophe
        };

        constexpr std::array<MouseButton, 6> MOUSE_BUTTON_TABLE
        {
            MouseButton::Unknown,
            MouseButton::Left,
            MouseButton::Right,
            MouseButton::Unknown,
            MouseButton::Extra1,
            MouseButton::Extra2
        };
    }

    /* --- CONVERSIONS --- */

    Key VirtualKeyCodeToKey(const UINT keyCode)
    {
        if (keyCode >= KEY_TABLE.size()) return Key::Unknown;
        return KEY_TABLE[keyCode];
    }

    MouseButton VirtualKeyCodeToMouseButton(const UINT keyCode)
    {
        if (keyCode >= MOUSE_BUTTON_TABLE.size()) return MouseButton::Unknown;
        return MOUSE_BUTTON_TABLE[keyCode];
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

    bool Win32InputManager::IsKeyPressed(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool Win32InputManager::IsKeyHeld(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool Win32InputManager::IsKeyReleased(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool Win32InputManager::IsKeyResting(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool Win32InputManager::IsMouseButtonPressed(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool Win32InputManager::IsMouseButtonHeld(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool Win32InputManager::IsMouseButtonReleased(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    bool Win32InputManager::IsMouseButtonResting(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    Vector2 Win32InputManager::GetMouseScroll() const noexcept
    {
        return mouseScroll;
    }

    WindowingBackendType Win32InputManager::GetBackendType() const noexcept
    {
        return WindowingBackendType::Win32;
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