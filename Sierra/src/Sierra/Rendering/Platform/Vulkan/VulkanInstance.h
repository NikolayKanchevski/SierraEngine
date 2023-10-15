//
// Created by Nikolay Kanchevski on 10.09.23.
//

#pragma once

#include "VulkanResource.h"

#define SR_VALIDATION_ENABLED (SR_ENABLE_LOGGING && !(SR_PLATFORM_APPLE && SR_PLATFORM_iOS)) // Validation layers are not present in mobile Apple devices

namespace Sierra
{

    struct VulkanInstanceCreateInfo
    {

    };

    class SIERRA_API VulkanAPIVersion
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline VulkanAPIVersion(const uint32 version) : version(version) { }
        inline VulkanAPIVersion(const uint8 major, const uint8 minor, const uint8 patch) : version(VK_MAKE_API_VERSION(0, major, minor, patch)) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint8 GetMajor() const { return VK_VERSION_MAJOR(version); }
        [[nodiscard]] inline uint8 GetMinor() const { return VK_VERSION_MINOR(version); }
        [[nodiscard]] inline uint8 GetPatch() const { return VK_VERSION_PATCH(version); }

        /* --- OPERATORS --- */
        inline operator uint32() const { return version; }

    private:
        const uint32 version;

    };

    class SIERRA_API VulkanInstance final : public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit VulkanInstance(const VulkanInstanceCreateInfo &createInfo);
        static UniquePtr<VulkanInstance> Create(const VulkanInstanceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkInstance GetVulkanInstance() const { return instance; }
        [[nodiscard]] bool IsExtensionLoaded(const String &extensionName) const;
        [[nodiscard]] VulkanAPIVersion GetAPIVersion() const;

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        VkInstance instance = VK_NULL_HANDLE;

        struct InstanceExtension
        {
            String name;
            bool requiredOnlyIfSupported = false;
        };

        const std::vector<InstanceExtension> INSTANCE_EXTENSIONS_TO_QUERY
        {
            { .name = VK_KHR_SURFACE_EXTENSION_NAME },
            #if SR_PLATFORM_APPLE
                {
                    .name = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
                    .requiredOnlyIfSupported = true
                },
            #endif
            #if SR_VALIDATION_ENABLED
                {
                    .name = VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                    .requiredOnlyIfSupported = true
                },
            #endif
        };

        std::vector<Hash> loadedExtensions;
        bool AddExtensionIfSupported(const InstanceExtension &extension, std::vector<const char*> &extensionList, const std::vector<VkExtensionProperties> &supportedExtensions);

        #if SR_VALIDATION_ENABLED
            VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
            static bool ValidationLayersSupported(const std::vector<const char*> &layers);
        #endif

    };

}
