//
// Created by Nikolay Kanchevski on 16.07.24.
//

#pragma once

#include "../ImageCompressor.h"

namespace SierraEngine
{

    class BasisUniversalCompressor final : public ImageCompressor
    {
    public:
        /* --- CONSTRUCTORS --- */
        BasisUniversalCompressor();

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<std::vector<uint8>> Compress(const ImageCompressInfo& compressInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] ImageCompressorType GetType() const override { return ImageCompressorType::BasisUniversal; }

        /* --- DESTRUCTOR --- */
        ~BasisUniversalCompressor() override;

    private:

    };

}
