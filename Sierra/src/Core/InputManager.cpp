//
// Created by Nikolay Kanchevski on 9.16.2023.
//

#include "InputManager.h"

namespace Sierra
{

    std::string_view GetKeyName(const Key key)
    {
        switch (key)
        {
            case Key::Unknown:                  break;
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

    char GetKeyCharacter(const Key key)
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
            case Key::Number0:
            case Key::KeypadNumber0:            return '0';
            case Key::Number1:
            case Key::KeypadNumber1:            return '1';
            case Key::Number2:
            case Key::KeypadNumber2:            return '2';
            case Key::Number3:
            case Key::KeypadNumber3:            return '3';
            case Key::Number4:
            case Key::KeypadNumber4:            return '4';
            case Key::Number5:
            case Key::KeypadNumber5:            return '5';
            case Key::Number6:
            case Key::KeypadNumber6:            return '6';
            case Key::Number7:
            case Key::KeypadNumber7:            return '7';
            case Key::Number8:
            case Key::KeypadNumber8:            return '8';
            case Key::Number9:
            case Key::KeypadNumber9:            return '9';
            case Key::Space:                    return ' ';
            case Key::Minus:
            case Key::KeypadSubtract:           return '-';
            case Key::Equals:
            case Key::KeypadEquals:             return '=';
            case Key::Comma:                    return ',';
            case Key::Period:
            case Key::KeypadDecimal:            return '.';
            case Key::Apostrophe:               return '\'';
            case Key::Semicolon:                return ';';
            case Key::Slash:
            case Key::KeypadDivide:             return '/';
            case Key::Backslash:                return '\\';
            case Key::LeftBracket:              return '(';
            case Key::RightBracket:             return ')';
            case Key::Grave:                    return '`';
            case Key::KeypadMultiply:           return '*';
            case Key::KeypadAdd:                return '+';
            default:                            break;
        }

        return '\0';
    }

    std::string_view GetMouseButtonName(const MouseButton mouseButton)
    {
        switch (mouseButton)
        {
            case MouseButton::Unknown:          break;
            case MouseButton::Left:             return "Left";
            case MouseButton::Right:            return "Right";
            case MouseButton::Middle:           return "Middle";
            case MouseButton::Extra1:           return "Extra 1";
            case MouseButton::Extra2:           return "Extra 2";
        }

        return "Unknown";
    }
    /* --- POLLING METHODS --- */

    void InputManager::RegisterKeyPress(const Key)
    {

    }

    void InputManager::RegisterKeyRelease(const Key)
    {

    }

    void InputManager::RegisterMouseButtonPress(const MouseButton)
    {

    }

    void InputManager::RegisterMouseButtonRelease(const MouseButton)
    {

    }

    void InputManager::RegisterMouseScroll(const Vector2)
    {

    }

    /* --- GETTER METHODS --- */

    bool InputManager::IsKeyPressed(const Key) const
    {
        return false;
    }

    bool InputManager::IsKeyHeld(const Key) const
    {
        return false;
    }

    bool InputManager::IsKeyReleased(const Key) const
    {
        return false;
    }

    bool InputManager::IsKeyResting(const Key) const
    {
        return false;
    }

    bool InputManager::IsMouseButtonPressed(const MouseButton) const
    {
        return false;
    }

    bool InputManager::IsMouseButtonHeld(const MouseButton) const
    {
        return false;
    }

    bool InputManager::IsMouseButtonReleased(const MouseButton) const
    {
        return false;
    }

    bool InputManager::IsMouseButtonResting(const MouseButton) const
    {
        return false;
    }

    Vector2 InputManager::GetMouseScroll() const
    {
        return { 0, 0 };
    }

    /* --- CONVERSIONS --- */


    /* --- PRIVATE METHODS --- */

    bool InputManager::IsKeyCombinationPressedImplementation(const std::initializer_list<Key> &keys) const
    {
        bool atLeastOneKeyPressed = false;
        for (const Key key : keys)
        {
            if (IsKeyHeld(key)) continue;

            if (IsKeyPressed(key))
            {
                atLeastOneKeyPressed = true;
                continue;
            }

            return false;
        }

        return atLeastOneKeyPressed;
    }

    bool InputManager::IsKeyCombinationHeldImplementation(const std::initializer_list<Key> &keys) const
    {
        return std::ranges::all_of(keys, [this](const Key key) -> bool { return IsKeyHeld(key); });
    }

    bool InputManager::IsMouseButtonCombinationPressedImplementation(const std::initializer_list<MouseButton> &mouseButtons) const
    {
        bool atLeastOneMouseButtonPressed = false;
        for (const MouseButton mouseButton : mouseButtons)
        {
            if (IsMouseButtonHeld(mouseButton)) continue;

            if (IsMouseButtonPressed(mouseButton))
            {
                atLeastOneMouseButtonPressed = true;
                continue;
            }

            return false;
        }

        return atLeastOneMouseButtonPressed;
    }

    bool InputManager::IsMouseButtonCombinationHeldImplementation(const std::initializer_list<MouseButton> &mouseButtons) const
    {
        return std::ranges::all_of(mouseButtons, [this](const MouseButton mouseButton) -> bool { return IsMouseButtonHeld(mouseButton); });
    }

}