//
// Created by Nikolay Kanchevski on 22.04.24.
//

#include "Tag.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Tag::Tag(const std::string_view givenName)
    {
        strncpy(name.data(), givenName.data(), glm::min(givenName.size(), name.max_size()));
    }

}