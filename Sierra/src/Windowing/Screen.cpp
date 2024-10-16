//
// Created by Nikolay Kanchevski on 10.31.23.
//

#include "Screen.h"

namespace Sierra
{

    /* --- POLLING METHODS --- */

    void Screen::RegisterScreenReorient(const ScreenOrientation orientation)
    {
        SR_THROW_IF(orientation == ScreenOrientation::PortraitAny, InvalidValueError(SR_FORMAT("Cannot register screen reorient event for screen [{0}], as specified orientation must not be [ScreenOrientation::PortraitAny]", GetName())));
        SR_THROW_IF(orientation == ScreenOrientation::LandscapeAny, InvalidValueError(SR_FORMAT("Cannot register screen reorient event for screen [{0}], as specified orientation must not be [ScreenOrientation::LandscapeAny]", GetName())));
    }

}