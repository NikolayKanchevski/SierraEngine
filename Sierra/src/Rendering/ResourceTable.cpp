//
// Created by Nikolay Kanchevski on 9.03.24.
//

#include "ResourceTable.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    ResourceTable::ResourceTable(const ResourceTableCreateInfo& createInfo)
    {
        SR_THROW_IF(createInfo.name.empty(), InvalidValueError("Cannot create resource table, as specified name must not be empty"));
    }

    /* --- POLLING METHODS --- */

    UniformBufferID ResourceTable::BindUniformBuffer(const Buffer& buffer, const uint64 offset, const uint64 memorySize)
    {
        const UniformBufferID ID = ReserveUniformBuffer();
        UpdateUniformBuffer(ID, buffer, offset, memorySize);
        return ID;
    }

    void ResourceTable::UpdateUniformBuffer(UniformBufferID ID, const Buffer& buffer, uint64 offset, uint64 memorySize)
    {
        SR_THROW_IF(!ID.HasValue() || ID.GetValue() >= GetUniformBufferCapacity(), InvalidValueError(SR_FORMAT("Cannot update uniform buffer of invalid slot with an ID [{0}] within resource table [{1}]", ID.GetValue(), GetName())));
        SR_THROW_IF(offset + memorySize > buffer.GetMemorySize(), InvalidRangeError(SR_FORMAT("Cannot update invalid uniform buffer memory range to resource table [{0}]", GetName()), offset, memorySize, static_cast<uint64>(0), buffer.GetMemorySize()));
    }

    StorageBufferID ResourceTable::BindStorageBuffer(const Buffer& buffer, const uint64 offset, const uint64 memorySize)
    {
        const StorageBufferID ID = ReserveStorageBuffer();
        UpdateStorageBuffer(ID, buffer, offset, memorySize);
        return ID;
    }

    void ResourceTable::UpdateStorageBuffer(StorageBufferID ID, const Buffer& buffer, uint64 offset, uint64 memorySize)
    {
        SR_THROW_IF(!ID.HasValue() || ID.GetValue() >= GetStorageBufferCapacity(), InvalidValueError(SR_FORMAT("Cannot update storage buffer of invalid slot with an ID [{0}] within resource table [{1}]", ID.GetValue(), GetName())));
        SR_THROW_IF(offset + memorySize > buffer.GetMemorySize(), InvalidRangeError(SR_FORMAT("Cannot update invalid storage buffer memory range to resource table [{0}]", GetName()), offset, memorySize, static_cast<uint64>(0), buffer.GetMemorySize()));
    }

    SampledImageID ResourceTable::BindSampledImage(const Image& image)
    {
        const SampledImageID ID = ReserveSampledImage();
        UpdateSampledImage(ID, image);
        return ID;
    }

    void ResourceTable::UpdateSampledImage(SampledImageID ID, const Image& image)
    {
        SR_THROW_IF(!ID.HasValue() || ID.GetValue() >= GetSampledImageCapacity(), InvalidValueError(SR_FORMAT("Cannot update sampled image of invalid slot with an ID [{0}] within resource table [{1}]", ID.GetValue(), GetName())));
    }

    StorageImageID ResourceTable::BindStorageImage(const Image& image)
    {
        const StorageImageID ID = ReserveStorageImage();
        UpdateStorageImage(ID, image);
        return ID;
    }

    void ResourceTable::UpdateStorageImage(StorageImageID ID, const Image& image)
    {
        SR_THROW_IF(!ID.HasValue() || ID.GetValue() >= GetStorageImageCapacity(), InvalidValueError(SR_FORMAT("Cannot update storage image of invalid slot with an ID [{0}] within resource table [{1}]", ID.GetValue(), GetName())));
    }

    SamplerID ResourceTable::BindSampler(const Sampler& sampler)
    {
        const SamplerID ID = ReserveSampler();
        UpdateSampler(ID, sampler);
        return ID;
    }

    void ResourceTable::UpdateSampler(SamplerID ID, const Sampler& sampler)
    {
        SR_THROW_IF(!ID.HasValue() || ID.GetValue() >= GetSamplerCapacity(), InvalidValueError(SR_FORMAT("Cannot update sampler of invalid slot with an ID [{0}] within resource table [{1}]", ID.GetValue(), GetName())));
    }

}