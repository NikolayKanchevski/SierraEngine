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

    MetalResourceTable::MetalResourceTable(const MetalDevice &device, const ResourceTableCreateInfo &createInfo)
        : ResourceTable(createInfo), MetalResource(createInfo.name), device(device)
    {
        SR_ERROR_IF([device.GetMetalDevice() argumentBuffersSupport] != MTLArgumentBuffersTier2, "[Metal]: Cannot create resource table [{0}], as the provided device [{1}] does not support the Argument Buffers!", GetName(), device.GetName());

        // Set up argument descriptors
        NSMutableArray<MTLArgumentDescriptor*>* const argumentDescriptors = [[NSMutableArray alloc] initWithCapacity: MetalDevice::BINDLESS_ARGUMENT_BUFFER_INDEX_COUNT];

        [argumentDescriptors addObject: [MTLArgumentDescriptor argumentDescriptor]];
        [[argumentDescriptors objectAtIndex: 0] setDataType: MTLDataTypePointer];
        [[argumentDescriptors objectAtIndex: 0] setIndex: UNIFORM_BUFFER_INDEX];
        [[argumentDescriptors objectAtIndex: 0] setAccess: MTLBindingAccessReadOnly];
        [[argumentDescriptors objectAtIndex: 0] setArrayLength: UNIFORM_BUFFER_CAPACITY];

        [argumentDescriptors addObject: [MTLArgumentDescriptor argumentDescriptor]];
        [[argumentDescriptors objectAtIndex: 1] setDataType: MTLDataTypePointer];
        [[argumentDescriptors objectAtIndex: 1] setIndex: STORAGE_BUFFER_INDEX];
        [[argumentDescriptors objectAtIndex: 1] setAccess: MTLBindingAccessReadWrite];
        [[argumentDescriptors objectAtIndex: 1] setArrayLength: STORAGE_BUFFER_CAPACITY];

        [argumentDescriptors addObject: [MTLArgumentDescriptor argumentDescriptor]];
        [[argumentDescriptors objectAtIndex: 2] setDataType: MTLDataTypeTexture];
        [[argumentDescriptors objectAtIndex: 2] setIndex: SAMPLED_IMAGE_INDEX];
        [[argumentDescriptors objectAtIndex: 2] setAccess: MTLBindingAccessReadOnly];
        [[argumentDescriptors objectAtIndex: 2] setTextureType: MTLTextureType2D];
        [[argumentDescriptors objectAtIndex: 2] setArrayLength: SAMPLED_IMAGE_CAPACITY];

        [argumentDescriptors addObject: [MTLArgumentDescriptor argumentDescriptor]];
        [[argumentDescriptors objectAtIndex: 3] setDataType: MTLDataTypeTexture];
        [[argumentDescriptors objectAtIndex: 3] setIndex: SAMPLED_CUBEMAP_INDEX];
        [[argumentDescriptors objectAtIndex: 3] setIndex: SAMPLED_CUBEMAP_INDEX];
        [[argumentDescriptors objectAtIndex: 3] setAccess: MTLBindingAccessReadOnly];
        [[argumentDescriptors objectAtIndex: 3] setTextureType: MTLTextureTypeCube];
        [[argumentDescriptors objectAtIndex: 3] setArrayLength: SAMPLED_CUBEMAP_CAPACITY];

        [argumentDescriptors addObject: [MTLArgumentDescriptor argumentDescriptor]];
        [[argumentDescriptors objectAtIndex: 4] setDataType: MTLDataTypeTexture];
        [[argumentDescriptors objectAtIndex: 4] setIndex: STORAGE_IMAGE_INDEX];
        [[argumentDescriptors objectAtIndex: 4] setAccess: MTLBindingAccessReadWrite];
        [[argumentDescriptors objectAtIndex: 4] setTextureType: MTLTextureType2D];
        [[argumentDescriptors objectAtIndex: 4] setArrayLength: STORAGE_IMAGE_CAPACITY];

        [argumentDescriptors addObject: [MTLArgumentDescriptor argumentDescriptor]];
        [[argumentDescriptors objectAtIndex: 5] setDataType: MTLDataTypeTexture];
        [[argumentDescriptors objectAtIndex: 5] setIndex: STORAGE_CUBEMAP_INDEX];
        [[argumentDescriptors objectAtIndex: 5] setAccess: MTLBindingAccessReadWrite];
        [[argumentDescriptors objectAtIndex: 5] setTextureType: MTLTextureTypeCube];
        [[argumentDescriptors objectAtIndex: 5] setArrayLength: STORAGE_CUBEMAP_CAPACITY];

        [argumentDescriptors addObject: [MTLArgumentDescriptor argumentDescriptor]];
        [[argumentDescriptors objectAtIndex: 6] setDataType:  MTLDataTypeSampler];
        [[argumentDescriptors objectAtIndex: 6] setIndex:  SAMPLER_INDEX];
        [[argumentDescriptors objectAtIndex: 6] setAccess:  MTLBindingAccessReadOnly];
        [[argumentDescriptors objectAtIndex: 6] setArrayLength:  SAMPLER_CAPACITY];

        // Create encoder
        argumentEncoder = [device.GetMetalDevice() newArgumentEncoderWithArguments: argumentDescriptors];
        device.SetResourceName(argumentEncoder, "Argument encoder of resource table [" + std::string(GetName()) + "]");

        // Create argument buffer
        argumentBuffer = [device.GetMetalDevice() newBufferWithLength: [argumentEncoder encodedLength] options: MTLResourceStorageModeShared];
        device.SetResourceName(argumentBuffer, "Argument buffer of resource table [" + std::string(GetName()) + "]");

        // Assign argument buffer
        [argumentEncoder setArgumentBuffer: argumentBuffer offset: 0];
        [argumentDescriptors release];
    }

    /* --- POLLING METHODS --- */

    void MetalResourceTable::BindUniformBuffer(uint32 index, const std::unique_ptr<Buffer> &buffer, const uint64 memoryRange, const uint64 byteOffset)
    {
        SR_ERROR_IF(buffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot not bind uniform buffer [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", buffer->GetName(), GetName());
        const MetalBuffer &metalBuffer = static_cast<const MetalBuffer&>(*buffer);

        if (index >= GetUniformBufferCapacity())
        {
            SR_WARNING("[Metal]: Cannot bind uniform buffer at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetUniformBufferCapacity() to query uniform buffer capacity.", index, GetName());
            return;
        }

        [argumentEncoder setBuffer: metalBuffer.GetMetalBuffer() offset: byteOffset atIndex: UNIFORM_BUFFER_INDEX + index];
        boundResources[BoundResourceEntry(index, MTLResourceUsageRead)] = metalBuffer.GetMetalBuffer();

    }

    void MetalResourceTable::BindStorageBuffer(const ResourceIndex index, const std::unique_ptr<Buffer> &buffer, const uint64 memoryRange, const uint64 byteOffset)
    {
        SR_ERROR_IF(buffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot not bind storage buffer [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", buffer->GetName(), GetName());
        const MetalBuffer &metalBuffer = static_cast<const MetalBuffer&>(*buffer);

        if (index >= GetStorageBufferCapacity())
        {
            SR_WARNING("[Metal]: Cannot bind storage buffer at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetStorageBufferCapacity() to query storage buffer capacity.", index, GetName());
            return;
        }

        [argumentEncoder setBuffer: metalBuffer.GetMetalBuffer() offset: byteOffset atIndex: STORAGE_BUFFER_INDEX + index];
        boundResources[BoundResourceEntry(index, MTLResourceUsageRead | MTLResourceUsageWrite)] = metalBuffer.GetMetalBuffer();
    }

    void MetalResourceTable::BindSampledImage(const ResourceIndex index, const std::unique_ptr<Image> &image)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot not bind sampled image [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", image->GetName(), GetName());
        const MetalImage &metalImage = static_cast<const MetalImage&>(*image);

        if (index >= GetSampledImageCapacity())
        {
            SR_WARNING("[Metal]: Cannot bind sampled image at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetSampledImageCapacity() to query sampled image capacity.", index, GetName());
            return;
        }

        [argumentEncoder setTexture: metalImage.GetMetalTexture() atIndex: SAMPLED_IMAGE_INDEX + index];
        boundResources[BoundResourceEntry(index, MTLResourceUsageRead)] = metalImage.GetMetalTexture();
    }

    void MetalResourceTable::BindSampledCubemap(const ResourceIndex index, const std::unique_ptr<Image> &image)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot not bind sampled cubemap [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", image->GetName(), GetName());
        const MetalImage &metalImage = static_cast<const MetalImage&>(*image);

        if (index >= GetSampledCubemapCapacity())
        {
            SR_WARNING("[Metal]: Cannot bind sampled cubemap at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetSampledCubemapCapacity() to query sampled cubemap capacity.", index, GetName());
            return;
        }

        [argumentEncoder setTexture: metalImage.GetMetalTexture() atIndex: SAMPLED_CUBEMAP_INDEX + index];
        boundResources[BoundResourceEntry(index, MTLResourceUsageRead)] = metalImage.GetMetalTexture();
    }

    void MetalResourceTable::BindSampler(const ResourceIndex index, const std::unique_ptr<Sampler> &sampler)
    {
        SR_ERROR_IF(sampler->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot not bind sampler [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", sampler->GetName(), GetName());
        const MetalSampler &metalSampler = static_cast<const MetalSampler&>(*sampler);

        if (index >= GetSamplerCapacity())
        {
            SR_WARNING("[Metal]: Cannot bind sampler at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetSamplerCapacity() to query sampler capacity.", index, GetName());
            return;
        }

        [argumentEncoder setSamplerState: metalSampler.GetSamplerState() atIndex: SAMPLER_INDEX + index];
        // NOTE: Sampler states do not derive from MTLResource, so we do not need to add them to bound resource map
    }

    void MetalResourceTable::BindStorageImage(const ResourceIndex index, const std::unique_ptr<Image> &image)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot not bind storage image [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", image->GetName(), GetName());
        const MetalImage &metalImage = static_cast<const MetalImage&>(*image);

        if (index >= GetStorageImageCapacity())
        {
            SR_WARNING("[Metal]: Cannot bind storage image at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetStorageImageCapacity() to query storage image capacity.", index, GetName());
            return;
        }

        [argumentEncoder setTexture: metalImage.GetMetalTexture() atIndex: STORAGE_IMAGE_INDEX + index];
        boundResources[BoundResourceEntry(index, MTLResourceUsageRead | MTLResourceUsageWrite)] = metalImage.GetMetalTexture();
    }

    void MetalResourceTable::BindStorageCubemap(const ResourceIndex index, const std::unique_ptr<Image> &image)
    {
        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot not bind storage cubemap [{0}] to resource table [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", image->GetName(), GetName());
        const MetalImage &metalImage = static_cast<const MetalImage&>(*image);

        if (index >= GetStorageCubemapCapacity())
        {
            SR_WARNING("[Metal]: Cannot bind storage cubemap at index [{0}] within resource table [{1}], as it is out of bounds! Use ResourceTable::GetStorageCubemapCapacity() to query storage cubemap capacity..", index, GetName());
            return;
        }

        [argumentEncoder setTexture: metalImage.GetMetalTexture() atIndex: STORAGE_IMAGE_INDEX + index];
        boundResources[BoundResourceEntry(index, MTLResourceUsageRead | MTLResourceUsageWrite)] = metalImage.GetMetalTexture();
    }

    /* --- DESTRUCTOR --- */

    MetalResourceTable::~MetalResourceTable()
    {
        [argumentEncoder release];
        [argumentBuffer release];
    }

}
