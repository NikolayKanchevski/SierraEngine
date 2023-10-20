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
        explicit Win32InputManager(const InputManagerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void OnUpdate() override;

        /* --- GETTER METHODS --- */
        bool IsKeyPressed(Key key) override;
        bool IsKeyHeld(Key key) override;
        bool IsKeyReleased(Key key) override;
        bool IsKeyResting(Key key) override;

        bool IsMouseButtonPressed(MouseButton mouseButton) override;
        bool IsMouseButtonHeld(MouseButton mouseButton) override;
        bool IsMouseButtonReleased(MouseButton mouseButton) override;
        bool IsMouseButtonResting(MouseButton mouseButton) override;
        Vector2 GetMouseScroll() override;

        /* --- EVENTS --- */
        void KeyMessage(UINT message, WPARAM wParam, LPARAM lParam);
        void MouseButtonMessage(UINT message, WPARAM wParam, LPARAM lParam);
        void MouseWheelMessage(UINT message, WPARAM wParam, LPARAM lParam);

    private:
        constexpr static Key KEY_TABLE[]
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

        InputAction lastKeyStates[KEY_COUNT] { };
        InputAction keyStates[KEY_COUNT] { };

        InputAction lastMouseButtonStates[MOUSE_BUTTON_COUNT] { };
        InputAction mouseButtonStates[MOUSE_BUTTON_COUNT] { };

        Vector2 mouseScroll = { 0, 0 };

    };

}
