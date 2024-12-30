//
// Created by Nikolay Kanchevski on 9.03.24.
//

#pragma once

#include "../ResourceTable.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"
#include "../../Utilities/IndexPool.hpp"

namespace Sierra
{

    class SIERRA_API VulkanResourceTable final : public ResourceTable, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanResourceTable(const VulkanDevice& device, const ResourceTableCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] UniformBufferID BindUniformBuffer(const Buffer& buffer, uint64 offset, uint64 memorySize) override;
        bool FreeUniformBuffer(UniformBufferID ID) override;

        [[nodiscard]] StorageBufferID BindStorageBuffer(const Buffer& buffer, uint64 offset, uint64 memorySize) override;
        bool FreeStorageBuffer(StorageBufferID ID) override;

        [[nodiscard]] SampledImageID BindSampledImage(const Image& image) override;
        bool FreeSampledImage(SampledImageID ID) override;

        [[nodiscard]] StorageImageID BindStorageImage(const Image& image) override;
        bool FreeStorageImage(StorageImageID ID) override;

        [[nodiscard]] SamplerID BindSampler(const Sampler& sampler) override;
        bool FreeSampler(SamplerID ID) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }

        [[nodiscard]] uint32 GetUniformBufferCapacity() const noexcept override;
        [[nodiscard]] uint32 GetStorageBufferCapacity() const noexcept override;

        [[nodiscard]] uint32 GetSampledImageCapacity() const noexcept override;
        [[nodiscard]] uint32 GetStorageImageCapacity() const noexcept override;
        [[nodiscard]] uint32 GetSamplerCapacity() const noexcept override;

        [[nodiscard]] VkDescriptorSet GetDescriptorSet() const noexcept { return descriptorSet; }

        /* --- COPY SEMANTICS --- */
        VulkanResourceTable(const VulkanResourceTable&) = delete;
        VulkanResourceTable& operator=(const VulkanResourceTable&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanResourceTable(VulkanResourceTable&&) noexcept = default;
        VulkanResourceTable& operator=(VulkanResourceTable&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~VulkanResourceTable() noexcept override;

    private:
        const VulkanDevice* device = nullptr;
        std::string name = { };

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

        IndexPool<UniformBufferID> uniformBufferIndexPool = { };
        IndexPool<StorageBufferID> storageBufferIndexPool = { };
        IndexPool<SampledImageID> sampledImageIndexPool = { };
        IndexPool<StorageImageID> storageImageIndexPool = { };
        IndexPool<SamplerID> samplerIndexPool = { };
    };

}
