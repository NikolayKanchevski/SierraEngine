//
// Created by Nikolay Kanchevski on 18.09.26.
//

#pragma once

#include <ryml.hpp>
#include <ryml_std.hpp>
#include <c4/std/std.hpp>

namespace SierraEngine
{

    namespace YAML
    {
        void SerializeString(ryml::NodeRef node, std::string_view string);
        void SerializeString(ryml::NodeRef node, std::string string);

        [[nodiscard]] std::optional<std::string> ImportString(ryml::ConstNodeRef node) noexcept;

        template<NumericType Numeric>
        void SerializeNumeric(ryml::NodeRef node, const Numeric value)
        {
            node.set_val_style(ryml::VAL_PLAIN);
            node.save(SR_FORMAT(!FloatingPointType<Numeric> ? "{0}" : "{0:.2f}", value));
        }

        template<NumericType Numeric>
        [[nodiscard]] std::optional<Numeric> ImportNumeric(const ryml::ConstNodeRef node) noexcept
        {
            if (node.invalid())
                return std::nullopt;

            Numeric value = { };
            node.load(&value);

            return value;
        }

        template<VectorType Vector>
        void SerializeVector(ryml::NodeRef node, const Vector value)
        {
            node.set_seq();
            node.set_container_style(ryml::FLOW_ML1);

            for (size i = 0; i < Vector::length(); i++)
            {
                ryml::NodeRef child = node.append_child();
                child.set_val_style(ryml::VAL_PLAIN);

                const std::string_view spacing = i == 0 ? "" : " ";
                child.save(SR_FORMAT(!FloatingPointType<typename Vector::value_type> ? "{0}{1}" : "{0}{1:.2f}", spacing, value[i]));
            }
        }

        template<VectorType Vector>
        [[nodiscard]] std::optional<Vector> ImportVector(const ryml::ConstNodeRef node) noexcept
        {
            if (node.invalid())
                return std::nullopt;

            Vector value = { };
            for (size i = 0; i < Vector::length(); i++)
            {
                if (node[i].val_is_null())
                    return std::nullopt;

                node[i].load(&value[i]);
            }

            return value;
        }

        template<EnumType Enum>
        void SerializeEnum(ryml::NodeRef node, const Enum value, std::string_view(*Converter)(Enum))
        {
            SerializeString(node, Converter(value));
        }

        template<EnumType Enum>
        [[nodiscard]] std::optional<Enum> ImportEnum(const ryml::ConstNodeRef node, Enum(*Converter)(std::string_view)) noexcept
        {
            if (node.invalid())
                return std::nullopt;

            return Converter(*ImportString(node));
        }

        template<typename T>
        void SerializeContainer(ryml::NodeRef node, const std::span<const T> container, void(*Serializer)(ryml::NodeRef, T)) // TODO: CUSTOM SERIALIZATION + IMPORT
        {
            node.set_seq();
            node.set_container_style(ryml::FLOW_ML1);

            for (const T& item : container)
            {
                ryml::NodeRef childNode = node.append_child();
                childNode.set_val_style(ryml::VAL_PLAIN);

                Serializer(childNode, item);
            }
        }

        template<typename T>
        [[nodiscard]] std::optional<std::vector<T>> ImportContainer(const ryml::ConstNodeRef node, std::optional<T>(*Importer)(ryml::ConstNodeRef) noexcept)
        {
            if (node.invalid())
                return std::nullopt;

            std::vector<T> container = { };
            container.reserve(node.num_children());

            for (const ryml::ConstNodeRef childNode : node.children())
            {
                if (const std::optional<T> item = Importer(childNode); item.has_value())
                {
                    container.emplace_back(*item);
                }
            }

            return container;
        }
    }

}