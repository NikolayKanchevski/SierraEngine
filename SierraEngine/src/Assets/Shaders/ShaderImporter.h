//
// Created by Nikolay Kanchevski on 18.11.26.
//

#pragma once

#include "../AssetImporter.h"

#include "ShaderSerializer.h"

namespace SierraEngine
{

    struct ShaderImportInfo
    {
        const SerializedShader& serializedShader;
        Sierra::RenderingBackendType backendType = Sierra::RenderingBackendType::Undefined;
    };

    struct ShaderEntryPointProperties
    {
        std::string name = { };
        Sierra::ShaderType type = Sierra::ShaderType::Undefined;

        size codeOffset = 0;
        size codeMemorySize = 0;
    };

    struct ShaderProperties
    {
        std::vector<ShaderEntryPointProperties> entryPoints = { };
        Sierra::RenderingBackendType backendType = Sierra::RenderingBackendType::Undefined;
    };

    struct ImportedShader
    {
        ShaderID ID = { };
        AssetHeader header = { };
        AssetMetadata metadata = { };

        ShaderProperties properties = { };
        std::vector<uint8> buffer = { };
    };

    class SIERRA_ENGINE_API ShaderImporter : public virtual AssetImporter
    {
    public:
        /* --- CONSTANTS METHODS --- */
        constexpr static AssetTypeSignature TYPE_SIGNATURE = { 'S', 'H' };

        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<ImportedShader> Import(const ShaderImportInfo& importInfo) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetTypeSignature GetTypeSignature() const noexcept override { return TYPE_SIGNATURE; }

        /* --- COPY SEMANTICS --- */
        ShaderImporter(const ShaderImporter&) = delete;
        ShaderImporter& operator=(const ShaderImporter&) = delete;

        /* --- DESTRUCTOR --- */
        ~ShaderImporter() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        ShaderImporter() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] Sierra::StreamResult ImportBlobAndProperties(Sierra::Stream& blob, std::span<const ShaderEntryPoint> entryPoints, Sierra::RenderingBackendType backendType, ShaderProperties& outProperties, Sierra::WriteStream& outBuffer) const;

        /* --- MOVE SEMANTICS --- */
        ShaderImporter(ShaderImporter&&) noexcept = default;
        ShaderImporter& operator=(ShaderImporter&&) noexcept = default;

    };

}
