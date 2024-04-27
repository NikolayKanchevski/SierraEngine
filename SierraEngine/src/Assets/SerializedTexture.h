//
// Created by Nikolay Kanchevski on 20.02.24.
//

#pragma once

#include "AssetID.hpp"

#include "SerializedAsset.h"
#include "Platform/Editor/Serializers/Processors/ImageSupercompressor.h"

namespace SierraEngine
{

    enum class TextureType : uint8
    {
        Undefined,
        Diffuse,
        Specular,
        Normal,
        Height,
        Skybox
    };

    struct SerializedTextureIndex
    {
        TextureType type = TextureType::Undefined;
        ImageSupercompressorType compressorType = ImageSupercompressorType::Undefined;
    };

    struct SerializedTexture
    {
        SerializedAssetHeader header = { };
        SerializedTextureIndex index = { };
    };

}
