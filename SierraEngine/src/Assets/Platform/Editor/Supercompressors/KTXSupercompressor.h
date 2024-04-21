    //
// Created by Nikolay Kanchevski on 3.03.24.
//

#pragma once

#include "../../../ImageSupercompressor.h"

namespace SierraEngine
{

    class KTXSupercompressor final : public ImageSupercompressor
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit KTXSupercompressor(const ImageSupercompressorCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<std::vector<uint8>> Supercompress(const Sierra::FileManager &fileManager, const ImageSupercompressInfo &compressInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline ImageSupercompressorType GetType() const override { return ImageSupercompressorType::KTX; }

        /* --- DESTRUCTOR --- */
        ~KTXSupercompressor() override = default;

    };

}
