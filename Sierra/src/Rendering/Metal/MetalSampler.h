//
// Created by Nikolay Kanchevski on 27.01.24.
//

#pragma once

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLSamplerState = void;
    }
#endif

#include "../Sampler.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API MTLSamplerMinMagFilter SamplerSampleModeToSamplerMinMagFilter(SamplerFilter sampleMode) noexcept;
    [[nodiscard]] SIERRA_API MTLSamplerAddressMode SamplerExtendModeToSamplerAddressMode(SamplerAddressMode extendMode) noexcept;
    [[nodiscard]] SIERRA_API NSUInteger SamplerAnisotropyToUInteger(SamplerAnisotropy anisotropy) noexcept;
    [[nodiscard]] SIERRA_API MTLCompareFunction SamplerCompareOperationToCompareFunction(SamplerCompareOperation compareOperation) noexcept;
    [[nodiscard]] SIERRA_API MTLSamplerBorderColor SamplerBorderColorToSamplerBorderColor(SamplerBorderColor borderColor) noexcept;

    class SIERRA_API MetalSampler final : public Sampler, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalSampler(const MetalDevice& device, const SamplerCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override;
        [[nodiscard]] id<MTLSamplerState> GetSamplerState() const noexcept { return samplerState; }

        /* --- COPY SEMANTICS --- */
        MetalSampler(const MetalSampler&) = delete;
        MetalSampler& operator=(const MetalSampler&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalSampler(MetalSampler&&) = delete;
        MetalSampler& operator=(MetalSampler&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MetalSampler() noexcept override;

    private:
        id<MTLSamplerState> samplerState = nil;

    };

}
