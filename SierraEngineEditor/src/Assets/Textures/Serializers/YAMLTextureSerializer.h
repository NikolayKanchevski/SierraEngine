//
// Created by Nikolay Kanchevski on 16.07.24.
//

#pragma once

#include "../TextureSerializer.h"
#include "../../Serializers/YAMLSerializer.h"

namespace SierraEngine
{

    class YAMLTextureSerializer final : public TextureSerializer, public YAMLSerializer
    {
    public:
        /* --- CONSTRUCTORS --- */
        YAMLTextureSerializer() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<std::vector<uint8>> Serialize(const TextureSerializeInfo& serializeInfo) const override;

        /* --- COPY SEMANTICS --- */
        YAMLTextureSerializer(const YAMLTextureSerializer&) = delete;
        YAMLTextureSerializer& operator=(const YAMLTextureSerializer&) = delete;

        /* --- MOVE SEMANTICS --- */
        YAMLTextureSerializer(YAMLTextureSerializer&&) noexcept = default;
        YAMLTextureSerializer& operator=(YAMLTextureSerializer&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~YAMLTextureSerializer() noexcept override = default;
    };

}
