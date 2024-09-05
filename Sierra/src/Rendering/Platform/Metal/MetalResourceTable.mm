//
// Created by Nikolay Kanchevski on 16.03.24.
//

#include "MetalResourceTable.h"

#include "MetalBuffer.h"
#include "MetalImage.h"
#include "MetalSampler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalResourceTable::MetalResourceTable(const MetalDevice& device, const ResourceTableCreateInfo& createInfo)
        : ResourceTable(createInfo), device(device), name(createInfo.name)
    {
        SR_ERROR_IF([device.GetMetalDevice() argumentBuffersSupport] != MTLArgumentBuffersTier2, "[Metal]: Cannot create resource table [{0}], as the provided device [{1}] does not support Argument Buffers!", name, device.GetName());

        // Set up argument descriptors
        constexpr uint32 ARGUMENT_BUFFER_INDEX_COUNT = 5;
        NSMutableArray<MTLArgumentDescriptor*>* const argumentDescriptors = [[NSMutableArray alloc] initWithCapacity: ARGUMENT_BUFFER_INDEX_COUNT];

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
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_SAMPLED_IMAGE_INDEX] setTextureType: MTLTextureTypeCube]; // This should be covering all texture types
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_SAMPLED_IMAGE_INDEX] setArrayLength: SAMPLED_IMAGE_CAPACITY];

        [argumentDescriptors addObject: [MTLArgumentDescriptor argumentDescriptor]];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_STORAGE_IMAGE_INDEX] setDataType: MTLDataTypeTexture];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_STORAGE_IMAGE_INDEX] setIndex: STORAGE_IMAGE_INDEX];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_STORAGE_IMAGE_INDEX] setAccess: MTLBindingAccessReadWrite];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_STORAGE_IMAGE_INDEX] setTextureType: MTLTextureTypeCube]; // This should be covering all texture types
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_STORAGE_IMAGE_INDEX] setArrayLength: STORAGE_IMAGE_CAPACITY];

        [argumentDescriptors addObject: [MTLArgumentDescriptor argumentDescriptor]];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_SAMPLER_INDEX] setDataType: MTLDataTypeSampler];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_SAMPLER_INDEX] setIndex: SAMPLER_INDEX];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_SAMPLER_INDEX] setAccess: MTLBindingAccessReadOnly];
        [[argumentDescriptors objectAtIndex: MetalDevice::BINDLESS_ARGUMENT_BUFFER_SAMPLER_INDEX] setArrayLength: SAMPLER_CAPACITY];

        // Create encoder
        argumentEncoder = [device.GetMetalDevice() newArgumentEncoderWithArguments: argumentDescriptors];
        device.SetResourceName(argumentEncoder, fmt::format("Argument encoder of resource table [{0}]", name));

        // Create argument buffer
        argumentBuffer = [device.GetMetalDevice() newBufferWithLength: [argumentEncoder encodedLength] options: MTLResourceStorageModeShared];
        device.SetResourceName(argumentBuffer, fmt::format("Argument buffer of resource table [{0}]", name));

        // Assign argument buffer
        [argumentEncoder setArgumentBuffer: argumentBuffer offset: 0];
        [argumentDescriptors release];
    }

    /* --- POLLING METHODS --- */

    void MetalResourceTable::BindUniformBuffer(ResourceIndex index, const Buffer& buffer, const uint64 memoryByteSize, const uint64 byteOffset)
    {
        SR_ERROR_IF(buffer.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot not bind uniform buffer [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", buffer.GetName(), name);
        const MetalBuffer& metalBuffer = static_cast<const MetalBuffer&>(buffer);

        if (index >= GetUniformBufferCapacity())
        {
            SR_WARNING("[Metal]: Cannot bind uniform buffer at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetUniformBufferCapacity() to query uniform buffer capacity.", index, name);
            return;
        }

        [argumentEncoder setBuffer: metalBuffer.GetMetalBuffer() offset: byteOffset atIndex: UNIFORM_BUFFER_INDEX + index];
        boundResources[BoundResourceEntry(index, MTLResourceUsageRead)] = metalBuffer.GetMetalBuffer();

    }

    void MetalResourceTable::BindStorageBuffer(const ResourceIndex index, const Buffer& buffer, const uint64 memoryByteSize, const uint64 byteOffset)
    {
        SR_ERROR_IF(buffer.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot not bind storage buffer [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", buffer.GetName(), name);
        const MetalBuffer& metalBuffer = static_cast<const MetalBuffer&>(buffer);

        if (index >= GetStorageBufferCapacity())
        {
            SR_WARNING("[Metal]: Cannot bind storage buffer at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetStorageBufferCapacity() to query storage buffer capacity.", index, name);
            return;
        }

        [argumentEncoder setBuffer: metalBuffer.GetMetalBuffer() offset: byteOffset atIndex: STORAGE_BUFFER_INDEX + index];
        boundResources[BoundResourceEntry(index, MTLResourceUsageRead | MTLResourceUsageWrite)] = metalBuffer.GetMetalBuffer();
    }

    void MetalResourceTable::BindSampledImage(const ResourceIndex index, const Image& image)
    {
        SR_ERROR_IF(image.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot not bind sampled image [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", image.GetName(), name);
        const MetalImage& metalImage = static_cast<const MetalImage&>(image);

        if (index >= GetSampledImageCapacity())
        {
            SR_WARNING("[Metal]: Cannot bind sampled image at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetSampledImageCapacity() to query sampled image capacity.", index, name);
            return;
        }

        [argumentEncoder setTexture: metalImage.GetMetalTexture() atIndex: SAMPLED_IMAGE_INDEX + index];
        boundResources[BoundResourceEntry(index, MTLResourceUsageRead)] = metalImage.GetMetalTexture();
    }

    void MetalResourceTable::BindStorageImage(const ResourceIndex index, const Image& image)
    {
        SR_ERROR_IF(image.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot not bind storage image [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", image.GetName(), name);
        const MetalImage& metalImage = static_cast<const MetalImage&>(image);

        if (index >= GetStorageImageCapacity())
        {
            SR_WARNING("[Metal]: Cannot bind storage image at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetStorageImageCapacity() to query storage image capacity.", index, name);
            return;
        }

        [argumentEncoder setTexture: metalImage.GetMetalTexture() atIndex: STORAGE_IMAGE_INDEX + index];
        boundResources[BoundResourceEntry(index, MTLResourceUsageRead | MTLResourceUsageWrite)] = metalImage.GetMetalTexture();
    }

    void MetalResourceTable::BindSampler(const ResourceIndex index, const Sampler& sampler)
    {
        SR_ERROR_IF(sampler.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot not bind sampler [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", sampler.GetName(), name);
        const MetalSampler& metalSampler = static_cast<const MetalSampler&>(sampler);

        if (index >= GetSamplerCapacity())
        {
            SR_WARNING("[Metal]: Cannot bind sampler at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetSamplerCapacity() to query sampler capacity.", index, name);
            return;
        }

        [argumentEncoder setSamplerState: metalSampler.GetSamplerState() atIndex: SAMPLER_INDEX + index];
        // NOTE: Sampler states do not derive from MTLResource, so we do not need to add them to bound resource map
    }

    /* --- GETTER METHODS --- */

    std::string_view MetalResourceTable::GetName() const
    {
        return name;
    }

    /* --- DESTRUCTOR --- */

    MetalResourceTable::~MetalResourceTable()
    {
        [argumentEncoder release];
        [argumentBuffer release];
    }

}
