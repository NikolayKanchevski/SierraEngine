//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

#include "../RenderingResource.h"

namespace Sierra
{

    class SIERRA_API VulkanResource : public virtual RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] RenderingBackendType GetBackendType() const noexcept override { return RenderingBackendType::Vulkan; }

        /* --- COPY SEMANTICS --- */
        VulkanResource(const VulkanResource&) = delete;
        VulkanResource& operator=(const VulkanResource&) = delete;

        /* --- DESTRUCTORS --- */
        ~VulkanResource() noexcept override = default;

    protected:
        /* --- TYPE DEFINITIONS --- */
        using VkHandle32 = ullong;
        using VkHandle64 = void*;

        /* --- CONSTRUCTORS --- */
        VulkanResource() = default;

        /* --- POLLING METHODS --- */
        void AddToPNextChain(void* mainStruct, void* newStruct) const noexcept;

        /* --- MOVE SEMANTICS --- */
        VulkanResource(VulkanResource&&) noexcept = default;
        VulkanResource& operator=(VulkanResource&&) noexcept = default;

    };

}
