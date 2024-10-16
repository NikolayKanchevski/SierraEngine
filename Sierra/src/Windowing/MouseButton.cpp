//
// Created by Nikolay Kanchevski on 22.09.24.
//

#include "MouseButton.h"

namespace Sierra
{

    std::string_view GetMouseButtonName(const MouseButton mouseButton) noexcept
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


}