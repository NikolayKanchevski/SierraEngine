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

    class SIERRA_API ResourceTable : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void BindUniformBuffer(uint32 index, const Buffer& buffer, size offset, size memorySize);
        virtual void BindStorageBuffer(uint32 index, const Buffer& buffer, size offset, size memorySize);

        virtual void BindSampledImage(uint32 index, const Image& image);
        virtual void BindStorageImage(uint32 index, const Image& image);
        virtual void BindSampler(uint32 index, const Sampler& sampler);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetUniformBufferCapacity() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetStorageBufferCapacity() const noexcept = 0;

        [[nodiscard]] virtual uint32 GetSampledImageCapacity() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetStorageImageCapacity() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetSamplerCapacity() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        ResourceTable(const ResourceTable&) = delete;
        ResourceTable& operator=(const ResourceTable&) = delete;

        /* --- MOVE SEMANTICS --- */
        ResourceTable(ResourceTable&&) = delete;
        ResourceTable& operator=(ResourceTable&&) = delete;

        /* --- DESTRUCTOR --- */
        ~ResourceTable() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit ResourceTable(const ResourceTableCreateInfo& createInfo);

    };

}
