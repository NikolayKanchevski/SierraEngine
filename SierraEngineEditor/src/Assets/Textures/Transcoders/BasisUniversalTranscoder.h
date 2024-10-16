//
// Created by Nikolay Kanchevski on 18.07.24.
//

#pragma once

#include "../ImageTranscoder.h"

namespace SierraEngine
{

    class BasisUniversalTranscoder final : public ImageTranscoder
    {
    public:
        /* --- CONSTRUCTORS --- */
        BasisUniversalTranscoder();

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<std::vector<uint8>> Transcode(const ImageTranscodeInfo& transcodeInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] ImageTranscoderType GetType() const noexcept override { return ImageTranscoderType::BasisUniversal; }

        /* --- COPY SEMANTICS --- */
        BasisUniversalTranscoder(const BasisUniversalTranscoder&) = delete;
        BasisUniversalTranscoder& operator=(const BasisUniversalTranscoder&) = delete;

        /* --- MOVE SEMANTICS --- */
        BasisUniversalTranscoder(BasisUniversalTranscoder&&) noexcept = default;
        BasisUniversalTranscoder& operator=(BasisUniversalTranscoder&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~BasisUniversalTranscoder() noexcept override = default;
    };

}
