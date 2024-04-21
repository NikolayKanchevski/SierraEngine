//
// Created by Nikolay Kanchevski on 3.03.24.
//

#pragma once

#include "../ImageTranscoder.h"

namespace SierraEngine
{

    class KTXTranscoder final : public ImageTranscoder
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit KTXTranscoder(const ImageTranscoderCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<TranscodedImage> Transcode(const ImageTranscodeInfo &transcodeInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline ImageSupercompressorType GetType() const override { return ImageSupercompressorType::KTX; };

        /* --- DESTRUCTOR --- */
        ~KTXTranscoder() override = default;

    private:

    };

}