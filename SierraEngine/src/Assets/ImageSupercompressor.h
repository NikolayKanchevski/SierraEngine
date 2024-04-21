//
// Created by Nikolay Kanchevski on 3.03.24.
//

#pragma once

namespace SierraEngine
{

    struct ImageSupercompressorCreateInfo
    {
        uint16 maxImageDimensions = 4096;
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

    struct ImageSupercompressInfo
    {
        const std::initializer_list<std::initializer_list<std::filesystem::path>> &levelFilePaths;

        bool normalMap = false;
        bool generateMipMaps = false;

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
        [[nodiscard]] virtual std::optional<std::vector<uint8>> Supercompress(const Sierra::FileManager &fileManager, const ImageSupercompressInfo &superCompressInfo) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual ImageSupercompressorType GetType() const = 0;

        /* --- OPERATORS --- */
        ImageSupercompressor(const ImageSupercompressor&) = delete;
        ImageSupercompressor& operator=(const ImageSupercompressor&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~ImageSupercompressor() = default;

    protected:
        explicit ImageSupercompressor(const ImageSupercompressorCreateInfo &createInfo);
        [[nodiscard]] inline uint16 GetMaxImageDimensions() const { return maxImageDimensions; }

    private:
        const uint16 maxImageDimensions = 0;

    };

}
