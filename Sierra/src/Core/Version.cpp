//
// Created by Nikolay Kanchevski on 26.08.23.
//

#include "Version.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Version::Version(const VersionCreateInfo &createInfo)
        : MAJOR(createInfo.major), MINOR(createInfo.minor), PATCH(createInfo.patch), STATE(createInfo.state)
    {

    }

}   