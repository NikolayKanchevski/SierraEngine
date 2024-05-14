//
// Created by Nikolay Kanchevski on 22.04.24.
//

#include "Tag.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Tag::Tag(const std::string_view tag)
    {
        SetName(tag);
    }

    /* --- SETTER METHODS --- */

    void Tag::SetName(const std::string_view newTag)
    {
        const uint32 length = std::min(static_cast<uint32>(newTag.size()), static_cast<uint32>(tag.max_size()));
        std::strncpy(tag.data(), newTag.data(), length);
        std::memset(&tag[length], '\0', static_cast<uint32>(tag.max_size()) - length);
    }

}
