//
// Created by Nikolay Kanchevski on 25.07.24.
//

#pragma once

#include <ryml.hpp>
#include <ryml_std.hpp>
#include <c4/std/std.hpp>

#include "../AssetSerializer.h"

namespace SierraEngine
{

    class YAMLSerializer : public virtual AssetSerializer
    {
    public:
        /* --- DESTRUCTOR --- */
        ~YAMLSerializer() override = default;

    protected:
        YAMLSerializer() = default;

        [[nodiscard]] static size GetMetadataNodeSize(const AssetMetadata& metadata);
        [[nodiscard]] static size GetMetadataArenaSize(const AssetMetadata& metadata);
        static void SerializeMetadata(ryml::NodeRef root, const AssetMetadata& metadata);

    };

}
