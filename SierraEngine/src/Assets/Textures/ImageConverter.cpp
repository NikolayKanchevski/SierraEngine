//
// Created by Nikolay Kanchevski on 29.12.24.
//

#include "ImageConverter.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    Image ImageConverter::Convert(const ImageConvertInfo& convertInfo) const
    {
        APP_THROW_IF(ImageFormatToBlockSize(convertInfo.image.GetFormat()) > 1, Sierra::InvalidValueError("Cannot convert image, as its format must not be compressed!"));
        APP_THROW_IF(ImageFormatToBlockSize(convertInfo.format) > 1, Sierra::InvalidValueError("Cannot convert image, as specified format must not be compressed!"));

        const uint8 originalChannelCount = ImageFormatToChannelCount(convertInfo.image.GetFormat());
        const uint8 channelCount = ImageFormatToChannelCount(convertInfo.format);

        Image convertedImage = Image({ .width = convertInfo.image.GetWidth(), .height = convertInfo.image.GetHeight(), .format = convertInfo.format });
        for (size x = 0; x < convertInfo.image.GetWidth(); x++)
        {
            for (size y = 0; y < convertInfo.image.GetHeight(); y++)
            {
                Color64 color = convertInfo.image.GetPixel({ x, y });
                color.w += (channelCount >= 4 && channelCount > originalChannelCount) * (1.0f - color.w);
                convertedImage.SetPixel({ x, y }, color);
            }
        }

        return convertedImage;
    }

}