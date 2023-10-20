//
// Created by Nikolay Kanchevski on 10.09.23.
//

#include "VulkanInstance.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanInstance::VulkanInstance(const VulkanInstanceCreateInfo &createInfo)
    {
        // Initialize Volk
        #if SR_USING_VOLK
            VK_VALIDATE(volkInitialize(), "Could not initialize Volk");
        #endif

        // Set up application info
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Sierra Engine Application";
        applicationInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        applicationInfo.pEngineName = "Sierra Engine";
        applicationInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        applicationInfo.apiVersion = GetAPIVersion();

        // Retrieve supported extension count
        uint32 supportedExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);

        // Retrieve supported extensions
        std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

        // Load queried extensions if supported
        std::vector<const char*> extensionsToLoad;
        extensionsToLoad.reserve(INSTANCE_EXTENSIONS_TO_QUERY.size());
        for (const auto &extension : INSTANCE_EXTENSIONS_TO_QUERY)
        {
            AddExtensionIfSupported(extension, extensionsToLoad, supportedExtensions);
        }

        // Set up instance creation info
        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.ppEnabledLayerNames = nullptr;
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32>(extensionsToLoad.size());
        instanceCreateInfo.ppEnabledExtensionNames = extensionsToLoad.data();
        instanceCreateInfo.flags = 0;
        #if SR_PLATFORM_APPLE
            if (IsExtensionLoaded(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        #endif
        instanceCreateInfo.pNext = nullptr;

        #if SR_VALIDATION_ENABLED
            // Define debug messenger info here, so it does not go out of scope
            VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
            const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

            // Handle validation
            bool validationSupported = ValidationLayersSupported(validationLayers);
            if (validationSupported)
            {
                // Set up debug messenger info
                debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
                debugMessengerCreateInfo.messageSeverity =  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                debugMessengerCreateInfo.messageType =  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
                debugMessengerCreateInfo.pfnUserCallback = [](const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, const VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void*) -> VkBool32
                {
                    switch (messageSeverity)
                    {
                        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                        {
                            SR_WARNING("Vulkan warning: {0}", callbackData->pMessage);
                            break;
                        }
                        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                        {
                            SR_ERROR("Vulkan error: {0}", callbackData->pMessage);
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }

                    return VK_FALSE;
                };

                // Re-assign layer information in instance info
                instanceCreateInfo.enabledLayerCount = static_cast<uint32>(validationLayers.size());
                instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
                instanceCreateInfo.pNext = &debugMessengerCreateInfo;
            }
            else
            {
                SR_WARNING("Vulkan validation layers requested, but none are supported!");
            }
        #endif

        // Create instance
        VK_VALIDATE(
            vkCreateInstance(&instanceCreateInfo, nullptr, &instance),
            "Could not create Vulkan instance!"
        );

        // Register instance for Volk
        #if SR_USING_VOLK
            volkLoadInstance(instance);
        #endif

        #if SR_VALIDATION_ENABLED
            #if !SR_USING_VOLK
               static const auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
            #endif
            if (validationSupported && vkCreateDebugUtilsMessengerEXT != nullptr) vkCreateDebugUtilsMessengerEXT(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger);
        #endif
    }

    UniquePtr<VulkanInstance> VulkanInstance::Create(const VulkanInstanceCreateInfo &createInfo)
    {
        return std::make_unique<VulkanInstance>(createInfo);
    }

    /* --- GETTER METHODS --- */

    bool VulkanInstance::IsExtensionLoaded(const String &extensionName) const
    {
        return std::find(loadedExtensions.begin(), loadedExtensions.end(), std::hash<String>{}(extensionName)) != loadedExtensions.end();
    }

    VulkanAPIVersion VulkanInstance::GetAPIVersion() const
    {
        #if SR_USING_VOLK
            if (vkEnumerateInstanceVersion != nullptr)
        #endif
        {
            uint32 version;
            vkEnumerateInstanceVersion(&version);
            version &= 0xFFFFF000;

            #if SR_PLATFORM_APPLE
                if (version >= VK_API_VERSION_1_3)
                {
                    return VK_API_VERSION_1_2;
                }
            #endif

            return version;
        }

        return VK_API_VERSION_1_0;
    }

    /* --- PRIVATE METHODS --- */

    bool VulkanInstance::AddExtensionIfSupported(const InstanceExtension &extension, std::vector<const char*> &extensionList, const std::vector<VkExtensionProperties> &supportedExtensions)
    {
        // Check if extension is found within the supported ones
        bool extensionSupported = false;
        for (const auto &supportedExtension : supportedExtensions)
        {
            if (strcmp(extension.name.c_str(), supportedExtension.extensionName) == 0)
            {
                extensionSupported = true;
                break;
            }
        }

        // If extension is not found, we do not load it
        if (!extensionSupported)
        {
            if (!extension.requiredOnlyIfSupported) SR_WARNING("Instance extension [{0}] requested but not supported! Extension will be discarded, but issues may occur if extensions' support is not checked before their usage", extension.name);
            return false;
        }

        // Add extension to the list
        loadedExtensions.push_back(std::hash<String>{}(extension.name));
        extensionList.push_back(extension.name.c_str());
        return true;
    }

    #if SR_VALIDATION_ENABLED
        bool VulkanInstance::ValidationLayersSupported(const std::vector<const char*> &layers)
        {
            // Retrieve supported layer count
            uint32 layerCount = 0;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            // Retrieve supported layers
            std::vector<VkLayerProperties> layerProperties(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

            // Check wether every layer is inside the supported ones
            for (const auto &layer : layers)
            {
                bool layerFound = false;
                for (const auto &layerProperty : layerProperties)
                {
                    if (strcmp(layer, layerProperty.layerName) == 0)
                    {
                        layerFound = true;
                        break;
                    }
                }

                if (!layerFound)
                {
                    return false;
                }
            }

            return true;
        }
    #endif

    /* --- DESTRUCTOR --- */

    void VulkanInstance::Destroy()
    {
        #if SR_VALIDATION_ENABLED
            #if !SR_USING_VOLK
                static const auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
            #endif
            if (debugMessenger != nullptr && vkDestroyDebugUtilsMessengerEXT != nullptr) vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        #endif
        vkDestroyInstance(instance, nullptr);
    }

}
