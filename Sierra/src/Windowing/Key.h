//
// Created by Nikolay Kanchevski on 22.09.24.
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
        Equal,
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
        KeypadPeriod,
        KeypadSlash,
        KeypadAsterisk,
        KeypadMinus,
        KeypadPlus,
        KeypadEnter,
        KeypadEqual,
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
        // Windows Keys
        LeftWindows = LeftSystem,
        RightWindows = RightSystem,
        // macOS Keys
        LeftCommand = LeftSystem,
        RightCommand = RightSystem,
        LeftOption = LeftAlt,
        RightOption = RightAlt,
        // Platform Distinction
        #if SR_PLATFORM_APPLE
            LeftPlatformModifier = LeftCommand,
            RightPlatformModifier = RightCommand,
        #else
            LeftPlatformModifier = LeftControl,
            RightPlatformModifier = RightControl,
        #endif
        // Shorthands
        Shift = LeftShift,
        Control = LeftControl,
        Alt = LeftAlt,
        System = LeftSystem,
        Windows = LeftWindows,
        Command = LeftCommand,
        Option = LeftOption,
        PlatformModifier = LeftPlatformModifier
    };

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API Key KeyToAlternativeKey(Key key) noexcept;
    [[nodiscard]] SIERRA_API char KeyToCharacter(Key key) noexcept;
    [[nodiscard]] SIERRA_API std::string_view KeyToString(Key key) noexcept;
    [[nodiscard]] SIERRA_API std::string KeyCombinationToString(std::span<const Key> keys) noexcept;

    template<size Size>
    class KeyCombination
    {
    public:
        /* --- CONSTRUCTORS --- */
        template<typename... Args> requires (sizeof...(Args) + 1 == Size && (std::same_as<Args, Key> && ...))
        consteval KeyCombination(const Key first, const Args... rest) noexcept
            : keys { first, rest... }
        {

        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] consteval operator std::span<const Key>() const noexcept { return keys; }

    private:
        std::array<Key, Size> keys = { };

    };

    template<typename... Args>
    KeyCombination(Key, Args...) -> KeyCombination<sizeof...(Args) + 1>;

}
