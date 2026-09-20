//
// Created by Nikolay Kanchevski on 25.07.24.
//

#pragma once

#include <ryml.hpp>

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
        /* --- CONSTANTS --- */
        constexpr static uint32 MANDATORY_NODE_COUNT = 2; // Root + ID

        /* --- CONSTRUCTORS --- */
        YAMLSerializer() noexcept = default;

        /* --- POLLING METHODS --- */
        void SerializeID(ryml::NodeRef rootNode, AssetID ID) const;
        void SerializeMetadata(ryml::NodeRef rootNode, const AssetMetadata& metadata) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetMetadataNodeCount(const AssetMetadata& metadata) const noexcept;
        [[nodiscard]] size GetMetadataArenaSize(const AssetMetadata& metadata) const noexcept;

        /* --- MOVE SEMANTICS --- */
        YAMLSerializer(YAMLSerializer&&) noexcept = default;
        YAMLSerializer& operator=(YAMLSerializer&&) noexcept = default;
    };

}
