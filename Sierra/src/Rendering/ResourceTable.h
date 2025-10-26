//
// Created by Nikolay Kanchevski on 9.03.24.
//

#pragma once

#include "RenderingResource.h"

#include "Buffer.h"
#include "Image.h"
#include "Sampler.h"
#include "../Utilities/Handle.hpp"

namespace Sierra
{

    struct ResourceTableCreateInfo
    {
        std::string_view name = "Resource Table";
    };

    /* --- TYPE DEFINITIONS --- */
    using ResourceID = Handle<uint32>;
    struct UniformBufferID final : public ResourceID { };
    struct StorageBufferID final : public ResourceID { };
    struct SampledImageID  final : public ResourceID { };
    struct StorageImageID  final : public ResourceID { };
    struct SamplerID       final : public ResourceID { };

    /* --- CONCEPTS --- */
    template<typename T>
    concept ResourceIDType = std::is_base_of_v<ResourceID, T> && !std::is_same_v<ResourceID, T>;

    class SIERRA_API ResourceTable : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual UniformBufferID BindUniformBuffer(const Buffer& buffer, uint64 offset, uint64 memorySize) = 0;
        virtual bool FreeUniformBuffer(UniformBufferID ID) = 0;

        [[nodiscard]] virtual StorageBufferID BindStorageBuffer(const Buffer& buffer, uint64 offset, uint64 memorySize) = 0;
        virtual bool FreeStorageBuffer(StorageBufferID ID) = 0;

        [[nodiscard]] virtual SampledImageID BindSampledImage(const Image& image) = 0;
        virtual bool FreeSampledImage(SampledImageID ID) = 0;

        [[nodiscard]] virtual StorageImageID BindStorageImage(const Image& image) = 0;
        virtual bool FreeStorageImage(StorageImageID ID) = 0;

        [[nodiscard]] virtual SamplerID BindSampler(const Sampler& sampler) = 0;
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
