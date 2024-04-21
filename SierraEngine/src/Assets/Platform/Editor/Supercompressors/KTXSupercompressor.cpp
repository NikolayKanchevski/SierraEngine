//
// Created by Nikolay Kanchevski on 3.03.24.
//

#include "KTXSupercompressor.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_NO_PIC
#define STBI_NO_PNM
#include "stb_image.h"

#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include "stb_image_resize2.h"

#include <ktx.h>
#include <vkformat_enum.h>

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    KTXSupercompressor::KTXSupercompressor(const ImageSupercompressorCreateInfo &createInfo)
        : ImageSupercompressor(createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    std::optional<std::vector<uint8>> KTXSupercompressor::Supercompress(const Sierra::FileManager &fileManager, const ImageSupercompressInfo &compressInfo) const
    {
        if (compressInfo.levelFilePaths.size() == 0 || compressInfo.levelFilePaths.begin()->size() == 0)
        {
            APP_WARNING("Cannot KTX compress texture with no file paths specified!");
            return std::nullopt;
        }

        int requestedBaseWidth = 0;
        int requestedBaseHeight = 0;
        int requestedBaseChannelCount = 0;

        // Retrieve image data
        {
            std::optional<Sierra::File> baseImageFile = fileManager.OpenFile(*compressInfo.levelFilePaths.begin()->begin(), Sierra::FileAccess::ReadOnly);
            if (!baseImageFile.has_value())
            {
                APP_WARNING("Cannot KTX compress texture, as image [{0}] of level [0] layer [0] could not be read!", compressInfo.levelFilePaths.begin()->begin()->string());
                return std::nullopt;
            }

            const std::vector<uint8> baseImageFileMemory = baseImageFile.value().Read();
            if (stbi_info_from_memory(reinterpret_cast<const stbi_uc*>(baseImageFileMemory.data()), static_cast<int>(baseImageFileMemory.size()), &requestedBaseWidth, &requestedBaseHeight, &requestedBaseChannelCount) != 1)
            {
                APP_WARNING("Cannot KTX compress texture [{0}], as image of level [0] layer [0] is not of a valid image format! Error: {1}.", compressInfo.levelFilePaths.begin()->begin()->string(), stbi_failure_reason());
                return std::nullopt;
            }
        }

        // Make sure to emulate loading 4 channels when image contains only 3, so we do not have to pad data to an even count when writing it to GPU
        requestedBaseChannelCount += static_cast<int>(requestedBaseChannelCount == 3);

        const float32 downscaleScalar = glm::max(1.0f, static_cast<float32>(glm::max(requestedBaseWidth, requestedBaseHeight)) / static_cast<float32>(GetMaxImageDimensions()));
        const uint32 baseWidth = static_cast<uint32>(static_cast<float32>(requestedBaseWidth) / downscaleScalar);
        const uint32 baseHeight = static_cast<uint32>(static_cast<float32>(requestedBaseHeight) / downscaleScalar);
        const uint32 levelCount = static_cast<uint32>(glm::floor(glm::log2(glm::max(baseWidth, baseHeight))) * static_cast<uint32>(compressInfo.levelFilePaths.size() > 1)) + 1;
        const uint32 layerCount = static_cast<uint32>(compressInfo.levelFilePaths.begin()->size());

        // Convert channel count to KTX format
        ktx_uint32_t ktxTextureFormat = VK_FORMAT_UNDEFINED;
        switch (requestedBaseChannelCount)
        {
            case 1:     { ktxTextureFormat = VK_FORMAT_R8_UNORM; break; }
            case 2:     { ktxTextureFormat = VK_FORMAT_R8G8_UNORM; break; }
            case 3:     { ktxTextureFormat = VK_FORMAT_R8G8B8_UNORM; break; }
            case 4:     { ktxTextureFormat = VK_FORMAT_R8G8B8A8_UNORM; break; }
            default:    break;
        }

        // Set up KTX texture create info
        ktxTextureCreateInfo ktxTextureCreateInfo
        {
            .vkFormat = ktxTextureFormat,
            .baseWidth = baseWidth,
            .baseHeight = baseHeight,
            .baseDepth = 1,
            .numDimensions = 2,
            .numLevels = levelCount,
            .numLayers = layerCount,
            .numFaces = 1,
            .isArray = layerCount > 1,
            .generateMipmaps = compressInfo.generateMipMaps
        };

        // Create KTX texture
        ktxTexture2* ktxTexture2 = nullptr;
        ktxResult result = ktxTexture2_Create(&ktxTextureCreateInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &ktxTexture2);
        if (result != KTX_SUCCESS)
        {
            APP_WARNING("Could create KTX texture [{0}], as an error occurred during texture allocation! Error code: {1}.", compressInfo.levelFilePaths.begin()->begin()->string(), result);
            return std::nullopt;
        }

        for (uint32 layer = 0; layer < layerCount; layer++)
        {
            for (uint32 level = 0; level < levelCount; level++)
            {
                int requestedWidth, requestedHeight, requestedChannelCount;
                std::unique_ptr<stbi_uc, decltype(stbi_image_free)*> levelMemory = { nullptr, stbi_image_free };
                {
                    // Read compressed image file for current level
                    std::optional<Sierra::File> levelFile = fileManager.OpenFile(*((compressInfo.levelFilePaths.begin() + level)->begin() + layer));
                    if (!levelFile.has_value())
                    {
                        APP_WARNING("Cannot KTX compress texture [{0}], as image of level [{1}] layer [{2}] could not be read!", compressInfo.levelFilePaths.begin()->begin()->string(), level, layer);
                        return std::nullopt;
                    }

                    // Extract image info
                    const std::vector<uint8> levelFileMemory = levelFile.value().Read();
                    levelMemory.reset(stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(levelFileMemory.data()), static_cast<int>(levelFileMemory.size()), &requestedWidth, &requestedHeight, &requestedChannelCount, requestedBaseChannelCount));
                }
                
                if (levelMemory == nullptr)
                {
                    APP_WARNING("Cannot KTX compress texture [{0}], as image of level [{1}] layer [{2}] is not of a valid image format! Error: {3}.", compressInfo.levelFilePaths.begin()->begin()->string(), level, layer, stbi_failure_reason());
                    return std::nullopt;
                }

                if (requestedWidth != (static_cast<uint32>(requestedBaseWidth) >> level) || requestedBaseHeight != (static_cast<uint32>(requestedHeight) >> level))
                {
                    APP_WARNING("Cannot KTX compress texture [{0}], as the dimensions of image file at level [{1}] layer [{2}] are not equal to the half of those of the previous level!", compressInfo.levelFilePaths.begin()->begin()->string(), layer, level);
                    return std::nullopt;
                }

                // Resize if needed
                const uint32 width = static_cast<uint32>(static_cast<float32>(requestedWidth) / downscaleScalar);
                const uint32 height = static_cast<uint32>(static_cast<float32>(requestedHeight) / downscaleScalar);
                const uint64 levelMemorySize = static_cast<uint64>(width) * height * requestedBaseChannelCount * 1;
                if (glm::max(requestedWidth, requestedHeight) > glm::max(width, height))
                {
                    // Get pixel layout
                    stbir_pixel_layout pixelLayout = STBIR_RGBA;
                    switch (requestedBaseChannelCount)
                    {
                        case 1:         { pixelLayout = STBIR_1CHANNEL; break; }
                        case 2:         { pixelLayout = STBIR_2CHANNEL; break; }
                        case 3:         { pixelLayout = STBIR_RGB; break; }
                        default:        break;
                    }

                    // Downscale image data
                    std::unique_ptr<stbi_uc, decltype(stbi_image_free)*> downscaledLevelMemory = { reinterpret_cast<stbi_uc*>(stbi__malloc(levelMemorySize)), stbi_image_free };
                    stbir_resize(levelMemory.get(), requestedWidth, requestedHeight, 0, downscaledLevelMemory.get(), static_cast<int>(width), static_cast<int>(height), 0, pixelLayout, STBIR_TYPE_UINT8, STBIR_EDGE_ZERO, STBIR_FILTER_DEFAULT);
                    levelMemory = std::move(downscaledLevelMemory);
                }

                // Copy raw image data to KTX texture
                result = ktxTexture_SetImageFromMemory(ktxTexture(ktxTexture2), level, layer, 0, reinterpret_cast<const uint8*>(levelMemory.get()), levelMemorySize);
                if (result != KTX_SUCCESS)
                {
                    APP_WARNING("Could not KTX compress [{0}], as an error occurred while writing raw pixel memory to level [{1}] layer [{2}] of KTX texture! Error code: {3}.", compressInfo.levelFilePaths.begin()->begin()->string(), level, layer, result);
                    ktxTexture_Destroy(ktxTexture(ktxTexture2));
                    return std::nullopt;
                }
            }
        }

        // See if texture needs to be compressed
        constexpr static uint16 MIN_DIMENSIONS_FOR_COMPRESSION = 64;
        if (compressInfo.compressionLevel != ImageSupercompressionLevel::None && (baseWidth > MIN_DIMENSIONS_FOR_COMPRESSION && baseHeight > MIN_DIMENSIONS_FOR_COMPRESSION))
        {
            // Set up Basis Universal settings
            ktxBasisParams basisParams
            {
                .structSize = sizeof(ktxBasisParams),
                .uastc = compressInfo.qualityLevel == ImageSupercompressionQualityLevel::High || compressInfo.qualityLevel == ImageSupercompressionQualityLevel::VeryHigh,
                .verbose = KTX_FALSE,
                .threadCount = std::thread::hardware_concurrency(),
                .normalMap = compressInfo.normalMap
            };
            switch (compressInfo.qualityLevel)
            {
                case ImageSupercompressionQualityLevel::Lowest:           { basisParams.qualityLevel = 1; break; }
                case ImageSupercompressionQualityLevel::VeryLow:          { basisParams.qualityLevel = 51; break; }
                case ImageSupercompressionQualityLevel::Low:              { basisParams.qualityLevel = 102; break; }
                case ImageSupercompressionQualityLevel::Standard:         { basisParams.qualityLevel = 153; break; }
                case ImageSupercompressionQualityLevel::High:             { basisParams.qualityLevel = 204; break; }
                case ImageSupercompressionQualityLevel::VeryHigh:         { basisParams.qualityLevel = 255; break; }
            }
            switch (compressInfo.compressionLevel)
            {
                case ImageSupercompressionLevel::None:             break;
                case ImageSupercompressionLevel::Lowest:           { basisParams.compressionLevel = 0; break; }
                case ImageSupercompressionLevel::VeryLow:          { basisParams.compressionLevel = 1; break; }
                case ImageSupercompressionLevel::Low:              { basisParams.compressionLevel = 2; break; }
                case ImageSupercompressionLevel::Standard:         { basisParams.compressionLevel = 3; break; }
                case ImageSupercompressionLevel::High:             { basisParams.compressionLevel = 4; break; }
                case ImageSupercompressionLevel::VeryHigh:         { basisParams.compressionLevel = 5; break; }
            }

            // Compress KTX texture into Basis Universal
            result = ktxTexture2_CompressBasisEx(ktxTexture2, &basisParams);
            if (result != KTX_SUCCESS)
            {
                APP_WARNING("Could not serialize texture [{0}], as an error occurred while compressing data into Basis Universal! Error code: {1}.", compressInfo.levelFilePaths.begin()->begin()->string(), result);
                ktxTexture_Destroy(ktxTexture(ktxTexture2));
                return std::nullopt;
            }
        }

        // Retrieve KTX memory
        ktx_size_t ktxMemorySize = 0;
        ktx_uint8_t* ktxMemory = nullptr;
        result = ktxTexture_WriteToMemory(ktxTexture(ktxTexture2), &ktxMemory, &ktxMemorySize);
        if (result != KTX_SUCCESS)
        {
            APP_WARNING("Could not serialize texture [{0}], as an error occurred while extracting KTX data from texture! Error code: {1}.", compressInfo.levelFilePaths.begin()->begin()->string(), result);
            ktxTexture_Destroy(ktxTexture(ktxTexture2));
            return std::nullopt;
        }

        // Deallocate KTX texture
        ktxTexture_Destroy(ktxTexture(ktxTexture2));

        std::vector<uint8> supercompressedMemory = { ktxMemory, ktxMemory + ktxMemorySize };
        std::free(ktxMemory);

        return supercompressedMemory;
    }

}
