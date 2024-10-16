//
// Created by Nikolay Kanchevski on 19.09.23.
//

#include "CocoaInputManager.h"

namespace Sierra
{

    namespace
    {
        constexpr static std::array<Key, 127> KEY_TABLE
        {
            Key::A,
            Key::S,
            Key::D,
            Key::F,
            Key::H,
            Key::G,
            Key::Z,
            Key::X,
            Key::C,
            Key::V,
            Key::Section,
            Key::B,
            Key::Q,
            Key::W,
            Key::E,
            Key::R,
            Key::Y,
            Key::T,
            Key::Number1,
            Key::Number2,
            Key::Number3,
            Key::Number4,
            Key::Number6,
            Key::Number5,
            Key::Equals,
            Key::Number9,
            Key::Number7,
            Key::Minus,
            Key::Number8,
            Key::Number0,
            Key::RightBracket,
            Key::O,
            Key::U,
            Key::LeftBracket,
            Key::I,
            Key::P,
            Key::Enter,
            Key::L,
            Key::J,
            Key::Apostrophe,
            Key::K,
            Key::Semicolon,
            Key::Backslash,
            Key::Comma,
            Key::Slash,
            Key::N,
            Key::M,
            Key::Period,
            Key::Tab,
            Key::Space,
            Key::Grave,
            Key::Backspace,
            Key::Unknown,
            Key::Escape,
            Key::LeftCommand,
            Key::RightCommand,
            Key::LeftShift,
            Key::CapsLock,
            Key::LeftOption,
            Key::LeftControl,
            Key::RightShift,
            Key::RightOption,
            Key::RightControl,
            Key::Function,
            Key::F17,
            Key::KeypadDecimal,
            Key::Unknown,
            Key::KeypadMultiply,
            Key::Unknown,
            Key::KeypadAdd,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::KeypadDivide,
            Key::KeypadEnter,
            Key::Unknown,
            Key::KeypadSubtract,
            Key::F18,
            Key::F19,
            Key::KeypadEquals,
            Key::KeypadNumber0,
            Key::KeypadNumber1,
            Key::KeypadNumber2,
            Key::KeypadNumber3,
            Key::KeypadNumber4,
            Key::KeypadNumber5,
            Key::KeypadNumber6,
            Key::KeypadNumber7,
            Key::F20,
            Key::KeypadNumber8,
            Key::KeypadNumber9,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::F5,
            Key::F6,
            Key::F7,
            Key::F3,
            Key::F8,
            Key::F9,
            Key::Unknown,
            Key::F11,
            Key::Unknown,
            Key::F13,
            Key::F16,
            Key::F14,
            Key::Unknown,
            Key::F10,
            Key::Menu,
            Key::F12,
            Key::Unknown,
            Key::F15,
            Key::Unknown,
            Key::Home,
            Key::PageUp,
            Key::Unknown,
            Key::F4,
            Key::End,
            Key::F2,
            Key::PageDown,
            Key::F1,
            Key::LeftArrow,
            Key::RightArrow,
            Key::DownArrow,
            Key::UpArrow
        };
    }

    /* --- CONVERSIONS --- */

    Key KeyCodeToKey(const uint32 keyCode) noexcept
    {
        if (keyCode >= KEY_TABLE.size()) return Key::Unknown;
        return KEY_TABLE[keyCode];
    }

    MouseButton ButtonNumberToMouseButton(const uint32 buttonNumber) noexcept
    {
        if (buttonNumber >= static_cast<uint32>(buttonNumber)) return MouseButton::Unknown;
        return static_cast<MouseButton>(buttonNumber + 1);
    }

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

    bool CocoaInputManager::IsKeyPressed(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool CocoaInputManager::IsKeyHeld(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool CocoaInputManager::IsKeyReleased(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool CocoaInputManager::IsKeyResting(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool CocoaInputManager::IsMouseButtonPressed(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool CocoaInputManager::IsMouseButtonHeld(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool CocoaInputManager::IsMouseButtonReleased(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    bool CocoaInputManager::IsMouseButtonResting(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    Vector2 CocoaInputManager::GetMouseScroll() const noexcept
    {
        return mouseScroll;
    }

    WindowingBackendType CocoaInputManager::GetBackendType() const noexcept
    {
        return WindowingBackendType::Cocoa;
    }

    /* --- PRIVATE METHODS --- */

    void CocoaInputManager::Update()
    {
        std::copy(keyStates.begin(), keyStates.end(), lastKeyStates.begin());
        std::copy(mouseButtonStates.begin(), mouseButtonStates.end(), lastMouseButtonStates.begin());
        mouseScroll = { 0, 0 };
    }

}