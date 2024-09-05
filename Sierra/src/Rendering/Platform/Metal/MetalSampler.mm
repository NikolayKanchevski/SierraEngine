//
// Created by Nikolay Kanchevski on 27.01.24.
//

#include "MetalSampler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalSampler::MetalSampler(const MetalDevice& device, const SamplerCreateInfo& createInfo)
        : Sampler(createInfo)
    {
        SR_ERROR_IF(!device.IsSamplerAnisotropySupported(createInfo.anisotropy), "[Metal]: Cannot create sampler [{0}] with unsupported sample mode! Make sure to query Device::IsSamplerAnisotropySupported() to query support.", createInfo.name);

        // Set up sampler descriptor
        MTLSamplerDescriptor* const samplerDescriptor = [[MTLSamplerDescriptor alloc] init];
        device.SetResourceName(samplerDescriptor, createInfo.name);
        [samplerDescriptor setMagFilter: SamplerSampleModeToSamplerMinMagFilter(createInfo.filter)];
        [samplerDescriptor setMinFilter: samplerDescriptor.magFilter];
        [samplerDescriptor setRAddressMode: SamplerExtendModeToSamplerAddressMode(createInfo.extendMode)];
        [samplerDescriptor setSAddressMode: samplerDescriptor.rAddressMode];
        [samplerDescriptor setTAddressMode: samplerDescriptor.rAddressMode];
        [samplerDescriptor setMaxAnisotropy: SamplerAnisotropyToUInteger(createInfo.anisotropy)];
        [samplerDescriptor setCompareFunction: SamplerCompareOperationToCompareFunction(createInfo.compareOperation)];
        [samplerDescriptor setLodMinClamp: 0.0f];
        [samplerDescriptor setLodMaxClamp: static_cast<float32>(createInfo.highestSampledLevel)];
        [samplerDescriptor setBorderColor: SamplerBorderColorToSamplerBorderColor(createInfo.borderColor)];
        [samplerDescriptor setNormalizedCoordinates: YES];
        [samplerDescriptor setSupportArgumentBuffers: YES];

        // Create sampler state
        samplerState = [device.GetMetalDevice() newSamplerStateWithDescriptor: samplerDescriptor];
        SR_ERROR_IF(samplerState == nil, "[Metal]: Could not create sampler [{0}]!", createInfo.name);

        [samplerDescriptor release];
    }

    /* --- GETTER METHODS --- */

    std::string_view MetalSampler::GetName() const
    {
        return { [samplerState.label UTF8String], [samplerState.label length] };
    }

    /* --- DESTRUCTOR --- */

    MetalSampler::~MetalSampler()
    {
        [samplerState release];
    }

    /* --- CONVERSIONS --- */

    MTLSamplerMinMagFilter MetalSampler::SamplerSampleModeToSamplerMinMagFilter(const SamplerFilter sampleMode)
    {
        switch (sampleMode)
        {
            case SamplerFilter::Linear:     return MTLSamplerMinMagFilterLinear;
            case SamplerFilter::Nearest:    return MTLSamplerMinMagFilterNearest;
        }

        return MTLSamplerMinMagFilterNearest;
    }

    MTLSamplerAddressMode MetalSampler::SamplerExtendModeToSamplerAddressMode(const SamplerAddressMode extendMode)
    {
        switch (extendMode)
        {
            case SamplerAddressMode::Repeat:             return MTLSamplerAddressModeRepeat;
            case SamplerAddressMode::MirroredRepeat:     return MTLSamplerAddressModeMirrorRepeat;
            case SamplerAddressMode::ClampToEdge:        return MTLSamplerAddressModeClampToEdge;
            case SamplerAddressMode::ClampToBorder:      return MTLSamplerAddressModeClampToBorderColor;
        }

        return MTLSamplerAddressModeRepeat;
    }

    NSUInteger MetalSampler::SamplerAnisotropyToUInteger(const SamplerAnisotropy anisotropy)
    {
        switch (anisotropy)
        {
            case SamplerAnisotropy::x1:     return 1;
            case SamplerAnisotropy::x2:     return 2;
            case SamplerAnisotropy::x4:     return 4;
            case SamplerAnisotropy::x8:     return 8;
            case SamplerAnisotropy::x16:    return 16;
            case SamplerAnisotropy::x32:    return 32;
            case SamplerAnisotropy::x64:    return 64;
        }

        return 1;
    }

    MTLCompareFunction MetalSampler::SamplerCompareOperationToCompareFunction(const SamplerCompareOperation compareOperation)
    {
        switch (compareOperation)
        {
            case SamplerCompareOperation::None:                return MTLCompareFunctionAlways;
            case SamplerCompareOperation::Equal:               return MTLCompareFunctionEqual;
            case SamplerCompareOperation::NotEqual:            return MTLCompareFunctionNotEqual;
            case SamplerCompareOperation::Less:                return MTLCompareFunctionLess;
            case SamplerCompareOperation::Greater:             return MTLCompareFunctionGreater;
            case SamplerCompareOperation::LessOrEqual:         return MTLCompareFunctionLessEqual;
            case SamplerCompareOperation::GreaterOrEqual:      return MTLCompareFunctionGreaterEqual;
        }

        return MTLCompareFunctionAlways;
    }

    MTLSamplerBorderColor MetalSampler::SamplerBorderColorToSamplerBorderColor(const SamplerBorderColor borderColor)
    {
        switch (borderColor)
        {
            case SamplerBorderColor::Transparent:       return MTLSamplerBorderColorTransparentBlack;
            case SamplerBorderColor::White:             return MTLSamplerBorderColorOpaqueWhite;
            case SamplerBorderColor::Black:             return MTLSamplerBorderColorOpaqueBlack;
        }

        return MTLSamplerBorderColorTransparentBlack;
    }

}