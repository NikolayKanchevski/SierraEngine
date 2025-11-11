//
// Created by Nikolay Kanchevski on 9.03.24.
//

#pragma once

#include "../ResourceTable.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanResourceTable final : public ResourceTable, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanResourceTable(const VulkanDevice& device, const ResourceTableCreateInfo& createInfo);

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

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

        HandleManager<UniformBufferID> uniformBuffers = { };
        HandleManager<StorageBufferID> storageBuffers = { };
        HandleManager<SampledImageID> sampledImages = { };
        HandleManager<StorageImageID> storageImages = { };
        HandleManager<SamplerID> samplers = { };

    };

}
