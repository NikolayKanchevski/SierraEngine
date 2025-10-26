//
// Created by Nikolay Kanchevski on 29.12.24.
//

#include "ImageConverter.h"

#include "ImageAccessor.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    LoadedImage ImageConverter::Convert(const ImageConvertInfo& convertInfo) const
    {
        if (convertInfo.format == convertInfo.image.format)
        {
            return convertInfo.image;
        }

        APP_THROW_IF(Sierra::ImageFormatToBlockSize(convertInfo.image.format) > 1, Sierra::InvalidValueError("Cannot convert image, as its format must not be compressed!"));
        APP_THROW_IF(ImageFormatToBlockSize(convertInfo.format) > 1, Sierra::InvalidValueError("Cannot convert image, as specified format must not be compressed!"));

        const uint8 originalChannelCount = Sierra::ImageFormatToChannelCount(convertInfo.image.format);
        const uint8 channelCount = ImageFormatToChannelCount(convertInfo.format);

        LoadedImage convertedImage = LoadedImage({ .width = convertInfo.image.width, .height = convertInfo.image.height, .format = convertInfo.format });
        convertedImage.memory.resize(convertedImage.width * convertedImage.height * Sierra::ImageFormatToBlockMemorySize(convertedImage.format));

        for (size x = 0; x < convertInfo.image.width; x++)
        {
            for (size y = 0; y < convertInfo.image.height; y++)
            {
                Color64 color = ImageAccessor::ReadPixel(convertInfo.image, { x, y });
                color.w += (channelCount >= 4 && channelCount > originalChannelCount) * (1.0f - color.w);
                ImageAccessor::WritePixel(convertedImage, { x, y }, color);
            }
        }

        return convertedImage;
    }

}
