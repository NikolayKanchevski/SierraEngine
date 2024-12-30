//
// Created by Nikolay Kanchevski on 25.07.24.
//

#pragma once

#include <ryml.hpp>
#include <ryml_std.hpp>
#include <c4/std/std.hpp>

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

        /* --- POLLING METHODS --- */
        void SerializeID(ryml::NodeRef root, AssetID ID) const;
        void SerializeMetadata(ryml::NodeRef root, const AssetMetadata& metadata) const;

        void SerializeString(ryml::NodeRef node, const std::string_view value) const
        {
            node |= ryml::VAL_PLAIN;
            node = c4::to_csubstr(value);
        }

        template<NumericType Numeric>
        void SerializeNumeric(ryml::NodeRef node, const Numeric value) const
        {
            node |= ryml::VAL_PLAIN;
            node << SR_FORMAT(!FloatingPointType<Numeric> ? "{0}" : "{0:.2f}", value);
        }

        template<VectorType Vector>
        void SerializeVector(ryml::NodeRef node, const Vector value) const
        {
            node |= ryml::SEQ | ryml::FLOW_SL;
            for (size i = 0; i < Vector::length(); i++)
            {
                ryml::NodeRef child = node.append_child();
                child |= ryml::VAL_PLAIN;

                const std::string_view spacing = i == 0 ? "" : " ";
                child << SR_FORMAT(!FloatingPointType<typename Vector::value_type> ? "{0}{1}" : "{0}{1:.2f}", spacing, value[i]);
            }
        }

        template<EnumType Enum>
        void SerializeEnum(ryml::NodeRef node, const Enum value, std::string_view(*Converter)(Enum)) const
        {
            SerializeString(node, Converter(value));
        }

        template<typename T, typename U = T> requires (std::is_convertible_v<T, U>)
        void SerializeContainer(ryml::NodeRef node, const std::span<const T> container, void(YAMLSerializer::*const Converter)(ryml::NodeRef, const U) const) const
        {
            node |= ryml::SEQ;
            for (const T& item : container)
            {
                (this->*Converter)(node.append_child(), item);
            }
        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetMetadataNodeCount(const AssetMetadata& metadata) const noexcept;
        [[nodiscard]] size GetMetadataArenaSize(const AssetMetadata& metadata) const noexcept;

        /* --- MOVE SEMANTICS --- */
        YAMLSerializer(YAMLSerializer&&) noexcept = default;
        YAMLSerializer& operator=(YAMLSerializer&&) noexcept = default;
    };

}
