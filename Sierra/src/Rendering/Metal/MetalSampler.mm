//
// Created by Nikolay Kanchevski on 27.01.24.
//

#include "MetalSampler.h"

#include "../DeviceErrors.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */

    MTLSamplerMinMagFilter SamplerSampleModeToSamplerMinMagFilter(const SamplerFilter sampleMode) noexcept
    {
        switch (sampleMode)
        {
            case SamplerFilter::Linear:     return MTLSamplerMinMagFilterLinear;
            case SamplerFilter::Nearest:    return MTLSamplerMinMagFilterNearest;
        }

        return MTLSamplerMinMagFilterNearest;
    }

    MTLSamplerAddressMode SamplerExtendModeToSamplerAddressMode(const SamplerAddressMode extendMode) noexcept
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

    NSUInteger SamplerAnisotropyToUInteger(const SamplerAnisotropy anisotropy) noexcept
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

    MTLCompareFunction SamplerCompareOperationToCompareFunction(const SamplerCompareOperation compareOperation) noexcept
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

    MTLSamplerBorderColor SamplerBorderColorToSamplerBorderColor(const SamplerBorderColor borderColor) noexcept
    {
        switch (borderColor)
        {
            case SamplerBorderColor::Transparent:       return MTLSamplerBorderColorTransparentBlack;
            case SamplerBorderColor::White:             return MTLSamplerBorderColorOpaqueWhite;
            case SamplerBorderColor::Black:             return MTLSamplerBorderColorOpaqueBlack;
        }

        return MTLSamplerBorderColorTransparentBlack;
    }

    /* --- CONSTRUCTORS --- */

    MetalSampler::MetalSampler(const MetalDevice& device, const SamplerCreateInfo& createInfo)
        : Sampler(createInfo)
    {
        SR_THROW_IF(!device.IsSamplerAnisotropySupported(createInfo.anisotropy), UnsupportedFeatureError(SR_FORMAT("Device [{0}] cannot create sampler [{1}] with unsupported anisotropy - use Device::IsSamplerAnisotropySupported() to query support", device.GetName(), createInfo.name)));

        MTLSamplerDescriptor* const samplerDescriptor = [[MTLSamplerDescriptor alloc] init];
        device.SetResourceName(samplerDescriptor, createInfo.name);

        // Set up sampler descriptor
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
        SR_THROW_IF(samplerState == nil, UnknownDeviceError(SR_FORMAT("Could not create sampler [{0}]", createInfo.name)));

        [samplerDescriptor release];
    }

    /* --- GETTER METHODS --- */

    std::string_view MetalSampler::GetName() const noexcept
    {
        return { samplerState.label.UTF8String, samplerState.label.length };
    }

    /* --- DESTRUCTOR --- */

    MetalSampler::~MetalSampler() noexcept
    {
        [samplerState release];
    }

}