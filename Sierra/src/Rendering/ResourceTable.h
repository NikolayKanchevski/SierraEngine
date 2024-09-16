//
// Created by Nikolay Kanchevski on 9.03.24.
//

#pragma once

#include "RenderingResource.h"

#include "Buffer.h"
#include "Image.h"
#include "Sampler.h"

namespace Sierra
{

    struct ResourceTableCreateInfo
    {
        std::string_view name = "Resource Table";
    };

    /* --- TYPE DEFINITIONS --- */
    using ResourceIndex = uint32;

    class SIERRA_API ResourceTable : public virtual RenderingResource
    {
    public:

        /* --- POLLING METHODS --- */
        virtual void BindUniformBuffer(ResourceIndex index, const Buffer& buffer, size memorySize = 0, size offset = 0) = 0;
        virtual void BindStorageBuffer(ResourceIndex index, const Buffer& buffer, size memorySize = 0, size offset = 0) = 0;

        virtual void BindSampledImage(ResourceIndex index, const Image& image) = 0;
        virtual void BindStorageImage(ResourceIndex index, const Image& image) = 0;
        virtual void BindSampler(ResourceIndex index, const Sampler& sampler) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetUniformBufferCapacity() const = 0;
        [[nodiscard]] virtual uint32 GetStorageBufferCapacity() const = 0;

        [[nodiscard]] virtual uint32 GetSampledImageCapacity() const = 0;
        [[nodiscard]] virtual uint32 GetStorageImageCapacity() const = 0;
        [[nodiscard]] virtual uint32 GetSamplerCapacity() const = 0;

        /* --- DESTRUCTOR --- */
        ~ResourceTable() override = default;

    protected:
        explicit ResourceTable(const ResourceTableCreateInfo& createInfo);

    };

}
