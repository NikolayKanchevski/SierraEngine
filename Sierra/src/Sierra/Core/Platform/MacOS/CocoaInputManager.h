//
// Created by Nikolay Kanchevski on 19.09.23.
//

#pragma once

#if !SR_PLATFORM_MACOS
    #error "Including the CocoaInputManager.h file is only allowed in macOS builds!"
#endif

#if defined(__OBJC__)
    #include <Cocoa/Cocoa.h>
#endif
#include "../../InputManager.h"

namespace Sierra
{

    class SIERRA_API CocoaInputManager final : public InputManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit CocoaInputManager(const InputManagerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void OnUpdate();

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

    private:
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
            Key::RightCommand,
            Key::LeftCommand,
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

        std::array<InputAction, KEY_COUNT> lastKeyStates { };
        std::array<InputAction, KEY_COUNT> keyStates { };

        std::array<InputAction, MOUSE_BUTTON_COUNT> lastMouseButtonStates { };
        std::array<InputAction, MOUSE_BUTTON_COUNT> mouseButtonStates { };

        Vector2 mouseScroll = { 0, 0 };

        friend class CocoaWindow;
        #if defined(__OBJC__)
            void KeyDown(const NSEvent* event);
            void FlagsChanged(const NSEvent* event);
            void KeyUp(const NSEvent* event);

            void MouseDown(const NSEvent* event);
            void RightMouseDown(const NSEvent* event);
            void OtherMouseDown(const NSEvent* event);
            void MouseUp(const NSEvent* event);
            void RightMouseUp(const NSEvent* event);
            void OtherMouseUp(const NSEvent* event);
            void ScrollWheel(const NSEvent* event);
        #endif

    };

}
