//
// Created by Nikolay Kanchevski on 10.29.23.
//

#pragma once

namespace Sierra
{

    enum class Key : uint8
    {
        Unknown,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Number0,
        Number1,
        Number2,
        Number3,
        Number4,
        Number5,
        Number6,
        Number7,
        Number8,
        Number9,
        Space,
        Escape,
        Minus,
        Equals,
        Comma,
        Period,
        Apostrophe,
        Semicolon,
        Slash,
        Backslash,
        LeftBracket,
        RightBracket,
        Grave,
        World1,
        World2,
        Enter,
        Backspace,
        Delete,
        Tab,
        CapsLock,
        Insert,
        Section,
        RightArrow,
        LeftArrow,
        DownArrow,
        UpArrow,
        PageUp,
        PageDown,
        Home,
        End,
        ScrollLock,
        NumpadLock,
        PrintScreen,
        Pause,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        F25,
        KeypadNumber0,
        KeypadNumber1,
        KeypadNumber2,
        KeypadNumber3,
        KeypadNumber4,
        KeypadNumber5,
        KeypadNumber6,
        KeypadNumber7,
        KeypadNumber8,
        KeypadNumber9,
        KeypadDecimal,
        KeypadDivide,
        KeypadMultiply,
        KeypadSubtract,
        KeypadAdd,
        KeypadEnter,
        KeypadEquals,
        LeftShift,
        RightShift,
        LeftControl,
        RightControl,
        Function,
        Menu,
        LeftAlt,
        RightAlt,
        LeftSystem,
        RightSystem,
        // Windows Keys:
        LeftWindows = LeftSystem,
        RightWindows = RightSystem,
        // macOS Keys
        LeftCommand = LeftSystem,
        RightCommand = RightSystem,
        LeftOption = LeftAlt,
        RightOption = RightAlt
    };

    constexpr static const char* GetKeyName(const Key key)
    {
        switch (key)
        {
            case Key::Unknown:                  return "Unknown";
            case Key::A:                        return "A";
            case Key::B:                        return "B";
            case Key::C:                        return "C";
            case Key::D:                        return "D";
            case Key::E:                        return "E";
            case Key::F:                        return "F";
            case Key::G:                        return "G";
            case Key::H:                        return "H";
            case Key::I:                        return "I";
            case Key::J:                        return "J";
            case Key::K:                        return "K";
            case Key::L:                        return "L";
            case Key::M:                        return "M";
            case Key::N:                        return "N";
            case Key::O:                        return "O";
            case Key::P:                        return "P";
            case Key::Q:                        return "Q";
            case Key::R:                        return "R";
            case Key::S:                        return "S";
            case Key::T:                        return "T";
            case Key::U:                        return "U";
            case Key::V:                        return "V";
            case Key::W:                        return "W";
            case Key::X:                        return "X";
            case Key::Y:                        return "Y";
            case Key::Z:                        return "Z";
            case Key::Number0:                  return "Number 0";
            case Key::Number1:                  return "Number 1";
            case Key::Number2:                  return "Number 2";
            case Key::Number3:                  return "Number 3";
            case Key::Number4:                  return "Number 4";
            case Key::Number5:                  return "Number 5";
            case Key::Number6:                  return "Number 6";
            case Key::Number7:                  return "Number 7";
            case Key::Number8:                  return "Number 8";
            case Key::Number9:                  return "Number 9";
            case Key::Space:                    return "Space";
            case Key::Escape:                   return "Escape";
            case Key::Minus:                    return "Minus";
            case Key::Equals:                   return "Equals";
            case Key::Comma:                    return "Comma";
            case Key::Period:                   return "Period";
            case Key::Apostrophe:               return "Apostrophe";
            case Key::Semicolon:                return "Semicolon";
            case Key::Slash:                    return "Slash";
            case Key::Backslash:                return "Backslash";
            case Key::LeftBracket:              return "Left Bracket";
            case Key::RightBracket:             return "Right Bracket";
            case Key::Grave:                    return "Grave";
            case Key::World1:                   return "World 1";
            case Key::World2:                   return "World 2";
            case Key::Enter:                    return "Enter";
            case Key::Backspace:                return "Backspace";
            case Key::Delete:                   return "Delete";
            case Key::Tab:                      return "Tab";
            case Key::CapsLock:                 return "CapsLock";
            case Key::Insert:                   return "Insert";
            case Key::Section:                  return "Section";
            case Key::RightArrow:               return "Right Arrow";
            case Key::LeftArrow:                return "Left Arrow";
            case Key::DownArrow:                return "Down Arrow";
            case Key::UpArrow:                  return "Up Arrow";
            case Key::PageUp:                   return "Page Up";
            case Key::PageDown:                 return "Page Down";
            case Key::Home:                     return "Home";
            case Key::End:                      return "End";
            case Key::ScrollLock:               return "Scroll Lock";
            case Key::NumpadLock:               return "Numpad Lock";
            case Key::PrintScreen:              return "Print Screen";
            case Key::Pause:                    return "Pause";
            case Key::F1:                       return "F1";
            case Key::F2:                       return "F2";
            case Key::F3:                       return "F3";
            case Key::F4:                       return "F4";
            case Key::F5:                       return "F5";
            case Key::F6:                       return "F6";
            case Key::F7:                       return "F7";
            case Key::F8:                       return "F8";
            case Key::F9:                       return "F9";
            case Key::F10:                      return "F10";
            case Key::F11:                      return "F11";
            case Key::F12:                      return "F12";
            case Key::F13:                      return "F13";
            case Key::F14:                      return "F14";
            case Key::F15:                      return "F15";
            case Key::F16:                      return "F16";
            case Key::F17:                      return "F17";
            case Key::F18:                      return "F18";
            case Key::F19:                      return "F19";
            case Key::F20:                      return "F20";
            case Key::F21:                      return "F21";
            case Key::F22:                      return "F22";
            case Key::F23:                      return "F23";
            case Key::F24:                      return "F24";
            case Key::F25:                      return "F25";
            case Key::KeypadNumber0:            return "Keypad Number 0";
            case Key::KeypadNumber1:            return "Keypad Number 1";
            case Key::KeypadNumber2:            return "Keypad Number 2";
            case Key::KeypadNumber3:            return "Keypad Number 3";
            case Key::KeypadNumber4:            return "Keypad Number 4";
            case Key::KeypadNumber5:            return "Keypad Number 5";
            case Key::KeypadNumber6:            return "Keypad Number 6";
            case Key::KeypadNumber7:            return "Keypad Number 7";
            case Key::KeypadNumber8:            return "Keypad Number 8";
            case Key::KeypadNumber9:            return "Keypad Number 9";
            case Key::KeypadDecimal:            return "Keypad Decimal";
            case Key::KeypadDivide:             return "Keypad Divide";
            case Key::KeypadMultiply:           return "Keypad Multiply";
            case Key::KeypadSubtract:           return "Keypad Subtract";
            case Key::KeypadAdd:                return "Keypad Add";
            case Key::KeypadEnter:              return "Keypad Enter";
            case Key::KeypadEquals:             return "Keypad Equals";
            case Key::LeftShift:                return "Left Shift";
            case Key::RightShift:               return "Right Shift";
            case Key::LeftControl:              return "Left Control";
            case Key::RightControl:             return "Right Control";
            case Key::Function:                 return "Function";
            case Key::Menu:                     return "Menu";
            #if SR_PLATFORM_WINDOWS
                case Key::LeftWindows:          return "Left Windows";
                case Key::RightWindows:         return "Right Windows";
                case Key::LeftAlt:              return "Left Alt";
                case Key::RightAlt:             return "Right Alt";
            #elif SR_PLATFORM_macOS
                case Key::LeftCommand:          return "Left Command";
                case Key::RightCommand:         return "Right Command";
                case Key::LeftOption:           return "Left Option";
                case Key::RightOption:          return "Right Option";
            #else
                case Key::LeftSystem:           return "Left System";
                case Key::RightSystem:          return "Right System";
                case Key::LeftAlt:              return "Left Alt";
                case Key::RightAlt:             return "Right Alt";
            #endif
        }

        return "Unknown";
    }

    constexpr static char GetKeyCharacter(const Key key)
    {
        switch (key)
        {
            case Key::A:                        return 'a';
            case Key::B:                        return 'b';
            case Key::C:                        return 'c';
            case Key::D:                        return 'd';
            case Key::E:                        return 'e';
            case Key::F:                        return 'f';
            case Key::G:                        return 'g';
            case Key::H:                        return 'h';
            case Key::I:                        return 'i';
            case Key::J:                        return 'j';
            case Key::K:                        return 'k';
            case Key::L:                        return 'l';
            case Key::M:                        return 'm';
            case Key::N:                        return 'n';
            case Key::O:                        return 'o';
            case Key::P:                        return 'p';
            case Key::Q:                        return 'q';
            case Key::R:                        return 'r';
            case Key::S:                        return 's';
            case Key::T:                        return 't';
            case Key::U:                        return 'u';
            case Key::V:                        return 'v';
            case Key::W:                        return 'w';
            case Key::X:                        return 'x';
            case Key::Y:                        return 'y';
            case Key::Z:                        return 'z';
            default:                            return ' ';
        }
    }

}