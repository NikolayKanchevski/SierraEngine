//
// Created by Nikolay Kanchevski on 1.03.24.
//

#pragma once

#include "../../TextureAsset.h"

namespace SierraEngine
{

    struct TextureSerializerCreateInfo
    {
        uint16 maxTextureDimensions = 0;
    };

    class TextureSerializer final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TextureSerializer(const TextureSerializerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<std::pair<SerializedTexture, SerializedTextureBlob>> Serialize(const Sierra::FileManager &fileManager, const std::initializer_list<std::initializer_list<std::filesystem::path>> &levelFilePaths, const TextureSerializeInfo &serializeInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Sierra::Version GetVersion() const { return VERSION; }

        /* --- OPERATORS --- */
        TextureSerializer(const TextureSerializer&) = delete;
        TextureSerializer& operator=(const TextureSerializer&) = delete;

        /* --- DESTRUCTOR --- */
        ~TextureSerializer() = default;

    private:
        uint16 maxTextureDimensions = 0;
        constexpr static Sierra::Version VERSION = Sierra::Version({ 1, 0, 0 });

    };

}
