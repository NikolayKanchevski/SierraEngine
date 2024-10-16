//
// Created by Nikolay Kanchevski on 16.03.24.
//

#include "MetalResourceTable.h"

#include "MetalBuffer.h"
#include "MetalImage.h"
#include "MetalSampler.h"

#include "../DeviceErrors.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalResourceTable::MetalResourceTable(const MetalDevice& device, const ResourceTableCreateInfo& createInfo)
            : ResourceTable(createInfo), device(device), name(createInfo.name)
    {
        SR_THROW_IF(device.GetMetalDevice().argumentBuffersSupport != MTLArgumentBuffersTier2, UnsupportedFeatureError(SR_FORMAT("Device [{0}] cannot create resource table [{1}]", device.GetName(), name)));

        // Set up argument descriptors
        constexpr uint32 ARGUMENT_BUFFER_INDEX_COUNT = 5;
        NSMutableArray<MTLArgumentDescriptor*>* const argumentDescriptors = [NSMutableArray arrayWithCapacity: ARGUMENT_BUFFER_INDEX_COUNT];

        [argumentDescriptors addObject: [MTLArgumentDescriptor argumentDescriptor]];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_UNIFORM_BUFFER_INDEX] setDataType: MTLDataTypePointer];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_UNIFORM_BUFFER_INDEX] setIndex: UNIFORM_BUFFER_INDEX];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_UNIFORM_BUFFER_INDEX] setAccess: MTLBindingAccessReadOnly];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_UNIFORM_BUFFER_INDEX] setArrayLength: UNIFORM_BUFFER_CAPACITY];

        [argumentDescriptors addObject: [MTLArgumentDescriptor argumentDescriptor]];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_STORAGE_BUFFER_INDEX] setDataType: MTLDataTypePointer];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_STORAGE_BUFFER_INDEX] setIndex: STORAGE_BUFFER_INDEX];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_STORAGE_BUFFER_INDEX] setAccess: MTLBindingAccessReadWrite];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_STORAGE_BUFFER_INDEX] setArrayLength: STORAGE_BUFFER_CAPACITY];

        [argumentDescriptors addObject: [MTLArgumentDescriptor argumentDescriptor]];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_SAMPLED_IMAGE_INDEX] setDataType: MTLDataTypeTexture];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_SAMPLED_IMAGE_INDEX] setIndex: SAMPLED_IMAGE_INDEX];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_SAMPLED_IMAGE_INDEX] setAccess: MTLBindingAccessReadOnly];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_SAMPLED_IMAGE_INDEX] setTextureType: MTLTextureTypeCube]; // This covers all texture types
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_SAMPLED_IMAGE_INDEX] setArrayLength: SAMPLED_IMAGE_CAPACITY];

        [argumentDescriptors addObject: [MTLArgumentDescriptor argumentDescriptor]];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_STORAGE_IMAGE_INDEX] setDataType: MTLDataTypeTexture];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_STORAGE_IMAGE_INDEX] setIndex: STORAGE_IMAGE_INDEX];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_STORAGE_IMAGE_INDEX] setAccess: MTLBindingAccessReadWrite];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_STORAGE_IMAGE_INDEX] setTextureType: MTLTextureTypeCube]; // This covers all texture types
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_STORAGE_IMAGE_INDEX] setArrayLength: STORAGE_IMAGE_CAPACITY];

        [argumentDescriptors addObject: [MTLArgumentDescriptor argumentDescriptor]];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_SAMPLER_INDEX] setDataType: MTLDataTypeSampler];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_SAMPLER_INDEX] setIndex: SAMPLER_INDEX];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_SAMPLER_INDEX] setAccess: MTLBindingAccessReadOnly];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_SAMPLER_INDEX] setArrayLength: SAMPLER_CAPACITY];

        // Create encoder
        argumentEncoder = [device.GetMetalDevice() newArgumentEncoderWithArguments: argumentDescriptors];
        device.SetResourceName(argumentEncoder, SR_FORMAT("Argument encoder of resource table [{0}]", name));

        // Create argument buffer
        argumentBuffer = [device.GetMetalDevice() newBufferWithLength: [argumentEncoder encodedLength] options: MTLResourceStorageModeShared | MTLResourceHazardTrackingModeUntracked];
        SR_THROW_IF(argumentBuffer == nil, UnknownDeviceError(SR_FORMAT("Could not create resource table [{0}], as creation of argument buffer failed", name)));
        device.SetResourceName(argumentBuffer, SR_FORMAT("Argument buffer of resource table [{0}]", name));

        // Assign argument buffer
        [argumentEncoder setArgumentBuffer: argumentBuffer offset: 0];
    }

    /* --- POLLING METHODS --- */

    void MetalResourceTable::BindUniformBuffer(const uint32 index, const Buffer& buffer, const size offset, const size memorySize)
    {
        SR_THROW_IF(buffer.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot bind uniform buffer [{0}] to resource table [{1}], as its backend type differs from [RenderingBackendType::Metal]", buffer.GetName(), name)));
        const MetalBuffer& metalBuffer = static_cast<const MetalBuffer&>(buffer);

        ResourceTable::BindUniformBuffer(index, metalBuffer, offset, memorySize);

        [argumentEncoder setBuffer: metalBuffer.GetMetalBuffer() offset: offset atIndex: UNIFORM_BUFFER_INDEX + index];
        boundUniformBuffers[index] = metalBuffer.GetMetalBuffer();

    }

    void MetalResourceTable::BindStorageBuffer(const uint32 index, const Buffer& buffer, const size offset, const size memorySize)
    {
        SR_THROW_IF(buffer.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot bind storage buffer [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", buffer.GetName(), name)));
        const MetalBuffer& metalBuffer = static_cast<const MetalBuffer&>(buffer);

        ResourceTable::BindStorageBuffer(index, metalBuffer, offset, memorySize);

        [argumentEncoder setBuffer: metalBuffer.GetMetalBuffer() offset: offset atIndex: STORAGE_BUFFER_INDEX + index];
        boundStorageBuffers[index] = metalBuffer.GetMetalBuffer();
    }

    void MetalResourceTable::BindSampledImage(const uint32 index, const Image& image)
    {
        SR_THROW_IF(image.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot bind sampled image [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", image.GetName(), name)));
        const MetalImage& metalImage = static_cast<const MetalImage&>(image);

        ResourceTable::BindSampledImage(index, metalImage);

        [argumentEncoder setTexture: metalImage.GetMetalTexture() atIndex: SAMPLED_IMAGE_INDEX + index];
        boundSampledImages[index] = metalImage.GetMetalTexture();
    }

    void MetalResourceTable::BindStorageImage(const uint32 index, const Image& image)
    {
        SR_THROW_IF(image.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot bind storage image [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", image.GetName(), name)));
        const MetalImage& metalImage = static_cast<const MetalImage&>(image);

        ResourceTable::BindStorageImage(index, metalImage);

        [argumentEncoder setTexture: metalImage.GetMetalTexture() atIndex: STORAGE_IMAGE_INDEX + index];
        boundStorageImages[index] = metalImage.GetMetalTexture();
    }

    void MetalResourceTable::BindSampler(const uint32 index, const Sampler& sampler)
    {
        SR_THROW_IF(sampler.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot bind sampler [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", sampler.GetName(), name)));
        const MetalSampler& metalSampler = static_cast<const MetalSampler&>(sampler);

        ResourceTable::BindSampler(index, sampler);

        [argumentEncoder setSamplerState: metalSampler.GetSamplerState() atIndex: SAMPLER_INDEX + index];
        // NOTE: Sampler states do not derive from MTLResource, so we do not need to add them to bound resource map
    }

    /* --- DESTRUCTOR --- */

    MetalResourceTable::~MetalResourceTable() noexcept
    {
        [argumentEncoder release];
        [argumentBuffer release];
    }

}
