//
// Created by Nikolay Kanchevski on 3.03.24.
//

#pragma once

namespace SierraEngine
{

    struct ImageSupercompressorCreateInfo
    {
        uint16 maximumImageDimensions = 4096;
    };

    enum class ImageSupercompressionQualityLevel : uint8
    {
        Lowest,
        VeryLow,
        Low,
        Standard,
        High,
        VeryHigh
    };

    enum class ImageSupercompressionLevel : uint8
    {
        None,
        Lowest,
        VeryLow,
        Low,
        Standard,
        High,
        VeryHigh
    };

    struct ImageSupercompressorSupercompressInfo
    {
        const std::vector<std::vector<std::filesystem::path>> &filePaths;
        bool normalMap = false;

        ImageSupercompressionLevel compressionLevel = ImageSupercompressionLevel::Standard;
        ImageSupercompressionQualityLevel qualityLevel = ImageSupercompressionQualityLevel::Standard;
    };

    enum class ImageSupercompressorType : bool
    {
        Undefined,
        KTX
    };

    class ImageSupercompressor
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual bool Supercompress(const ImageSupercompressorSupercompressInfo &superCompressInfo, void*& compressedMemory, uint64 &compressedMemorySize) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual ImageSupercompressorType GetType() const = 0;

        /* --- OPERATORS --- */
        ImageSupercompressor(const ImageSupercompressor&) = delete;
        ImageSupercompressor &operator=(const ImageSupercompressor&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~ImageSupercompressor() = default;

    protected:
        explicit ImageSupercompressor(const ImageSupercompressorCreateInfo &createInfo);
        [[nodiscard]] inline uint16 GetMaximumImageDimensions() const { return maximumImageDimensions; }

    private:
        uint16 maximumImageDimensions = 0;

    };

}
