//
// Created by Nikolay Kanchevski on 25.07.24.
//

#pragma once

#include <ryml.hpp>

namespace SierraEngine
{

    class YAMLImporter : public virtual AssetImporter
    {
    public:
        /* --- COPY SEMANTICS --- */
        YAMLImporter(const YAMLImporter&) = delete;
        YAMLImporter& operator=(const YAMLImporter&) = delete;

        /* --- DESTRUCTOR --- */
        ~YAMLImporter() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        YAMLImporter() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<AssetID> ImportID(ryml::ConstNodeRef rootNode) const noexcept;
        [[nodiscard]] std::optional<AssetMetadata> ImportMetadata(ryml::ConstNodeRef rootNode) const noexcept;

        /* --- MOVE SEMANTICS --- */
        YAMLImporter(YAMLImporter&&) noexcept = default;
        YAMLImporter& operator=(YAMLImporter&&) noexcept = default;

    };

}
