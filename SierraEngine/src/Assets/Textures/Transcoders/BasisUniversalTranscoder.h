//
// Created by Nikolay Kanchevski on 18.07.24.
//

#pragma once

#include <transcoder/basisu_transcoder.h>

#include "../ImageTranscoder.h"

namespace SierraEngine
{

    class SIERRA_ENGINE_API BasisUniversalTranscoder final : public ImageTranscoder
    {
    public:
        /* --- CONSTRUCTORS --- */
        BasisUniversalTranscoder();

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<TranscodedImage> Transcode(const ImageTranscodeInfo& transcodeInfo) const override;

        /* --- COPY SEMANTICS --- */
        BasisUniversalTranscoder(const BasisUniversalTranscoder&) = delete;
        BasisUniversalTranscoder& operator=(const BasisUniversalTranscoder&) = delete;

        /* --- MOVE SEMANTICS --- */
        BasisUniversalTranscoder(BasisUniversalTranscoder&&) = delete;
        BasisUniversalTranscoder& operator=(BasisUniversalTranscoder&&) = delete;

        /* --- DESTRUCTOR --- */
        ~BasisUniversalTranscoder() noexcept override = default;

    private:
        mutable basist::basisu_transcoder transcoder = { };

    };

}
