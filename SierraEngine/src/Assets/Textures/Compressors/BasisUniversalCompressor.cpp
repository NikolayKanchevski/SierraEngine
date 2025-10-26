//
// Created by Nikolay Kanchevski on 16.07.24.
//

#include "BasisUniversalCompressor.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-builtins"
#include <encoder/basisu_enc.h>
#include <encoder/basisu_comp.h>
#pragma clang diagnostic pop

namespace SierraEngine
{

    namespace
    {
        uint32 compressorCount = 0;
    }

    /* --- CONSTRUCTORS --- */

    BasisUniversalCompressor::BasisUniversalCompressor()
    {
        if (compressorCount == 0) basisu::basisu_encoder_init();
        compressorCount++;
    }

    /* --- POLLING METHODS --- */

    std::optional<CompressedImage> BasisUniversalCompressor::Compress(const ImageCompressInfo& compressInfo) const noexcept
    {
        if (compressInfo.levels.empty())
        {
            APP_WARNING("Cannot Basis Universal compress image, as specified levels must not be empty!");
            return std::nullopt;
        }

        if (compressInfo.levels[0].layers.empty())
        {
            APP_WARNING("Cannot Basis Universal compress image, as specified level layers must not be empty!");
            return std::nullopt;
        }
        
        bool useUASTC = false;
        int qualityLevel = 0;
        int compressionLevel = 0;

        switch (compressInfo.compressionLevel)
        {
            case ImageCompressionLevel::Lowest:
            {
                compressionLevel = 1;
                break;
            }
            case ImageCompressionLevel::Low:
            {
                compressionLevel = 3;
                break;
            }
            case ImageCompressionLevel::Standard:
            {
                compressionLevel = 4;
                break;
            }
            case ImageCompressionLevel::High:
            {
                useUASTC = false;
                compressionLevel = 5;
                break;
            }
            case ImageCompressionLevel::Highest:
            {
                useUASTC = false;
                compressionLevel = 6;
                break;
            }
        }

        switch (compressInfo.qualityLevel)
        {
            case ImageCompressionQualityLevel::Lowest:
            {
                qualityLevel = basisu::BASISU_QUALITY_MIN;
                break;
            }
            case ImageCompressionQualityLevel::Low:
            {
                qualityLevel = (basisu::BASISU_QUALITY_MIN + basisu::BASISU_DEFAULT_QUALITY) / 2;
                break;
            }
            case ImageCompressionQualityLevel::Standard:
            {
                qualityLevel = basisu::BASISU_DEFAULT_QUALITY;
                break;
            }
            case ImageCompressionQualityLevel::High:
            {
                useUASTC = true;
                qualityLevel = (basisu::BASISU_DEFAULT_QUALITY + basisu::BASISU_QUALITY_MAX) / 2;
                break;
            }
            case ImageCompressionQualityLevel::Highest:
            {
                useUASTC = true;
                qualityLevel = basisu::BASISU_QUALITY_MAX;
                break;
            }
        }

        basisu::job_pool jobPool(std::thread::hardware_concurrency());

        constexpr bool ENABLE_DEBUGGING = false;
        basisu::enable_debug_printf(ENABLE_DEBUGGING);

        basisu::basis_compressor_params compressorParameters = { };
        compressorParameters.m_uastc = useUASTC;
        compressorParameters.m_status_output = ENABLE_DEBUGGING;
        compressorParameters.m_debug = ENABLE_DEBUGGING;
        compressorParameters.m_validate_etc1s = false;
        compressorParameters.m_debug_images = false;
        compressorParameters.m_compression_level = compressionLevel;
        compressorParameters.m_perceptual = true;
        compressorParameters.m_print_stats = ENABLE_DEBUGGING;
        compressorParameters.m_mip_gen = false;
        compressorParameters.m_etc1s_quality_level = qualityLevel;
        compressorParameters.m_pJob_pool = &jobPool;

        const LoadedImage& rootImage = compressInfo.levels[0].layers[0];
        compressorParameters.m_source_images.resize(compressInfo.levels[0].layers.size());
        for (size layer = 0; layer < compressInfo.levels[0].layers.size(); layer++)
        {
            compressorParameters.m_source_images[layer].init(rootImage.memory.data(), rootImage.width, rootImage.height, Sierra::ImageFormatToChannelCount(rootImage.format));
        }

        compressorParameters.m_source_mipmap_images.resize(compressInfo.levels.size() - 1);
        for (size level = 1; level < compressInfo.levels.size(); level++)
        {
            for (size layer = 0; layer < compressInfo.levels.size(); layer++)
            {
                const LoadedImage& layerImage = compressInfo.levels[level].layers[layer];
                compressorParameters.m_source_mipmap_images[level][layer].init(layerImage.memory.data(), glm::max(1U, rootImage.width >> level), glm::max(1U, rootImage.height >> level), Sierra::ImageFormatToChannelCount(rootImage.format));
            }
        }

        basisu::basis_compressor compressor = { };
        if (!compressor.init(compressorParameters))
        {
            APP_WARNING("Cannot Basis Universal compress image, as the Basis Universal compressor failed to initialize!");
            return std::nullopt;
        }

        const basisu::basis_compressor::error_code result = compressor.process();
        if (result != basisu::basis_compressor::cECSuccess)
        {
            APP_WARNING("Could not Basis Universal compress image! Error code: {0}.", static_cast<uint32>(result));
            return std::nullopt;
        }

        CompressedImage compressedImage
        {
            .memory = { compressor.get_output_basis_file().data(), compressor.get_output_basis_file().data() + compressor.get_output_basis_file().size() }
        };

        return compressedImage;
    }

    /* --- DESTRUCTORS --- */

    BasisUniversalCompressor::~BasisUniversalCompressor() noexcept
    {
        compressorCount--;
        if (compressorCount == 0) basisu::basisu_encoder_deinit();
    }

}