//
// Created by Nikolay Kanchevski on 3.03.24.
//

#include "KTXTranscoder.h"

#include <ktx.h>

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    KTXTranscoder::KTXTranscoder(const ImageTranscoderCreateInfo &createInfo)
        : ImageTranscoder(createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    std::optional<TranscodedImage> KTXTranscoder::Transcode(const ImageTranscodeInfo &transcodeInfo) const
    {
        // Load KTX texture
        ktxTexture2* ktxTexture2 = nullptr;
        ktxResult result = ktxTexture2_CreateFromMemory(reinterpret_cast<const ktx_uint8_t*>(transcodeInfo.compressedMemory.data()), transcodeInfo.compressedMemory.size(), KTX_TEXTURE_CREATE_NO_STORAGE, &ktxTexture2);
        if (result != KTX_SUCCESS)
        {
            APP_WARNING("Could not KTX transcode texture, as an error occurred during texture loading! Error code: {0}.",  result);
            return std::nullopt;
        }

        // If data is nullptr, then it must not have been compressed, so we need to re-load it, this time allocating its memory
        if (ktxTexture_GetData(ktxTexture(ktxTexture2)) == nullptr)
        {
            ktxTexture_Destroy(ktxTexture(ktxTexture2));
            result = ktxTexture2_CreateFromMemory(reinterpret_cast<const ktx_uint8_t*>(transcodeInfo.compressedMemory.data()), transcodeInfo.compressedMemory.size(), KTX_TEXTURE_CREATE_ALLOC_STORAGE, &ktxTexture2);
            if (result != KTX_SUCCESS)
            {
                APP_WARNING("Could not KTX transcode texture, as an error occurred during texture loading! Error code: {0}.",  result);
                return std::nullopt;
            }
        }

        // Check if compressed and determine image format
        Sierra::ImageFormat bestImageFormat = Sierra::ImageFormat::Undefined;
        if (!ktxTexture2_NeedsTranscoding(ktxTexture2))
        {
            switch (ktxTexture2_GetNumComponents(ktxTexture2))
            {
                case 1:         { bestImageFormat = Sierra::ImageFormat::R8_UNorm; break; }
                case 2:         { bestImageFormat = Sierra::ImageFormat::R8G8_UNorm; break; }
                case 3:         { bestImageFormat = Sierra::ImageFormat::R8G8B8_UNorm; break; }
                case 4:         { bestImageFormat = Sierra::ImageFormat::R8G8B8A8_UNorm; break; }
                default:        break;
            }
        }
        else
        {
            // Convert native format to KTX
            ktx_transcode_fmt_e transcodedFormat = KTX_TTF_NOSELECTION;
            switch (ktxTexture2_GetNumComponents(ktxTexture2))
            {
                #if SR_PLATFORM_DESKTOP
                    case 1:         { bestImageFormat = Sierra::ImageFormat::BC4_R_UNorm; transcodedFormat = KTX_TTF_BC4_R; break; }
                    case 2:         { bestImageFormat = Sierra::ImageFormat::BC5_RG_UNorm; transcodedFormat = KTX_TTF_BC5_RG; break; }
                    case 3:         { bestImageFormat = Sierra::ImageFormat::BC1_RGB_UNorm; transcodedFormat = KTX_TTF_BC1_RGB; break; }
                    case 4:         { bestImageFormat = Sierra::ImageFormat::BC3_RGBA_UNorm; transcodedFormat = KTX_TTF_BC3_RGBA; break; }
                    default:        break;
                #elif SR_PLATFORM_MOBILE
                    default:        { bestImageFormat = Sierra::ImageFormat::ASTC_4x4_UNorm; transcodedFormat = KTX_TTF_ASTC_4x4_RGBA; break; }
                #endif
            }

            // Transcode Basis Universal into requested format
            result = ktxTexture2_TranscodeBasis(ktxTexture2, transcodedFormat, 0);
            if (result != KTX_SUCCESS)
            {
                APP_WARNING("Could not KTX transcode texture, as an error occurred during texture transcoding! Error code: {0}.", result);
                ktxTexture_Destroy(ktxTexture(ktxTexture2));
                return std::nullopt;
            }
        }

        // Extract memory of every level
        std::vector<std::vector<uint8>> levelMemories(ktxTexture2->numLevels);
        {
            std::vector<uint8> transcodedMemory(ktxTexture2->dataSize);
            std::memcpy(transcodedMemory.data(), ktxTexture_GetData(ktxTexture(ktxTexture2)), ktxTexture2->dataSize);

            const uint64 levelMemorySize = ktxTexture2->dataSize / ktxTexture2->numLevels;
            for (uint32 i = 0; i < ktxTexture2->numLevels; i++)
            {
                levelMemories[i].resize(levelMemorySize);
                std::memcpy(levelMemories[i].data() + (i * levelMemorySize), transcodedMemory.data(), levelMemorySize);
            }
        }

        // Set up returned image
        const uint8 blockSize = Sierra::ImageFormatToBlockSize(bestImageFormat);
        TranscodedImage transcodedImage
        {
            .width = static_cast<uint32>(glm::ceil(static_cast<float32>(ktxTexture2->baseWidth) / static_cast<float32>(blockSize))) * blockSize,
            .height = static_cast<uint32>(glm::ceil(static_cast<float32>(ktxTexture2->baseHeight) / static_cast<float32>(blockSize))) * blockSize,
            .format = bestImageFormat,
            .levelMemories = std::move(levelMemories),
            .layerCount = ktxTexture2->numLayers
        };

        // Deallocate KTX texture
        ktxTexture_Destroy(ktxTexture(ktxTexture2));

        return transcodedImage;
    }

}
