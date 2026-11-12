//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

#include "../RenderingInstance.h"
#include "VulkanResource.h"

#include "VulkanInstanceFunctionTable.h"

namespace Sierra
{

    class SIERRA_API VulkanInstance final : public RenderingInstance, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit VulkanInstance(const RenderingInstanceCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Device> CreateDevice(const DeviceCreateInfo& createInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }
        [[nodiscard]] Version GetBackendVersion() const noexcept override { return vulkanVersion; }

        [[nodiscard]] VkInstance GetVulkanInstance() const noexcept { return instance; }
        [[nodiscard]] const VulkanInstanceFunctionTable& GetFunctionTable() const noexcept { return functionTable; }
        [[nodiscard]] bool IsExtensionLoaded(std::string_view extensionName) const noexcept { return std::find(loadedExtensions.begin(), loadedExtensions.end(), std::hash<std::string_view>{}(extensionName.data())) != loadedExtensions.end(); }

        /* --- COPY SEMANTICS --- */
        VulkanInstance(const VulkanInstance&) = delete;
        VulkanInstance& operator=(const VulkanInstance&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanInstance(VulkanInstance&&) noexcept = default;
        VulkanInstance& operator=(VulkanInstance&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~VulkanInstance() noexcept override;

    private:
        std::string name = { };
        Version vulkanVersion = Version({ 1, 0, 0 });

        VkInstance instance = VK_NULL_HANDLE;
        std::vector<size> loadedExtensions = { };
        VulkanInstanceFunctionTable functionTable = { };

        #if SR_ENABLE_LOGGING
            VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        #endif

    };

}
