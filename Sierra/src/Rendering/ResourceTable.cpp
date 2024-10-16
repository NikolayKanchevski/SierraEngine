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

    void ResourceTable::BindUniformBuffer(const uint32 index, const Buffer& buffer, const size offset, const size memorySize)
    {
    }

    void ResourceTable::BindStorageBuffer(const uint32 index, const Buffer& buffer, const size offset, const size memorySize)
    {
    }

    void ResourceTable::BindSampledImage(const uint32 index, const Image& image)
    {
    }

    void ResourceTable::BindStorageImage(const uint32 index, const Image& image)
    {
    }

    void ResourceTable::BindSampler(const uint32 index, const Sampler& sampler)
    {
    }


}