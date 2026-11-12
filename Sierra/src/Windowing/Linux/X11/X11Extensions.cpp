//
// Created by Nikolay Kanchevski on 10.28.23.
//

#include "X11Extensions.h"

#include <X11/XKBlib.h>
#include <X11/extensions/Xrandr.h>

namespace Sierra
{

    namespace
    {
        struct KeyNameEntry
        {
            Key key = Key::Unknown;
            std::string_view name = { };
        };

        constexpr std::array<KeyNameEntry, 121> KEY_TABLE
        {
            KeyNameEntry { .key = Key::Grave, .name = "TLDE" },
            KeyNameEntry { .key = Key::Number1, .name = "AE01" },
            KeyNameEntry { .key = Key::Number2, .name = "AE02" },
            KeyNameEntry { .key = Key::Number3, .name = "AE03" },
            KeyNameEntry { .key = Key::Number4, .name = "AE04" },
            KeyNameEntry { .key = Key::Number5, .name = "AE05" },
            KeyNameEntry { .key = Key::Number6, .name = "AE06" },
            KeyNameEntry { .key = Key::Number7, .name = "AE07" },
            KeyNameEntry { .key = Key::Number8, .name = "AE08" },
            KeyNameEntry { .key = Key::Number9, .name = "AE09" },
            KeyNameEntry { .key = Key::Number0, .name = "AE10" },
            KeyNameEntry { .key = Key::Minus, .name = "AE11" },
            KeyNameEntry { .key = Key::Equal, .name = "AE12" },
            KeyNameEntry { .key = Key::Q, .name = "AD01" },
            KeyNameEntry { .key = Key::W, .name = "AD02" },
            KeyNameEntry { .key = Key::E, .name = "AD03" },
            KeyNameEntry { .key = Key::R, .name = "AD04" },
            KeyNameEntry { .key = Key::T, .name = "AD05" },
            KeyNameEntry { .key = Key::Y, .name = "AD06" },
            KeyNameEntry { .key = Key::U, .name = "AD07" },
            KeyNameEntry { .key = Key::I, .name = "AD08" },
            KeyNameEntry { .key = Key::O, .name = "AD09" },
            KeyNameEntry { .key = Key::P, .name = "AD10" },
            KeyNameEntry { .key = Key::LeftBracket, .name = "AD11" },
            KeyNameEntry { .key = Key::RightBracket, .name = "AD12" },
            KeyNameEntry { .key = Key::A, .name = "AC01" },
            KeyNameEntry { .key = Key::S, .name = "AC02" },
            KeyNameEntry { .key = Key::D, .name = "AC03" },
            KeyNameEntry { .key = Key::F, .name = "AC04" },
            KeyNameEntry { .key = Key::G, .name = "AC05" },
            KeyNameEntry { .key = Key::H, .name = "AC06" },
            KeyNameEntry { .key = Key::J, .name = "AC07" },
            KeyNameEntry { .key = Key::K, .name = "AC08" },
            KeyNameEntry { .key = Key::L, .name = "AC09" },
            KeyNameEntry { .key = Key::Semicolon, .name = "AC10" },
            KeyNameEntry { .key = Key::Apostrophe, .name = "AC11" },
            KeyNameEntry { .key = Key::Z, .name = "AB01" },
            KeyNameEntry { .key = Key::X, .name = "AB02" },
            KeyNameEntry { .key = Key::C, .name = "AB03" },
            KeyNameEntry { .key = Key::V, .name = "AB04" },
            KeyNameEntry { .key = Key::B, .name = "AB05" },
            KeyNameEntry { .key = Key::N, .name = "AB06" },
            KeyNameEntry { .key = Key::M, .name = "AB07" },
            KeyNameEntry { .key = Key::Comma, .name = "AB08" },
            KeyNameEntry { .key = Key::Period, .name = "AB09" },
            KeyNameEntry { .key = Key::Slash, .name = "AB10" },
            KeyNameEntry { .key = Key::Backslash, .name = "BKSL" },
            KeyNameEntry { .key = Key::World1, .name = "LSGT" },
            KeyNameEntry { .key = Key::Space, .name = "SPCE" },
            KeyNameEntry { .key = Key::Escape, .name = "ESC" },
            KeyNameEntry { .key = Key::Enter, .name = "RTRN" },
            KeyNameEntry { .key = Key::Tab, .name = "TAB" },
            KeyNameEntry { .key = Key::Backspace, .name = "BKSP" },
            KeyNameEntry { .key = Key::Insert, .name = "INS" },
            KeyNameEntry { .key = Key::Delete, .name = "DELE" },
            KeyNameEntry { .key = Key::RightArrow, .name = "RGHT" },
            KeyNameEntry { .key = Key::LeftArrow, .name = "LEFT" },
            KeyNameEntry { .key = Key::DownArrow, .name = "DOWN" },
            KeyNameEntry { .key = Key::UpArrow, .name = "UP" },
            KeyNameEntry { .key = Key::PageUp, .name = "PGUP" },
            KeyNameEntry { .key = Key::PageDown, .name = "PGDN" },
            KeyNameEntry { .key = Key::Home, .name = "HOME" },
            KeyNameEntry { .key = Key::End, .name = "END" },
            KeyNameEntry { .key = Key::CapsLock, .name = "CAPS" },
            KeyNameEntry { .key = Key::ScrollLock, .name = "SCLK" },
            KeyNameEntry { .key = Key::NumpadLock, .name = "NMLK" },
            KeyNameEntry { .key = Key::PrintScreen, .name = "PRSC" },
            KeyNameEntry { .key = Key::Pause, .name = "PAUS" },
            KeyNameEntry { .key = Key::F1, .name = "FK01" },
            KeyNameEntry { .key = Key::F2, .name = "FK02" },
            KeyNameEntry { .key = Key::F3, .name = "FK03" },
            KeyNameEntry { .key = Key::F4, .name = "FK04" },
            KeyNameEntry { .key = Key::F5, .name = "FK05" },
            KeyNameEntry { .key = Key::F6, .name = "FK06" },
            KeyNameEntry { .key = Key::F7, .name = "FK07" },
            KeyNameEntry { .key = Key::F8, .name = "FK08" },
            KeyNameEntry { .key = Key::F9, .name = "FK09" },
            KeyNameEntry { .key = Key::F10, .name = "FK10" },
            KeyNameEntry { .key = Key::F11, .name = "FK11" },
            KeyNameEntry { .key = Key::F12, .name = "FK12" },
            KeyNameEntry { .key = Key::F13, .name = "FK13" },
            KeyNameEntry { .key = Key::F14, .name = "FK14" },
            KeyNameEntry { .key = Key::F15, .name = "FK15" },
            KeyNameEntry { .key = Key::F16, .name = "FK16" },
            KeyNameEntry { .key = Key::F17, .name = "FK17" },
            KeyNameEntry { .key = Key::F18, .name = "FK18" },
            KeyNameEntry { .key = Key::F19, .name = "FK19" },
            KeyNameEntry { .key = Key::F20, .name = "FK20" },
            KeyNameEntry { .key = Key::F21, .name = "FK21" },
            KeyNameEntry { .key = Key::F22, .name = "FK22" },
            KeyNameEntry { .key = Key::F23, .name = "FK23" },
            KeyNameEntry { .key = Key::F24, .name = "FK24" },
            KeyNameEntry { .key = Key::F25, .name = "FK25" },
            KeyNameEntry { .key = Key::KeypadNumber0, .name = "KP0" },
            KeyNameEntry { .key = Key::KeypadNumber1, .name = "KP1" },
            KeyNameEntry { .key = Key::KeypadNumber2, .name = "KP2" },
            KeyNameEntry { .key = Key::KeypadNumber3, .name = "KP3" },
            KeyNameEntry { .key = Key::KeypadNumber4, .name = "KP4" },
            KeyNameEntry { .key = Key::KeypadNumber5, .name = "KP5" },
            KeyNameEntry { .key = Key::KeypadNumber6, .name = "KP6" },
            KeyNameEntry { .key = Key::KeypadNumber7, .name = "KP7" },
            KeyNameEntry { .key = Key::KeypadNumber8, .name = "KP8" },
            KeyNameEntry { .key = Key::KeypadNumber9, .name = "KP9" },
            KeyNameEntry { .key = Key::KeypadPeriod, .name = "KPDL" },
            KeyNameEntry { .key = Key::KeypadSlash, .name = "KPDV" },
            KeyNameEntry { .key = Key::KeypadAsterisk, .name = "KPMU" },
            KeyNameEntry { .key = Key::KeypadMinus, .name = "KPSU" },
            KeyNameEntry { .key = Key::KeypadPlus, .name = "KPAD" },
            KeyNameEntry { .key = Key::KeypadEnter, .name = "KPEN" },
            KeyNameEntry { .key = Key::KeypadEqual, .name = "KPEQ" },
            KeyNameEntry { .key = Key::LeftShift, .name = "LFSH" },
            KeyNameEntry { .key = Key::LeftControl, .name = "LCTL" },
            KeyNameEntry { .key = Key::LeftAlt, .name = "LALT" },
            KeyNameEntry { .key = Key::LeftSystem, .name = "LWIN" },
            KeyNameEntry { .key = Key::RightShift, .name = "RTSH" },
            KeyNameEntry { .key = Key::RightControl, .name = "RCTL" },
            KeyNameEntry { .key = Key::RightAlt, .name = "RALT" },
            KeyNameEntry { .key = Key::RightAlt, .name = "LVL3" },
            KeyNameEntry { .key = Key::RightAlt, .name = "MDSW" },
            KeyNameEntry { .key = Key::RightSystem, .name = "RWIN" },
            KeyNameEntry { .key = Key::Menu, .name = "MENU" }
        };
    }

    /* --- CONSTRUCTORS --- */

    XkbExtension::XkbExtension(const XkbExtensionCreateInfo& createInfo)
    {
        // Check if X11's XKB extension is supported
        int versionMajor, versionMinor;
        int minKeyCode, maxKeyCode;
        if (XkbQueryExtension(createInfo.display, nullptr, nullptr, nullptr, &versionMajor, &versionMinor))
        {
            // Check for auto-key-repeat support
            Bool autoRepeatDetectionSupported;
            if (XkbSetDetectableAutoRepeat(createInfo.display, True, &autoRepeatDetectionSupported))
            {
                autoRepeatDetectable = autoRepeatDetectionSupported;
            }

            // Set key event filtering
            XkbSelectEventDetails(createInfo.display, XkbUseCoreKbd, XkbStateNotify, XkbGroupStateMask, XkbGroupStateMask);

            // Get keyboard info to later determine key locations on the current keyboard layout
            XkbDescPtr keyboardDescription = XkbGetMap(createInfo.display, 0, XkbUseCoreKbd);
            XkbGetNames(createInfo.display, XkbKeyNamesMask | XkbKeyAliasesMask, keyboardDescription);

            // Save key code boundaries
            minKeyCode = keyboardDescription->min_key_code;
            maxKeyCode = keyboardDescription->max_key_code;

            // Map every X11 key code to an index in the key table
            for (size i = minKeyCode; i <= glm::min(static_cast<size>(maxKeyCode), keyTable.size() - 1); i++)
            {
                Key key = Key::Unknown;

                // Try to map key using the US layout
                for (const auto [currentKey, currentKeyName] : KEY_TABLE)
                {
                    if (std::strncmp(keyboardDescription->names->keys[i].name, currentKeyName.data(), XkbKeyNameLength) == 0)
                    {
                        key = currentKey;
                        break;
                    }
                }

                // If mapping failed, try to match key with its alias
                if (key == Key::Unknown)
                {
                    for (uint32 j = keyboardDescription->names->num_key_aliases; j--;)
                    {
                        if (std::strncmp(keyboardDescription->names->key_aliases[j].real, keyboardDescription->names->keys[i].name, XkbKeyNameLength) != 0)
                        {
                            continue;
                        }

                        for (const auto [currentKey, currentKeyName] : KEY_TABLE)
                        {
                            if (std::strncmp(keyboardDescription->names->key_aliases[j].alias, currentKeyName.data(), XkbKeyNameLength) == 0)
                            {
                                key = currentKey;
                                break;
                            }
                        }
                    }
                }

                keyTable[i] = key;
            }

            // Deallocate keyboard info memory
            XkbFreeNames(keyboardDescription, XkbKeyNamesMask, True);
            XkbFreeKeyboard(keyboardDescription, 0, True);
        }
        else
        {
            // Get raw X11's key codes for the display
            XDisplayKeycodes(createInfo.display, &minKeyCode, &maxKeyCode);
        }

        // Retrieve standard key sym mapping
        int width;
        std::unique_ptr<KeySym, std::function<void(void*)>> keySyms = { XGetKeyboardMapping(createInfo.display, minKeyCode, maxKeyCode - minKeyCode + 1, &width), XFree };

        // Translate unmapped entries using regular X11 KeySym lookups
        for (uint32 scancode = minKeyCode; scancode <= glm::min(maxKeyCode, static_cast<int32>(keyTable.size())); scancode++)
        {
            if (keyTable[scancode] == Key::Unknown) keyTable[scancode] = TranslateKeySyms(&keySyms.get()[(scancode - minKeyCode) * width], width);
        }
    }

    /* --- POLLING METHODS --- */

    Key XkbExtension::TranslateKeySyms(const KeySym* keySyms, const int32 width) const
    {
        if (width > 1)
        {
            switch (keySyms[1])
            {
                case XK_KP_0:           return Key::KeypadNumber0;
                case XK_KP_1:           return Key::KeypadNumber1;
                case XK_KP_2:           return Key::KeypadNumber2;
                case XK_KP_3:           return Key::KeypadNumber3;
                case XK_KP_4:           return Key::KeypadNumber4;
                case XK_KP_5:           return Key::KeypadNumber5;
                case XK_KP_6:           return Key::KeypadNumber6;
                case XK_KP_7:           return Key::KeypadNumber7;
                case XK_KP_8:           return Key::KeypadNumber8;
                case XK_KP_9:           return Key::KeypadNumber9;
                case XK_KP_Separator:
                case XK_KP_Decimal:     return Key::KeypadPeriod;
                case XK_KP_Equal:       return Key::KeypadEqual;
                case XK_KP_Enter:       return Key::KeypadEnter;
                default:                break;
            }
        }

        switch (keySyms[0])
        {
            case XK_Escape:         return Key::Escape;
            case XK_Tab:            return Key::Tab;
            case XK_Shift_L:        return Key::LeftShift;
            case XK_Shift_R:        return Key::RightShift;
            case XK_Control_L:      return Key::LeftControl;
            case XK_Control_R:      return Key::RightControl;
            case XK_Meta_L:
            case XK_Alt_L:          return Key::LeftAlt;
            case XK_Mode_switch:
            case XK_ISO_Level3_Shift:
            case XK_Meta_R:
            case XK_Alt_R:          return Key::RightAlt;
            case XK_Super_L:        return Key::LeftSystem;
            case XK_Super_R:        return Key::RightSystem;
            case XK_Menu:           return Key::Menu;
            case XK_Num_Lock:       return Key::NumpadLock;
            case XK_Caps_Lock:      return Key::CapsLock;
            case XK_Print:          return Key::PrintScreen;
            case XK_Scroll_Lock:    return Key::ScrollLock;
            case XK_Pause:          return Key::Pause;
            case XK_Delete:         return Key::Delete;
            case XK_BackSpace:      return Key::Backspace;
            case XK_Return:         return Key::Enter;
            case XK_Home:           return Key::Home;
            case XK_End:            return Key::End;
            case XK_Page_Up:        return Key::PageUp;
            case XK_Page_Down:      return Key::PageDown;
            case XK_Insert:         return Key::Insert;
            case XK_Left:           return Key::LeftArrow;
            case XK_Right:          return Key::RightArrow;
            case XK_Down:           return Key::DownArrow;
            case XK_Up:             return Key::UpArrow;
            case XK_F1:             return Key::F1;
            case XK_F2:             return Key::F2;
            case XK_F3:             return Key::F3;
            case XK_F4:             return Key::F4;
            case XK_F5:             return Key::F5;
            case XK_F6:             return Key::F6;
            case XK_F7:             return Key::F7;
            case XK_F8:             return Key::F8;
            case XK_F9:             return Key::F9;
            case XK_F10:            return Key::F10;
            case XK_F11:            return Key::F11;
            case XK_F12:            return Key::F12;
            case XK_F13:            return Key::F13;
            case XK_F14:            return Key::F14;
            case XK_F15:            return Key::F15;
            case XK_F16:            return Key::F16;
            case XK_F17:            return Key::F17;
            case XK_F18:            return Key::F18;
            case XK_F19:            return Key::F19;
            case XK_F20:            return Key::F20;
            case XK_F21:            return Key::F21;
            case XK_F22:            return Key::F22;
            case XK_F23:            return Key::F23;
            case XK_F24:            return Key::F24;
            case XK_F25:            return Key::F25;
            case XK_KP_Divide:      return Key::KeypadSlash;
            case XK_KP_Multiply:    return Key::KeypadAsterisk;
            case XK_KP_Subtract:    return Key::KeypadMinus;
            case XK_KP_Add:         return Key::KeypadPlus;
            case XK_KP_Insert:      return Key::KeypadNumber0;
            case XK_KP_End:         return Key::KeypadNumber1;
            case XK_KP_Down:        return Key::KeypadNumber2;
            case XK_KP_Page_Down:   return Key::KeypadNumber3;
            case XK_KP_Left:        return Key::KeypadNumber4;
            case XK_KP_Right:       return Key::KeypadNumber6;
            case XK_KP_Home:        return Key::KeypadNumber7;
            case XK_KP_Up:          return Key::KeypadNumber8;
            case XK_KP_Page_Up:     return Key::KeypadNumber9;
            case XK_KP_Delete:      return Key::KeypadPeriod;
            case XK_KP_Equal:       return Key::KeypadEqual;
            case XK_KP_Enter:       return Key::KeypadEnter;
            case XK_a:              return Key::A;
            case XK_b:              return Key::B;
            case XK_c:              return Key::C;
            case XK_d:              return Key::D;
            case XK_e:              return Key::E;
            case XK_f:              return Key::F;
            case XK_g:              return Key::G;
            case XK_h:              return Key::H;
            case XK_i:              return Key::I;
            case XK_j:              return Key::J;
            case XK_k:              return Key::K;
            case XK_l:              return Key::L;
            case XK_m:              return Key::M;
            case XK_n:              return Key::N;
            case XK_o:              return Key::O;
            case XK_p:              return Key::P;
            case XK_q:              return Key::Q;
            case XK_r:              return Key::R;
            case XK_s:              return Key::S;
            case XK_t:              return Key::T;
            case XK_u:              return Key::U;
            case XK_v:              return Key::V;
            case XK_w:              return Key::W;
            case XK_x:              return Key::X;
            case XK_y:              return Key::Y;
            case XK_z:              return Key::Z;
            case XK_1:              return Key::Number1;
            case XK_2:              return Key::Number2;
            case XK_3:              return Key::Number3;
            case XK_4:              return Key::Number4;
            case XK_5:              return Key::Number5;
            case XK_6:              return Key::Number6;
            case XK_7:              return Key::Number7;
            case XK_8:              return Key::Number8;
            case XK_9:              return Key::Number9;
            case XK_0:              return Key::Number0;
            case XK_space:          return Key::Space;
            case XK_minus:          return Key::Minus;
            case XK_equal:          return Key::Equal;
            case XK_bracketleft:    return Key::LeftBracket;
            case XK_bracketright:   return Key::RightBracket;
            case XK_backslash:      return Key::Backslash;
            case XK_semicolon:      return Key::Semicolon;
            case XK_apostrophe:     return Key::Apostrophe;
            case XK_grave:          return Key::Grave;
            case XK_comma:          return Key::Comma;
            case XK_period:         return Key::Period;
            case XK_slash:          return Key::Slash;
            case XK_less:           return Key::World1;
            default:                break;
        }

        return Key::Unknown;
    }

    /* --- CONSTRUCTORS --- */

    XrandrExtension::XrandrExtension(const XrandrExtensionCreateInfo& createInfo)
    {
        int versionMajor, versionMinor;
        if (XRRQueryExtension(createInfo.display, &eventBase, &versionMajor))
        {
            supported = true;
            if (XRRQueryVersion(createInfo.display, &versionMajor, &versionMinor))
            {
                SR_THROW_IF(!(versionMajor > 1 || versionMinor >= 3), UnsupportedFeatureError("X11 does not support the Xrandr extension, which is mandatory"));
            }
        }
    }

}