//
// Created by Nikolay Kanchevski on 16.03.24.
//

#pragma once

#include "../../ResourceTable.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalResourceTable : public ResourceTable, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalResourceTable(const MetalDevice &device, const ResourceTableCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void BindUniformBuffer(ResourceIndex index, const std::unique_ptr<Buffer> &buffer, uint64 memoryRange = 0, uint64 byteOffset = 0) override;
        void BindStorageBuffer(ResourceIndex index, const std::unique_ptr<Buffer> &buffer, uint64 memoryRange = 0, uint64 byteOffset = 0) override;

        void BindSampledImage(ResourceIndex index, const std::unique_ptr<Image> &image) override;
        void BindStorageImage(ResourceIndex index, const std::unique_ptr<Image> &image) override;
        void BindSampler(ResourceIndex index, const std::unique_ptr<Sampler> &sampler) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetUniformBufferCapacity() const override { return UNIFORM_BUFFER_CAPACITY; };
        [[nodiscard]] inline uint32 GetStorageBufferCapacity() const override { return STORAGE_BUFFER_CAPACITY; };

        [[nodiscard]] inline uint32 GetSampledImageCapacity() const override { return SAMPLED_IMAGE_CAPACITY; };
        [[nodiscard]] inline uint32 GetStorageImageCapacity() const override { return STORAGE_IMAGE_CAPACITY; };
        [[nodiscard]] inline uint32 GetSamplerCapacity() const override { return SAMPLER_CAPACITY; };

        [[nodiscard]] inline id<MTLBuffer> GetMetalArgumentBuffer() const { return argumentBuffer; }
        [[nodiscard]] inline const auto& GetBoundResources() const { return boundResources; }

        /* --- DESTRUCTOR --- */
        ~MetalResourceTable() override;

    private:
        const MetalDevice &device;

        // NOTE: These must match those of https://github.com/NikichaTV/ShaderConnect/blob/sierra/src/Platform/MetalSL/MetalSLShaderCompiler.cpp#L104
        constexpr static uint32 UNIFORM_BUFFER_CAPACITY         = 500'000;
        constexpr static uint32 STORAGE_BUFFER_CAPACITY         = 500'000;
        constexpr static uint32 SAMPLED_IMAGE_CAPACITY          = 500'000;
        constexpr static uint32 STORAGE_IMAGE_CAPACITY          = 500'000;
        constexpr static uint32 SAMPLER_CAPACITY                = 1024;

        constexpr static uint32 UNIFORM_BUFFER_INDEX        = 0;
        constexpr static uint32 STORAGE_BUFFER_INDEX        = UNIFORM_BUFFER_INDEX + UNIFORM_BUFFER_CAPACITY;
        constexpr static uint32 SAMPLED_IMAGE_INDEX         = STORAGE_BUFFER_INDEX + STORAGE_BUFFER_CAPACITY;
        constexpr static uint32 STORAGE_IMAGE_INDEX         = SAMPLED_IMAGE_INDEX + SAMPLED_IMAGE_CAPACITY;
        constexpr static uint32 SAMPLER_INDEX               = STORAGE_IMAGE_INDEX + STORAGE_IMAGE_CAPACITY;

        id<MTLArgumentEncoder> argumentEncoder = nil;
        id<MTLBuffer> argumentBuffer = nil;

        class SIERRA_API BoundResourceEntry
        {
        public:
            /* --- CONSTRUCTORS --- */
            inline BoundResourceEntry(const uint32 index, const MTLResourceUsage usage) : value((index & 0x7FFFF) << 5 | (usage & 0x3) << 3) { }

            /* --- GETTER METHODS --- */
            [[nodiscard]] inline uint32 GetIndex() const { return (value >> 5) & 0x7FFFF; }
            [[nodiscard]] inline MTLResourceUsage GetUsage() const { return (value >> 3) & 0x3; }
            
            /* --- TYPE DEFINITIONS --- */
            class SIERRA_API Hasher
            {
            public:
                /* --- OPERATORS --- */
                [[nodiscard]] inline std::size_t operator()(const BoundResourceEntry entry) const { return entry.value; }

            };

            /* --- OPERATORS --- */
            [[nodiscard]] inline bool operator==(const BoundResourceEntry other) const { return value == other.value; }

        private:
            // Data is a masked uint24, whose bit ranges contain the following information:
            // [0-18 - resource's index | 19-20 - resource usage | 21-24 - unused]
            // [0000000000000000000     | 00                     | 000            ]
            uint value : 24;

        };
        std::unordered_map<BoundResourceEntry, id<MTLResource>, BoundResourceEntry::Hasher> boundResources;

    };

}
