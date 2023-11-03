//
// Created by Nikolay Kanchevski on 10.29.23.
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

    constexpr static const char* GetMouseButtonName(const MouseButton button)
    {
        switch (button)
        {
            case MouseButton::Unknown:          return "Unknown";
            case MouseButton::Left:             return "Left";
            case MouseButton::Right:            return "Right";
            case MouseButton::Middle:           return "Middle";
            case MouseButton::Extra1:           return "Extra 1";
            case MouseButton::Extra2:           return "Extra 2";
        }

        return "Unknown";
    }

}