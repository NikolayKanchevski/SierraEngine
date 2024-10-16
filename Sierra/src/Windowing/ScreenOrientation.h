//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

namespace Sierra
{

    enum class ScreenOrientation : uint8
    {
        Unknown                   = 0x0000,
        Portrait                  = 0x0001,
        PortraitFlipped           = 0x0002,
        PortraitAny               = Portrait | PortraitFlipped,
        Landscape                 = 0x0004,
        LandscapeFlipped          = 0x0008,
        LandscapeAny              = Landscape | LandscapeFlipped,
        Any                       = PortraitAny | LandscapeAny
    };
    SR_DEFINE_ENUM_FLAG_OPERATORS(ScreenOrientation);

}
