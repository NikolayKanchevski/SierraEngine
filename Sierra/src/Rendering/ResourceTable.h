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
        virtual void BindUniformBuffer(uint32 index, const std::unique_ptr<Buffer> &buffer, uint64 memoryRange = 0, uint64 byteOffset = 0) = 0;
        virtual void BindStorageBuffer(uint32 index, const std::unique_ptr<Buffer> &buffer, uint64 memoryRange = 0, uint64 byteOffset = 0) = 0;

        virtual void BindSampledImage(uint32 index, const std::unique_ptr<Image> &image) = 0;
        virtual void BindSampledCubemap(uint32 index, const std::unique_ptr<Image> &image) = 0;
        virtual void BindSampler(uint32 index, const std::unique_ptr<Sampler> &sampler) = 0;

        virtual void BindStorageImage(uint32 index, const std::unique_ptr<Image> &image) = 0;
        virtual void BindStorageCubemap(uint32 index, const std::unique_ptr<Image> &image) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetUniformBufferCapacity() const = 0;
        [[nodiscard]] virtual uint32 GetStorageBufferCapacity() const = 0;

        [[nodiscard]] virtual uint32 GetSampledImageCapacity() const = 0;
        [[nodiscard]] virtual uint32 GetSampledCubemapCapacity() const = 0;
        [[nodiscard]] virtual uint32 GetSamplerCapacity() const = 0;

        [[nodiscard]] virtual uint32 GetStorageImageCapacity() const = 0;
        [[nodiscard]] virtual uint32 GetStorageCubemapCapacity() const = 0;

        /* --- OPERATORS --- */
        ResourceTable(const ResourceTable&) = delete;
        ResourceTable &operator=(const ResourceTable&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~ResourceTable() = default;

    protected:
        explicit ResourceTable(const ResourceTableCreateInfo &createInfo);

    };

}
