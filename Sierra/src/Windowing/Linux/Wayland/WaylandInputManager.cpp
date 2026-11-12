//
// Created by Nikolay Kanchevski on 11.12.26.
//

#include "WaylandInputManager.h"

#include <linux/input-event-codes.h>

namespace Sierra
{

    namespace
    {
        // Maps Linux evdev scancodes (see <linux/input-event-codes.h>), which is what Wayland keyboards report, to keys
        constexpr std::array<Key, 211> KEY_TABLE
        {
            Key::Unknown,         // KEY_RESERVED            0
            Key::Escape,          // KEY_ESC                 1
            Key::Number1,         // KEY_1                   2
            Key::Number2,         // KEY_2                   3
            Key::Number3,         // KEY_3                   4
            Key::Number4,         // KEY_4                   5
            Key::Number5,         // KEY_5                   6
            Key::Number6,         // KEY_6                   7
            Key::Number7,         // KEY_7                   8
            Key::Number8,         // KEY_8                   9
            Key::Number9,         // KEY_9                   10
            Key::Number0,         // KEY_0                   11
            Key::Minus,           // KEY_MINUS               12
            Key::Equal,           // KEY_EQUAL               13
            Key::Backspace,       // KEY_BACKSPACE           14
            Key::Tab,             // KEY_TAB                 15
            Key::Q,               // KEY_Q                   16
            Key::W,               // KEY_W                   17
            Key::E,               // KEY_E                   18
            Key::R,               // KEY_R                   19
            Key::T,               // KEY_T                   20
            Key::Y,               // KEY_Y                   21
            Key::U,               // KEY_U                   22
            Key::I,               // KEY_I                   23
            Key::O,               // KEY_O                   24
            Key::P,               // KEY_P                   25
            Key::LeftBracket,     // KEY_LEFTBRACE           26
            Key::RightBracket,    // KEY_RIGHTBRACE          27
            Key::Enter,           // KEY_ENTER               28
            Key::LeftControl,     // KEY_LEFTCTRL            29
            Key::A,               // KEY_A                   30
            Key::S,               // KEY_S                   31
            Key::D,               // KEY_D                   32
            Key::F,               // KEY_F                   33
            Key::G,               // KEY_G                   34
            Key::H,               // KEY_H                   35
            Key::J,               // KEY_J                   36
            Key::K,               // KEY_K                   37
            Key::L,               // KEY_L                   38
            Key::Semicolon,       // KEY_SEMICOLON           39
            Key::Apostrophe,      // KEY_APOSTROPHE          40
            Key::Grave,           // KEY_GRAVE               41
            Key::LeftShift,       // KEY_LEFTSHIFT           42
            Key::Backslash,       // KEY_BACKSLASH           43
            Key::Z,               // KEY_Z                   44
            Key::X,               // KEY_X                   45
            Key::C,               // KEY_C                   46
            Key::V,               // KEY_V                   47
            Key::B,               // KEY_B                   48
            Key::N,               // KEY_N                   49
            Key::M,               // KEY_M                   50
            Key::Comma,           // KEY_COMMA               51
            Key::Period,          // KEY_DOT                 52
            Key::Slash,           // KEY_SLASH               53
            Key::RightShift,      // KEY_RIGHTSHIFT          54
            Key::KeypadAsterisk,  // KEY_KPASTERISK          55
            Key::LeftAlt,         // KEY_LEFTALT             56
            Key::Space,           // KEY_SPACE               57
            Key::CapsLock,        // KEY_CAPSLOCK            58
            Key::F1,              // KEY_F1                  59
            Key::F2,              // KEY_F2                  60
            Key::F3,              // KEY_F3                  61
            Key::F4,              // KEY_F4                  62
            Key::F5,              // KEY_F5                  63
            Key::F6,              // KEY_F6                  64
            Key::F7,              // KEY_F7                  65
            Key::F8,              // KEY_F8                  66
            Key::F9,              // KEY_F9                  67
            Key::F10,             // KEY_F10                 68
            Key::NumpadLock,      // KEY_NUMLOCK             69
            Key::ScrollLock,      // KEY_SCROLLLOCK          70
            Key::KeypadNumber7,   // KEY_KP7                 71
            Key::KeypadNumber8,   // KEY_KP8                 72
            Key::KeypadNumber9,   // KEY_KP9                 73
            Key::KeypadMinus,  // KEY_KPMINUS             74
            Key::KeypadNumber4,   // KEY_KP4                 75
            Key::KeypadNumber5,   // KEY_KP5                 76
            Key::KeypadNumber6,   // KEY_KP6                 77
            Key::KeypadPlus,       // KEY_KPPLUS              78
            Key::KeypadNumber1,   // KEY_KP1                 79
            Key::KeypadNumber2,   // KEY_KP2                 80
            Key::KeypadNumber3,   // KEY_KP3                 81
            Key::KeypadNumber0,   // KEY_KP0                 82
            Key::KeypadPeriod,   // KEY_KPDOT               83
            Key::Unknown,
            Key::Unknown,         // KEY_ZENKAKUHANKAKU      85
            Key::World1,          // KEY_102ND               86
            Key::F11,             // KEY_F11                 87
            Key::F12,             // KEY_F12                 88
            Key::Unknown,         // KEY_RO                  89
            Key::Unknown,         // KEY_KATAKANA            90
            Key::Unknown,         // KEY_HIRAGANA            91
            Key::Unknown,         // KEY_HENKAN              92
            Key::Unknown,         // KEY_KATAKANAHIRAGANA    93
            Key::Unknown,         // KEY_MUHENKAN            94
            Key::Unknown,         // KEY_KPJPCOMMA           95
            Key::KeypadEnter,     // KEY_KPENTER             96
            Key::RightControl,    // KEY_RIGHTCTRL           97
            Key::KeypadSlash,    // KEY_KPSLASH             98
            Key::PrintScreen,     // KEY_SYSRQ               99
            Key::RightAlt,        // KEY_RIGHTALT            100
            Key::Unknown,         // KEY_LINEFEED            101
            Key::Home,            // KEY_HOME                102
            Key::UpArrow,         // KEY_UP                  103
            Key::PageUp,          // KEY_PAGEUP              104
            Key::LeftArrow,       // KEY_LEFT                105
            Key::RightArrow,      // KEY_RIGHT               106
            Key::End,             // KEY_END                 107
            Key::DownArrow,       // KEY_DOWN                108
            Key::PageDown,        // KEY_PAGEDOWN            109
            Key::Insert,          // KEY_INSERT              110
            Key::Delete,          // KEY_DELETE              111
            Key::Unknown,         // KEY_MACRO               112
            Key::Unknown,         // KEY_MUTE                113
            Key::Unknown,         // KEY_VOLUMEDOWN          114
            Key::Unknown,         // KEY_VOLUMEUP            115
            Key::Unknown,         // KEY_POWER               116	/* SC System Power Down */
            Key::KeypadEqual,     // KEY_KPEQUAL             117
            Key::KeypadMinus,  // KEY_KPPLUSMINUS         118
            Key::Pause,           // KEY_PAUSE               119
            Key::Unknown,         // KEY_SCALE               120	/* AL Compiz Scale (Expose) */
            Key::KeypadPeriod,   // KEY_KPCOMMA             121
            Key::Unknown,         // KEY_HANGEUL             122
            Key::Unknown,         // KEY_HANJA               123
            Key::World2,          // KEY_YEN                 124
            Key::LeftSystem,      // KEY_LEFTMETA            125
            Key::RightSystem,     // KEY_RIGHTMETA           126
            Key::Menu,            // KEY_COMPOSE             127
            Key::Unknown,         // KEY_STOP                128	/* AC Stop */
            Key::Unknown,         // KEY_AGAIN               129
            Key::Unknown,         // KEY_PROPS               130	/* AC Properties */
            Key::Unknown,         // KEY_UNDO                131	/* AC Undo */
            Key::Unknown,         // KEY_FRONT               132
            Key::Unknown,         // KEY_COPY                133	/* AC Copy */
            Key::Unknown,         // KEY_OPEN                134	/* AC Open */
            Key::Unknown,         // KEY_PASTE               135	/* AC Paste */
            Key::Unknown,         // KEY_FIND                136	/* AC Search */
            Key::Unknown,         // KEY_CUT                 137	/* AC Cut */
            Key::Unknown,         // KEY_HELP                138	/* AL Integrated Help Center */
            Key::Unknown,         // KEY_MENU                139	/* Menu (show menu) */
            Key::Unknown,         // KEY_CALC                140	/* AL Calculator */
            Key::Unknown,         // KEY_SETUP               141
            Key::Unknown,         // KEY_SLEEP               142	/* SC System Sleep */
            Key::Unknown,         // KEY_WAKEUP              143	/* System Wake Up */
            Key::Unknown,         // KEY_FILE                144	/* AL Local Machine Browser */
            Key::Unknown,         // KEY_SENDFILE            145
            Key::Unknown,         // KEY_DELETEFILE          146
            Key::Unknown,         // KEY_XFER                147
            Key::Unknown,         // KEY_PROG1               148
            Key::Unknown,         // KEY_PROG2               149
            Key::Unknown,         // KEY_WWW                 150	/* AL Internet Browser */
            Key::Unknown,         // KEY_MSDOS               151
            Key::Unknown,         // KEY_COFFEE              152	/* AL Terminal Lock/Screensaver */
            Key::Unknown,         // KEY_ROTATE_DISPLAY      153	/* Display orientation for e.g. tablets */
            Key::Unknown,         // KEY_CYCLEWINDOWS        154
            Key::Unknown,         // KEY_MAIL                155
            Key::Unknown,         // KEY_BOOKMARKS           156	/* AC Bookmarks */
            Key::Unknown,         // KEY_COMPUTER            157
            Key::Unknown,         // KEY_BACK                158	/* AC Back */
            Key::Unknown,         // KEY_FORWARD             159	/* AC Forward */
            Key::Unknown,         // KEY_CLOSECD             160
            Key::Unknown,         // KEY_EJECTCD             161
            Key::Unknown,         // KEY_EJECTCLOSECD        162
            Key::Unknown,         // KEY_NEXTSONG            163
            Key::Unknown,         // KEY_PLAYPAUSE           164
            Key::Unknown,         // KEY_PREVIOUSSONG        165
            Key::Unknown,         // KEY_STOPCD              166
            Key::Unknown,         // KEY_RECORD              167
            Key::Unknown,         // KEY_REWIND              168
            Key::Unknown,         // KEY_PHONE               169	/* Media Select Telephone */
            Key::Unknown,         // KEY_ISO                 170
            Key::Unknown,         // KEY_CONFIG              171	/* AL Consumer Control Configuration */
            Key::Unknown,         // KEY_HOMEPAGE            172	/* AC Home */
            Key::Unknown,         // KEY_REFRESH             173	/* AC Refresh */
            Key::Unknown,         // KEY_EXIT                174	/* AC Exit */
            Key::Unknown,         // KEY_MOVE                175
            Key::Unknown,         // KEY_EDIT                176
            Key::Unknown,         // KEY_SCROLLUP            177
            Key::Unknown,         // KEY_SCROLLDOWN          178
            Key::Unknown,         // KEY_KPLEFTPAREN         179
            Key::Unknown,         // KEY_KPRIGHTPAREN        180
            Key::Unknown,         // KEY_NEW                 181	/* AC New */
            Key::Unknown,         // KEY_REDO                182	/* AC Redo/Repeat */
            Key::F13,             // KEY_F13                 183
            Key::F14,             // KEY_F14                 184
            Key::F15,             // KEY_F15                 185
            Key::F16,             // KEY_F16                 186
            Key::F17,             // KEY_F17                 187
            Key::F18,             // KEY_F18                 188
            Key::F19,             // KEY_F19                 189
            Key::F20,             // KEY_F20                 190
            Key::F21,             // KEY_F21                 191
            Key::F22,             // KEY_F22                 192
            Key::F23,             // KEY_F23                 193
            Key::F24,             // KEY_F24                 194
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,
            Key::Unknown,         // KEY_PLAYCD              200
            Key::Unknown,         // KEY_PAUSECD             201
            Key::Unknown,         // KEY_PROG3               202
            Key::Unknown,         // KEY_PROG4               203
            Key::Unknown,         // KEY_ALL_APPLICATIONS    204	/* AC Desktop Show All Applications */
            Key::Unknown,         // KEY_SUSPEND             205
            Key::Unknown,         // KEY_CLOSE               206	/* AC Close */
            Key::Unknown,         // KEY_PLAY                207
            Key::Unknown,         // KEY_FASTFORWARD         208
            Key::Unknown,         // KEY_BASSBOOST           209
            Key::PrintScreen      // KEY_PRINT               210	/* AC Print */
        };
    }

    /* --- GETTER METHODS --- */

    bool WaylandInputManager::IsKeyPressed(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool WaylandInputManager::IsKeyHeld(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool WaylandInputManager::IsKeyReleased(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool WaylandInputManager::IsKeyResting(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool WaylandInputManager::IsMouseButtonPressed(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool WaylandInputManager::IsMouseButtonHeld(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool WaylandInputManager::IsMouseButtonReleased(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    bool WaylandInputManager::IsMouseButtonResting(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    Vector2 WaylandInputManager::GetMouseScroll() const noexcept
    {
        return mouseScroll;
    }

    WindowingBackendType WaylandInputManager::GetBackendType() const noexcept
    {
        return WindowingBackendType::Wayland;
    }

    /* --- POLLING METHODS --- */

    void WaylandInputManager::RegisterKeyPress(const Key key)
    {
        if (key == Key::Unknown) return;
        keyStates[GetKeyIndex(key)] = InputAction::Press;
        GetKeyPressDispatcher().DispatchEvent(key);
    }

    void WaylandInputManager::RegisterKeyRelease(const Key key)
    {
        if (key == Key::Unknown) return;
        keyStates[GetKeyIndex(key)] = InputAction::Release;
        GetKeyReleaseDispatcher().DispatchEvent(key);
    }

    void WaylandInputManager::RegisterMouseButtonPress(const MouseButton mouseButton)
    {
        if (mouseButton == MouseButton::Unknown) return;
        mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
        GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
    }

    void WaylandInputManager::RegisterMouseButtonRelease(const MouseButton mouseButton)
    {
        if (mouseButton == MouseButton::Unknown) return;
        mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
        GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
    }

    void WaylandInputManager::RegisterMouseScroll(const Vector2 scroll)
    {
        // Compositors may report scrolling multiple times in between two updates (high-resolution wheels, touchpads), so it is accumulated
        mouseScroll += scroll;
        GetMouseScrollDispatcher().DispatchEvent(scroll);
    }

    /* --- CONVERSIONS --- */

    Key WaylandInputManager::ScancodeToKey(const uint32 scancode) noexcept
    {
        return scancode < KEY_TABLE.size() ? KEY_TABLE[scancode] : Key::Unknown;
    }

    MouseButton WaylandInputManager::ButtonCodeToMouseButton(const uint32 buttonCode) noexcept
    {
        switch (buttonCode)
        {
            case BTN_LEFT:      return MouseButton::Left;
            case BTN_RIGHT:     return MouseButton::Right;
            case BTN_MIDDLE:    return MouseButton::Middle;
            case BTN_SIDE:      return MouseButton::Extra1;
            case BTN_EXTRA:     return MouseButton::Extra2;
            default:            break;
        }

        return MouseButton::Unknown;
    }

    /* --- PRIVATE METHODS --- */

    void WaylandInputManager::Update()
    {
        std::ranges::copy(keyStates, lastKeyStates.begin());
        std::ranges::copy(mouseButtonStates, lastMouseButtonStates.begin());
        mouseScroll = { 0, 0 };
    }

    void WaylandInputManager::ReleaseKeys()
    {
        for (uint32 i = 0; i < KEY_COUNT; i++)
        {
            if (keyStates[i] == InputAction::Press) RegisterKeyRelease(static_cast<Key>(i));
        }
    }

    void WaylandInputManager::ReleaseMouseButtons()
    {
        for (uint32 i = 0; i < MOUSE_BUTTON_COUNT; i++)
        {
            if (mouseButtonStates[i] == InputAction::Press) RegisterMouseButtonRelease(static_cast<MouseButton>(i));
        }
    }

}
