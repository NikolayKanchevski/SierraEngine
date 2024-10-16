//
// Created by Nikolay Kanchevski on 22.09.24.
//

#include "Version.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Version::Version(const VersionCreateInfo createInfo) noexcept
        : major(createInfo.major), minor(createInfo.minor), patch(createInfo.patch)
    {

    }

}