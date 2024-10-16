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
        /* --- COPY SEMANTICS --- */
        YAMLSerializer(const YAMLSerializer&) = delete;
        YAMLSerializer& operator=(const YAMLSerializer&) = delete;
        
        /* --- DESTRUCTOR --- */
        ~YAMLSerializer() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        YAMLSerializer() noexcept = default;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetMetadataNodeSize(const AssetMetadata& metadata) const noexcept;
        [[nodiscard]] size GetMetadataArenaSize(const AssetMetadata& metadata) const noexcept;
        void SerializeMetadata(ryml::NodeRef root, const AssetMetadata& metadata) const noexcept;

        /* --- MOVE SEMANTICS --- */
        YAMLSerializer(YAMLSerializer&&) noexcept = default;
        YAMLSerializer& operator=(YAMLSerializer&&) noexcept = default;

    };

}
