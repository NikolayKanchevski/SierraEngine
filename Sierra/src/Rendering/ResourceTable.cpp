//
// Created by Nikolay Kanchevski on 9.03.24.
//

#include "ResourceTable.h"

#include "../Utilities/Handle.hpp"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    ResourceTable::ResourceTable(const ResourceTableCreateInfo& createInfo)
    {
        SR_THROW_IF(createInfo.name.empty(), InvalidValueError("Cannot create resource table, as specified name must not be empty"));
    }

}