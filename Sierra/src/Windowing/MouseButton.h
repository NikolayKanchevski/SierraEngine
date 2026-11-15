//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

namespace Sierra
{

    enum class MouseButton : uint8
    {
        Unknown,
        Left,
        Right,
        Middle,
        Extra1,
        Extra2
    };

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API std::string_view MouseButtonToString(MouseButton mouseButton) noexcept;
    [[nodiscard]] SIERRA_API std::string MouseButtonCombinationToString(std::span<const MouseButton> mouseButtons) noexcept;

    template<size Size>
    class MouseButtonCombination
    {
    public:
        /* --- CONSTRUCTORS --- */
        template<typename... Args> requires (sizeof...(Args) + 1 == Size && (std::same_as<Args, MouseButton> && ...))
        consteval MouseButtonCombination(const MouseButton first, const Args... rest) noexcept
            : keys { first, rest... }
        {

        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] consteval operator std::span<const MouseButton>() const noexcept { return keys; }

    private:
        std::array<MouseButton, Size> keys = { };

    };

    template<typename... Args>
    MouseButtonCombination(MouseButton, Args...) -> MouseButtonCombination<sizeof...(Args) + 1>;

}
