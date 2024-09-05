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

    /* --- CONSTRUCTORS --- */

    BasisUniversalCompressor::BasisUniversalCompressor()
    {
        basisu::basisu_encoder_init();
    }

    /* --- POLLING METHODS --- */

    std::optional<std::vector<uint8>> BasisUniversalCompressor::Compress(const ImageCompressInfo& compressInfo) const
    {
        if (compressInfo.imageLevels.empty() || compressInfo.imageLevels[0].layers.empty())
        {
            APP_WARNING("Cannot Basis Universal compress image with no level and layers passed!");
            return std::nullopt;
        }

        bool useUASTC;
        int compressionLevel, qualityLevel;

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
            case ImageQualityLevel::Lowest:
            {
                qualityLevel = 51;
                break;
            }
            case ImageQualityLevel::Low:
            {
                qualityLevel = 102;
                break;
            }
            case ImageQualityLevel::Standard:
            {
                qualityLevel = 153;
                break;
            }
            case ImageQualityLevel::High:
            {
                useUASTC = true;
                qualityLevel = 204;
                break;
            }
            case ImageQualityLevel::Highest:
            {
                useUASTC = true;
                qualityLevel = 255;
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
        compressorParameters.m_quality_level = qualityLevel;
        compressorParameters.m_pJob_pool = &jobPool;

        compressorParameters.m_source_images.resize(compressInfo.imageLevels[0].layers.size());
        for (size layer = 0; layer < compressInfo.imageLevels[0].layers.size(); layer++)
        {
            compressorParameters.m_source_images[layer].init(compressInfo.imageLevels[0].layers[0].image.GetMemory().data(), compressInfo.imageLevels[0].layers[0].image.GetWidth(), compressInfo.imageLevels[0].layers[0].image.GetHeight(), ImageFormatToChannelCount(compressInfo.imageLevels[0].layers[layer].image.GetFormat()));
        }

        compressorParameters.m_source_mipmap_images.resize(compressInfo.imageLevels.size() - 1);
        for (size level = 1; level < compressInfo.imageLevels.size(); level++)
        {
            for (size layer = 0; layer < compressInfo.imageLevels.size(); layer++)
            {
                compressorParameters.m_source_images[level].init(compressInfo.imageLevels[level].layers[layer].image.GetMemory().data(), compressInfo.imageLevels[0].layers[0].image.GetWidth() >> level, compressInfo.imageLevels[0].layers[0].image.GetHeight() >> level, ImageFormatToChannelCount(compressInfo.imageLevels[level].layers[layer].image.GetFormat()));
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

        std::vector<uint8> compressedMemory(compressor.get_output_basis_file().data(), compressor.get_output_basis_file().data() + compressor.get_output_basis_file().size());
        return compressedMemory;
    }

    /* --- DESTRUCTORS --- */

    BasisUniversalCompressor::~BasisUniversalCompressor()
    {
        basisu::basisu_encoder_deinit();
    }


}