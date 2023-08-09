//
// Created by Nikolay Kanchevski on 15.12.22.
//

#pragma once

#include "../Internal/Macros.h"
#include "../Internal/Definitions.h"

namespace Sierra::Rendering
{
    enum class ImageChannels
    {
        R = 1,
        RG = 2,
        RGB = 3,
        RGBA = 4
    };

    inline constexpr ImageChannels ImageChannels_FromString(const char* string)
    {
        if (StringView(string) == "R")          return ImageChannels::R;
        if (StringView(string) == "RG")         return ImageChannels::RG;
        if (StringView(string) == "RGB")        return ImageChannels::RGB;
        if (StringView(string) == "RGBA")       return ImageChannels::RGBA;
        return ImageChannels::RGBA;
    }

    inline constexpr const char* ImageChannels_ToString(const ImageChannels channels)
    {
        switch (channels)
        {
            case ImageChannels::R:          return "R";
            case ImageChannels::RG:         return "RG";
            case ImageChannels::RGB:        return "RGB";
            case ImageChannels::RGBA:       return "RGBA";
        }
    }

    enum class ImageMemoryType
    {
        UINT8_NORM,
        UINT16_NORM,
        UINT8,
        UINT16,
        UINT32,
        UINT64,
        INT8_NORM,
        INT16_NORM,
        INT8,
        INT16,
        INT32,
        INT64,
        FLOAT16,
        FLOAT32,
        FLOAT64,
        SRGB
    };

    inline constexpr const char* ImageMemoryType_ToString(const ImageMemoryType type)
    {
        switch (type)
        {
            case ImageMemoryType::UINT8_NORM:       return "UINT8_NORM";
            case ImageMemoryType::UINT16_NORM:      return "UINT16_NORM";
            case ImageMemoryType::UINT8:            return "UINT8";
            case ImageMemoryType::UINT16:           return "UINT16";
            case ImageMemoryType::UINT32:           return "UINT32";
            case ImageMemoryType::UINT64:           return "UINT64";
            case ImageMemoryType::INT8_NORM:        return "INT8_NORM";
            case ImageMemoryType::INT16_NORM:       return "INT16_NORM";
            case ImageMemoryType::INT8:             return "INT8";
            case ImageMemoryType::INT16:            return "INT16";
            case ImageMemoryType::INT32:            return "INT32";
            case ImageMemoryType::INT64:            return "INT64";
            case ImageMemoryType::FLOAT16:          return "FLOAT16";
            case ImageMemoryType::FLOAT32:          return "FLOAT32";
            case ImageMemoryType::FLOAT64:          return "FLOAT64";
            case ImageMemoryType::SRGB:             return "SRGB";
        }
    }

    constexpr ImageMemoryType ImageMemoryType_FromString(const char* string)
    {
        if (StringView(string) == "UINT8_NORM")       return ImageMemoryType::UINT8_NORM;
        if (StringView(string) == "UINT16_NORM")      return ImageMemoryType::UINT16_NORM;
        if (StringView(string) == "UINT8")            return ImageMemoryType::UINT8;
        if (StringView(string) == "UINT16")           return ImageMemoryType::UINT16;
        if (StringView(string) == "UINT32")           return ImageMemoryType::UINT32;
        if (StringView(string) == "UINT64")           return ImageMemoryType::UINT64;
        if (StringView(string) == "INT8_NORM")        return ImageMemoryType::INT8_NORM;
        if (StringView(string) == "INT16_NORM")       return ImageMemoryType::INT16_NORM;
        if (StringView(string) == "INT8")             return ImageMemoryType::INT8;
        if (StringView(string) == "INT16")            return ImageMemoryType::INT16;
        if (StringView(string) == "INT32")            return ImageMemoryType::INT32;
        if (StringView(string) == "INT64")            return ImageMemoryType::INT64;
        if (StringView(string) == "FLOAT16")          return ImageMemoryType::FLOAT16;
        if (StringView(string) == "FLOAT32")          return ImageMemoryType::FLOAT32;
        if (StringView(string) == "FLOAT64")          return ImageMemoryType::FLOAT64;
        if (StringView(string) == "SRGB")             return ImageMemoryType::SRGB;
        return ImageMemoryType::UINT8_NORM;
    }

    enum class ImageFormat
    {
        UNDEFINED                  =       VK_FORMAT_UNDEFINED,
        R8_UNORM                   =       VK_FORMAT_R8_UNORM,
        R8_SNORM                   =       VK_FORMAT_R8_SNORM,
        R8_UINT                    =       VK_FORMAT_R8_UINT,
        R8_SINT                    =       VK_FORMAT_R8_SINT,
        R8_SRGB                    =       VK_FORMAT_R8_SRGB,
        R8G8_UNORM                 =       VK_FORMAT_R8G8_UNORM,
        R8G8_SNORM                 =       VK_FORMAT_R8G8_SNORM,
        R8G8_UINT                  =       VK_FORMAT_R8G8_UINT,
        R8G8_SINT                  =       VK_FORMAT_R8G8_SINT,
        R8G8_SRGB                  =       VK_FORMAT_R8G8_SRGB,
        R8G8B8_UNORM               =       VK_FORMAT_R8G8B8_UNORM,
        R8G8B8_SNORM               =       VK_FORMAT_R8G8B8_SNORM,
        R8G8B8_UINT                =       VK_FORMAT_R8G8B8_UINT,
        R8G8B8_SINT                =       VK_FORMAT_R8G8B8_SINT,
        R8G8B8_SRGB                =       VK_FORMAT_R8G8B8_SRGB,
        B8G8R8_UNORM               =       VK_FORMAT_B8G8R8_UNORM,
        B8G8R8_SNORM               =       VK_FORMAT_B8G8R8_SNORM,
        B8G8R8_UINT                =       VK_FORMAT_B8G8R8_UINT,
        B8G8R8_SINT                =       VK_FORMAT_B8G8R8_SINT,
        B8G8R8_SRGB                =       VK_FORMAT_B8G8R8_SRGB,
        R8G8B8A8_UNORM             =       VK_FORMAT_R8G8B8A8_UNORM,
        R8G8B8A8_SNORM             =       VK_FORMAT_R8G8B8A8_SNORM,
        R8G8B8A8_UINT              =       VK_FORMAT_R8G8B8A8_UINT,
        R8G8B8A8_SINT              =       VK_FORMAT_R8G8B8A8_SINT,
        R8G8B8A8_SRGB              =       VK_FORMAT_R8G8B8A8_SRGB,
        B8G8R8A8_UNORM             =       VK_FORMAT_B8G8R8A8_UNORM,
        B8G8R8A8_SNORM             =       VK_FORMAT_B8G8R8A8_SNORM,
        B8G8R8A8_UINT              =       VK_FORMAT_B8G8R8A8_UINT,
        B8G8R8A8_SINT              =       VK_FORMAT_B8G8R8A8_SINT,
        B8G8R8A8_SRGB              =       VK_FORMAT_B8G8R8A8_SRGB,
        R16_UNORM                  =       VK_FORMAT_R16_UNORM,
        R16_SNORM                  =       VK_FORMAT_R16_SNORM,
        R16_UINT                   =       VK_FORMAT_R16_UINT,
        R16_SINT                   =       VK_FORMAT_R16_SINT,
        R16_SFLOAT                 =       VK_FORMAT_R16_SFLOAT,
        R16G16_UNORM               =       VK_FORMAT_R16G16_UNORM,
        R16G16_SNORM               =       VK_FORMAT_R16G16_SNORM,
        R16G16_UINT                =       VK_FORMAT_R16G16_UINT,
        R16G16_SINT                =       VK_FORMAT_R16G16_SINT,
        R16G16_SFLOAT              =       VK_FORMAT_R16G16_SFLOAT,
        R16G16B16_UNORM            =       VK_FORMAT_R16G16B16_UNORM,
        R16G16B16_SNORM            =       VK_FORMAT_R16G16B16_SNORM,
        R16G16B16_UINT             =       VK_FORMAT_R16G16B16_UINT,
        R16G16B16_SINT             =       VK_FORMAT_R16G16B16_SINT,
        R16G16B16_SFLOAT           =       VK_FORMAT_R16G16B16_SFLOAT,
        R16G16B16A16_UNORM         =       VK_FORMAT_R16G16B16A16_UNORM,
        R16G16B16A16_SNORM         =       VK_FORMAT_R16G16B16A16_SNORM,
        R16G16B16A16_UINT          =       VK_FORMAT_R16G16B16A16_UINT,
        R16G16B16A16_SINT          =       VK_FORMAT_R16G16B16A16_SINT,
        R16G16B16A16_SFLOAT        =       VK_FORMAT_R16G16B16A16_SFLOAT,
        R32_UINT                   =       VK_FORMAT_R32_UINT,
        R32_SINT                   =       VK_FORMAT_R32_SINT,
        R32_SFLOAT                 =       VK_FORMAT_R32_SFLOAT,
        R32G32_UINT                =       VK_FORMAT_R32G32_UINT,
        R32G32_SINT                =       VK_FORMAT_R32G32_SINT,
        R32G32_SFLOAT              =       VK_FORMAT_R32G32_SFLOAT,
        R32G32B32_UINT             =       VK_FORMAT_R32G32B32_UINT,
        R32G32B32_SINT             =       VK_FORMAT_R32G32B32_SINT,
        R32G32B32_SFLOAT           =       VK_FORMAT_R32G32B32_SFLOAT,
        R32G32B32A32_UINT          =       VK_FORMAT_R32G32B32A32_UINT,
        R32G32B32A32_SINT          =       VK_FORMAT_R32G32B32A32_SINT,
        R32G32B32A32_SFLOAT        =       VK_FORMAT_R32G32B32A32_SFLOAT,
        R64_UINT                   =       VK_FORMAT_R64_UINT,
        R64_SINT                   =       VK_FORMAT_R64_SINT,
        R64_SFLOAT                 =       VK_FORMAT_R64_SFLOAT,
        R64G64_UINT                =       VK_FORMAT_R64G64_UINT,
        R64G64_SINT                =       VK_FORMAT_R64G64_SINT,
        R64G64_SFLOAT              =       VK_FORMAT_R64G64_SFLOAT,
        R64G64B64_UINT             =       VK_FORMAT_R64G64B64_UINT,
        R64G64B64_SINT             =       VK_FORMAT_R64G64B64_SINT,
        R64G64B64_SFLOAT           =       VK_FORMAT_R64G64B64_SFLOAT,
        R64G64B64A64_UINT          =       VK_FORMAT_R64G64B64A64_UINT,
        R64G64B64A64_SINT          =       VK_FORMAT_R64G64B64A64_SINT,
        R64G64B64A64_SFLOAT        =       VK_FORMAT_R64G64B64A64_SFLOAT,
        D16_UNORM                  =       VK_FORMAT_D16_UNORM,
        D32_SFLOAT                 =       VK_FORMAT_D32_SFLOAT,
        S8_UINT                    =       VK_FORMAT_S8_UINT
    };

    constexpr uint32 GetChannelCountForImageFormat(const ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::UNDEFINED:                                            return 0;
            case ImageFormat::R8_UNORM:                                             return 1;
            case ImageFormat::R8_SNORM:                                             return 1;
            case ImageFormat::R8_UINT:                                              return 1;
            case ImageFormat::R8_SINT:                                              return 1;
            case ImageFormat::R8_SRGB:                                              return 1;
            case ImageFormat::R8G8_UNORM:                                           return 2;
            case ImageFormat::R8G8_SNORM:                                           return 2;
            case ImageFormat::R8G8_UINT:                                            return 2;
            case ImageFormat::R8G8_SINT:                                            return 2;
            case ImageFormat::R8G8_SRGB:                                            return 2;
            case ImageFormat::R8G8B8_UNORM:                                         return 3;
            case ImageFormat::R8G8B8_SNORM:                                         return 3;
            case ImageFormat::R8G8B8_UINT:                                          return 3;
            case ImageFormat::R8G8B8_SINT:                                          return 3;
            case ImageFormat::R8G8B8_SRGB:                                          return 3;
            case ImageFormat::B8G8R8_UNORM:                                         return 3;
            case ImageFormat::B8G8R8_SNORM:                                         return 3;
            case ImageFormat::B8G8R8_UINT:                                          return 3;
            case ImageFormat::B8G8R8_SINT:                                          return 3;
            case ImageFormat::B8G8R8_SRGB:                                          return 3;
            case ImageFormat::R8G8B8A8_UNORM:                                       return 4;
            case ImageFormat::R8G8B8A8_SNORM:                                       return 4;
            case ImageFormat::R8G8B8A8_UINT:                                        return 4;
            case ImageFormat::R8G8B8A8_SINT:                                        return 4;
            case ImageFormat::R8G8B8A8_SRGB:                                        return 4;
            case ImageFormat::B8G8R8A8_UNORM:                                       return 4;
            case ImageFormat::B8G8R8A8_SNORM:                                       return 4;
            case ImageFormat::B8G8R8A8_UINT:                                        return 4;
            case ImageFormat::B8G8R8A8_SINT:                                        return 4;
            case ImageFormat::B8G8R8A8_SRGB:                                        return 4;
            case ImageFormat::R16_UNORM:                                            return 1;
            case ImageFormat::R16_SNORM:                                            return 1;
            case ImageFormat::R16_UINT:                                             return 1;
            case ImageFormat::R16_SINT:                                             return 1;
            case ImageFormat::R16_SFLOAT:                                           return 1;
            case ImageFormat::R16G16_UNORM:                                         return 2;
            case ImageFormat::R16G16_SNORM:                                         return 2;
            case ImageFormat::R16G16_UINT:                                          return 2;
            case ImageFormat::R16G16_SINT:                                          return 2;
            case ImageFormat::R16G16_SFLOAT:                                        return 2;
            case ImageFormat::R16G16B16_UNORM:                                      return 3;
            case ImageFormat::R16G16B16_SNORM:                                      return 3;
            case ImageFormat::R16G16B16_UINT:                                       return 3;
            case ImageFormat::R16G16B16_SINT:                                       return 3;
            case ImageFormat::R16G16B16_SFLOAT:                                     return 3;
            case ImageFormat::R16G16B16A16_UNORM:                                   return 4;
            case ImageFormat::R16G16B16A16_SNORM:                                   return 4;
            case ImageFormat::R16G16B16A16_UINT:                                    return 4;
            case ImageFormat::R16G16B16A16_SINT:                                    return 4;
            case ImageFormat::R16G16B16A16_SFLOAT:                                  return 4;
            case ImageFormat::R32_UINT:                                             return 1;
            case ImageFormat::R32_SINT:                                             return 1;
            case ImageFormat::R32_SFLOAT:                                           return 1;
            case ImageFormat::R32G32_UINT:                                          return 2;
            case ImageFormat::R32G32_SINT:                                          return 2;
            case ImageFormat::R32G32_SFLOAT:                                        return 2;
            case ImageFormat::R32G32B32_UINT:                                       return 3;
            case ImageFormat::R32G32B32_SINT:                                       return 3;
            case ImageFormat::R32G32B32_SFLOAT:                                     return 3;
            case ImageFormat::R32G32B32A32_UINT:                                    return 4;
            case ImageFormat::R32G32B32A32_SINT:                                    return 4;
            case ImageFormat::R32G32B32A32_SFLOAT:                                  return 4;
            case ImageFormat::R64_UINT:                                             return 1;
            case ImageFormat::R64_SINT:                                             return 1;
            case ImageFormat::R64_SFLOAT:                                           return 1;
            case ImageFormat::R64G64_UINT:                                          return 2;
            case ImageFormat::R64G64_SINT:                                          return 2;
            case ImageFormat::R64G64_SFLOAT:                                        return 2;
            case ImageFormat::R64G64B64_UINT:                                       return 3;
            case ImageFormat::R64G64B64_SINT:                                       return 3;
            case ImageFormat::R64G64B64_SFLOAT:                                     return 3;
            case ImageFormat::R64G64B64A64_UINT:                                    return 4;
            case ImageFormat::R64G64B64A64_SINT:                                    return 4;
            case ImageFormat::R64G64B64A64_SFLOAT:                                  return 4;
            case ImageFormat::D16_UNORM:                                            return 1;
            case ImageFormat::D32_SFLOAT:                                           return 1;
            case ImageFormat::S8_UINT:                                              return 1;
        }
    }

    inline ImageFormat CreateImageFormat(const ImageChannels channels, const ImageMemoryType memoryType)
    {
        switch (memoryType)
        {
            case ImageMemoryType::UINT8_NORM:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R8_UNORM;
                    case ImageChannels::RG:         return ImageFormat::R8G8_UNORM;
                    case ImageChannels::RGB:        return ImageFormat::R8G8B8_UNORM;
                    case ImageChannels::RGBA:       return ImageFormat::R8G8B8A8_UNORM;
                }
            }
            case ImageMemoryType::UINT16_NORM:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R16_UNORM;
                    case ImageChannels::RG:         return ImageFormat::R16G16_UNORM;
                    case ImageChannels::RGB:        return ImageFormat::R16G16B16_UNORM;
                    case ImageChannels::RGBA:       return ImageFormat::R16G16B16A16_UNORM;
                }
            }
            case ImageMemoryType::UINT8:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R8_UINT;
                    case ImageChannels::RG:         return ImageFormat::R8G8_UINT;
                    case ImageChannels::RGB:        return ImageFormat::R8G8B8_UINT;
                    case ImageChannels::RGBA:       return ImageFormat::R8G8B8A8_UINT;
                }
            }
            case ImageMemoryType::UINT16:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R16_UINT;
                    case ImageChannels::RG:         return ImageFormat::R16G16_UINT;
                    case ImageChannels::RGB:        return ImageFormat::R16G16B16_UINT;
                    case ImageChannels::RGBA:       return ImageFormat::R16G16B16A16_UINT;
                }
            }
            case ImageMemoryType::UINT32:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R32_UINT;
                    case ImageChannels::RG:         return ImageFormat::R32G32_UINT;
                    case ImageChannels::RGB:        return ImageFormat::R32G32B32_UINT;
                    case ImageChannels::RGBA:       return ImageFormat::R32G32B32A32_UINT;
                }
            }
            case ImageMemoryType::UINT64:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R64_UINT;
                    case ImageChannels::RG:         return ImageFormat::R64G64_UINT;
                    case ImageChannels::RGB:        return ImageFormat::R64G64B64_UINT;
                    case ImageChannels::RGBA:       return ImageFormat::R64G64B64A64_UINT;
                }
            }

            case ImageMemoryType::INT8_NORM:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R8_SNORM;
                    case ImageChannels::RG:         return ImageFormat::R8G8_SNORM;
                    case ImageChannels::RGB:        return ImageFormat::R8G8B8_SNORM;
                    case ImageChannels::RGBA:       return ImageFormat::R8G8B8A8_SNORM;
                }
            }
            case ImageMemoryType::INT16_NORM:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R16_SNORM;
                    case ImageChannels::RG:         return ImageFormat::R16G16_SNORM;
                    case ImageChannels::RGB:        return ImageFormat::R16G16B16_SNORM;
                    case ImageChannels::RGBA:       return ImageFormat::R16G16B16A16_SNORM;
                }
            }
            case ImageMemoryType::INT8:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R8_SINT;
                    case ImageChannels::RG:         return ImageFormat::R8G8_SINT;
                    case ImageChannels::RGB:        return ImageFormat::R8G8B8_SINT;
                    case ImageChannels::RGBA:       return ImageFormat::R8G8B8A8_SINT;
                }
            }
            case ImageMemoryType::INT16:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R16_SINT;
                    case ImageChannels::RG:         return ImageFormat::R16G16_SINT;
                    case ImageChannels::RGB:        return ImageFormat::R16G16B16_SINT;
                    case ImageChannels::RGBA:       return ImageFormat::R16G16B16A16_SINT;
                }
            }
            case ImageMemoryType::INT32:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R32_SINT;
                    case ImageChannels::RG:         return ImageFormat::R32G32_SINT;
                    case ImageChannels::RGB:        return ImageFormat::R32G32B32_SINT;
                    case ImageChannels::RGBA:       return ImageFormat::R32G32B32A32_SINT;
                }
            }
            case ImageMemoryType::INT64:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R64_SINT;
                    case ImageChannels::RG:         return ImageFormat::R64G64_SINT;
                    case ImageChannels::RGB:        return ImageFormat::R64G64B64_SINT;
                    case ImageChannels::RGBA:       return ImageFormat::R64G64B64A64_SINT;
                }
            }

            case ImageMemoryType::FLOAT16:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R16_SFLOAT;
                    case ImageChannels::RG:         return ImageFormat::R16G16_SFLOAT;
                    case ImageChannels::RGB:        return ImageFormat::R16G16B16_SFLOAT;
                    case ImageChannels::RGBA:       return ImageFormat::R16G16B16A16_SFLOAT;
                }
            }
            case ImageMemoryType::FLOAT32:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R32_SFLOAT;
                    case ImageChannels::RG:         return ImageFormat::R32G32_SFLOAT;
                    case ImageChannels::RGB:        return ImageFormat::R32G32B32_SFLOAT;
                    case ImageChannels::RGBA:       return ImageFormat::R32G32B32A32_SFLOAT;
                }
            }
            case ImageMemoryType::FLOAT64:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R64_SFLOAT;
                    case ImageChannels::RG:         return ImageFormat::R64G64_SFLOAT;
                    case ImageChannels::RGB:        return ImageFormat::R64G64B64_SFLOAT;
                    case ImageChannels::RGBA:       return ImageFormat::R64G64B64A64_SFLOAT;
                }
            }
            case ImageMemoryType::SRGB:
            {
                switch (channels)
                {
                    case ImageChannels::R:          return ImageFormat::R8_SRGB;
                    case ImageChannels::RG:         return ImageFormat::R8G8_SRGB;
                    case ImageChannels::RGB:        return ImageFormat::R8G8B8_SRGB;
                    case ImageChannels::RGBA:       return ImageFormat::R8G8B8A8_SRGB;
                }
            }
        }

        return ImageFormat::UNDEFINED;
    }

    enum class ImageUsage
    {
        UNDEFINED = 0,
        TRANSFER_SRC = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        TRANSFER_DST = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        SAMPLED = VK_IMAGE_USAGE_SAMPLED_BIT,
        STORAGE = VK_IMAGE_USAGE_STORAGE_BIT,
        COLOR_ATTACHMENT = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        DEPTH_STENCIL_ATTACHMENT = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        TRANSIENT_ATTACHMENT = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
        INPUT_ATTACHMENT = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
        FRAGMENT_DENSITY_MAP = VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT,
        FRAGMENT_SHADING_RATE_ATTACHMENT = VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
        SHADING_RATE = FRAGMENT_SHADING_RATE_ATTACHMENT
    };

    DEFINE_ENUM_FLAG_OPERATORS(ImageUsage);

    enum class ImageLayout
    {
        UNDEFINED = 0,
        GENERAL = VK_IMAGE_LAYOUT_GENERAL,
        COLOR_ATTACHMENT_OPTIMAL = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        DEPTH_STENCIL_ATTACHMENT_OPTIMAL = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        DEPTH_STENCIL_READ_ONLY_OPTIMAL = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
        SHADER_READ_ONLY_OPTIMAL = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        TRANSFER_SRC_OPTIMAL = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        TRANSFER_DST_OPTIMAL = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        PREINITIALIZED = VK_IMAGE_LAYOUT_PREINITIALIZED,
        DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
        DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
        DEPTH_ATTACHMENT_OPTIMAL = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        DEPTH_READ_ONLY_OPTIMAL = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
        STENCIL_ATTACHMENT_OPTIMAL = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
        STENCIL_READ_ONLY_OPTIMAL = VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL,
        READ_ONLY_OPTIMAL = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR,
        ATTACHMENT_OPTIMAL = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
        PRESENT_SRC = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        SHARED_PRESENT = VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR,
        FRAGMENT_DENSITY_MAP_OPTIMAL_EXT = VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT,
        FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR,
    };

    enum class ImageAspectFlags
    {
        UNDEFINED = 0,
        COLOR = VK_IMAGE_ASPECT_COLOR_BIT,
        DEPTH = VK_IMAGE_ASPECT_DEPTH_BIT,
        STENCIL = VK_IMAGE_ASPECT_STENCIL_BIT,
        METADATA = VK_IMAGE_ASPECT_METADATA_BIT,
        PLANE_0 = VK_IMAGE_ASPECT_PLANE_0_BIT,
        PLANE_1 = VK_IMAGE_ASPECT_PLANE_1_BIT,
        PLANE_2 = VK_IMAGE_ASPECT_PLANE_2_BIT,
        MEMORY_PLANE_0_EXT = VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT,
        MEMORY_PLANE_1_EXT = VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT,
        MEMORY_PLANE_2_EXT = VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT,
        MEMORY_PLANE_3_EXT = VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT,
        PLANE_0_KHR = VK_IMAGE_ASPECT_PLANE_0_BIT_KHR,
        PLANE_1_KHR = VK_IMAGE_ASPECT_PLANE_1_BIT_KHR,
        PLANE_2_KHR = VK_IMAGE_ASPECT_PLANE_2_BIT_KHR,
        FLAG_BITS_MAX_ENUM = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM
    };

    DEFINE_ENUM_FLAG_OPERATORS(ImageAspectFlags)

    enum class ImageCreateFlags
    {
        UNDEFINED = 0,
        SPARSE_BINDING = VK_IMAGE_CREATE_SPARSE_BINDING_BIT,
        SPARSE_RESIDENCY = VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT,
        SPARSE_ALIASED = VK_IMAGE_CREATE_SPARSE_ALIASED_BIT,
        MUTABLE_FORMAT = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT,
        CUBE_COMPATIBLE = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
        ALIAS = VK_IMAGE_CREATE_ALIAS_BIT,
        SPLIT_INSTANCE_BIND_REGIONS = VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT,
        BLOCK_TEXEL_VIEW_COMPATIBLE = VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT,
        EXTENDED_USAGE = VK_IMAGE_CREATE_EXTENDED_USAGE_BIT,
        PROTECTED = VK_IMAGE_CREATE_PROTECTED_BIT,
        DISJOINT = VK_IMAGE_CREATE_DISJOINT_BIT,
        SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT = VK_IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT,
        SUBSAMPLED_BIT_EXT = VK_IMAGE_CREATE_SUBSAMPLED_BIT_EXT,
        SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR = VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT,
        ARRAY_2D_COMPATIBLE_BIT_KHR = VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT,
        BLOCK_TEXEL_VIEW_COMPATIBLE_BIT_KHR = VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT,
        EXTENDED_USAGE_BIT_KHR = VK_IMAGE_CREATE_EXTENDED_USAGE_BIT,
        DISJOINT_BIT_KHR = VK_IMAGE_CREATE_DISJOINT_BIT,
        ALIAS_BIT_KHR = VK_IMAGE_CREATE_ALIAS_BIT
    };

    enum class ImageType
    {
        SINGLE_DIMENSIONAL = VK_IMAGE_TYPE_1D,
        TEXTURE = VK_IMAGE_TYPE_2D,
        CUBEMAP = VK_IMAGE_TYPE_3D,
        MAX_ENUM = VK_IMAGE_TYPE_MAX_ENUM
    };

    enum class ImageViewType
    {
        SINGLE_DIMENSIONAL = VK_IMAGE_VIEW_TYPE_1D,
        TEXTURE = VK_IMAGE_VIEW_TYPE_2D,
        TEXTURE_3D = VK_IMAGE_VIEW_TYPE_3D,
        CUBEMAP = VK_IMAGE_VIEW_TYPE_CUBE,
        ARRAY_1D = VK_IMAGE_VIEW_TYPE_1D_ARRAY,
        ARRAY_2D = VK_IMAGE_VIEW_TYPE_2D_ARRAY,
        CUBE_ARRAY = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY,
        MAX_ENUM = VK_IMAGE_VIEW_TYPE_MAX_ENUM
    };

    enum class ImageTiling
    {
        OPTIMAL = VK_IMAGE_TILING_OPTIMAL,
        LINEAR = VK_IMAGE_TILING_LINEAR,
        DRM_FORMAT_MODIFIER_EXT = VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT,
        MAX_ENUM = VK_IMAGE_TILING_MAX_ENUM
    };

    enum class Sampling
    {
        MSAAx1 = VK_SAMPLE_COUNT_1_BIT,
        MSAAx2 = VK_SAMPLE_COUNT_2_BIT,
        MSAAx4 = VK_SAMPLE_COUNT_4_BIT,
        MSAAx8 = VK_SAMPLE_COUNT_8_BIT,
        MSAAx16 = VK_SAMPLE_COUNT_16_BIT,
        MSAAx32 = VK_SAMPLE_COUNT_32_BIT,
        MSAAx64 = VK_SAMPLE_COUNT_64_BIT,
        MSAA_BITS_MAX = VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM
    };

    enum class Filter
    {
        NEAREST = VK_FILTER_NEAREST,
        LINEAR = VK_FILTER_LINEAR,
        CUBIC_IMG = VK_FILTER_CUBIC_IMG,
        CUBIC_EXT = VK_FILTER_CUBIC_EXT,
        MAX_ENUM = VK_FILTER_MAX_ENUM
    };

    enum class SamplerAddressMode
    {
        REPEAT = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        MIRRORED_REPEAT = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
        CLAMP_TO_EDGE = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        CLAMP_TO_BORDER = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        MIRROR_CLAMP_TO_EDGE = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE,
        MIRROR_CLAMP_TO_EDGE_KHR = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE_KHR,
        MAX_ENUM = VK_SAMPLER_ADDRESS_MODE_MAX_ENUM
    };

    enum class SamplerBorderColor
    {
        FLOAT_TRANSPARENT_BLACK = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
        INT_TRANSPARENT_BLACK = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK,
        FLOAT_OPAQUE_BLACK = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
        INT_OPAQUE_BLACK = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        FLOAT_OPAQUE_WHITE = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
        INT_OPAQUE_WHITE = VK_BORDER_COLOR_INT_OPAQUE_WHITE,
        FLOAT_CUSTOM_EXT = VK_BORDER_COLOR_FLOAT_CUSTOM_EXT,
        INT_CUSTOM_EXT = VK_BORDER_COLOR_INT_CUSTOM_EXT
    };

    enum class SamplerMipMode
    {
        NEAREST = VK_SAMPLER_MIPMAP_MODE_NEAREST,
        LINEAR = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        MAX_ENUM = VK_SAMPLER_MIPMAP_MODE_MAX_ENUM
    };

    enum class SamplerCompareOp
    {
        NEVER = VK_COMPARE_OP_NEVER,
        LESS = VK_COMPARE_OP_LESS,
        EQUAL = VK_COMPARE_OP_EQUAL,
        LESS_OR_EQUAL = VK_COMPARE_OP_LESS_OR_EQUAL,
        GREATER = VK_COMPARE_OP_GREATER,
        NOT_EQUAL = VK_COMPARE_OP_NOT_EQUAL,
        GREATER_OR_EQUAL = VK_COMPARE_OP_GREATER_OR_EQUAL,
        ALWAYS = VK_COMPARE_OP_ALWAYS,
        MAX_ENUM = VK_COMPARE_OP_MAX_ENUM
    };

    enum class ComponentSwizzle
    {
        IDENTITY = VK_COMPONENT_SWIZZLE_IDENTITY,
        ZERO = VK_COMPONENT_SWIZZLE_ZERO,
        ONE = VK_COMPONENT_SWIZZLE_ONE,
        R = VK_COMPONENT_SWIZZLE_R,
        G = VK_COMPONENT_SWIZZLE_G,
        B = VK_COMPONENT_SWIZZLE_B,
        A = VK_COMPONENT_SWIZZLE_A,
        MAX_ENUM = VK_COMPONENT_SWIZZLE_MAX_ENUM
    };

    struct ComponentMapping
    {
        ComponentSwizzle r;
        ComponentSwizzle g;
        ComponentSwizzle b;
        ComponentSwizzle a;

        operator VkComponentMapping const&() const
        {
            return *reinterpret_cast<const VkComponentMapping*>(this);
        }

        operator VkComponentMapping&()
        {
            return *reinterpret_cast<VkComponentMapping*>(this);
        }
    };

    enum class BufferUsage
    {
        UNDEFINED = 0,
        TRANSFER_SRC = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        TRANSFER_DST = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        UNIFORM_TEXEL = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,
        STORAGE_TEXEL = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
        UNIFORM = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        STORAGE = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        INDEX = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VERTEX = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        INDIRECT = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
        SHADER_DEVICE_ADDRESS = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
    };

    DEFINE_ENUM_FLAG_OPERATORS(BufferUsage)

    enum class LoadOp
    {
        UNDEFINED = 0,
        LOAD = VK_ATTACHMENT_LOAD_OP_LOAD,
        CLEAR = VK_ATTACHMENT_LOAD_OP_CLEAR,
        DONT_CARE = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        NONE_EXT = VK_ATTACHMENT_LOAD_OP_NONE_EXT,
        MAX_ENUM = VK_ATTACHMENT_LOAD_OP_MAX_ENUM
    };

    enum class StoreOp
    {
        UNDEFINED = 0,
        STORE = VK_ATTACHMENT_STORE_OP_STORE,
        DONT_CARE = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        NONE = VK_ATTACHMENT_STORE_OP_NONE_EXT,
        MAX_ENUM = VK_ATTACHMENT_STORE_OP_MAX_ENUM
    };

    enum class MemoryFlags
    {
        UNDEFINED = 0,
        DEVICE_LOCAL = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        HOST_VISIBLE = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        HOST_COHERENT = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    DEFINE_ENUM_FLAG_OPERATORS(MemoryFlags)

    enum class VertexAttributeType
    {
        FLOAT = VK_FORMAT_R32_SFLOAT,
        VECTOR_2 = VK_FORMAT_R32G32_SFLOAT,
        VECTOR_3 = VK_FORMAT_R32G32B32_SFLOAT,
        VECTOR_4 = VK_FORMAT_R32G32B32A32_SFLOAT,

        POSITION = VECTOR_3,
        NORMAL = VECTOR_3,
        TANGENT = VECTOR_3,
        BITANGENT = VECTOR_3,
        COLOR_RGB = VECTOR_3,
        COLOR_RGBA = VECTOR_4,
        UV = VECTOR_2,
        TEXTURE_COORDINATE = UV
    };

    constexpr uint32 GetVertexAttributeTypeSize(const VertexAttributeType vertexAttributeType)
    {
        switch (vertexAttributeType)
        {
            case VertexAttributeType::FLOAT: return FLOAT_SIZE * 1;
            case VertexAttributeType::VECTOR_2: return FLOAT_SIZE * 2;
            case VertexAttributeType::VECTOR_3: return FLOAT_SIZE * 3;
            case VertexAttributeType::VECTOR_4: return FLOAT_SIZE * 4;
        }
    }

    enum class SpecializationConstantType
    {
        BOOL = BOOL_SIZE,
        INT = INT_SIZE,
        INT32 = INT,
        UINT = UINT_SIZE,
        UINT32 = UINT,
        FLOAT = FLOAT_SIZE,
        DOUBLE = DOUBLE_SIZE
    };

    enum class FrontFace
    {
        CLOCKWISE = VK_FRONT_FACE_CLOCKWISE,
        COUNTER_CLOCKWISE = VK_FRONT_FACE_COUNTER_CLOCKWISE
    };

    enum class CullMode
    {
        NONE = VK_CULL_MODE_NONE,
        FRONT = VK_CULL_MODE_FRONT_BIT,
        BACK = VK_CULL_MODE_BACK_BIT,
        FRONT_AND_BACK = VK_CULL_MODE_FRONT_AND_BACK
    };

    enum class ShadingType
    {
        FILL = VK_POLYGON_MODE_FILL,
        WIREFRAME = VK_POLYGON_MODE_LINE
    };

    enum class ShaderType
    {
        NONE = 0,
        VERTEX = VK_SHADER_STAGE_VERTEX_BIT,
        FRAGMENT = VK_SHADER_STAGE_FRAGMENT_BIT,
        TESSELATION_CONTROL = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
        TESSELATION_EVALUATION = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
        GEOMETRY = VK_SHADER_STAGE_GEOMETRY_BIT,
        COMPUTE = VK_SHADER_STAGE_COMPUTE_BIT,
        ALL = VK_SHADER_STAGE_ALL
    };

    DEFINE_ENUM_FLAG_OPERATORS(ShaderType)

    enum class DescriptorType
    {
        NONE = 0,
        SAMPLER = VK_DESCRIPTOR_TYPE_SAMPLER,
        COMBINED_IMAGE_SAMPLER = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        SAMPLED_IMAGE = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        STORAGE_IMAGE = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        UNIFORM_TEXEL_BUFFER = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
        STORAGE_TEXEL_BUFFER = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
        UNIFORM_BUFFER = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        STORAGE_BUFFER = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        UNIFORM_BUFFER_DYNAMIC = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        STORAGE_BUFFER_DYNAMIC = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
        INPUT_ATTACHMENT = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
        INLINE_UNIFORM_BLOCK_EXT = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT,
        ACCELERATION_STRUCTURE_KHR = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
        ACCELERATION_STRUCTURE_NV = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV,
        MUTABLE_VALVE = VK_DESCRIPTOR_TYPE_MUTABLE_VALVE
    };

    enum class DescriptorSetLayoutFlag
    {
        NONE = 0,
        UPDATE_AFTER_BIND_POOL = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
        PUSH_DESCRIPTOR_KHR = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR,
        DESCRIPTOR_BUFFER_EXT = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT,
        EMBEDDED_IMMUTABLE_SAMPLERS_EXT = VK_DESCRIPTOR_SET_LAYOUT_CREATE_EMBEDDED_IMMUTABLE_SAMPLERS_BIT_EXT,
        HOST_ONLY_POOL_EXT = VK_DESCRIPTOR_SET_LAYOUT_CREATE_HOST_ONLY_POOL_BIT_EXT,
        UPDATE_AFTER_BIND_POOL_EXT = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT,
        HOST_ONLY_POOL = VK_DESCRIPTOR_SET_LAYOUT_CREATE_HOST_ONLY_POOL_BIT_VALVE
    };

    DEFINE_ENUM_FLAG_OPERATORS(DescriptorSetLayoutFlag)

    enum class PipelineCopyOp
    {
        PUSH_CONSTANTS = 0,
        UNIFORM_BUFFERS = 1,
        STORAGE_BUFFERS = 2
    };

    DEFINE_ENUM_FLAG_OPERATORS(PipelineCopyOp)

    enum class ResolveMode
    {
        NONE = VK_RESOLVE_MODE_NONE,
        SAMPLE_ZERO = VK_RESOLVE_MODE_SAMPLE_ZERO_BIT,
        AVERAGE = VK_RESOLVE_MODE_AVERAGE_BIT,
        MIN = VK_RESOLVE_MODE_MIN_BIT,
        MAX = VK_RESOLVE_MODE_MAX_BIT
    };

    enum class CommandBufferUsage
    {
        NONE = 0,
        ONE_TIME_SUBMIT = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        RENDER_PASS_CONTINUE = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
        SIMULTANEOUS_USE = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
    };

}