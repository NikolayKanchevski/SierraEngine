//
// Created by Nikolay Kanchevski on 18.07.24.
//

#include "BasisUniversalTranscoder.h"

#include <transcoder/basisu_transcoder.h>

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    BasisUniversalTranscoder::BasisUniversalTranscoder()
    {
        basist::basisu_transcoder_init();
    }

    /* --- POLLING METHODS --- */

    std::optional<std::vector<uint8>> BasisUniversalTranscoder::Transcode(const ImageTranscodeInfo& transcodeInfo) const
    {
        basist::basisu_transcoder transcoder = { };
        if (!transcoder.start_transcoding(transcodeInfo.memory.data(), transcodeInfo.memory.size_bytes()))
        {
            APP_WARNING("Could not start Basis Universal transcoding image!");
            return std::nullopt;
        }

        const float32 pixelMemorySize = Sierra::ImageFormatToPixelMemorySize(transcodeInfo.transcodeFormat);
        basist::transcoder_texture_format format;

        switch (transcodeInfo.transcodeFormat)
        {
            case Sierra::ImageFormat::R8G8B8A8_UNorm:       { format = basist::transcoder_texture_format::cTFRGBA32;        break; }
            case Sierra::ImageFormat::BC1_RGB_UNorm:        { format = basist::transcoder_texture_format::cTFBC1_RGB;       break; }
            case Sierra::ImageFormat::BC3_RGBA_UNorm:       { format = basist::transcoder_texture_format::cTFBC3_RGBA;      break; }
            case Sierra::ImageFormat::BC4_R_UNorm:          { format = basist::transcoder_texture_format::cTFBC4_R;         break; }
            case Sierra::ImageFormat::BC5_RG_UNorm:         { format = basist::transcoder_texture_format::cTFBC5_RG;        break; }
            case Sierra::ImageFormat::BC7_RGBA_UNorm:       { format = basist::transcoder_texture_format::cTFBC7_RGBA;      break; }
            case Sierra::ImageFormat::ASTC_4x4_UNorm:       { format = basist::transcoder_texture_format::cTFASTC_4x4_RGBA; break; }
            default:
            {
                APP_WARNING("Cannot Basis Universal transcode image to an unsupported format! Transcode format must be one of the following: [ImageFormat::R8G8B8A8_UNorm, ImageFormat::BC1_RGB_UNorm, ImageFormat::BC3_RGBA_UNorm, ImageFormat::BC4_R_UNorm, ImageFormat::BC5_RG_UNorm, ImageFormat::BC7_RGBA_UNorm, ImageFormat::ASTC_4x4_UNorm]!");
                return std::nullopt;
            }
        }

        size memorySize = 0;
        for (uint32 i = 0; i < transcodeInfo.levelCount; i++)
        {
            memorySize += static_cast<uint64>(static_cast<float32>((transcodeInfo.width >> i) * (transcodeInfo.height >> i)) * pixelMemorySize) * transcodeInfo.layerCount;
        }

        std::vector<uint8> memory(memorySize);
        uint64 currentOffset = 0;

        for (uint32 level = 0; level < transcodeInfo.levelCount; level++)
        {
            const size layerSize = static_cast<uint64>(static_cast<float32>((transcodeInfo.width >> level) * (transcodeInfo.height >> level)) * pixelMemorySize);
            for (uint32 layer = 0; layer < transcodeInfo.layerCount; layer++)
            {
                transcoder.transcode_image_level(transcodeInfo.memory.data(), transcodeInfo.memory.size_bytes(), layer, level, memory.data() + currentOffset, memorySize - currentOffset, format);
                currentOffset += layerSize;
            }
        }

        return memory;
    }


}