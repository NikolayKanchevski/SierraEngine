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

#include "../../Version.h"
#include "Abstractions/Texture.h"
#include "../../../Engine/Classes/File.h"

#define MAX_IMGUI_DESCRIPTOR_COUNT 2048

using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering::Vulkan
{
    VK VK::m_Instance;

    /* --- CONSTRUCTORS --- */

    void VK::Initialize(const VkPhysicalDeviceFeatures givenPhysicalDeviceFeatures)
    {
        m_Instance.requiredPhysicalDeviceFeatures = givenPhysicalDeviceFeatures;

        m_Instance.InitializeVolk();
        m_Instance.CreateInstance();

        #if VALIDATION_ENABLED
            m_Instance.CreateDebugMessenger();
        #endif

        m_Instance.CreateDevice();
        m_Instance.CreateVulkanMemoryAllocator();

        m_Instance.CreateCommandPool();
        m_Instance.CreateQueryPool();

        m_Instance.CreateImGuiDescriptorPool();

        m_Instance.CreateDefaultTextures();

        Sampler::Initialize();
    }

    /* --- SETTER METHODS --- */

    void VK::InitializeVolk()
    {
        VK_ASSERT(volkInitialize(), "Could not load Volk");
    }

    void VK::CreateInstance()
    {
        using namespace Sierra::Engine;

        // Create application information
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Sierra Engine";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(Version::MAJOR, Version::MINOR, Version::PATCH);
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
            debugMessengerCreateInfo = new VkDebugUtilsMessengerCreateInfoEXT();
            debugMessengerCreateInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugMessengerCreateInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debugMessengerCreateInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debugMessengerCreateInfo->pfnUserCallback = Debugger::ValidationCallback;
            debugMessengerCreateInfo->pUserData = nullptr;

            // Set instance to use the debug messenger
            instanceCreateInfo.enabledLayerCount = static_cast<uint>(validationLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
            instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) debugMessengerCreateInfo;
        #endif

        // Create instance
        VK_ASSERT(
            vkCreateInstance(&instanceCreateInfo, nullptr, &instance),
            "Could not create Vulkan instance"
        );

        // Connect instance with Volk
        volkLoadInstance(instance);
    }

    #if VALIDATION_ENABLED
    void VK::CreateDebugMessenger()
    {
        VK_ASSERT(
            CreateDebugUtilsMessengerEXT(instance, debugMessengerCreateInfo, nullptr, &debugMessenger),
            "Failed to create validation messenger"
        );

        delete(debugMessengerCreateInfo);
    }
    #endif

    void VK::CreateDevice()
    {
        device = Device::Create({ requiredPhysicalDeviceFeatures });
    }

    void VK::CreateVulkanMemoryAllocator()
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
        vmaCreteInfo.instance = instance;
        vmaCreteInfo.physicalDevice = device->GetPhysicalDevice();
        vmaCreteInfo.device = device->GetLogicalDevice();
        vmaCreteInfo.vulkanApiVersion = VK_VERSION;
        vmaCreteInfo.pVulkanFunctions = &vulkanFunctions;
        vmaCreteInfo.flags =  VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;

        // Create VMA allocator
        vmaCreateAllocator(&vmaCreteInfo, &vmaAllocator);
    }

    void VK::CreateCommandPool()
    {
        // Set up the command pool creation info
        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = device->GetGraphicsQueueFamily();

        // Create the command pool
        VK_ASSERT(
            vkCreateCommandPool(device->GetLogicalDevice(), &commandPoolCreateInfo, nullptr, &commandPool),
            "Failed to create command pool"
        );
    }

    void VK::CreateQueryPool()
    {
        // Create query pool
        queryPool = QueryPool::Create({ .queryType = VK_QUERY_TYPE_TIMESTAMP });
    }

    void VK::CreateImGuiDescriptorPool()
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
            vkCreateDescriptorPool(VK::GetDevice()->GetLogicalDevice(), &imGuiDescriptorPoolCreateInfo, nullptr, &imGuiDescriptorPool),
            "Could not create ImGui descriptor pool"
        );
    }

    void VK::CreateDefaultTextures()
    {
        // Create default diffuse texture
        Texture::Create({
            .filePath = File::OUTPUT_FOLDER_PATH + "Textures/Null/DiffuseNull.jpg",
            .textureType = TextureType::DIFFUSE,
            .samplerCreateInfo {
                .enableAnisotropy = false,
                .applyBilinearFiltering = false
            }
        }, true);

        // Create default specular texture
        Texture::Create({
            .filePath = File::OUTPUT_FOLDER_PATH + "Textures/Null/SpecularNull.jpg",
            .textureType = TextureType::SPECULAR,
            .samplerCreateInfo {
                .enableAnisotropy = false,
                .applyBilinearFiltering = false
            }
        }, true);

        // Create default specular texture
        Texture::Create({
            .filePath = File::OUTPUT_FOLDER_PATH + "Textures/Null/NormalMapNull.jpg",
            .textureType = TextureType::NORMAL_MAP,
            .samplerCreateInfo {
                .enableAnisotropy = false,
                .applyBilinearFiltering = false,
            }
        }, true);

        // Create default height map texture
        Texture::Create({
            .filePath = File::OUTPUT_FOLDER_PATH + "Textures/Null/HeightMapNull.jpg",
            .textureType = TextureType::HEIGHT_MAP,
            .samplerCreateInfo {
                .enableAnisotropy = false,
                .applyBilinearFiltering = false
            }
        }, true);
    }

    /* --- DESTRUCTOR --- */

    void VK::Destroy()
    {
        vkDeviceWaitIdle(m_Instance.device->GetLogicalDevice());

        if (ImGui::GetCurrentContext() != nullptr) ImGui_ImplVulkan_Shutdown();

        Sampler::Shutdown();

        vkDestroyDescriptorPool(m_Instance.GetLogicalDevice(), m_Instance.imGuiDescriptorPool, nullptr);

        DescriptorPool::DisposePools();
        m_Instance.queryPool->Destroy();

        vkDestroyCommandPool(m_Instance.device->GetLogicalDevice(), m_Instance.commandPool, nullptr);

        vmaDestroyAllocator(m_Instance.vmaAllocator);

        m_Instance.device->Destroy();

        #if VALIDATION_ENABLED
            DestroyDebugUtilsMessengerEXT(m_Instance.instance, m_Instance.debugMessenger, nullptr);
        #endif

        vkDestroyInstance(m_Instance.instance, nullptr);
    }

}