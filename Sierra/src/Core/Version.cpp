//
// Created by Nikolay Kanchevski on 26.08.23.
//

#include "Version.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Version::Version(const VersionCreateInfo &createInfo)
        : major(createInfo.major), minor(createInfo.minor), patch(createInfo.patch)
    {

    }

}   