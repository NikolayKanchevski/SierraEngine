//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

#include "VulkanResource.h"
#include "../../RenderingContext.h"

#include "VulkanInstance.h"
#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanContext final : public RenderingContext, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanContext(const RenderingContextCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline UniquePtr<VulkanInstance>& GetInstance() { return instance; }
        [[nodiscard]] inline UniquePtr<VulkanDevice>& GetDevice() { return device; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

        /* --- OPERATORS --- */
        VulkanContext(const VulkanContext&) = delete;
        VulkanContext& operator=(const VulkanContext&) = delete;

    private:
        UniquePtr<VulkanInstance> instance;
        UniquePtr<VulkanDevice> device;

    };

}
