//
// Created by Nikolay Kanchevski on 10.28.23.
//

#include "X11Extensions.h"

#include <X11/keysym.h>

#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/Xrandr.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    XkbExtension::XkbExtension(const XkbExtensionCreateInfo &createInfo)
    {
        // Check if X11's XKB extension is supported
        int versionMajor, versionMinor;
        int minimumKeyCode, maximumKeyCode;
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
            minimumKeyCode = keyboardDescription->min_key_code;
            maximumKeyCode = keyboardDescription->max_key_code;

            struct KeyNameEntry
            {
                Key key;
                const char* name;
            };

            constexpr static KeyNameEntry keyNameTable[] =
            {
                { Key::Grave, "TLDE" },
                { Key::Number1, "AE01" },
                { Key::Number2, "AE02" },
                { Key::Number3, "AE03" },
                { Key::Number4, "AE04" },
                { Key::Number5, "AE05" },
                { Key::Number6, "AE06" },
                { Key::Number7, "AE07" },
                { Key::Number8, "AE08" },
                { Key::Number9, "AE09" },
                { Key::Number0, "AE10" },
                { Key::Minus, "AE11" },
                { Key::Equals, "AE12" },
                { Key::Q, "AD01" },
                { Key::W, "AD02" },
                { Key::E, "AD03" },
                { Key::R, "AD04" },
                { Key::T, "AD05" },
                { Key::Y, "AD06" },
                { Key::U, "AD07" },
                { Key::I, "AD08" },
                { Key::O, "AD09" },
                { Key::P, "AD10" },
                { Key::LeftBracket, "AD11" },
                { Key::RightBracket, "AD12" },
                { Key::A, "AC01" },
                { Key::S, "AC02" },
                { Key::D, "AC03" },
                { Key::F, "AC04" },
                { Key::G, "AC05" },
                { Key::H, "AC06" },
                { Key::J, "AC07" },
                { Key::K, "AC08" },
                { Key::L, "AC09" },
                { Key::Semicolon, "AC10" },
                { Key::Apostrophe, "AC11" },
                { Key::Z, "AB01" },
                { Key::X, "AB02" },
                { Key::C, "AB03" },
                { Key::V, "AB04" },
                { Key::B, "AB05" },
                { Key::N, "AB06" },
                { Key::M, "AB07" },
                { Key::Comma, "AB08" },
                { Key::Period, "AB09" },
                { Key::Slash, "AB10" },
                { Key::Backslash, "BKSL" },
                { Key::World1, "LSGT" },
                { Key::Space, "SPCE" },
                { Key::Escape, "ESC" },
                { Key::Enter, "RTRN" },
                { Key::Tab, "TAB" },
                { Key::Backspace, "BKSP" },
                { Key::Insert, "INS" },
                { Key::Delete, "DELE" },
                { Key::RightArrow, "RGHT" },
                { Key::LeftArrow, "LEFT" },
                { Key::DownArrow, "DOWN" },
                { Key::UpArrow, "UP" },
                { Key::PageUp, "PGUP" },
                { Key::PageDown, "PGDN" },
                { Key::Home, "HOME" },
                { Key::End, "END" },
                { Key::CapsLock, "CAPS" },
                { Key::ScrollLock, "SCLK" },
                { Key::NumpadLock, "NMLK" },
                { Key::PrintScreen, "PRSC" },
                { Key::Pause, "PAUS" },
                { Key::F1, "FK01" },
                { Key::F2, "FK02" },
                { Key::F3, "FK03" },
                { Key::F4, "FK04" },
                { Key::F5, "FK05" },
                { Key::F6, "FK06" },
                { Key::F7, "FK07" },
                { Key::F8, "FK08" },
                { Key::F9, "FK09" },
                { Key::F10, "FK10" },
                { Key::F11, "FK11" },
                { Key::F12, "FK12" },
                { Key::F13, "FK13" },
                { Key::F14, "FK14" },
                { Key::F15, "FK15" },
                { Key::F16, "FK16" },
                { Key::F17, "FK17" },
                { Key::F18, "FK18" },
                { Key::F19, "FK19" },
                { Key::F20, "FK20" },
                { Key::F21, "FK21" },
                { Key::F22, "FK22" },
                { Key::F23, "FK23" },
                { Key::F24, "FK24" },
                { Key::F25, "FK25" },
                { Key::KeypadNumber0, "KP0" },
                { Key::KeypadNumber1, "KP1" },
                { Key::KeypadNumber2, "KP2" },
                { Key::KeypadNumber3, "KP3" },
                { Key::KeypadNumber4, "KP4" },
                { Key::KeypadNumber5, "KP5" },
                { Key::KeypadNumber6, "KP6" },
                { Key::KeypadNumber7, "KP7" },
                { Key::KeypadNumber8, "KP8" },
                { Key::KeypadNumber9, "KP9" },
                { Key::KeypadDecimal, "KPDL" },
                { Key::KeypadDivide, "KPDV" },
                { Key::KeypadMultiply, "KPMU" },
                { Key::KeypadSubtract, "KPSU" },
                { Key::KeypadAdd, "KPAD" },
                { Key::KeypadEnter, "KPEN" },
                { Key::KeypadEquals, "KPEQ" },
                { Key::LeftShift, "LFSH" },
                { Key::LeftControl, "LCTL" },
                { Key::LeftAlt, "LALT" },
                { Key::LeftSystem, "LWIN" },
                { Key::RightShift, "RTSH" },
                { Key::RightControl, "RCTL" },
                { Key::RightAlt, "RALT" },
                { Key::RightAlt, "LVL3" },
                { Key::RightAlt, "MDSW" },
                { Key::RightSystem, "RWIN" },
                { Key::Menu, "MENU" }
            };

            // Map every X11 key code to an index in the key table
            for (uint32 i = minimumKeyCode; i <= std::min(maximumKeyCode, static_cast<int32>(KEY_TABLE.size()) - 1); i++)
            {
                Key key = Key::Unknown;

                // Try to map key using the US layout
                for (const auto [currentKey, currentKeyName] : keyNameTable)
                {
                    if (std::strncmp(keyboardDescription->names->keys[i].name, currentKeyName, XkbKeyNameLength) == 0)
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

                        for (const auto [currentKey, currentKeyName] : keyNameTable)
                        {
                            if (std::strncmp(keyboardDescription->names->key_aliases[j].alias, currentKeyName, XkbKeyNameLength) == 0)
                            {
                                key = currentKey;
                                break;
                            }
                        }
                    }
                }

                KEY_TABLE[i] = key;
            }

            // Deallocate keyboard info memory
            XkbFreeNames(keyboardDescription, XkbKeyNamesMask, True);
            XkbFreeKeyboard(keyboardDescription, 0, True);
        }
        else
        {
            // Get raw X11's key codes for the display
            XDisplayKeycodes(createInfo.display, &minimumKeyCode, &maximumKeyCode);
        }

        // Retrieve standard key sym mapping
        int width;
        KeySym* keySyms = XGetKeyboardMapping(createInfo.display, minimumKeyCode, maximumKeyCode - minimumKeyCode + 1, &width);

        // Translate unmapped entries using regular X11 KeySym lookups
        for (uint32 scancode = minimumKeyCode; scancode <= std::min(maximumKeyCode, static_cast<int32>(KEY_TABLE.size())); scancode++)
        {
            if (KEY_TABLE[scancode] == Key::Unknown) KEY_TABLE[scancode] = TranslateKeySyms(&keySyms[(scancode - minimumKeyCode) * width], width);
        }

        // Free key syms and mark table as configured
        XFree(keySyms);
    }

    /* --- PRIVATE METHODS --- */

    Key XkbExtension::TranslateKeySyms(const KeySym* keySyms, int32 width)
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
                case XK_KP_Decimal:     return Key::KeypadDecimal;
                case XK_KP_Equal:       return Key::KeypadEquals;
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
            case XK_KP_Divide:      return Key::KeypadDivide;
            case XK_KP_Multiply:    return Key::KeypadMultiply;
            case XK_KP_Subtract:    return Key::KeypadSubtract;
            case XK_KP_Add:         return Key::KeypadAdd;
            case XK_KP_Insert:      return Key::KeypadNumber0;
            case XK_KP_End:         return Key::KeypadNumber1;
            case XK_KP_Down:        return Key::KeypadNumber2;
            case XK_KP_Page_Down:   return Key::KeypadNumber3;
            case XK_KP_Left:        return Key::KeypadNumber4;
            case XK_KP_Right:       return Key::KeypadNumber6;
            case XK_KP_Home:        return Key::KeypadNumber7;
            case XK_KP_Up:          return Key::KeypadNumber8;
            case XK_KP_Page_Up:     return Key::KeypadNumber9;
            case XK_KP_Delete:      return Key::KeypadDecimal;
            case XK_KP_Equal:       return Key::KeypadEquals;
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
            case XK_equal:          return Key::Equals;
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

    XrandrExtension::XrandrExtension(const XrandrExtensionCreateInfo &createInfo)
    {
        int versionMajor, versionMinor;
        if (XRRQueryExtension(createInfo.display, &eventBase, &versionMajor))
        {
            if (XRRQueryVersion(createInfo.display, &versionMajor, &versionMinor))
            {
                SR_ERROR_IF(!(versionMajor > 1 || versionMinor >= 3), "X11 does not support the Xrandr extension, which is mandatory!");
            }
        }
    }

}