//
// Created by Nikolay Kanchevski on 22.04.24.
//

#include "Tag.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Tag::Tag(const std::string_view tag) noexcept
        : tag(tag)
    {

    }

    /* --- SETTER METHODS --- */

    void Tag::SetTag(const std::string_view givenTag) noexcept
    {
        tag = givenTag;
    }

}
