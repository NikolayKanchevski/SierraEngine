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
        void BindUniformBuffer(ResourceIndex index, const Buffer &buffer, uint64 memoryByteSize = 0, uint64 byteOffset = 0) override;
        void BindStorageBuffer(ResourceIndex index, const Buffer &buffer, uint64 memoryByteSize = 0, uint64 byteOffset = 0) override;

        void BindSampledImage(ResourceIndex index, const Image &image) override;
        void BindStorageImage(ResourceIndex index, const Image &image) override;
        void BindSampler(ResourceIndex index, const Sampler &sampler) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const override { return name; }

        [[nodiscard]] uint32 GetUniformBufferCapacity() const override;
        [[nodiscard]] uint32 GetStorageBufferCapacity() const override;

        [[nodiscard]] uint32 GetSampledImageCapacity() const override;
        [[nodiscard]] uint32 GetStorageImageCapacity() const override;
        [[nodiscard]] uint32 GetSamplerCapacity() const override;

        [[nodiscard]] VkDescriptorSet GetDescriptorSet() const { return descriptorSet; }

        /* --- DESTRUCTOR --- */
        ~VulkanResourceTable() override;

    private:
        const VulkanDevice &device;

        std::string name;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

    };

}
