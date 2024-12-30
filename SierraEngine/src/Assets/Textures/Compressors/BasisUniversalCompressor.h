//
// Created by Nikolay Kanchevski on 16.07.24.
//

#pragma once

#include "../ImageCompressor.h"

namespace SierraEngine
{

    class SIERRA_ENGINE_API BasisUniversalCompressor final : public ImageCompressor
    {
    public:
        /* --- CONSTRUCTORS --- */
        BasisUniversalCompressor();

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<CompressedImage> Compress(const ImageCompressInfo& compressInfo) const noexcept override;

        /* --- COPY SEMANTICS --- */
        BasisUniversalCompressor(const BasisUniversalCompressor&) = delete;
        BasisUniversalCompressor& operator=(const BasisUniversalCompressor&) = delete;

        /* --- MOVE SEMANTICS --- */
        BasisUniversalCompressor(BasisUniversalCompressor&&) noexcept = default;
        BasisUniversalCompressor& operator=(BasisUniversalCompressor&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~BasisUniversalCompressor() noexcept override;
    };

}
