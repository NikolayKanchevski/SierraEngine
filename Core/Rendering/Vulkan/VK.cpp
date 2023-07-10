//
// Created by Nikolay Kanchevski on 19.12.22.
//

#include "VK.h"

#define VMA_IMPLEMENTATION
#ifdef VMA_STATS_STRING_ENABLED
    #undef VMA_STATS_STRING_ENABLED
#endif
#define VMA_STATS_STRING_ENABLED 0
#define VMA_STATIC_VULKAN_FUNCTIONS 1
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0

#define VOLK_IMPLEMENTATION
#include <volk.h>
#include <vk_mem_alloc.h>

#include "Abstractions/Texture.h"
#include "../../../../Engine/Components/Model.h"

#define MAX_IMGUI_DESCRIPTOR_COUNT 2048

namespace Sierra::Rendering
{
    struct InstanceVK
    {
        VkInstance instance = VK_NULL_HANDLE;
        #if VALIDATION_ENABLED
            VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
            VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfo = nullptr;
        #endif

        UniquePtr<Device> device = nullptr;
        VmaAllocator vmaAllocator = VMA_NULL;

        VkCommandPool commandPool = VK_NULL_HANDLE;
        UniquePtr<QueryPool> queryPool = VK_NULL_HANDLE;
        VkDescriptorPool imGuiDescriptorPool = VK_NULL_HANDLE;
    };
    InstanceVK instance;

    /* --- GETTER METHODS --- */

    VkInstance                  VK::GetInstance()                   { return instance.instance; }
    VmaAllocator&               VK::GetMemoryAllocator()            { return instance.vmaAllocator; }
    UniquePtr<Device>&          VK::GetDevice()                     { return instance.device; }
    VkPhysicalDevice            VK::GetPhysicalDevice()             { return instance.device->GetPhysicalDevice(); }
    VkDevice                    VK::GetLogicalDevice()              { return instance.device->GetLogicalDevice(); }
    VkCommandPool               VK::GetCommandPool()                { return instance.commandPool; }
    UniquePtr<QueryPool>&       VK::GetQueryPool()                  { return instance.queryPool; }
    VkDescriptorPool            VK::GetImGuiDescriptorPool()        { return instance.imGuiDescriptorPool; }

    /* --- METHOD DEFINITIONS --- */

    void InitializeVolk();
    void CreateInstance();
    #if VALIDATION_ENABLED
        void CreateDebugMessenger();
    #endif
    void CreateDevice();
    void CreateVulkanMemoryAllocator();
    void CreateCommandPool();
    void CreateQueryPool();
    void CreateImGuiDescriptorPool();
    void CreateDefaultTextures();

    /* --- CONSTRUCTOR --- */

    void VK::Initialize()
    {
        InitializeVolk();
        CreateInstance();

        #if VALIDATION_ENABLED
            CreateDebugMessenger();
        #endif

        CreateDevice();
        CreateVulkanMemoryAllocator();

        CreateCommandPool();
        CreateQueryPool();
        CreateImGuiDescriptorPool();

        Sampler::Initialize();
        CreateDefaultTextures();
    }

    /* --- PREDEFINED MEMBERS --- */

    std::vector<const char*> requiredInstanceExtensions
    {
        #if VALIDATION_ENABLED
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        #endif
        VK_KHR_SURFACE_EXTENSION_NAME,
        #if PLATFORM_APPLE
            VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
        #endif
    };

    #if VALIDATION_ENABLED
        const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

        VkResult CreateDebugUtilsMessengerEXT(VkInstance givenInstance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
        {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(givenInstance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr)
            {
                return func(givenInstance, pCreateInfo, pAllocator, pDebugMessenger);
            }
            else
            {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        void DestroyDebugUtilsMessengerEXT(VkInstance givenInstance, VkDebugUtilsMessengerEXT givenDebugMessenger, const VkAllocationCallbacks* pAllocator)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(givenInstance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr)
            {
                func(givenInstance, givenDebugMessenger, pAllocator);
            }
        }

    bool ValidationLayersSupported()
    {
        // Get how many validation layers in total are supported
        uint layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        // Create an array and store the supported layers
        std::vector<VkLayerProperties> layerProperties(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

        // Check if the given layers are in the supported array
        for (const auto &requiredLayer : validationLayers)
        {
            bool layerFound = false;
            for (const auto &layerProperty : layerProperties)
            {
                if (strcmp(requiredLayer, layerProperty.layerName) == 0)
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

    bool ExtensionsSupported(std::vector<const char*> &givenExtensions)
    {
        // Get how many extensions are supported in total
        uint extensionCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        // Create an array to store the supported extensions
        std::vector<VkExtensionProperties> extensionProperties(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());

        // Check if each given extension is in the supported array
        int64 extensionIndex = 0;

        bool success = true;
        for (const auto &requiredExtension : givenExtensions)
        {
            bool extensionFound = false;
            for (const auto &extensionProperty : extensionProperties)
            {
                if (strcmp(requiredExtension, extensionProperty.extensionName) == 0)
                {
                    extensionFound = true;
                    break;
                }
            }

            if (!extensionFound)
            {
                success = false;

                ASSERT_WARNING_FORMATTED("Instance extension [{0}] not supported", String(requiredExtension));
                givenExtensions.erase(givenExtensions.begin() + extensionIndex);
            }

            extensionIndex++;
        }

        return success;
    }

    /* --- SETTER METHODS --- */

    void InitializeVolk()
    {
        VK_ASSERT(volkInitialize(), "Could not load Volk");
    }

    void CreateInstance()
    {
        // Create application information
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Sierra Engine";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "No Engine";
        applicationInfo.engineVersion = VK_VERSION_1_3;
        applicationInfo.apiVersion = VK_VERSION;

        // Get GLFW extensions
        uint glfwExtensionCount;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        requiredInstanceExtensions.insert(requiredInstanceExtensions.end(), glfwExtensions, glfwExtensions + glfwExtensionCount);

        // Check whether all extensions are supported
        ASSERT_WARNING_IF(!ExtensionsSupported(requiredInstanceExtensions), "Some requested extensions are not supported. They have been automatically disabled but this could lead to issues");

        // Set up instance creation info
        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = static_cast<uint>(requiredInstanceExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = requiredInstanceExtensions.data();
        instanceCreateInfo.enabledLayerCount = 0;
        #if PLATFORM_APPLE
            instanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        #endif
        instanceCreateInfo.pNext = nullptr;

        // If validation is enabled check validation layers support and bind them to instance
        #if VALIDATION_ENABLED
            ASSERT_ERROR_IF(!ValidationLayersSupported(), "Validation layers requested but are not supported");

            // Set up debug messenger info
            instance.debugMessengerCreateInfo = new VkDebugUtilsMessengerCreateInfoEXT();
            instance.debugMessengerCreateInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            instance.debugMessengerCreateInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            instance.debugMessengerCreateInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            instance.debugMessengerCreateInfo->pfnUserCallback = Internal::Debugger::ValidationCallback;
            instance.debugMessengerCreateInfo->pUserData = nullptr;

            // Set instance to use the debug messenger
            instanceCreateInfo.enabledLayerCount = static_cast<uint>(validationLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
            instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) instance.debugMessengerCreateInfo;
        #endif

        // Create instance
        VK_ASSERT(
            vkCreateInstance(&instanceCreateInfo, nullptr, &instance.instance),
            "Could not create Vulkan instance"
        );

        // Connect instance with Volk
        volkLoadInstance(instance.instance);
    }

    #if VALIDATION_ENABLED
        void CreateDebugMessenger()
        {
            VK_ASSERT(
                CreateDebugUtilsMessengerEXT(instance.instance, instance.debugMessengerCreateInfo, nullptr, &instance.debugMessenger),
                "Failed to create validation messenger"
            );

            delete(instance.debugMessengerCreateInfo);
        }
    #endif

    void CreateDevice()
    {
        instance.device = Device::Create({ });
    }

    void CreateVulkanMemoryAllocator()
    {
        // Get pointers to required Vulkan methods
        VmaVulkanFunctions vulkanFunctions{};
        vulkanFunctions.vkGetInstanceProcAddr               = vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr                 = vkGetDeviceProcAddr;
        vulkanFunctions.vkAllocateMemory                    = vkAllocateMemory;
        vulkanFunctions.vkBindBufferMemory                  = vkBindBufferMemory;
        vulkanFunctions.vkBindImageMemory                   = vkBindImageMemory;
        vulkanFunctions.vkCreateBuffer                      = vkCreateBuffer;
        vulkanFunctions.vkCreateImage                       = vkCreateImage;
        vulkanFunctions.vkDestroyBuffer                     = vkDestroyBuffer;
        vulkanFunctions.vkDestroyImage                      = vkDestroyImage;
        vulkanFunctions.vkFlushMappedMemoryRanges           = vkFlushMappedMemoryRanges;
        vulkanFunctions.vkFreeMemory                        = vkFreeMemory;
        vulkanFunctions.vkGetBufferMemoryRequirements       = vkGetBufferMemoryRequirements;
        vulkanFunctions.vkGetImageMemoryRequirements        = vkGetImageMemoryRequirements;
        vulkanFunctions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
        vulkanFunctions.vkGetPhysicalDeviceProperties       = vkGetPhysicalDeviceProperties;
        vulkanFunctions.vkInvalidateMappedMemoryRanges      = vkInvalidateMappedMemoryRanges;
        vulkanFunctions.vkMapMemory                         = vkMapMemory;
        vulkanFunctions.vkUnmapMemory                       = vkUnmapMemory;
        vulkanFunctions.vkCmdCopyBuffer                     = vkCmdCopyBuffer;

        // Set up VMA creation info
        VmaAllocatorCreateInfo vmaCreteInfo{};
        vmaCreteInfo.instance = instance.instance;
        vmaCreteInfo.physicalDevice = instance.device->GetPhysicalDevice();
        vmaCreteInfo.device = instance.device->GetLogicalDevice();
        vmaCreteInfo.vulkanApiVersion = VK_VERSION;
        vmaCreteInfo.pVulkanFunctions = &vulkanFunctions;
        vmaCreteInfo.flags =  VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;

        // Create VMA allocator
        vmaCreateAllocator(&vmaCreteInfo, &instance.vmaAllocator);
    }

    void CreateCommandPool()
    {
        // Set up the command pool creation info
        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = instance.device->GetGraphicsQueueFamily();

        // Create the command pool
        VK_ASSERT(
            vkCreateCommandPool(instance.device->GetLogicalDevice(), &commandPoolCreateInfo, nullptr, &instance.commandPool),
            "Failed to create command pool"
        );
    }

    void CreateQueryPool()
    {
        // Create query pool
        instance.queryPool = QueryPool::Create({ .queryType = VK_QUERY_TYPE_TIMESTAMP });
    }

    void CreateImGuiDescriptorPool()
    {
        // Set up example pool sizes
        std::vector<VkDescriptorPoolSize> poolSizes =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER,                MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       MAX_IMGUI_DESCRIPTOR_COUNT }
        };

        // Set up descriptor pool creation info
        VkDescriptorPoolCreateInfo imGuiDescriptorPoolCreateInfo{};
        imGuiDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        imGuiDescriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        imGuiDescriptorPoolCreateInfo.maxSets = MAX_IMGUI_DESCRIPTOR_COUNT;
        imGuiDescriptorPoolCreateInfo.poolSizeCount = poolSizes.size();
        imGuiDescriptorPoolCreateInfo.pPoolSizes = poolSizes.data();

        // Create descriptor pool
        VK_ASSERT(
            vkCreateDescriptorPool(instance.device->GetLogicalDevice(), &imGuiDescriptorPoolCreateInfo, nullptr, &instance.imGuiDescriptorPool),
            "Could not create ImGui descriptor pool"
        );
    }

    const uint8 DIFFUSE_NULL_TEXTURE_DATA[] = {
        250, 65, 249, 255,
        0, 0, 0, 255,
        0, 0, 0, 255,
        250, 65, 249, 255
    };

    const uint8 NORMAL_NULL_TEXTURE_DATA[] = {
        128, 127, 255, 255
    };

    const uint8 BLACK_NULL_TEXTURE[] = {
        0, 0, 0, 255
    };

    void CreateDefaultTextures()
    {
        // Create default diffuse texture
        Texture::Load({
            .data = DIFFUSE_NULL_TEXTURE_DATA,
            .width = 2,
            .height = 2,
            .channels = TextureChannels::RGBA,
            .textureType = TextureType::DIFFUSE,
            .imageFormat = ImageFormat::R8G8B8A8_UNORM,
            .samplerCreateInfo {
                .enableAnisotropy = false,
                .applyBilinearFiltering = false
            },
            .setDefaultTexture = true
        });

        // Create default specular texture
        Texture::Load({
            .data = BLACK_NULL_TEXTURE,
            .width = 1,
            .height = 1,
            .channels = TextureChannels::RGBA,
            .textureType = TextureType::SPECULAR,
            .imageFormat = ImageFormat::R8G8B8A8_UNORM,
            .samplerCreateInfo {
                .enableAnisotropy = false,
                .applyBilinearFiltering = false
            },
            .setDefaultTexture = true
        });

        // Create default normal texture
        Texture::Load({
            .data = NORMAL_NULL_TEXTURE_DATA,
            .width = 1,
            .height = 1,
            .channels = TextureChannels::RGBA,
            .textureType = TextureType::NORMAL,
            .imageFormat = ImageFormat::R8G8B8A8_UNORM,
            .samplerCreateInfo {
                .enableAnisotropy = false,
                .applyBilinearFiltering = false
            },
            .setDefaultTexture = true
        });

        // Create default height map texture
        Texture::Load({
            .data = BLACK_NULL_TEXTURE,
            .width = 1,
            .height = 1,
            .channels = TextureChannels::RGBA,
            .textureType = TextureType::HEIGHT,
            .imageFormat = ImageFormat::R8G8B8A8_UNORM,
            .samplerCreateInfo {
                .enableAnisotropy = false,
                .applyBilinearFiltering = false
            },
            .setDefaultTexture = true
        });
    }

    /* --- DESTRUCTOR --- */

    void VK::Destroy()
    {
        instance.device->WaitUntilIdle();
        
        DescriptorPool::DisposePools();
        Sampler::Shutdown();

        // Destroy local resources
        if (ImGui::GetCurrentContext() != nullptr) ImGui_ImplVulkan_Shutdown();
        if (instance.imGuiDescriptorPool != VK_NULL_HANDLE) vkDestroyDescriptorPool(GetLogicalDevice(), instance.imGuiDescriptorPool, nullptr);

        instance.queryPool->Destroy();
        vkDestroyCommandPool(instance.device->GetLogicalDevice(), instance.commandPool, nullptr);

        vmaDestroyAllocator(instance.vmaAllocator);
        instance.device->Destroy();

        #if VALIDATION_ENABLED
            DestroyDebugUtilsMessengerEXT(instance.instance, instance.debugMessenger, nullptr);
        #endif
        vkDestroyInstance(instance.instance, nullptr);
    }

}