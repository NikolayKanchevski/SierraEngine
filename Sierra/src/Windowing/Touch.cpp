//
// Created by Nikolay Kanchevski on 22.09.24.
//

#include "Touch.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Touch::Touch(const TouchCreateInfo& createInfo)
        : ID(createInfo.ID), type(createInfo.type), tapTime(createInfo.tapTime), force(createInfo.force), position(createInfo.position), lastPosition(createInfo.lastPosition)
    {

    }

}