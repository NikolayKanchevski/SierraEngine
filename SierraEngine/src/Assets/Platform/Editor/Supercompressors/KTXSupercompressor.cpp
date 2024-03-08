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

#include "ktx.h"
#include "vkformat_enum.h"


namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    KTXSupercompressor::KTXSupercompressor(const ImageSupercompressorCreateInfo &createInfo)
        : ImageSupercompressor(createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    bool KTXSupercompressor::Supercompress(const ImageSupercompressorSupercompressInfo &compressInfo, void*& compressedMemory, uint64 &compressedMemorySize) const
    {
        if (compressInfo.filePaths.empty() || compressInfo.filePaths[0].empty())
        {
            APP_WARNING("Cannot KTX compress texture with no file paths specified!");
            return false;
        }

        const std::filesystem::path &baseFilePath = compressInfo.filePaths[0][0];

        int requestedBaseWidth = 0;
        int requestedBaseHeight = 0;
        int requestedBaseChannelCount = 0;
        int requestedBaseChannelMemorySize;

        // Retrieve image data
        {
            const std::vector<uint8> baseImageFileMemory = Sierra::File::ReadFile(baseFilePath);

            stbi_info_from_memory(reinterpret_cast<const stbi_uc*>(baseImageFileMemory.data()), static_cast<int>(baseImageFileMemory.size()), &requestedBaseWidth, &requestedBaseHeight, &requestedBaseChannelCount);
            requestedBaseChannelMemorySize = stbi_is_16_bit_from_memory(reinterpret_cast<const stbi_uc*>(baseImageFileMemory.data()), static_cast<int>(baseImageFileMemory.size())) + 1;
        }

        const float32 downscaleScalar = glm::max(1.0f, static_cast<float32>(glm::max(requestedBaseWidth, requestedBaseHeight)) / GetMaximumImageDimensions());
        const uint32 baseWidth = static_cast<uint32>(requestedBaseWidth / downscaleScalar);
        const uint32 baseHeight = static_cast<uint32>(requestedBaseHeight / downscaleScalar);
        const uint32 mipLevelCount = (glm::floor(glm::log2(glm::max(baseWidth, baseHeight))) * static_cast<uint32>(compressInfo.filePaths[0].size() > 1)) + 1;

        // Convert channel count to KTX format
        ktx_uint32_t ktxTextureFormat = VK_FORMAT_UNDEFINED;
        switch (requestedBaseChannelCount)
        {
            case 1:     { ktxTextureFormat = (requestedBaseChannelMemorySize == 1) ? VK_FORMAT_R8_UNORM : VK_FORMAT_R16_UNORM; break; }
            case 2:     { ktxTextureFormat = (requestedBaseChannelMemorySize == 1) ? VK_FORMAT_R8G8_UNORM : VK_FORMAT_R16G16_UNORM; break; }
            case 3:     { ktxTextureFormat = (requestedBaseChannelMemorySize == 1) ? VK_FORMAT_R8G8B8_UNORM : VK_FORMAT_R16G16B16_UNORM; break; }
            case 4:     { ktxTextureFormat = (requestedBaseChannelMemorySize == 1) ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R16G16B16A16_UNORM; break; }
            default:    break;
        }

        // Set up KTX texture create info
        ktxTextureCreateInfo ktxTextureCreateInfo = { };
        ktxTextureCreateInfo.vkFormat = ktxTextureFormat;
        ktxTextureCreateInfo.baseWidth = baseWidth;
        ktxTextureCreateInfo.baseHeight = baseHeight;
        ktxTextureCreateInfo.baseDepth = 1;
        ktxTextureCreateInfo.numDimensions = 2;
        ktxTextureCreateInfo.numLevels = mipLevelCount;
        ktxTextureCreateInfo.numLayers = static_cast<uint32>(compressInfo.filePaths.size());
        ktxTextureCreateInfo.numFaces = 1;
        ktxTextureCreateInfo.isArray = KTX_FALSE;
        ktxTextureCreateInfo.generateMipmaps = KTX_FALSE;

        // Create KTX texture
        ktxTexture2* ktxTexture2 = nullptr;
        ktxResult result = ktxTexture2_Create(&ktxTextureCreateInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &ktxTexture2);
        if (result != KTX_SUCCESS)
        {
            APP_WARNING("Could not serialize texture [{0}], as an error occurred during KTX texture allocation! Error code: {1}.", baseFilePath.string(), result);
            return false;
        }

        // Store every layer's mip level pixel data
        for (uint32 i = 0; i < 1; i++)
        {
            for (uint32 j = 0; j < mipLevelCount; j++)
            {
                // Read compressed image file for current level
                const std::vector<uint8> levelFileMemory = Sierra::File::ReadFile(compressInfo.filePaths[i][j]);

                // Extract image info
                int requestedWidth, requestedHeight, channelCount;
                stbi_uc* rawImageMemory = (requestedBaseChannelMemorySize == 2)
                    ? reinterpret_cast<stbi_uc*>(stbi_load_16_from_memory(reinterpret_cast<const stbi_uc*>(levelFileMemory.data()), static_cast<int>(levelFileMemory.size()), &requestedWidth, &requestedHeight, &channelCount, requestedBaseChannelCount))
                    : stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(levelFileMemory.data()), static_cast<int>(levelFileMemory.size()), &requestedWidth, &requestedHeight, &channelCount, requestedBaseChannelCount)
                ;

                if (requestedWidth != requestedBaseWidth >> j || requestedBaseHeight != requestedHeight >> j)
                {
                    APP_WARNING("Cannot KTX compress texture [{0}], as the dimensions of image file at layer [{1}] level [{2}] are not equal to the half of those of the previous level!", baseFilePath.string(), i, j);
                    return false;
                }

                // Resize if needed
                uint32 width = static_cast<uint32>(requestedWidth / downscaleScalar);
                uint32 height = static_cast<uint32>(requestedHeight / downscaleScalar);
                if (glm::max(requestedWidth, requestedHeight) > glm::max(width, height))
                {
                    // Get pixel layout
                    stbir_pixel_layout pixelLayout = STBIR_RGBA;
                    switch (channelCount)
                    {
                        case 1:         { pixelLayout = STBIR_1CHANNEL; break; }
                        case 2:         { pixelLayout = STBIR_2CHANNEL; break; }
                        case 3:         { pixelLayout = STBIR_RGB; break; }
                        default:        break;
                    }

                    // Update dimensions
                    width = static_cast<uint32>(static_cast<float32>(width) / downscaleScalar);
                    height = static_cast<uint32>(static_cast<float32>(height) / downscaleScalar);

                    // Downscale image data
                    stbi_uc* downscaledRawImageMemory = new stbi_uc[static_cast<uint64>(width) * height * channelCount * requestedBaseChannelMemorySize];
                    stbir_resize(rawImageMemory, requestedWidth, requestedHeight, 0, downscaledRawImageMemory, width, height, 0, pixelLayout, (requestedBaseChannelMemorySize == 2) ? STBIR_TYPE_UINT16 : STBIR_TYPE_UINT8, STBIR_EDGE_ZERO, STBIR_FILTER_DEFAULT);

                    // Free and override image data
                    stbi_image_free(rawImageMemory);
                    rawImageMemory = downscaledRawImageMemory;
                }

                // Copy raw image data to KTX texture
                result = ktxTexture_SetImageFromMemory(ktxTexture(ktxTexture2), j, i, KTX_FACESLICE_WHOLE_LEVEL, reinterpret_cast<uint8*>(rawImageMemory), static_cast<uint64>(width) * height * channelCount * requestedBaseChannelMemorySize);
                stbi_image_free(rawImageMemory);

                if (result != KTX_SUCCESS)
                {
                    APP_WARNING("Could not serialize texture [{0}], as an error occurred while writing raw pixel memory to layer [{1}] level [{2}] of KTX texture! Error code: {3}.", baseFilePath.string(), i, j, result);
                    ktxTexture_Destroy(ktxTexture(ktxTexture2));
                    return false;
                }
            }
        }

        // See if texture needs to be compressed
        constexpr static uint16 MINIMUM_DIMENSIONS_FOR_COMPRESSION = 64;
        if (compressInfo.compressionLevel != ImageSupercompressionLevel::None && (baseWidth > MINIMUM_DIMENSIONS_FOR_COMPRESSION && baseHeight > MINIMUM_DIMENSIONS_FOR_COMPRESSION))
        {
            // Set up Basis Universal settings
            ktxBasisParams basisParams = { };
            basisParams.structSize = sizeof(ktxBasisParams);
            basisParams.uastc = compressInfo.qualityLevel == ImageSupercompressionQualityLevel::High || compressInfo.qualityLevel == ImageSupercompressionQualityLevel::VeryHigh;
            basisParams.verbose = KTX_FALSE;
            basisParams.threadCount = std::thread::hardware_concurrency();
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
            basisParams.normalMap = compressInfo.isNormalMap;

            // Compress KTX texture into Basis Universal
            result = ktxTexture2_CompressBasisEx(ktxTexture2, &basisParams);
            if (result != KTX_SUCCESS)
            {
                APP_WARNING("Could not serialize texture [{0}], as an error occurred while compressing data into Basis Universal! Error code: {1}.", baseFilePath.string(), result);
                ktxTexture_Destroy(ktxTexture(ktxTexture2));
                return false;
            }
        }

        // Retrieve KTX memory
        ktx_size_t ktxMemorySize = 0;
        ktx_uint8_t* ktxMemory = nullptr;
        result = ktxTexture_WriteToMemory(ktxTexture(ktxTexture2), &ktxMemory, &ktxMemorySize);
        if (result != KTX_SUCCESS)
        {
            APP_WARNING("Could not serialize texture [{0}], as an error occurred while extracting KTX data from texture! Error code: {1}.", baseFilePath.string(), result);
            ktxTexture_Destroy(ktxTexture(ktxTexture2));
            return false;
        }

        // Deallocate KTX texture (ktxMemory must be freed by user)
        ktxTexture_Destroy(ktxTexture(ktxTexture2));

        compressedMemory = ktxMemory;
        compressedMemorySize = ktxMemorySize;

        return true;
    }

}
