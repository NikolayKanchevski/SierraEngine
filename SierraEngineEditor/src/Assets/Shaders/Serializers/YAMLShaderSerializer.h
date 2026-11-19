//
// Created by Nikolay Kanchevski on 18.11.26.
//

#pragma once

#include "../../Serializers/YAMLSerializer.h"

namespace SierraEngine
{

    class YAMLShaderSerializer final : public ShaderSerializer, public YAMLSerializer
    {
    public:
        /* --- CONSTRUCTORS --- */
        YAMLShaderSerializer() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<SerializedShader> Serialize(const ShaderSerializeInfo& serializeInfo, ShaderID& outID) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetSignature GetSignature() const noexcept override { return { 'Y', 'A', 'S', 'H' }; }
        [[nodiscard]] AssetVersion GetVersion() const noexcept override { return AssetVersion({ 1, 0, 0 }); }

        /* --- COPY SEMANTICS --- */
        YAMLShaderSerializer(const YAMLShaderSerializer&) = delete;
        YAMLShaderSerializer& operator=(const YAMLShaderSerializer&) = delete;

        /* --- MOVE SEMANTICS --- */
        YAMLShaderSerializer(YAMLShaderSerializer&&) noexcept = default;
        YAMLShaderSerializer& operator=(YAMLShaderSerializer&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~YAMLShaderSerializer() noexcept override = default;

    };

}
