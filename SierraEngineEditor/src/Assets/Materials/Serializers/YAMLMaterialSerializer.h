//
// Created by Nikolay Kanchevski on 19.11.24.
//

#pragma once

#include "../../Serializers/YAMLSerializer.h"

namespace SierraEngine
{

    class YAMLMaterialSerializer final : public MaterialSerializer, public YAMLSerializer
    {
    public:
        /* --- CONSTRUCTORS --- */
        YAMLMaterialSerializer() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<SerializedMaterial> Serialize(const MaterialSerializeInfo& serializeInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetSignature GetSignature() const noexcept override { return { 'Y', 'A', 'M', 'T' }; }
        [[nodiscard]] AssetVersion GetVersion() const noexcept override { return AssetVersion({ 1, 0, 0 }); }

        /* --- COPY SEMANTICS --- */
        YAMLMaterialSerializer(const YAMLMaterialSerializer&) = delete;
        YAMLMaterialSerializer& operator=(const YAMLMaterialSerializer&) = delete;

        /* --- MOVE SEMANTICS --- */
        YAMLMaterialSerializer(YAMLMaterialSerializer&&) noexcept = default;
        YAMLMaterialSerializer& operator=(YAMLMaterialSerializer&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~YAMLMaterialSerializer() noexcept override = default;

    private:
        /* --- POLLING METHODS --- */
        void SerializeProperties(ryml::NodeRef root, const MaterialProperties& properties) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetPropertiesNodeCount() const noexcept;
        [[nodiscard]] size GetPropertiesArenaSize() const noexcept;

    };

}