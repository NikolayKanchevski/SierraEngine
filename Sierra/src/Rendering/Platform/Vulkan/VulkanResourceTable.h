//
// Created by Nikolay Kanchevski on 9.03.24.
//

#pragma once

#include "../../ResourceTable.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanResourceTable final : public ResourceTable, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanResourceTable(const VulkanDevice &device, const ResourceTableCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void BindUniformBuffer(ResourceIndex index, const std::unique_ptr<Buffer> &buffer, uint64 memoryRange = 0, uint64 byteOffset = 0) override;
        void BindStorageBuffer(ResourceIndex index, const std::unique_ptr<Buffer> &buffer, uint64 memoryRange = 0, uint64 byteOffset = 0) override;

        void BindSampledImage(ResourceIndex index, const std::unique_ptr<Image> &image) override;
        void BindSampledCubemap(ResourceIndex index, const std::unique_ptr<Image> &image) override;
        void BindSampler(ResourceIndex ResourceTableIndex, const std::unique_ptr<Sampler> &sampler) override;

        void BindStorageImage(ResourceIndex index, const std::unique_ptr<Image> &image) override;
        void BindStorageCubemap(ResourceIndex index, const std::unique_ptr<Image> &image) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetUniformBufferCapacity() const override;
        [[nodiscard]] uint32 GetStorageBufferCapacity() const override;

        [[nodiscard]] uint32 GetSampledImageCapacity() const override;
        [[nodiscard]] uint32 GetSampledCubemapCapacity() const override;
        [[nodiscard]] uint32 GetSamplerCapacity() const override;

        [[nodiscard]] uint32 GetStorageImageCapacity() const override;
        [[nodiscard]] uint32 GetStorageCubemapCapacity() const override;

        [[nodiscard]] inline VkDescriptorSet GetDescriptorSet() const { return descriptorSet; }

        /* --- DESTRUCTOR --- */
        ~VulkanResourceTable() override;

    private:
        const VulkanDevice &device;

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

    };

}
