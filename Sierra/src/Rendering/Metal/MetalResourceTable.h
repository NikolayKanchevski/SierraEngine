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
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }

        void BindUniformBuffer(uint32 index, const Buffer& buffer, size offset, size memorySize) override;
        void BindStorageBuffer(uint32 index, const Buffer& buffer, size offset, size memorySize) override;

        void BindSampledImage(uint32 index, const Image& image) override;
        void BindStorageImage(uint32 index, const Image& image) override;
        void BindSampler(uint32 index, const Sampler& sampler) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetUniformBufferCapacity() const noexcept override { return UNIFORM_BUFFER_CAPACITY; }
        [[nodiscard]] uint32 GetStorageBufferCapacity() const noexcept override { return STORAGE_BUFFER_CAPACITY; }

        [[nodiscard]] uint32 GetSampledImageCapacity() const noexcept override { return SAMPLED_IMAGE_CAPACITY; }
        [[nodiscard]] uint32 GetStorageImageCapacity() const noexcept override { return STORAGE_IMAGE_CAPACITY; }
        [[nodiscard]] uint32 GetSamplerCapacity() const noexcept override { return SAMPLER_CAPACITY; }

        [[nodiscard]] id<MTLBuffer> GetMetalArgumentBuffer() const noexcept { return argumentBuffer; }
        [[nodiscard]] const std::unordered_map<uint32, id<MTLBuffer>>& GetBoundUniformBuffers() const noexcept { return boundUniformBuffers; }
        [[nodiscard]] const std::unordered_map<uint32, id<MTLBuffer>>& GetBoundStorageBuffers() const noexcept { return boundStorageBuffers; }
        [[nodiscard]] const std::unordered_map<uint32, id<MTLTexture>>& GetBoundSampledImages() const noexcept { return boundSampledImages; }
        [[nodiscard]] const std::unordered_map<uint32, id<MTLTexture>>& GetBoundStorageImages() const noexcept { return boundStorageImages; }

        /* --- COPY SEMANTICS --- */
        MetalResourceTable(const MetalResourceTable&) = delete;
        MetalResourceTable& operator=(const MetalResourceTable&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalResourceTable(MetalResourceTable&&) = delete;
        MetalResourceTable& operator=(MetalResourceTable&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MetalResourceTable() noexcept override;

    private:
        #if !defined(__OBJC__)
            using MTLArgumentEncoder = void;
            using MTLResource = void;
        #endif

        const MetalDevice& device;
        const std::string name;

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

        std::unordered_map<uint32, id<MTLBuffer>> boundUniformBuffers;
        std::unordered_map<uint32, id<MTLBuffer>> boundStorageBuffers;
        std::unordered_map<uint32, id<MTLTexture>> boundSampledImages;
        std::unordered_map<uint32, id<MTLTexture>> boundStorageImages;

    };

}
