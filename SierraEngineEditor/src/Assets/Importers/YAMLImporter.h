//
// Created by Nikolay Kanchevski on 25.07.24.
//

#pragma once

#include <ryml.hpp>
#include <ryml_std.hpp>
#include <c4/std/std.hpp>

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
        [[nodiscard]] std::optional<AssetID> ImportID(ryml::ConstNodeRef root) const noexcept;
        [[nodiscard]] std::optional<AssetMetadata> ImportMetadata(ryml::ConstNodeRef root) const noexcept;

        [[nodiscard]] std::optional<std::string> ImportString(const ryml::ConstNodeRef node) const noexcept
        {
            if (node.val_is_null()) return std::nullopt;
            std::string value = { }; node >> value;
            return value;
        }

        template<NumericType Numeric>
        [[nodiscard]] std::optional<Numeric> ImportNumeric(const ryml::ConstNodeRef node) const noexcept
        {
            if (node.val_is_null()) return std::nullopt;
            Numeric value = { }; node >> value;
            return value;
        }

        template<VectorType Vector>
        [[nodiscard]] std::optional<Vector> ImportVector(const ryml::ConstNodeRef node) const noexcept
        {
            if (node.key_is_null()) return std::nullopt;

            Vector value = { };
            for (size i = 0; i < Vector::length(); i++)
            {
                if (node[i].val_is_null()) return std::nullopt;
                node[i] >> value[i];
            }

            return value;
        }

        template<EnumType Enum>
        [[nodiscard]] std::optional<Enum> ImportEnum(const ryml::ConstNodeRef node, Enum(*Converter)(std::string_view)) const noexcept
        {
            if (node.val_is_null()) return std::nullopt;
            return Converter(*ImportString(node));
        }

        /* --- MOVE SEMANTICS --- */
        YAMLImporter(YAMLImporter&&) noexcept = default;
        YAMLImporter& operator=(YAMLImporter&&) noexcept = default;

    };

}
