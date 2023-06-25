//
// Created by Nikolay Kanchevski on 15.12.22.
//

#pragma once

#include "../../Internal/Definitions.h"
#include "../../Internal/Macros.h"

namespace Sierra::Core::Rendering::Vulkan
{

    enum class ImageFormat
    {
        UNDEFINED = VK_FORMAT_UNDEFINED,
        R4G4_UNORM_PACK8 = VK_FORMAT_R4G4_UNORM_PACK8,
        R4G4B4A4_UNORM_PACK16 = VK_FORMAT_R4G4B4A4_UNORM_PACK16,
        B4G4R4A4_UNORM_PACK16 = VK_FORMAT_B4G4R4A4_UNORM_PACK16,
        R5G6B5_UNORM_PACK16 = VK_FORMAT_R5G6B5_UNORM_PACK16,
        B5G6R5_UNORM_PACK16 = VK_FORMAT_B5G6R5_UNORM_PACK16,
        R5G5B5A1_UNORM_PACK16 = VK_FORMAT_R5G5B5A1_UNORM_PACK16,
        B5G5R5A1_UNORM_PACK16 = VK_FORMAT_B5G5R5A1_UNORM_PACK16,
        A1R5G5B5_UNORM_PACK16 = VK_FORMAT_A1R5G5B5_UNORM_PACK16,
        R8_UNORM = VK_FORMAT_R8_UNORM,
        R8_SNORM = VK_FORMAT_R8_SNORM,
        R8_USCALED = VK_FORMAT_R8_USCALED,
        R8_SSCALED = VK_FORMAT_R8_SSCALED,
        R8_UINT = VK_FORMAT_R8_UINT,
        R8_SINT = VK_FORMAT_R8_SINT,
        R8_SRGB = VK_FORMAT_R8_SRGB,
        R8G8_UNORM = VK_FORMAT_R8G8_UNORM,
        R8G8_SNORM = VK_FORMAT_R8G8_SNORM,
        R8G8_USCALED = VK_FORMAT_R8G8_USCALED,
        R8G8_SSCALED = VK_FORMAT_R8G8_SSCALED,
        R8G8_UINT = VK_FORMAT_R8G8_UINT,
        R8G8_SINT = VK_FORMAT_R8G8_SINT,
        R8G8_SRGB = VK_FORMAT_R8G8_SRGB,
        R8G8B8_UNORM = VK_FORMAT_R8G8B8_UNORM,
        R8G8B8_SNORM = VK_FORMAT_R8G8B8_SNORM,
        R8G8B8_USCALED = VK_FORMAT_R8G8B8_USCALED,
        R8G8B8_SSCALED = VK_FORMAT_R8G8B8_SSCALED,
        R8G8B8_UINT = VK_FORMAT_R8G8B8_UINT,
        R8G8B8_SINT = VK_FORMAT_R8G8B8_SINT,
        R8G8B8_SRGB = VK_FORMAT_R8G8B8_SRGB,
        B8G8R8_UNORM = VK_FORMAT_B8G8R8_UNORM,
        B8G8R8_SNORM = VK_FORMAT_B8G8R8_SNORM,
        B8G8R8_USCALED = VK_FORMAT_B8G8R8_USCALED,
        B8G8R8_SSCALED = VK_FORMAT_B8G8R8_SSCALED,
        B8G8R8_UINT = VK_FORMAT_B8G8R8_UINT,
        B8G8R8_SINT = VK_FORMAT_B8G8R8_SINT,
        B8G8R8_SRGB = VK_FORMAT_B8G8R8_SRGB,
        R8G8B8A8_UNORM = VK_FORMAT_R8G8B8A8_UNORM,
        R8G8B8A8_SNORM = VK_FORMAT_R8G8B8A8_SNORM,
        R8G8B8A8_USCALED = VK_FORMAT_R8G8B8A8_USCALED,
        R8G8B8A8_SSCALED = VK_FORMAT_R8G8B8A8_SSCALED,
        R8G8B8A8_UINT = VK_FORMAT_R8G8B8A8_UINT,
        R8G8B8A8_SINT = VK_FORMAT_R8G8B8A8_SINT,
        R8G8B8A8_SRGB = VK_FORMAT_R8G8B8A8_SRGB,
        B8G8R8A8_UNORM = VK_FORMAT_B8G8R8A8_UNORM,
        B8G8R8A8_SNORM = VK_FORMAT_B8G8R8A8_SNORM,
        B8G8R8A8_USCALED = VK_FORMAT_B8G8R8A8_USCALED,
        B8G8R8A8_SSCALED = VK_FORMAT_B8G8R8A8_SSCALED,
        B8G8R8A8_UINT = VK_FORMAT_B8G8R8A8_UINT,
        B8G8R8A8_SINT = VK_FORMAT_B8G8R8A8_SINT,
        B8G8R8A8_SRGB = VK_FORMAT_B8G8R8A8_SRGB,
        A8B8G8R8_UNORM_PACK32 = VK_FORMAT_A8B8G8R8_UNORM_PACK32,
        A8B8G8R8_SNORM_PACK32 = VK_FORMAT_A8B8G8R8_SNORM_PACK32,
        A8B8G8R8_USCALED_PACK32 = VK_FORMAT_A8B8G8R8_USCALED_PACK32,
        A8B8G8R8_SSCALED_PACK32 = VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
        A8B8G8R8_UINT_PACK32 = VK_FORMAT_A8B8G8R8_UINT_PACK32,
        A8B8G8R8_SINT_PACK32 = VK_FORMAT_A8B8G8R8_SINT_PACK32,
        A8B8G8R8_SRGB_PACK32 = VK_FORMAT_A8B8G8R8_SRGB_PACK32,
        A2R10G10B10_UNORM_PACK32 = VK_FORMAT_A2R10G10B10_UNORM_PACK32,
        A2R10G10B10_SNORM_PACK32 = VK_FORMAT_A2R10G10B10_SNORM_PACK32,
        A2R10G10B10_USCALED_PACK32 = VK_FORMAT_A2R10G10B10_USCALED_PACK32,
        A2R10G10B10_SSCALED_PACK32 = VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
        A2R10G10B10_UINT_PACK32 = VK_FORMAT_A2R10G10B10_UINT_PACK32,
        A2R10G10B10_SINT_PACK32 = VK_FORMAT_A2R10G10B10_SINT_PACK32,
        A2B10G10R10_UNORM_PACK32 = VK_FORMAT_A2B10G10R10_UNORM_PACK32,
        A2B10G10R10_SNORM_PACK32 = VK_FORMAT_A2B10G10R10_SNORM_PACK32,
        A2B10G10R10_USCALED_PACK32 = VK_FORMAT_A2B10G10R10_USCALED_PACK32,
        A2B10G10R10_SSCALED_PACK32 = VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
        A2B10G10R10_UINT_PACK32 = VK_FORMAT_A2B10G10R10_UINT_PACK32,
        A2B10G10R10_SINT_PACK32 = VK_FORMAT_A2B10G10R10_SINT_PACK32,
        R16_UNORM = VK_FORMAT_R16_UNORM,
        R16_SNORM = VK_FORMAT_R16_SNORM,
        R16_USCALED = VK_FORMAT_R16_USCALED,
        R16_SSCALED = VK_FORMAT_R16_SSCALED,
        R16_UINT = VK_FORMAT_R16_UINT,
        R16_SINT = VK_FORMAT_R16_SINT,
        R16_SFLOAT = VK_FORMAT_R16_SFLOAT,
        R16G16_UNORM = VK_FORMAT_R16G16_UNORM,
        R16G16_SNORM = VK_FORMAT_R16G16_SNORM,
        R16G16_USCALED = VK_FORMAT_R16G16_USCALED,
        R16G16_SSCALED = VK_FORMAT_R16G16_SSCALED,
        R16G16_UINT = VK_FORMAT_R16G16_UINT,
        R16G16_SINT = VK_FORMAT_R16G16_SINT,
        R16G16_SFLOAT = VK_FORMAT_R16G16_SFLOAT,
        R16G16B16_UNORM = VK_FORMAT_R16G16B16_UNORM,
        R16G16B16_SNORM = VK_FORMAT_R16G16B16_SNORM,
        R16G16B16_USCALED = VK_FORMAT_R16G16B16_USCALED,
        R16G16B16_SSCALED = VK_FORMAT_R16G16B16_SSCALED,
        R16G16B16_UINT = VK_FORMAT_R16G16B16_UINT,
        R16G16B16_SINT = VK_FORMAT_R16G16B16_SINT,
        R16G16B16_SFLOAT = VK_FORMAT_R16G16B16_SFLOAT,
        R16G16B16A16_UNORM = VK_FORMAT_R16G16B16A16_UNORM,
        R16G16B16A16_SNORM = VK_FORMAT_R16G16B16A16_SNORM,
        R16G16B16A16_USCALED = VK_FORMAT_R16G16B16A16_USCALED,
        R16G16B16A16_SSCALED = VK_FORMAT_R16G16B16A16_SSCALED,
        R16G16B16A16_UINT = VK_FORMAT_R16G16B16A16_UINT,
        R16G16B16A16_SINT = VK_FORMAT_R16G16B16A16_SINT,
        R16G16B16A16_SFLOAT = VK_FORMAT_R16G16B16A16_SFLOAT,
        R32_UINT = VK_FORMAT_R32_UINT,
        R32_SINT = VK_FORMAT_R32_SINT,
        R32_SFLOAT = VK_FORMAT_R32_SFLOAT,
        R32G32_UINT = VK_FORMAT_R32G32_UINT,
        R32G32_SINT = VK_FORMAT_R32G32_SINT,
        R32G32_SFLOAT = VK_FORMAT_R32G32_SFLOAT,
        R32G32B32_UINT = VK_FORMAT_R32G32B32_UINT,
        R32G32B32_SINT = VK_FORMAT_R32G32B32_SINT,
        R32G32B32_SFLOAT = VK_FORMAT_R32G32B32_SFLOAT,
        R32G32B32A32_UINT = VK_FORMAT_R32G32B32A32_UINT,
        R32G32B32A32_SINT = VK_FORMAT_R32G32B32A32_SINT,
        R32G32B32A32_SFLOAT = VK_FORMAT_R32G32B32A32_SFLOAT,
        R64_UINT = VK_FORMAT_R64_UINT,
        R64_SINT = VK_FORMAT_R64_SINT,
        R64_SFLOAT = VK_FORMAT_R64_SFLOAT,
        R64G64_UINT = VK_FORMAT_R64G64_UINT,
        R64G64_SINT = VK_FORMAT_R64G64_SINT,
        R64G64_SFLOAT = VK_FORMAT_R64G64_SFLOAT,
        R64G64B64_UINT = VK_FORMAT_R64G64B64_UINT,
        R64G64B64_SINT = VK_FORMAT_R64G64B64_SINT,
        R64G64B64_SFLOAT = VK_FORMAT_R64G64B64_SFLOAT,
        R64G64B64A64_UINT = VK_FORMAT_R64G64B64A64_UINT,
        R64G64B64A64_SINT = VK_FORMAT_R64G64B64A64_SINT,
        R64G64B64A64_SFLOAT = VK_FORMAT_R64G64B64A64_SFLOAT,
        B10G11R11_UFLOAT_PACK32 = VK_FORMAT_B10G11R11_UFLOAT_PACK32,
        E5B9G9R9_UFLOAT_PACK32 = VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
        D16_UNORM = VK_FORMAT_D16_UNORM,
        X8_D24_UNORM_PACK32 = VK_FORMAT_X8_D24_UNORM_PACK32,
        D32_SFLOAT = VK_FORMAT_D32_SFLOAT,
        S8_UINT = VK_FORMAT_S8_UINT,
        D16_UNORM_S8_UINT = VK_FORMAT_D16_UNORM_S8_UINT,
        D24_UNORM_S8_UINT = VK_FORMAT_D24_UNORM_S8_UINT,
        D32_SFLOAT_S8_UINT = VK_FORMAT_D32_SFLOAT_S8_UINT,
        BC1_RGB_UNORM_BLOCK = VK_FORMAT_BC1_RGB_UNORM_BLOCK,
        BC1_RGB_SRGB_BLOCK = VK_FORMAT_BC1_RGB_SRGB_BLOCK,
        BC1_RGBA_UNORM_BLOCK = VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
        BC1_RGBA_SRGB_BLOCK = VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
        BC2_UNORM_BLOCK = VK_FORMAT_BC2_UNORM_BLOCK,
        BC2_SRGB_BLOCK = VK_FORMAT_BC2_SRGB_BLOCK,
        BC3_UNORM_BLOCK = VK_FORMAT_BC3_UNORM_BLOCK,
        BC3_SRGB_BLOCK = VK_FORMAT_BC3_SRGB_BLOCK,
        BC4_UNORM_BLOCK = VK_FORMAT_BC4_UNORM_BLOCK,
        BC4_SNORM_BLOCK = VK_FORMAT_BC4_SNORM_BLOCK,
        BC5_UNORM_BLOCK = VK_FORMAT_BC5_UNORM_BLOCK,
        BC5_SNORM_BLOCK = VK_FORMAT_BC5_SNORM_BLOCK,
        BC6H_UFLOAT_BLOCK = VK_FORMAT_BC6H_UFLOAT_BLOCK,
        BC6H_SFLOAT_BLOCK = VK_FORMAT_BC6H_SFLOAT_BLOCK,
        BC7_UNORM_BLOCK = VK_FORMAT_BC7_UNORM_BLOCK,
        BC7_SRGB_BLOCK = VK_FORMAT_BC7_SRGB_BLOCK,
        ETC2_R8G8B8_UNORM_BLOCK = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
        ETC2_R8G8B8_SRGB_BLOCK = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
        ETC2_R8G8B8A1_UNORM_BLOCK = VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
        ETC2_R8G8B8A1_SRGB_BLOCK = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
        ETC2_R8G8B8A8_UNORM_BLOCK = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
        ETC2_R8G8B8A8_SRGB_BLOCK = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
        EAC_R11_UNORM_BLOCK = VK_FORMAT_EAC_R11_UNORM_BLOCK,
        EAC_R11_SNORM_BLOCK = VK_FORMAT_EAC_R11_SNORM_BLOCK,
        EAC_R11G11_UNORM_BLOCK = VK_FORMAT_EAC_R11G11_UNORM_BLOCK,
        EAC_R11G11_SNORM_BLOCK = VK_FORMAT_EAC_R11G11_SNORM_BLOCK,
        ASTC_4x4_UNORM_BLOCK = VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
        ASTC_4x4_SRGB_BLOCK = VK_FORMAT_ASTC_4x4_SRGB_BLOCK,
        ASTC_5x4_UNORM_BLOCK = VK_FORMAT_ASTC_5x4_UNORM_BLOCK,
        ASTC_5x4_SRGB_BLOCK = VK_FORMAT_ASTC_5x4_SRGB_BLOCK,
        ASTC_5x5_UNORM_BLOCK = VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
        ASTC_5x5_SRGB_BLOCK = VK_FORMAT_ASTC_5x5_SRGB_BLOCK,
        ASTC_6x5_UNORM_BLOCK = VK_FORMAT_ASTC_6x5_UNORM_BLOCK,
        ASTC_6x5_SRGB_BLOCK = VK_FORMAT_ASTC_6x5_SRGB_BLOCK,
        ASTC_6x6_UNORM_BLOCK = VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
        ASTC_6x6_SRGB_BLOCK = VK_FORMAT_ASTC_6x6_SRGB_BLOCK,
        ASTC_8x5_UNORM_BLOCK = VK_FORMAT_ASTC_8x5_UNORM_BLOCK,
        ASTC_8x5_SRGB_BLOCK = VK_FORMAT_ASTC_8x5_SRGB_BLOCK,
        ASTC_8x6_UNORM_BLOCK = VK_FORMAT_ASTC_8x6_UNORM_BLOCK,
        ASTC_8x6_SRGB_BLOCK = VK_FORMAT_ASTC_8x6_SRGB_BLOCK,
        ASTC_8x8_UNORM_BLOCK = VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
        ASTC_8x8_SRGB_BLOCK = VK_FORMAT_ASTC_8x8_SRGB_BLOCK,
        ASTC_10x5_UNORM_BLOCK = VK_FORMAT_ASTC_10x5_UNORM_BLOCK,
        ASTC_10x5_SRGB_BLOCK = VK_FORMAT_ASTC_10x5_SRGB_BLOCK,
        ASTC_10x6_UNORM_BLOCK = VK_FORMAT_ASTC_10x6_UNORM_BLOCK,
        ASTC_10x6_SRGB_BLOCK = VK_FORMAT_ASTC_10x6_SRGB_BLOCK,
        ASTC_10x8_UNORM_BLOCK = VK_FORMAT_ASTC_10x8_UNORM_BLOCK,
        ASTC_10x8_SRGB_BLOCK = VK_FORMAT_ASTC_10x8_SRGB_BLOCK,
        ASTC_10x10_UNORM_BLOCK = VK_FORMAT_ASTC_10x10_UNORM_BLOCK,
        ASTC_10x10_SRGB_BLOCK = VK_FORMAT_ASTC_10x10_SRGB_BLOCK,
        ASTC_12x10_UNORM_BLOCK = VK_FORMAT_ASTC_12x10_UNORM_BLOCK,
        ASTC_12x10_SRGB_BLOCK = VK_FORMAT_ASTC_12x10_SRGB_BLOCK,
        ASTC_12x12_UNORM_BLOCK = VK_FORMAT_ASTC_12x12_UNORM_BLOCK,
        ASTC_12x12_SRGB_BLOCK = VK_FORMAT_ASTC_12x12_SRGB_BLOCK,
        G8B8G8R8_422_UNORM = VK_FORMAT_G8B8G8R8_422_UNORM,
        B8G8R8G8_422_UNORM = VK_FORMAT_B8G8R8G8_422_UNORM,
        G8_B8_R8_3PLANE_420_UNORM = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
        G8_B8R8_2PLANE_420_UNORM = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,
        G8_B8_R8_3PLANE_422_UNORM = VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM,
        G8_B8R8_2PLANE_422_UNORM = VK_FORMAT_G8_B8R8_2PLANE_422_UNORM,
        G8_B8_R8_3PLANE_444_UNORM = VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM,
        R10X6_UNORM_PACK16 = VK_FORMAT_R10X6_UNORM_PACK16,
        R10X6G10X6_UNORM_2PACK16 = VK_FORMAT_R10X6G10X6_UNORM_2PACK16,
        R10X6G10X6B10X6A10X6_UNORM_4PACK16 = VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16,
        G10X6B10X6G10X6R10X6_422_UNORM_4PACK16 = VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
        B10X6G10X6R10X6G10X6_422_UNORM_4PACK16 = VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
        G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
        G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
        G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
        G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
        G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
        R12X4_UNORM_PACK16 = VK_FORMAT_R12X4_UNORM_PACK16,
        R12X4G12X4_UNORM_2PACK16 = VK_FORMAT_R12X4G12X4_UNORM_2PACK16,
        R12X4G12X4B12X4A12X4_UNORM_4PACK16 = VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16,
        G12X4B12X4G12X4R12X4_422_UNORM_4PACK16 = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
        B12X4G12X4R12X4G12X4_422_UNORM_4PACK16 = VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
        G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
        G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
        G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
        G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
        G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
        G16B16G16R16_422_UNORM = VK_FORMAT_G16B16G16R16_422_UNORM,
        B16G16R16G16_422_UNORM = VK_FORMAT_B16G16R16G16_422_UNORM,
        G16_B16_R16_3PLANE_420_UNORM = VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM,
        G16_B16R16_2PLANE_420_UNORM = VK_FORMAT_G16_B16R16_2PLANE_420_UNORM,
        G16_B16_R16_3PLANE_422_UNORM = VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM,
        G16_B16R16_2PLANE_422_UNORM = VK_FORMAT_G16_B16R16_2PLANE_422_UNORM,
        G16_B16_R16_3PLANE_444_UNORM = VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM,
        PVRTC1_2BPP_UNORM_BLOCK_IMG = VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG,
        PVRTC1_4BPP_UNORM_BLOCK_IMG = VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG,
        PVRTC2_2BPP_UNORM_BLOCK_IMG = VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG,
        PVRTC2_4BPP_UNORM_BLOCK_IMG = VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG,
        PVRTC1_2BPP_SRGB_BLOCK_IMG = VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG,
        PVRTC1_4BPP_SRGB_BLOCK_IMG = VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG,
        PVRTC2_2BPP_SRGB_BLOCK_IMG = VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG,
        PVRTC2_4BPP_SRGB_BLOCK_IMG = VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG,
        ASTC_4x4_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT,
        ASTC_5x4_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT,
        ASTC_5x5_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT,
        ASTC_6x5_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT,
        ASTC_6x6_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT,
        ASTC_8x5_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT,
        ASTC_8x6_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT,
        ASTC_8x8_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT,
        ASTC_10x5_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT,
        ASTC_10x6_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT,
        ASTC_10x8_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT,
        ASTC_10x10_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT,
        ASTC_12x10_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT,
        ASTC_12x12_SFLOAT_BLOCK_EXT = VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT,
        G8B8G8R8_422_UNORM_KHR = VK_FORMAT_G8B8G8R8_422_UNORM_KHR,
        B8G8R8G8_422_UNORM_KHR = VK_FORMAT_B8G8R8G8_422_UNORM_KHR,
        G8_B8_R8_3PLANE_420_UNORM_KHR = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM_KHR,
        G8_B8R8_2PLANE_420_UNORM_KHR = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM_KHR,
        G8_B8_R8_3PLANE_422_UNORM_KHR = VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM_KHR,
        G8_B8R8_2PLANE_422_UNORM_KHR = VK_FORMAT_G8_B8R8_2PLANE_422_UNORM_KHR,
        G8_B8_R8_3PLANE_444_UNORM_KHR = VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM_KHR,
        R10X6_UNORM_PACK16_KHR = VK_FORMAT_R10X6_UNORM_PACK16_KHR,
        R10X6G10X6_UNORM_2PACK16_KHR = VK_FORMAT_R10X6G10X6_UNORM_2PACK16_KHR,
        R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR = VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR,
        G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR = VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR,
        B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR = VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR,
        G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR,
        G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR,
        G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR,
        G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR,
        G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR,
        R12X4_UNORM_PACK16_KHR = VK_FORMAT_R12X4_UNORM_PACK16_KHR,
        R12X4G12X4_UNORM_2PACK16_KHR = VK_FORMAT_R12X4G12X4_UNORM_2PACK16_KHR,
        R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR = VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR,
        G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR,
        B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR = VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR,
        G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR,
        G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR,
        G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR,
        G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR,
        G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR,
        G16B16G16R16_422_UNORM_KHR = VK_FORMAT_G16B16G16R16_422_UNORM_KHR,
        B16G16R16G16_422_UNORM_KHR = VK_FORMAT_B16G16R16G16_422_UNORM_KHR,
        G16_B16_R16_3PLANE_420_UNORM_KHR = VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM_KHR,
        G16_B16R16_2PLANE_420_UNORM_KHR = VK_FORMAT_G16_B16R16_2PLANE_420_UNORM_KHR,
        G16_B16_R16_3PLANE_422_UNORM_KHR = VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM_KHR,
        G16_B16R16_2PLANE_422_UNORM_KHR = VK_FORMAT_G16_B16R16_2PLANE_422_UNORM_KHR,
        G16_B16_R16_3PLANE_444_UNORM_KHR = VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM_KHR,
        G8_B8R8_2PLANE_444_UNORM_EXT = VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT,
        G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT,
        G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT,
        G16_B16R16_2PLANE_444_UNORM_EXT = VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT,
        A4R4G4B4_UNORM_PACK16_EXT = VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT,
        A4B4G4R4_UNORM_PACK16_EXT = VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT,
    };

    constexpr uint GetChannelCountForImageFormat(const ImageFormat format)
    {
        switch (format) 
        {
            case ImageFormat::UNDEFINED:                                            return 0;
            case ImageFormat::R4G4_UNORM_PACK8:                                     return 2;
            case ImageFormat::R4G4B4A4_UNORM_PACK16:                                return 4;
            case ImageFormat::B4G4R4A4_UNORM_PACK16:                                return 4;
            case ImageFormat::R5G6B5_UNORM_PACK16:                                  return 3;
            case ImageFormat::B5G6R5_UNORM_PACK16:                                  return 3;
            case ImageFormat::R5G5B5A1_UNORM_PACK16:                                return 4;
            case ImageFormat::B5G5R5A1_UNORM_PACK16:                                return 4;
            case ImageFormat::A1R5G5B5_UNORM_PACK16:                                return 4;
            case ImageFormat::R8_UNORM:                                             return 1;
            case ImageFormat::R8_SNORM:                                             return 1;
            case ImageFormat::R8_USCALED:                                           return 1;
            case ImageFormat::R8_SSCALED:                                           return 1;
            case ImageFormat::R8_UINT:                                              return 1;
            case ImageFormat::R8_SINT:                                              return 1;
            case ImageFormat::R8_SRGB:                                              return 1;
            case ImageFormat::R8G8_UNORM:                                           return 2;
            case ImageFormat::R8G8_SNORM:                                           return 2;
            case ImageFormat::R8G8_USCALED:                                         return 2;
            case ImageFormat::R8G8_SSCALED:                                         return 2;
            case ImageFormat::R8G8_UINT:                                            return 2;
            case ImageFormat::R8G8_SINT:                                            return 2;
            case ImageFormat::R8G8_SRGB:                                            return 2;
            case ImageFormat::R8G8B8_UNORM:                                         return 3;
            case ImageFormat::R8G8B8_SNORM:                                         return 3;
            case ImageFormat::R8G8B8_USCALED:                                       return 3;
            case ImageFormat::R8G8B8_SSCALED:                                       return 3;
            case ImageFormat::R8G8B8_UINT:                                          return 3;
            case ImageFormat::R8G8B8_SINT:                                          return 3;
            case ImageFormat::R8G8B8_SRGB:                                          return 3;
            case ImageFormat::B8G8R8_UNORM:                                         return 3;
            case ImageFormat::B8G8R8_SNORM:                                         return 3;
            case ImageFormat::B8G8R8_USCALED:                                       return 3;
            case ImageFormat::B8G8R8_SSCALED:                                       return 3;
            case ImageFormat::B8G8R8_UINT:                                          return 3;
            case ImageFormat::B8G8R8_SINT:                                          return 3;
            case ImageFormat::B8G8R8_SRGB:                                          return 3;
            case ImageFormat::R8G8B8A8_UNORM:                                       return 4;
            case ImageFormat::R8G8B8A8_SNORM:                                       return 4;
            case ImageFormat::R8G8B8A8_USCALED:                                     return 4;
            case ImageFormat::R8G8B8A8_SSCALED:                                     return 4;
            case ImageFormat::R8G8B8A8_UINT:                                        return 4;
            case ImageFormat::R8G8B8A8_SINT:                                        return 4;
            case ImageFormat::R8G8B8A8_SRGB:                                        return 4;
            case ImageFormat::B8G8R8A8_UNORM:                                       return 4;
            case ImageFormat::B8G8R8A8_SNORM:                                       return 4;
            case ImageFormat::B8G8R8A8_USCALED:                                     return 4;
            case ImageFormat::B8G8R8A8_SSCALED:                                     return 4;
            case ImageFormat::B8G8R8A8_UINT:                                        return 4;
            case ImageFormat::B8G8R8A8_SINT:                                        return 4;
            case ImageFormat::B8G8R8A8_SRGB:                                        return 4;
            case ImageFormat::A8B8G8R8_UNORM_PACK32:                                return 4;
            case ImageFormat::A8B8G8R8_SNORM_PACK32:                                return 4;
            case ImageFormat::A8B8G8R8_USCALED_PACK32:                              return 4;
            case ImageFormat::A8B8G8R8_SSCALED_PACK32:                              return 4;
            case ImageFormat::A8B8G8R8_UINT_PACK32:                                 return 4;
            case ImageFormat::A8B8G8R8_SINT_PACK32:                                 return 4;
            case ImageFormat::A8B8G8R8_SRGB_PACK32:                                 return 4;
            case ImageFormat::A2R10G10B10_UNORM_PACK32:                             return 4;
            case ImageFormat::A2R10G10B10_SNORM_PACK32:                             return 4;
            case ImageFormat::A2R10G10B10_USCALED_PACK32:                           return 4;
            case ImageFormat::A2R10G10B10_SSCALED_PACK32:                           return 4;
            case ImageFormat::A2R10G10B10_UINT_PACK32:                              return 4;
            case ImageFormat::A2R10G10B10_SINT_PACK32:                              return 4;
            case ImageFormat::A2B10G10R10_UNORM_PACK32:                             return 4;
            case ImageFormat::A2B10G10R10_SNORM_PACK32:                             return 4;
            case ImageFormat::A2B10G10R10_USCALED_PACK32:                           return 4;
            case ImageFormat::A2B10G10R10_SSCALED_PACK32:                           return 4;
            case ImageFormat::A2B10G10R10_UINT_PACK32:                              return 4;
            case ImageFormat::A2B10G10R10_SINT_PACK32:                              return 4;
            case ImageFormat::R16_UNORM:                                            return 1;
            case ImageFormat::R16_SNORM:                                            return 1;
            case ImageFormat::R16_USCALED:                                          return 1;
            case ImageFormat::R16_SSCALED:                                          return 1;
            case ImageFormat::R16_UINT:                                             return 1;
            case ImageFormat::R16_SINT:                                             return 1;
            case ImageFormat::R16_SFLOAT:                                           return 1;
            case ImageFormat::R16G16_UNORM:                                         return 2;
            case ImageFormat::R16G16_SNORM:                                         return 2;
            case ImageFormat::R16G16_USCALED:                                       return 2;
            case ImageFormat::R16G16_SSCALED:                                       return 2;
            case ImageFormat::R16G16_UINT:                                          return 2;
            case ImageFormat::R16G16_SINT:                                          return 2;
            case ImageFormat::R16G16_SFLOAT:                                        return 2;
            case ImageFormat::R16G16B16_UNORM:                                      return 3;
            case ImageFormat::R16G16B16_SNORM:                                      return 3;
            case ImageFormat::R16G16B16_USCALED:                                    return 3;
            case ImageFormat::R16G16B16_SSCALED:                                    return 3;
            case ImageFormat::R16G16B16_UINT:                                       return 3;
            case ImageFormat::R16G16B16_SINT:                                       return 3;
            case ImageFormat::R16G16B16_SFLOAT:                                     return 3;
            case ImageFormat::R16G16B16A16_UNORM:                                   return 4;
            case ImageFormat::R16G16B16A16_SNORM:                                   return 4;
            case ImageFormat::R16G16B16A16_USCALED:                                 return 4;
            case ImageFormat::R16G16B16A16_SSCALED:                                 return 4;
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
            case ImageFormat::B10G11R11_UFLOAT_PACK32:                              return 3;
            case ImageFormat::E5B9G9R9_UFLOAT_PACK32:                               return 4;
            case ImageFormat::D16_UNORM:                                            return 1;
            case ImageFormat::X8_D24_UNORM_PACK32:                                  return 2;
            case ImageFormat::D32_SFLOAT:                                           return 1;
            case ImageFormat::S8_UINT:                                              return 1;
            case ImageFormat::D16_UNORM_S8_UINT:                                    return 2;
            case ImageFormat::D24_UNORM_S8_UINT:                                    return 2;
            case ImageFormat::D32_SFLOAT_S8_UINT:                                   return 2;
            case ImageFormat::BC1_RGB_UNORM_BLOCK:                                  return -1;
            case ImageFormat::BC1_RGB_SRGB_BLOCK:                                   return -1;
            case ImageFormat::BC1_RGBA_UNORM_BLOCK:                                 return -1;
            case ImageFormat::BC1_RGBA_SRGB_BLOCK:                                  return -1;
            case ImageFormat::BC2_UNORM_BLOCK:                                      return -1;
            case ImageFormat::BC2_SRGB_BLOCK:                                       return -1;
            case ImageFormat::BC3_UNORM_BLOCK:                                      return -1;
            case ImageFormat::BC3_SRGB_BLOCK:                                       return -1;
            case ImageFormat::BC4_UNORM_BLOCK:                                      return -1;
            case ImageFormat::BC4_SNORM_BLOCK:                                      return -1;
            case ImageFormat::BC5_UNORM_BLOCK:                                      return -1;
            case ImageFormat::BC5_SNORM_BLOCK:                                      return -1;
            case ImageFormat::BC6H_UFLOAT_BLOCK:                                    return -1;
            case ImageFormat::BC6H_SFLOAT_BLOCK:                                    return -1;
            case ImageFormat::BC7_UNORM_BLOCK:                                      return -1;
            case ImageFormat::BC7_SRGB_BLOCK:                                       return -1;
            case ImageFormat::ETC2_R8G8B8_UNORM_BLOCK:                              return -1;
            case ImageFormat::ETC2_R8G8B8_SRGB_BLOCK:                               return -1;
            case ImageFormat::ETC2_R8G8B8A1_UNORM_BLOCK:                            return -1;
            case ImageFormat::ETC2_R8G8B8A1_SRGB_BLOCK:                             return -1;
            case ImageFormat::ETC2_R8G8B8A8_UNORM_BLOCK:                            return -1;
            case ImageFormat::ETC2_R8G8B8A8_SRGB_BLOCK:                             return -1;
            case ImageFormat::EAC_R11_UNORM_BLOCK:                                  return -1;
            case ImageFormat::EAC_R11_SNORM_BLOCK:                                  return -1;
            case ImageFormat::EAC_R11G11_UNORM_BLOCK:                               return -1;
            case ImageFormat::EAC_R11G11_SNORM_BLOCK:                               return -1;
            case ImageFormat::ASTC_4x4_UNORM_BLOCK:                                 return -1;
            case ImageFormat::ASTC_4x4_SRGB_BLOCK:                                  return -1;
            case ImageFormat::ASTC_5x4_UNORM_BLOCK:                                 return -1;
            case ImageFormat::ASTC_5x4_SRGB_BLOCK:                                  return -1;
            case ImageFormat::ASTC_5x5_UNORM_BLOCK:                                 return -1;
            case ImageFormat::ASTC_5x5_SRGB_BLOCK:                                  return -1;
            case ImageFormat::ASTC_6x5_UNORM_BLOCK:                                 return -1;
            case ImageFormat::ASTC_6x5_SRGB_BLOCK:                                  return -1;
            case ImageFormat::ASTC_6x6_UNORM_BLOCK:                                 return -1;
            case ImageFormat::ASTC_6x6_SRGB_BLOCK:                                  return -1;
            case ImageFormat::ASTC_8x5_UNORM_BLOCK:                                 return -1;
            case ImageFormat::ASTC_8x5_SRGB_BLOCK:                                  return -1;
            case ImageFormat::ASTC_8x6_UNORM_BLOCK:                                 return -1;
            case ImageFormat::ASTC_8x6_SRGB_BLOCK:                                  return -1;
            case ImageFormat::ASTC_8x8_UNORM_BLOCK:                                 return -1;
            case ImageFormat::ASTC_8x8_SRGB_BLOCK:                                  return -1;
            case ImageFormat::ASTC_10x5_UNORM_BLOCK:                                return -1;
            case ImageFormat::ASTC_10x5_SRGB_BLOCK:                                 return -1;
            case ImageFormat::ASTC_10x6_UNORM_BLOCK:                                return -1;
            case ImageFormat::ASTC_10x6_SRGB_BLOCK:                                 return -1;
            case ImageFormat::ASTC_10x8_UNORM_BLOCK:                                return -1;
            case ImageFormat::ASTC_10x8_SRGB_BLOCK:                                 return -1;
            case ImageFormat::ASTC_10x10_UNORM_BLOCK:                               return -1;
            case ImageFormat::ASTC_10x10_SRGB_BLOCK:                                return -1;
            case ImageFormat::ASTC_12x10_UNORM_BLOCK:                               return -1;
            case ImageFormat::ASTC_12x10_SRGB_BLOCK:                                return -1;
            case ImageFormat::ASTC_12x12_UNORM_BLOCK:                               return -1;
            case ImageFormat::ASTC_12x12_SRGB_BLOCK:                                return -1;
            case ImageFormat::G8B8G8R8_422_UNORM:                                   return -1;
            case ImageFormat::B8G8R8G8_422_UNORM:                                   return -1;
            case ImageFormat::G8_B8_R8_3PLANE_420_UNORM:                            return -1;
            case ImageFormat::G8_B8R8_2PLANE_420_UNORM:                             return -1;
            case ImageFormat::G8_B8_R8_3PLANE_422_UNORM:                            return -1;
            case ImageFormat::G8_B8R8_2PLANE_422_UNORM:                             return -1;
            case ImageFormat::G8_B8_R8_3PLANE_444_UNORM:                            return -1;
            case ImageFormat::R10X6_UNORM_PACK16:                                   return -1;
            case ImageFormat::R10X6G10X6_UNORM_2PACK16:                             return -1;
            case ImageFormat::R10X6G10X6B10X6A10X6_UNORM_4PACK16:                   return -1;
            case ImageFormat::G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:               return -1;
            case ImageFormat::B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:               return -1;
            case ImageFormat::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:           return -1;
            case ImageFormat::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:            return -1;
            case ImageFormat::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:           return -1;
            case ImageFormat::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:            return -1;
            case ImageFormat::G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:           return -1;
            case ImageFormat::R12X4_UNORM_PACK16:                                   return -1;
            case ImageFormat::R12X4G12X4_UNORM_2PACK16:                             return -1;
            case ImageFormat::R12X4G12X4B12X4A12X4_UNORM_4PACK16:                   return -1;
            case ImageFormat::G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:               return -1;
            case ImageFormat::B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:               return -1;
            case ImageFormat::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:           return -1;
            case ImageFormat::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:            return -1;
            case ImageFormat::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:           return -1;
            case ImageFormat::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:            return -1;
            case ImageFormat::G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:           return -1;
            case ImageFormat::G16B16G16R16_422_UNORM:                               return -1;
            case ImageFormat::B16G16R16G16_422_UNORM:                               return -1;
            case ImageFormat::G16_B16_R16_3PLANE_420_UNORM:                         return -1;
            case ImageFormat::G16_B16R16_2PLANE_420_UNORM:                          return -1;
            case ImageFormat::G16_B16_R16_3PLANE_422_UNORM:                         return -1;
            case ImageFormat::G16_B16R16_2PLANE_422_UNORM:                          return -1;
            case ImageFormat::G16_B16_R16_3PLANE_444_UNORM:                         return -1;
            case ImageFormat::PVRTC1_2BPP_UNORM_BLOCK_IMG:                          return -1;
            case ImageFormat::PVRTC1_4BPP_UNORM_BLOCK_IMG:                          return -1;
            case ImageFormat::PVRTC2_2BPP_UNORM_BLOCK_IMG:                          return -1;
            case ImageFormat::PVRTC2_4BPP_UNORM_BLOCK_IMG:                          return -1;
            case ImageFormat::PVRTC1_2BPP_SRGB_BLOCK_IMG:                           return -1;
            case ImageFormat::PVRTC1_4BPP_SRGB_BLOCK_IMG:                           return -1;
            case ImageFormat::PVRTC2_2BPP_SRGB_BLOCK_IMG:                           return -1;
            case ImageFormat::PVRTC2_4BPP_SRGB_BLOCK_IMG:                           return -1;
            case ImageFormat::ASTC_4x4_SFLOAT_BLOCK_EXT:                            return -1;
            case ImageFormat::ASTC_5x4_SFLOAT_BLOCK_EXT:                            return -1;
            case ImageFormat::ASTC_5x5_SFLOAT_BLOCK_EXT:                            return -1;
            case ImageFormat::ASTC_6x5_SFLOAT_BLOCK_EXT:                            return -1;
            case ImageFormat::ASTC_6x6_SFLOAT_BLOCK_EXT:                            return -1;
            case ImageFormat::ASTC_8x5_SFLOAT_BLOCK_EXT:                            return -1;
            case ImageFormat::ASTC_8x6_SFLOAT_BLOCK_EXT:                            return -1;
            case ImageFormat::ASTC_8x8_SFLOAT_BLOCK_EXT:                            return -1;
            case ImageFormat::ASTC_10x5_SFLOAT_BLOCK_EXT:                           return -1;
            case ImageFormat::ASTC_10x6_SFLOAT_BLOCK_EXT:                           return -1;
            case ImageFormat::ASTC_10x8_SFLOAT_BLOCK_EXT:                           return -1;
            case ImageFormat::ASTC_10x10_SFLOAT_BLOCK_EXT:                          return -1;
            case ImageFormat::ASTC_12x10_SFLOAT_BLOCK_EXT:                          return -1;
            case ImageFormat::ASTC_12x12_SFLOAT_BLOCK_EXT:                          return -1;
            case ImageFormat::G8_B8R8_2PLANE_444_UNORM_EXT:                         return -1;
            case ImageFormat::G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT:        return -1;
            case ImageFormat::G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT:        return -1;
            case ImageFormat::G16_B16R16_2PLANE_444_UNORM_EXT:                      return -1;
            case ImageFormat::A4R4G4B4_UNORM_PACK16_EXT:                            return -1;
            case ImageFormat::A4B4G4R4_UNORM_PACK16_EXT:                            return -1;
        }
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

    DEFINE_ENUM_FLAG_OPERATORS(ImageUsage)

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

        operator VkComponentMapping const&() const noexcept
        {
            return *reinterpret_cast<const VkComponentMapping*>(this);
        }

        operator VkComponentMapping&() noexcept
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
        COLOR_RGB = VECTOR_3,
        COLOR_RGBA = VECTOR_4,
        UV = VECTOR_2,
        TEXTURE_COORDINATE = UV
    };

    constexpr uint GetVertexAttributeTypeSize(const VertexAttributeType vertexAttributeType)
    {
        switch (vertexAttributeType)
        {
            case VertexAttributeType::FLOAT:
                return FLOAT_SIZE * 1;
            case VertexAttributeType::VECTOR_2:
                return FLOAT_SIZE * 2;
            case VertexAttributeType::VECTOR_3:
                return FLOAT_SIZE * 3;
            case VertexAttributeType::VECTOR_4:
                return FLOAT_SIZE * 4;
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

    typedef uint BoolGLSL;
}