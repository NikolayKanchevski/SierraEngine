//
// Created by Nikolay Kanchevski on 22.04.24.
//

#include "Tag.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Tag::Tag(const std::string_view tag)
        : tag(tag)
    {

    }

    /* --- SETTER METHODS --- */

    void Tag::SetTag(const std::string_view givenTag)
    {
        tag = givenTag;
    }

}
