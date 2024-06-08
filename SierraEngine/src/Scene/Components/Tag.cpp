//
// Created by Nikolay Kanchevski on 22.04.24.
//

#include "Tag.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Tag::Tag(const std::string_view tag)
    {
        SetTag(tag);
    }

    /* --- SETTER METHODS --- */

    void Tag::SetTag(const std::string_view tag)
    {
        const uint32 length = std::min(static_cast<uint32>(tag.size()), static_cast<uint32>(tagMemory.max_size()));
        APP_WARNING_IF(length < tag.size(), "Could not properly update tag from [{0}] to [{1}], as new tag exceeds the [{2}] character limit, so only the first [{3}] characters were updated.", std::string_view(tagMemory.data(), tagMemory.size()), tag, tagMemory.max_size(), tagMemory.max_size());

        std::strncpy(tagMemory.data(), tag.data(), length);
        std::memset(&tagMemory[length], '\0', static_cast<uint32>(tagMemory.max_size()) - length);
    }

}
