//
// Created by Nikolay Kanchevski on 16.03.24.
//

#pragma once

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLBuffer = void;
    }
#endif

#include "../ResourceTable.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalResourceTable final : public ResourceTable, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalResourceTable(const MetalDevice& device, const ResourceTableCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] UniformBufferID ReserveUniformBuffer() override;
        void UpdateUniformBuffer(UniformBufferID ID, const Buffer& buffer, uint64 offset, uint64 memorySize) override;
        bool FreeUniformBuffer(UniformBufferID ID) override;

        [[nodiscard]] StorageBufferID ReserveStorageBuffer() override;
        void UpdateStorageBuffer(StorageBufferID ID, const Buffer& buffer, uint64 offset, uint64 memorySize) override;
        bool FreeStorageBuffer(StorageBufferID ID) override;

        [[nodiscard]] SampledImageID ReserveSampledImage() override;
        void UpdateSampledImage(SampledImageID ID, const Image& image) override;
        bool FreeSampledImage(SampledImageID ID) override;

        [[nodiscard]] StorageImageID ReserveStorageImage() override;
        void UpdateStorageImage(StorageImageID ID, const Image& image) override;
        bool FreeStorageImage(StorageImageID ID) override;

        [[nodiscard]] SamplerID ReserveSampler() override;
        void UpdateSampler(SamplerID ID, const Sampler& sampler) override;
        bool FreeSampler(SamplerID ID) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetUniformBufferCapacity() const noexcept override { return UNIFORM_BUFFER_CAPACITY; }
        [[nodiscard]] uint32 GetStorageBufferCapacity() const noexcept override { return STORAGE_BUFFER_CAPACITY; }

        [[nodiscard]] uint32 GetSampledImageCapacity() const noexcept override { return SAMPLED_IMAGE_CAPACITY; }
        [[nodiscard]] uint32 GetStorageImageCapacity() const noexcept override { return STORAGE_IMAGE_CAPACITY; }
        [[nodiscard]] uint32 GetSamplerCapacity() const noexcept override { return SAMPLER_CAPACITY; }

        [[nodiscard]] id<MTLBuffer> GetMetalArgumentBuffer() const noexcept { return argumentBuffer; }
        [[nodiscard]] const HandleManager<UniformBufferID, id<MTLBuffer>>& GetUniformBuffers() const noexcept { return uniformBuffers; }
        [[nodiscard]] const HandleManager<StorageBufferID, id<MTLBuffer>>& GetStorageBuffers() const noexcept { return storageBuffers; }
        [[nodiscard]] const HandleManager<SampledImageID, id<MTLTexture>>& GetSampledImages() const noexcept { return sampledImages; }
        [[nodiscard]] const HandleManager<StorageImageID, id<MTLTexture>>& GetStorageImages() const noexcept { return storageImages; }

        /* --- COPY SEMANTICS --- */
        MetalResourceTable(const MetalResourceTable&) = delete;
        MetalResourceTable& operator=(const MetalResourceTable&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalResourceTable(MetalResourceTable&&) noexcept = default;
        MetalResourceTable& operator=(MetalResourceTable&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~MetalResourceTable() noexcept override;

    private:
        #if !defined(__OBJC__)
            using MTLArgumentEncoder = void;
            using MTLResource = void;
        #endif

        // NOTE: These must match the values in specified in https://github.com/NikolayKanchevski/ShaderConnect/blob/sierra/src/Platform/MetalSL/MetalSLShaderCompiler.cpp#L104
        constexpr static uint32 UNIFORM_BUFFER_CAPACITY         = 8192;
        constexpr static uint32 STORAGE_BUFFER_CAPACITY         = 8192;
        constexpr static uint32 SAMPLED_IMAGE_CAPACITY          = 8192;
        constexpr static uint32 STORAGE_IMAGE_CAPACITY          = 8192;
        constexpr static uint32 SAMPLER_CAPACITY                = 1024;

        constexpr static uint32 UNIFORM_BUFFER_INDEX        = 0;
        constexpr static uint32 STORAGE_BUFFER_INDEX        = UNIFORM_BUFFER_INDEX + UNIFORM_BUFFER_CAPACITY;
        constexpr static uint32 SAMPLED_IMAGE_INDEX         = STORAGE_BUFFER_INDEX + STORAGE_BUFFER_CAPACITY;
        constexpr static uint32 STORAGE_IMAGE_INDEX         = SAMPLED_IMAGE_INDEX + SAMPLED_IMAGE_CAPACITY;
        constexpr static uint32 SAMPLER_INDEX               = STORAGE_IMAGE_INDEX + STORAGE_IMAGE_CAPACITY;

        id<MTLArgumentEncoder> argumentEncoder = nil;
        id<MTLBuffer> argumentBuffer = nil;

        HandleManager<UniformBufferID, id<MTLBuffer>> uniformBuffers = { };
        HandleManager<StorageBufferID, id<MTLBuffer>> storageBuffers = { };
        HandleManager<SampledImageID, id<MTLTexture>> sampledImages = { };
        HandleManager<StorageImageID, id<MTLTexture>> storageImages = { };
        HandleManager<SamplerID> samplers = { };

    };

}
