//
// Created by Nikolay Kanchevski on 18.09.26.
//

#include "YAML.h"

namespace SierraEngine
{

    void YAML::SerializeString(ryml::NodeRef node, const std::string_view string)
    {
        node |= ryml::VAL_PLAIN;
        node = c4::to_csubstr(string);
    }

    // ReSharper disable once CppPassValueParameterByConstReference
    void YAML::SerializeString(const ryml::NodeRef node, const std::string string)
    {
        SerializeString(node, std::string_view(string));
    }


    [[nodiscard]] std::optional<std::string> YAML::ImportString(const ryml::ConstNodeRef node) noexcept
    {
        if (node.invalid())
            return std::nullopt;

        std::string value = { }; node >> value;
        return value;
    }

}