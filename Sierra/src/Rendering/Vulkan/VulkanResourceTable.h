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
        void BindUniformBuffer(uint32 index, const Buffer& buffer, size offset, size memorySize) override;
        void BindStorageBuffer(uint32 index, const Buffer& buffer, size offset, size memorySize) override;
        void BindSampledImage(uint32 index, const Image& image) override;
        void BindStorageImage(uint32 index, const Image& image) override;
        void BindSampler(uint32 index, const Sampler& sampler) override;

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
        VulkanResourceTable(VulkanResourceTable&&) = delete;
        VulkanResourceTable& operator=(VulkanResourceTable&&) = delete;

        /* --- DESTRUCTOR --- */
        ~VulkanResourceTable() noexcept override;

    private:
        const VulkanDevice& device;
        const std::string name;

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

    };

}
