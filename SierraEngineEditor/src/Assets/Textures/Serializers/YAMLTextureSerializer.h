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
        YAMLTextureSerializer() = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<std::vector<uint8>> Serialize(const TextureSerializeInfo &serializeInfo) const override;

        /* --- DESTRUCTOR --- */
        ~YAMLTextureSerializer() override = default;

    };

}
