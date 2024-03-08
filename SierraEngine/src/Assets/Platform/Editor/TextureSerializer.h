//
// Created by Nikolay Kanchevski on 1.03.24.
//

#pragma once

#include "../../TextureAsset.h"

#include "../../ImageSupercompressor.h"

namespace SierraEngine
{

    struct TextureSerializerCreateInfo
    {
        Sierra::Version version = Sierra::Version({ 1, 0, 0 });
        uint16 maximumTextureDimensions = 0;
    };

    class TextureSerializer final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TextureSerializer(const TextureSerializerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<std::pair<SerializedTexture, void*>> Serialize(const std::filesystem::path &filePath, const TextureSerializeInfo &serializeInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Sierra::Version GetVersion() const { return version; }

        /* --- OPERATORS --- */
        TextureSerializer(const TextureSerializer&) = delete;
        TextureSerializer &operator=(const TextureSerializer&) = delete;

        /* --- DESTRUCTOR --- */
        ~TextureSerializer() = default;

    private:
        Sierra::Version version = Sierra::Version({ 1, 0, 0 });
        uint16 maximumTextureDimensions = 0;

    };

}
