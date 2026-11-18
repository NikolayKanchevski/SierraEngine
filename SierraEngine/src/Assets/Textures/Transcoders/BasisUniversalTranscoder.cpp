//
// Created by Nikolay Kanchevski on 18.07.24.
//

#include "BasisUniversalTranscoder.h"

namespace SierraEngine
{

    namespace
    {
        bool basisTranscoderInitialized = false;
    }

    /* --- CONSTRUCTORS --- */

    BasisUniversalTranscoder::BasisUniversalTranscoder()
    {
        if (!basisTranscoderInitialized)
        {
            basist::basisu_transcoder_init();
            basisTranscoderInitialized = true;
        }
    }

    /* --- POLLING METHODS --- */

    std::optional<TranscodedImage> BasisUniversalTranscoder::Transcode(const ImageTranscodeInfo& transcodeInfo) const
    {
        std::vector<uint8> compressedBuffer = { };
        if (transcodeInfo.stream.ReadToEnd(compressedBuffer) != Sierra::StreamResult::Success)
        {
            APP_INFO("Could not read Basis Universal image from stream");
            return std::nullopt;
        }

        transcoder.start_transcoding(compressedBuffer.data(), static_cast<uint32>(compressedBuffer.size()));
        if (!transcoder.get_ready_to_transcode())
        {
            APP_WARNING("Could not start transcoding Basis Universal image!");
            return { };
        }

        basist::transcoder_texture_format format;
        switch (transcodeInfo.format)
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
                APP_WARNING("Cannot transcode Basis Universal image to an unsupported format! Transcode format must be one of the following: [ImageFormat::R8G8B8A8_UNorm, ImageFormat::BC1_RGB_UNorm, ImageFormat::BC3_RGBA_UNorm, ImageFormat::BC4_R_UNorm, ImageFormat::BC5_RG_UNorm, ImageFormat::BC7_RGBA_UNorm, ImageFormat::ASTC_4x4_UNorm]!");
                return { };
            }
        }

        basist::basisu_image_info imageInfo = { };
        transcoder.get_image_info(compressedBuffer.data(), static_cast<uint32>(compressedBuffer.size()), imageInfo, 0);

        const uint32 blockSize = Sierra::ImageFormatToBlockSize(transcodeInfo.format);
        const uint8 blockMemorySize = Sierra::ImageFormatToBlockMemorySize(transcodeInfo.format);

        const uint32 horizontalBlockCount = (imageInfo.m_orig_width + blockSize - 1) / blockSize;
        const uint32 verticalBlockCount = (imageInfo.m_orig_height + blockSize - 1) / blockSize;

        const uint32 layerCount = transcoder.get_total_images(compressedBuffer.data(), static_cast<uint32>(compressedBuffer.size()));
        const uint32 levelCount = imageInfo.m_total_levels;

        /* === Reference: https://gaim.umbc.edu/2010/05/27/mip-size/ === */
        const size approximatedMemorySize = (4 * (horizontalBlockCount * verticalBlockCount * blockMemorySize) - ((horizontalBlockCount >> (levelCount - 1)) * (verticalBlockCount >> (levelCount - 1)) * blockMemorySize)) / 3;

        Sierra::WriteStream trasncodedBuffer(approximatedMemorySize);
        for (uint32 layer = 0; layer < layerCount; layer++)
        {
            for (uint32 level = 0; level < levelCount; level++)
            {
                const size memorySize = (horizontalBlockCount >> level) * (verticalBlockCount >> level) * blockMemorySize;
                std::vector<uint8> memory(memorySize);

                if (!transcoder.transcode_image_level(compressedBuffer.data(), static_cast<uint32>(compressedBuffer.size()), layer, level, memory.data(), imageInfo.m_orig_width * imageInfo.m_orig_height, format, 0, 0))
                {
                    APP_WARNING("Could not transcode level [{0}] layer [{1}] of Basis Universal image", level, layer);
                    return std::nullopt;
                }

                if (const Sierra::StreamResult result = trasncodedBuffer.Write(memory); result != Sierra::StreamResult::Success)
                {
                    APP_WARNING("Could not write trasncoded level [{0}] layer [{1}] of Basis Universal image to buffer", level, layer);
                    return std::nullopt;
                }
            }
        }

        TranscodedImage transcodedImage
        {
            .width = horizontalBlockCount * blockSize,
            .height = verticalBlockCount * blockSize,
            .levelCount = levelCount,
            .layerCount = layerCount,
            .buffer = trasncodedBuffer.Release()
        };

        return transcodedImage;
    }

}
