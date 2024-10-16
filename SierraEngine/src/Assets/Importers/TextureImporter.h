//
// Created by Nikolay Kanchevski on 15.07.24.
//

#pragma once

#include "../Assets/Texture.h"

namespace SierraEngine
{

    struct ImportedTexture
    {
        std::array<char, 64> name = { };
        uint64 hash = 0;

        uint32 width = 0;
        uint32 height = 0;

        Sierra::ImageType imageType = Sierra::ImageType::Plane;
        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;
        Sierra::SamplerFilter filter = Sierra::SamplerFilter::Nearest;

        uint32 layerCount = 1;
        uint32 levelCount = 1;
        std::vector<std::vector<uint8>> levelMemory;
    };

    class SIERRA_ENGINE_API TextureImporter
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<ImportedTexture> Import() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        TextureImporter(const TextureImporter&) = delete;
        TextureImporter& operator=(const TextureImporter&) = delete;

        /* --- MOVE SEMANTICS --- */
        TextureImporter(TextureImporter&&) noexcept = default;
        TextureImporter& operator=(TextureImporter&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        virtual ~TextureImporter() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        TextureImporter() noexcept = default;

    };

}
