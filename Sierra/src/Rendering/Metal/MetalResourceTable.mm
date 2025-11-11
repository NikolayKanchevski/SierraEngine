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
        : MetalResource(createInfo.name), ResourceTable(createInfo)
    {
        SR_THROW_IF(device.GetMetalDevice().argumentBuffersSupport != MTLArgumentBuffersTier2, UnsupportedFeatureError(SR_FORMAT("Device [{0}] cannot create resource table [{1}]", device.GetName(), GetName())));

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
        device.SetResourceName(argumentEncoder, SR_FORMAT("Argument encoder of resource table [{0}]", GetName()));

        // Create argument buffer
        argumentBuffer = [device.GetMetalDevice() newBufferWithLength: [argumentEncoder encodedLength] options: MTLResourceStorageModeShared | MTLResourceHazardTrackingModeUntracked];
        SR_THROW_IF(argumentBuffer == nil, UnknownDeviceError(SR_FORMAT("Could not create resource table [{0}], as creation of argument buffer failed", GetName())));
        device.SetResourceName(argumentBuffer, SR_FORMAT("Argument buffer of resource table [{0}]", GetName()));

        // Assign argument buffer
        [argumentEncoder setArgumentBuffer: argumentBuffer offset: 0];
    }

    /* --- POLLING METHODS --- */

    UniformBufferID MetalResourceTable::ReserveUniformBuffer()
    {
        return uniformBuffers.AddItem();
    }

    void MetalResourceTable::UpdateUniformBuffer(const UniformBufferID ID, const Buffer& buffer, const uint64 offset, const uint64 memorySize)
    {
        ResourceTable::UpdateUniformBuffer(ID, buffer, offset, memorySize);

        SR_THROW_IF(buffer.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot bind uniform buffer [{0}] to resource table [{1}], as its backend type differs from [RenderingBackendType::Metal]", buffer.GetName(), GetName())));
        const MetalBuffer& metalBuffer = static_cast<const MetalBuffer&>(buffer);

        [argumentEncoder setBuffer: metalBuffer.GetMetalBuffer() offset: offset atIndex: UNIFORM_BUFFER_INDEX + ID.GetValue()];
        *uniformBuffers.GetItem(ID) = metalBuffer.GetMetalBuffer();
    }

    bool MetalResourceTable::FreeUniformBuffer(const UniformBufferID ID)
    {
        if (uniformBuffers.RemoveItem(ID))
        {
            [argumentEncoder setBuffer: nil offset: 0 atIndex: UNIFORM_BUFFER_INDEX + ID];
            return true;
        }

        return false;
    }

    StorageBufferID MetalResourceTable::ReserveStorageBuffer()
    {
        return storageBuffers.AddItem();
    }

    void MetalResourceTable::UpdateStorageBuffer(const StorageBufferID ID, const Buffer& buffer, const uint64 offset, const uint64 memorySize)
    {
        ResourceTable::UpdateStorageBuffer(ID, buffer, offset, memorySize);

        SR_THROW_IF(buffer.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot bind storage buffer [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", buffer.GetName(), GetName())));
        const MetalBuffer& metalBuffer = static_cast<const MetalBuffer&>(buffer);

        [argumentEncoder setBuffer: metalBuffer.GetMetalBuffer() offset: offset atIndex: STORAGE_BUFFER_INDEX + ID.GetValue()];
        *storageBuffers.GetItem(ID) = metalBuffer.GetMetalBuffer();
    }

    bool MetalResourceTable::FreeStorageBuffer(const StorageBufferID ID)
    {
        if (storageBuffers.RemoveItem(ID))
        {
            [argumentEncoder setBuffer: nil offset: 0 atIndex: STORAGE_BUFFER_INDEX + ID];
            return true;
        }

        return false;
    }

    SampledImageID MetalResourceTable::ReserveSampledImage()
    {
        return sampledImages.AddItem();
    }

    void MetalResourceTable::UpdateSampledImage(const SampledImageID ID, const Image& image)
    {
        ResourceTable::UpdateSampledImage(ID, image);

        SR_THROW_IF(image.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot bind sampled image [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", image.GetName(), GetName())));
        const MetalImage& metalImage = static_cast<const MetalImage&>(image);

        [argumentEncoder setTexture: metalImage.GetMetalTexture() atIndex: SAMPLED_IMAGE_INDEX + ID.GetValue()];
        *sampledImages.GetItem(ID) = metalImage.GetMetalTexture();
    }

    bool MetalResourceTable::FreeSampledImage(const SampledImageID ID)
    {
        if (sampledImages.RemoveItem(ID))
        {
            [argumentEncoder setTexture: nil atIndex: SAMPLED_IMAGE_INDEX + ID];
            return true;
        }

        return false;
    }

    StorageImageID MetalResourceTable::ReserveStorageImage()
    {
        return storageImages.AddItem();
    }

    void MetalResourceTable::UpdateStorageImage(const StorageImageID ID, const Image& image)
    {
        ResourceTable::UpdateStorageImage(ID, image);

        SR_THROW_IF(image.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot bind storage image [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", image.GetName(), GetName())));
        const MetalImage& metalImage = static_cast<const MetalImage&>(image);

        [argumentEncoder setTexture: metalImage.GetMetalTexture() atIndex: STORAGE_IMAGE_INDEX + ID.GetValue()];
        *storageImages.GetItem(ID) = metalImage.GetMetalTexture();
    }

    bool MetalResourceTable::FreeStorageImage(const StorageImageID ID)
    {
        if (storageImages.RemoveItem(ID))
        {
            [argumentEncoder setTexture: nil atIndex: STORAGE_IMAGE_INDEX + ID];
            return true;
        }

        return false;
    }

    SamplerID MetalResourceTable::ReserveSampler()
    {
        return samplers.AddItem();
    }

    void MetalResourceTable::UpdateSampler(const SamplerID ID, const Sampler& sampler)
    {
        ResourceTable::UpdateSampler(ID, sampler);

        SR_THROW_IF(sampler.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot bind sampler [{0}] within command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", sampler.GetName(), GetName())));
        const MetalSampler& metalSampler = static_cast<const MetalSampler&>(sampler);

        [argumentEncoder setSamplerState: metalSampler.GetSamplerState() atIndex: SAMPLER_INDEX + ID.GetValue()];
        // NOTE: Sampler states do not derive from MTLResource, so we do not need to add them to bound resource map
    }

    bool MetalResourceTable::FreeSampler(const SamplerID ID)
    {
        if (samplers.RemoveItem(ID))
        {
            [argumentEncoder setSamplerState: nil atIndex: SAMPLER_INDEX + ID];
            return true;
        }

        return false;
    }

    /* --- DESTRUCTOR --- */

    MetalResourceTable::~MetalResourceTable() noexcept
    {
        [argumentEncoder release];
        [argumentBuffer release];
    }

}
