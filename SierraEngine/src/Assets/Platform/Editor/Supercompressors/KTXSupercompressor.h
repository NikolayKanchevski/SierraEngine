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
        [[nodiscard]] bool Supercompress(const ImageSupercompressorSupercompressInfo &compressInfo, void*& compressedMemory, uint64 &compressedMemorySize) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline ImageSupercompressorType GetType() const override { return ImageSupercompressorType::KTX; }

        /* --- DESTRUCTOR --- */
        ~KTXSupercompressor() override = default;

    };

}
