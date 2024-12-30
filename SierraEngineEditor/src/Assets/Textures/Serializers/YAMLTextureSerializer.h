//
// Created by Nikolay Kanchevski on 16.07.24.
//

#pragma once

#include "../../Serializers/YAMLSerializer.h"

namespace SierraEngine
{

    class YAMLTextureSerializer final : public TextureSerializer, public YAMLSerializer
    {
    public:
        /* --- CONSTRUCTORS --- */
        YAMLTextureSerializer() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<SerializedTexture> Serialize(const TextureSerializeInfo& serializeInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetSignature GetSignature() const noexcept override { return { 'Y', 'A', 'T', 'X' }; }
        [[nodiscard]] AssetVersion GetVersion() const noexcept override { return AssetVersion({ 1, 0, 0 }); }

        /* --- COPY SEMANTICS --- */
        YAMLTextureSerializer(const YAMLTextureSerializer&) = delete;
        YAMLTextureSerializer& operator=(const YAMLTextureSerializer&) = delete;

        /* --- MOVE SEMANTICS --- */
        YAMLTextureSerializer(YAMLTextureSerializer&&) noexcept = default;
        YAMLTextureSerializer& operator=(YAMLTextureSerializer&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~YAMLTextureSerializer() noexcept override = default;

    private:
        /* --- POLLING METHODS --- */
        void SerializeProperties(ryml::NodeRef root, const TextureProperties& properties) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetPropertiesNodeCount() const noexcept;
        [[nodiscard]] size GetPropertiesArenaSize() const noexcept;

    };

}