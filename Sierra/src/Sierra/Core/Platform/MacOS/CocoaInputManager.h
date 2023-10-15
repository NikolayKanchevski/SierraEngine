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

    private:
        constexpr static Key KEY_TABLE[]
        {
            Key::A,                     //        A                     | 0
            Key::S,                     //        S                     | 1
            Key::D,                     //        D                     | 2
            Key::F,                     //        F                     | 3
            Key::H,                     //        H                     | 4
            Key::G,                     //        G                     | 5
            Key::Z,                     //        Z                     | 6
            Key::X,                     //        X                     | 7
            Key::C,                     //        C                     | 8
            Key::V,                     //        V                     | 9
            Key::Section,               //        Section Sign          | 10
            Key::B,                     //        B                     | 11
            Key::Q,                     //        Q                     | 12
            Key::W,                     //        W                     | 13
            Key::E,                     //        E                     | 14
            Key::R,                     //        R                     | 15
            Key::Y,                     //        Y                     | 16
            Key::T,                     //        T                     | 17
            Key::Number1,               //        1                     | 18
            Key::Number2,               //        2                     | 19
            Key::Number3,               //        3                     | 20
            Key::Number4,               //        4                     | 21
            Key::Number6,               //        6                     | 22
            Key::Number5,               //        5                     | 23
            Key::Equals,                //        Equals                | 24
            Key::Number9,               //        9                     | 25
            Key::Number7,               //        7                     | 26
            Key::Minus,                 //        Minus                 | 27
            Key::Number8,               //        8                     | 28
            Key::Number0,               //        0                     | 29
            Key::RightBracket,          //        Right Bracket         | 30
            Key::O,                     //        O                     | 31
            Key::U,                     //        U                     | 32
            Key::LeftBracket,           //        Left Bracket          | 33
            Key::I,                     //        I                     | 34
            Key::P,                     //        P                     | 35
            Key::Enter,                 //        Return                | 36
            Key::L,                     //        L                     | 37
            Key::J,                     //        J                     | 38
            Key::Apostrophe,            //        Quote                 | 39
            Key::K,                     //        K                     | 40
            Key::Semicolon,             //        Semicolon             | 41
            Key::Backslash,             //        Backslash             | 42
            Key::Comma,                 //        Comma                 | 43
            Key::Slash,                 //        Slash                 | 44
            Key::N,                     //        N                     | 45
            Key::M,                     //        M                     | 46
            Key::Period,                //        Period                | 47
            Key::Tab,                   //        Tab                   | 48
            Key::Space,                 //        Space                 | 49
            Key::Grave,                 //        Grave                 | 50
            Key::Backspace,             //        Backspace             | 51
            Key::Unknown,               //        Linefeed              | 52
            Key::Escape,                //        Escape                | 53
            Key::RightCommand,          //        Right Command         | 54
            Key::LeftCommand,           //        Left Command          | 55
            Key::LeftShift,             //        Shift                 | 56
            Key::CapsLock,              //        Caps Lock             | 57
            Key::LeftOption,            //        Option                | 58
            Key::LeftControl,           //        Control               | 59
            Key::RightShift,            //        Right Shift           | 60
            Key::RightOption,           //        Right Option          | 61
            Key::RightControl,          //        Right Control         | 62
            Key::Function,              //        Function              | 63
            Key::F17,                   //        F17                   | 64
            Key::KeypadDecimal,         //        Keypad Decimal        | 65
            Key::Unknown,
            Key::KeypadMultiply,        //        Keypad Multiply       | 67
            Key::Unknown,
            Key::KeypadAdd,             //        Keypad Plus           | 69
            Key::Unknown,
            Key::Unknown,               //        Keypad Clear          | 71
            Key::Unknown,               //        Volume Up             | 72
            Key::Unknown,               //        Volume Down           | 73
            Key::Unknown,               //        Mute                  | 74
            Key::KeypadDivide,          //        Keypad Divide         | 75
            Key::KeypadEnter,           //        Keypad Enter          | 76
            Key::Unknown,
            Key::KeypadSubtract,        //        Keypad Minus          | 78
            Key::F18,                   //        F18                   | 79
            Key::F19,                   //        F19                   | 80
            Key::KeypadEquals,          //        Keypad Equals        | 81
            Key::KeypadNumber0,         //        Keypad 0              | 82
            Key::KeypadNumber1,         //        Keypad 1              | 83
            Key::KeypadNumber2,         //        Keypad 2              | 84
            Key::KeypadNumber3,         //        Keypad 3              | 85
            Key::KeypadNumber4,         //        Keypad 4              | 86
            Key::KeypadNumber5,         //        Keypad 5              | 87
            Key::KeypadNumber6,         //        Keypad 6              | 88
            Key::KeypadNumber7,         //        Keypad 7              | 89
            Key::F20,                   //        F20                   | 90
            Key::KeypadNumber8,         //        Keypad 8              | 91
            Key::KeypadNumber9,         //        Keypad 9              | 92
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::F5,                    //        F5                    | 96
            Key::F6,                    //        F6                    | 97
            Key::F7,                    //        F7                    | 98
            Key::F3,                    //        F3                    | 99
            Key::F8,                    //        F8                    | 100
            Key::F9,                    //        F9                    | 101
            Key::Unknown,
            Key::F11,                   //        F11                   | 103
            Key::Unknown,
            Key::F13,                   //        F13                   | 105
            Key::F16,                   //        F16                   | 106
            Key::F14,                   //        F14                   | 107
            Key::Unknown,
            Key::F10,                   //        F10                   | 109
            Key::Menu,                  //        Menu                  | 110
            Key::F12,                   //        F12                   | 111
            Key::Unknown,
            Key::F15,                   //        F15                   | 113
            Key::Unknown,               //        Help                  | 114
            Key::Home,                  //        Home                  | 115
            Key::PageUp,                //        Page Up               | 116
            Key::Unknown,               //        Forward Delete        | 117
            Key::F4,                    //        F4                    | 118
            Key::End,                   //        End                   | 119
            Key::F2,                    //        F2                    | 120
            Key::PageDown,              //        Page Down             | 121
            Key::F1,                    //        F1                    | 122
            Key::LeftArrow,             //        Left Arrow            | 123
            Key::RightArrow,            //        Right Arrow           | 124
            Key::DownArrow,             //        Down Arrow            | 125
            Key::UpArrow                //        Up Arrow              | 126
        };

        InputAction lastKeyStates[static_cast<std::underlying_type<Key>::type>(Key::RightSystem) + 1] { };
        InputAction keyStates[static_cast<std::underlying_type<Key>::type>(Key::RightSystem) + 1] { };

        InputAction lastMouseButtonStates[static_cast<std::underlying_type<MouseButton>::type>(MouseButton::Extra2) + 1];
        InputAction mouseButtonStates[static_cast<std::underlying_type<MouseButton>::type>(MouseButton::Extra2) + 1];

        Vector2 mouseScroll = { 0, 0 };
    };

}
