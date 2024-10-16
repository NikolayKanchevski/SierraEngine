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
    [[nodiscard]] SIERRA_API std::string_view GetMouseButtonName(MouseButton mouseButton) noexcept;

}
