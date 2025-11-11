//
// Created by Nikolay Kanchevski on 9.03.24.
//

#pragma once

#include "RenderingResource.h"

#include "Buffer.h"
#include "Image.h"
#include "Sampler.h"
#include "../Core/Handle.hpp"

namespace Sierra
{

    struct ResourceTableCreateInfo
    {
        std::string_view name = "Resource Table";
    };

    /* --- TYPE DEFINITIONS --- */
    using ResourceID = Handle<uint32>;

    struct UniformBufferID final : ResourceID { };
    struct StorageBufferID final : ResourceID { };
    struct SampledImageID  final : ResourceID { };
    struct StorageImageID  final : ResourceID { };
    struct SamplerID       final : ResourceID { };

    /* --- CONCEPTS --- */
    template<typename T>
    concept ResourceIDType = std::is_base_of_v<ResourceID, T> && !std::is_same_v<ResourceID, T>;

    class SIERRA_API ResourceTable : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] UniformBufferID BindUniformBuffer(const Buffer& buffer, uint64 offset, uint64 memorySize);
        [[nodiscard]] virtual UniformBufferID ReserveUniformBuffer() = 0;
        virtual void UpdateUniformBuffer(UniformBufferID ID, const Buffer& buffer, uint64 offset, uint64 memorySize);
        virtual bool FreeUniformBuffer(UniformBufferID ID) = 0;

        [[nodiscard]] StorageBufferID BindStorageBuffer(const Buffer& buffer, uint64 offset, uint64 memorySize);
        [[nodiscard]] virtual StorageBufferID ReserveStorageBuffer() = 0;
        virtual void UpdateStorageBuffer(StorageBufferID ID, const Buffer& buffer, uint64 offset, uint64 memorySize);
        virtual bool FreeStorageBuffer(StorageBufferID ID) = 0;

        [[nodiscard]] SampledImageID BindSampledImage(const Image& image);
        [[nodiscard]] virtual SampledImageID ReserveSampledImage() = 0;
        virtual void UpdateSampledImage(SampledImageID ID, const Image& image);
        virtual bool FreeSampledImage(SampledImageID ID) = 0;

        [[nodiscard]] StorageImageID BindStorageImage(const Image& image);
        [[nodiscard]] virtual StorageImageID ReserveStorageImage() = 0;
        virtual void UpdateStorageImage(StorageImageID ID, const Image& image);
        virtual bool FreeStorageImage(StorageImageID ID) = 0;

        [[nodiscard]] SamplerID BindSampler(const Sampler& sampler);
        [[nodiscard]] virtual SamplerID ReserveSampler() = 0;
        virtual void UpdateSampler(SamplerID ID, const Sampler& sampler);
        virtual bool FreeSampler(SamplerID ID) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetUniformBufferCapacity() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetStorageBufferCapacity() const noexcept = 0;

        [[nodiscard]] virtual uint32 GetSampledImageCapacity() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetStorageImageCapacity() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetSamplerCapacity() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        ResourceTable(const ResourceTable&) = delete;
        ResourceTable& operator=(const ResourceTable&) = delete;

        /* --- DESTRUCTOR --- */
        ~ResourceTable() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit ResourceTable(const ResourceTableCreateInfo& createInfo);

        /* --- MOVE SEMANTICS --- */
        ResourceTable(ResourceTable&&) noexcept = default;
        ResourceTable& operator=(ResourceTable&&) noexcept = default;

    };

}
