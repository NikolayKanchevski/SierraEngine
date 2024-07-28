//
// Created by Nikolay Kanchevski on 27.01.24.
//

#pragma once

#include "RenderingResource.h"

namespace Sierra
{

    enum class SamplerFilter : bool
    {
        Nearest,
        Linear
    };

    enum class SamplerAddressMode : uint8
    {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder
    };

    enum class SamplerAnisotropy : uint8
    {
        x1,
        x2,
        x4,
        x8,
        x16,
        x32,
        x64
    };

    enum class SamplerCompareOperation : uint8
    {
        None,
        Equal,
        NotEqual,
        Less,
        Greater,
        LessOrEqual,
        GreaterOrEqual
    };

    enum class SamplerBorderColor : uint8
    {
        Transparent,
        White,
        Black
    };

    struct SamplerCreateInfo
    {
        std::string_view name = "Sampler";
        SamplerFilter filter = SamplerFilter::Nearest;
        SamplerAddressMode extendMode = SamplerAddressMode::Repeat;
        SamplerCompareOperation compareOperation = SamplerCompareOperation::None;

        uint32 highestSampledLevel = 1;
        SamplerAnisotropy anisotropy = SamplerAnisotropy::x1;
        SamplerBorderColor borderColor = SamplerBorderColor::Black;
    };

    class SIERRA_API Sampler : public virtual RenderingResource
    {
    public:
        /* --- DESTRUCTOR --- */
        ~Sampler() override = default;

    protected:
        explicit Sampler(const SamplerCreateInfo &createInfo);

    };

}
