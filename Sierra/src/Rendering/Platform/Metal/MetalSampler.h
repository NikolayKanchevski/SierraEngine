//
// Created by Nikolay Kanchevski on 27.01.24.
//

#pragma once

#include "../../Sampler.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalSampler : public Sampler, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalSampler(const MetalDevice &device, const SamplerCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline MTL::SamplerState* GetSamplerState() const { return samplerState; }

        /* --- DESTRUCTOR --- */
        ~MetalSampler() override;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static MTL::SamplerMinMagFilter SamplerSampleModeToSamplerMinMagFilter(SamplerSampleMode sampleMode);
        [[nodiscard]] static MTL::SamplerAddressMode SamplerExtendModeToSamplerAddressMode(SamplerExtendMode extendMode);
        [[nodiscard]] static NS::UInteger SamplerAnisotropyToUInteger(SamplerAnisotropy anisotropy);
        [[nodiscard]] static MTL::CompareFunction SamplerCompareOperationToCompareFunction(SamplerCompareOperation compareOperation);
        [[nodiscard]] static MTL::SamplerBorderColor SamplerBorderColorToSamplerBorderColor(SamplerBorderColor borderColor);

    private:
        MTL::SamplerState* samplerState = nullptr;

    };

}
