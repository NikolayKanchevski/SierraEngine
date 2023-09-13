//
// Created by Nikolay Kanchevski on 14.08.23.
//

#pragma once

#include "Event.h"

namespace Sierra
{

    BETTER_ENUM(
        Key, uint16,
        UNKNOWN,
        SPACE,
        APOSTROPHE,
        COMMA,
        MINUS,
        PERIOD,
        SLASH,
        NUM0,
        NUM1,
        NUM2,
        NUM3,
        NUM4,
        NUM5,
        NUM6,
        NUM7,
        NUM8,
        NUM9,
        SEMICOLON,
        EQUAL,
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
        LEFT_BRACKET,
        BACKSLASH,
        RIGHT_BRACKET,
        GRAVE_ACCENT,
        WORLD_1,
        WORLD_2,
        ESCAPE,
        ENTER,
        TAB,
        BACKSPACE,
        INSERT,
        DELETE,
        RIGHT,
        LEFT,
        DOWN,
        UP,
        PAGE_UP,
        PAGE_DOWN,
        HOME,
        END,
        CAPS_LOCK,
        SCROLL_LOCK,
        NUM_LOCK,
        PRINT_SCREEN,
        PAUSE,
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
        KP_0,
        KP_1,
        KP_2,
        KP_3,
        KP_4,
        KP_5,
        KP_6,
        KP_7,
        KP_8,
        KP_9,
        KP_DECIMAL,
        KP_DIVIDE,
        KP_MULTIPLY,
        KP_SUBTRACT,
        KP_ADD,
        KP_ENTER,
        KP_EQUAL,
        LEFT_SHIFT,
        LEFT_CONTROL,
        LEFT_ALT,
        LEFT_SUPER,
        RIGHT_SHIFT,
        RIGHT_CONTROL,
        RIGHT_ALT,
        RIGHT_SUPER,
        MENU
    );

    class SIERRA_API KeyEvent : public Event
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Key GetKey() const { return key; }

    protected:
        inline explicit KeyEvent(const Key key) : key(key) { }

    private:
        Key key = Key::UNKNOWN;

    };

    class SIERRA_API KeyPressedEvent final : public KeyEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit KeyPressedEvent(const Key pressedKey, const uint32 repeatCount = 1) : KeyEvent(pressedKey), repeatCount(repeatCount) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetRepeatCount() const { return repeatCount; }
        [[nodiscard]] inline bool IsHeld() const { return repeatCount > 1; }

        /* --- PROPERTIES --- */
        DEFINE_EVENT_CLASS_STRING_OPERATOR("Key Pressed - [Key Name: " << GetKey()._to_string() << " | Key Code: " << static_cast<uint32>(GetKey()) << "]");

    private:
        uint32 repeatCount;

    };

    class SIERRA_API KeyReleasedEvent final : public KeyEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit KeyReleasedEvent(const Key releasedKey) : KeyEvent(releasedKey) { }

        /* --- PROPERTIES --- */
        DEFINE_EVENT_CLASS_STRING_OPERATOR("Key Released - [Key Name: " << GetKey()._to_string() << " | Key Code: " << static_cast<uint32>(GetKey()) << "]");

    };

}