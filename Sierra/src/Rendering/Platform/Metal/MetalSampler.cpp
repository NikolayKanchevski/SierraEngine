//
// Created by Nikolay Kanchevski on 27.01.24.
//

#include "MetalSampler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalSampler::MetalSampler(const MetalDevice &device, const SamplerCreateInfo &createInfo)
        : Sampler(createInfo), MetalResource(createInfo.name)
    {
        SR_ERROR_IF(!device.IsSamplerAnisotropySupported(createInfo.anisotropy), "[Metal]: Cannot create sampler [{0}] with unsupported sample mode! Make sure to query Device::IsSamplerAnisotropySupported() to query support.", GetName());

        // Set up sampler descriptor
        MTL::SamplerDescriptor* samplerDescriptor = MTL::SamplerDescriptor::alloc()->init();
        device.SetResourceName(samplerDescriptor, GetName());
        samplerDescriptor->setMagFilter(SamplerSampleModeToSamplerMinMagFilter(createInfo.filter));
        samplerDescriptor->setMinFilter(samplerDescriptor->magFilter());
        samplerDescriptor->setRAddressMode(SamplerExtendModeToSamplerAddressMode(createInfo.extendMode));
        samplerDescriptor->setSAddressMode(samplerDescriptor->rAddressMode());
        samplerDescriptor->setTAddressMode(samplerDescriptor->rAddressMode());
        samplerDescriptor->setMaxAnisotropy(SamplerAnisotropyToUInteger(createInfo.anisotropy));
        samplerDescriptor->setCompareFunction(SamplerCompareOperationToCompareFunction(createInfo.compareOperation));
        samplerDescriptor->setLodMinClamp(0.0f);
        samplerDescriptor->setLodMaxClamp(static_cast<float32>(createInfo.highestSampledMipLevel));
        samplerDescriptor->setBorderColor(SamplerBorderColorToSamplerBorderColor(createInfo.borderColor));
        samplerDescriptor->setNormalizedCoordinates(true);

        // Create sampler state
        samplerState = device.GetMetalDevice()->newSamplerState(samplerDescriptor);
        SR_ERROR_IF(samplerState == nullptr, "[Metal]: Could not create sampler [{0}]!", GetName());

        samplerDescriptor->release();
    }

    /* --- DESTRUCTOR --- */

    MetalSampler::~MetalSampler()
    {
        samplerState->release();
    }

    /* --- CONVERSIONS --- */

    MTL::SamplerMinMagFilter MetalSampler::SamplerSampleModeToSamplerMinMagFilter(const SamplerFilter sampleMode)
    {
        switch (sampleMode)
        {
            case SamplerFilter::Linear:     return MTL::SamplerMinMagFilterLinear;
            case SamplerFilter::Nearest:    return MTL::SamplerMinMagFilterNearest;
        }

        return MTL::SamplerMinMagFilterNearest;
    }

    MTL::SamplerAddressMode MetalSampler::SamplerExtendModeToSamplerAddressMode(const SamplerAddressMode extendMode)
    {
        switch (extendMode)
        {
            case SamplerAddressMode::Repeat:             return MTL::SamplerAddressModeRepeat;
            case SamplerAddressMode::MirroredRepeat:     return MTL::SamplerAddressModeMirrorRepeat;
            case SamplerAddressMode::ClampToEdge:        return MTL::SamplerAddressModeClampToEdge;
            case SamplerAddressMode::ClampToBorder:      return MTL::SamplerAddressModeClampToBorderColor;
        }

        return MTL::SamplerAddressModeRepeat;
    }

    NS::UInteger MetalSampler::SamplerAnisotropyToUInteger(const SamplerAnisotropy anisotropy)
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

    MTL::CompareFunction MetalSampler::SamplerCompareOperationToCompareFunction(const SamplerCompareOperation compareOperation)
    {
        switch (compareOperation)
        {
            case SamplerCompareOperation::None:                return MTL::CompareFunctionAlways;
            case SamplerCompareOperation::Equal:               return MTL::CompareFunctionEqual;
            case SamplerCompareOperation::NotEqual:            return MTL::CompareFunctionNotEqual;
            case SamplerCompareOperation::Less:                return MTL::CompareFunctionLess;
            case SamplerCompareOperation::Greater:             return MTL::CompareFunctionGreater;
            case SamplerCompareOperation::LessOrEqual:         return MTL::CompareFunctionLessEqual;
            case SamplerCompareOperation::GreaterOrEqual:      return MTL::CompareFunctionGreaterEqual;
        }

        return MTL::CompareFunctionAlways;
    }

    MTL::SamplerBorderColor MetalSampler::SamplerBorderColorToSamplerBorderColor(const SamplerBorderColor borderColor)
    {
        switch (borderColor)
        {
            case SamplerBorderColor::Transparent: return MTL::SamplerBorderColorTransparentBlack;
            case SamplerBorderColor::White:       return MTL::SamplerBorderColorOpaqueWhite;
            case SamplerBorderColor::Black:       return MTL::SamplerBorderColorOpaqueBlack;
        }

        return MTL::SamplerBorderColorTransparentBlack;
    }

}