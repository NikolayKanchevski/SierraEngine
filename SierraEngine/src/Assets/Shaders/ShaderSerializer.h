//
// Created by Nikolay Kanchevski on 18.11.26.
//

#pragma once

#include "../AssetSerializer.h"
#include "../AssetMetadata.h"

#include "ShaderCompiler.h"

namespace SierraEngine
{

    struct ShaderSerializeInfo
    {
        AssetMetadata metadata = { };

        std::span<const ShaderEntryPoint> entryPoints = { };
        std::span<const uint8> buffer = { };
    };

    using SerializedShader = SerializedAsset;

    class SIERRA_ENGINE_API ShaderSerializer : public virtual AssetSerializer
    {
    public:
        /* --- CONSTANTS METHODS --- */
        constexpr static AssetTypeSignature TYPE_SIGNATURE = { 'S', 'H' };

        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<SerializedShader> Serialize(const ShaderSerializeInfo& serializeInfo, ShaderID& outID) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetTypeSignature GetTypeSignature() const noexcept override { return TYPE_SIGNATURE; }

        /* --- COPY SEMANTICS --- */
        ShaderSerializer(const ShaderSerializer&) = delete;
        ShaderSerializer& operator=(const ShaderSerializer&) = delete;

        /* --- DESTRUCTOR --- */
        ~ShaderSerializer() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        ShaderSerializer() noexcept = default;

        /* --- POLLING METHODS --- */
        // Writes the code of every entry point, in order, as [ SPIR-V | macOS Metal source | iOS Metal source ] (sizes are to be stored by the format itself)
        [[nodiscard]] Sierra::StreamResult SerializeBlob(Sierra::Stream& blob, const ShaderSerializeInfo& serializeInfo) const;

        /* --- MOVE SEMANTICS --- */
        ShaderSerializer(ShaderSerializer&&) noexcept = default;
        ShaderSerializer& operator=(ShaderSerializer&&) noexcept = default;

    };

}
