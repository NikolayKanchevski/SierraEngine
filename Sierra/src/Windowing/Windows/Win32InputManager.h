//
// Created by Nikolay Kanchevski on 10.17.2023.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the Win32InputManager.h file is only allowed in Windows builds!"
#endif

#include "../../InputManager.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Sierra
{

    class SIERRA_API Win32InputManager final : public InputManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Win32InputManager();

        /* --- POLLING METHODS --- */
        void RegisterKeyPress(Key key) override;
        void RegisterKeyRelease(Key key) override;

        void RegisterMouseButtonPress(MouseButton mouseButton) override;
        void RegisterMouseButtonRelease(MouseButton mouseButton) override;
        void RegisterMouseScroll(Vector2 scroll) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsKeyPressed(Key key) const override;
        [[nodiscard]] bool IsKeyHeld(Key key) const override;
        [[nodiscard]] bool IsKeyReleased(Key key) const override;
        [[nodiscard]] bool IsKeyResting(Key key) const override;

        [[nodiscard]] bool IsMouseButtonPressed(MouseButton mouseButton) const override;
        [[nodiscard]] bool IsMouseButtonHeld(MouseButton mouseButton) const override;
        [[nodiscard]] bool IsMouseButtonReleased(MouseButton mouseButton) const override;
        [[nodiscard]] bool IsMouseButtonResting(MouseButton mouseButton) const override;
        [[nodiscard]] Vector2 GetMouseScroll() const override;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static Key VirtualKeyCodeToKey(UINT keyCode);
        [[nodiscard]] static MouseButton VirtualKeyCodeToMouseButton(UINT keyCode);

    private:
        /* === Reference: https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes === */
        constexpr static std::array<Key, 223> KEY_TABLE
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

        constexpr static std::array<MouseButton, 6> MOUSE_BUTTON_TABLE
        {
            MouseButton::Unknown,
            MouseButton::Left,
            MouseButton::Right,
            MouseButton::Unknown,
            MouseButton::Extra1,
            MouseButton::Extra2
        };

        std::array<InputAction, KEY_COUNT> lastKeyStates { };
        std::array<InputAction, KEY_COUNT> keyStates { };

        std::array<InputAction, MOUSE_BUTTON_COUNT> lastMouseButtonStates { };
        std::array<InputAction, MOUSE_BUTTON_COUNT> mouseButtonStates { };

        Vector2 mouseScroll = { 0, 0 };

        friend class Win32Window;
        void Update();

    };

}
