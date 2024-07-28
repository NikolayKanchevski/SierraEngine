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

#include "../../Sampler.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalSampler final : public Sampler, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalSampler(const MetalDevice &device, const SamplerCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const override;

        [[nodiscard]] id<MTLSamplerState> GetSamplerState() const { return samplerState; }

        /* --- DESTRUCTOR --- */
        ~MetalSampler() override;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static MTLSamplerMinMagFilter SamplerSampleModeToSamplerMinMagFilter(SamplerFilter sampleMode);
        [[nodiscard]] static MTLSamplerAddressMode SamplerExtendModeToSamplerAddressMode(SamplerAddressMode extendMode);
        [[nodiscard]] static NSUInteger SamplerAnisotropyToUInteger(SamplerAnisotropy anisotropy);
        [[nodiscard]] static MTLCompareFunction SamplerCompareOperationToCompareFunction(SamplerCompareOperation compareOperation);
        [[nodiscard]] static MTLSamplerBorderColor SamplerBorderColorToSamplerBorderColor(SamplerBorderColor borderColor);

    private:
        id<MTLSamplerState> samplerState = nil;

    };

}
