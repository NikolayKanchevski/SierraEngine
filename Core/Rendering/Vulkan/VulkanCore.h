//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include <vulkan/vulkan.h>
#include <glm/mat4x4.hpp>
#include "../Window.h"
#include "Abstractions/Descriptors.h"
#include "../../../Engine/Components/Lighting/DirectionalLight.h"
#include "../../../Engine/Components/Lighting/PointLight.h"

using Sierra::Core::Rendering::Vulkan::Abstractions::DescriptorPool;
using namespace Sierra::Engine::Components;

/* ! NOTE: Remember to change these in shaders too ! */
#define MAX_MESHES 8192 // Changed as @kael wouldn't stop bitching about it
#define MAX_TEXTURES MAX_MESHES * TOTAL_TEXTURE_TYPES_COUNT
#define MAX_POINT_LIGHTS 64
#define MAX_DIRECTIONAL_LIGHTS 16

#define MAIN_IMAGE_FORMAT VK_FORMAT_R8G8B8A8_UNORM

struct UniformData
{
    /* Vertex Uniform Data */
    glm::mat4x4 view;
    glm::mat4x4 projection;

    uint32_t directionalLightCount;
    uint32_t pointLightCount;
    float _align1_;
    float _align2_;
};

struct ObjectData
{
    glm::mat4x4 model;
};

struct StorageData
{
    ObjectData objectDatas[MAX_MESHES];
    DirectionalLight::ShaderDirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight::ShaderPointLight pointLights[MAX_POINT_LIGHTS];
};

namespace Sierra::Core::Rendering::Vulkan
{
    using Abstractions::BindlessDescriptorSet;

    class VulkanCore
    {
    public:
        [[nodiscard]] static inline VulkanCore& GetInstance() { return instance; }

        [[nodiscard]] static inline Window* GetWindow() { return instance.window; }
        [[nodiscard]] static inline GLFWwindow* GetCoreWindow() { return instance.window->GetCoreWindow(); }

        [[nodiscard]] static inline std::shared_ptr<Device>& GetDevice() { return instance.device; }
        [[nodiscard]] static inline VkDevice GetLogicalDevice() { return instance.device->GetLogicalDevice(); }
        [[nodiscard]] static inline VkPhysicalDevice GetPhysicalDevice() { return instance.device->GetPhysicalDevice(); }

        [[nodiscard]] static inline VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures() { return instance.device->GetPhysicalDeviceFeatures(); };
        [[nodiscard]] static inline VkPhysicalDeviceProperties GetPhysicalDeviceProperties() { return instance.device->GetPhysicalDeviceProperties(); };

        [[nodiscard]] static inline VkExtent2D& GetSwapchainExtent() { return instance.swapchainExtent; }
        [[nodiscard]] static inline VkQueue GetGraphicsQueue() { return instance.device->GetGraphicsQueue(); }
        [[nodiscard]] static inline VkCommandPool GetCommandPool() { return instance.device->GetCommandPool(); }
        [[nodiscard]] static inline std::shared_ptr<DescriptorPool>& GetDescriptorPool() { return instance.descriptorPool; }
        [[nodiscard]] static inline std::shared_ptr<BindlessDescriptorSet>& GetGlobalBindlessDescriptorSet() { return instance.globalBindlessDescriptorSet; }
        [[nodiscard]] static inline bool GetDescriptorIndexingSupported()
        {
            #if __APPLE__
                return false;
            #else
                return instance.physicalDeviceFeatures.shaderSampledImageArrayDynamicIndexing;
            #endif
        }

        [[nodiscard]] inline static UniformData* GetUniformDataPtr() { return &instance.uniformData; }
        [[nodiscard]] inline static StorageData* GetStorageDataPtr() { return &instance.storageData; }

        inline static void SetWindow(Window *window) { instance.window = window; }
        inline static void SetDevice(std::shared_ptr<Device> &givenDevice) { instance.device = givenDevice; }

        inline static void SetSwapchainExtent(VkExtent2D swapchainExtent) { instance.swapchainExtent = swapchainExtent; }
        inline static void SetDescriptorPool(std::shared_ptr<DescriptorPool> descriptorPool) { instance.descriptorPool = std::move(descriptorPool); }
        inline static void SetGlobalBindlessDescriptorSet(std::shared_ptr<BindlessDescriptorSet> bindlessDescriptorSet) { instance.globalBindlessDescriptorSet = std::move(bindlessDescriptorSet); }

    private:
        VulkanCore() = default;
        static VulkanCore instance;

        Window *window;
        std::shared_ptr<Device> device;

        VkExtent2D swapchainExtent;

        std::shared_ptr<DescriptorPool> descriptorPool;
        std::shared_ptr<BindlessDescriptorSet> globalBindlessDescriptorSet;

        UniformData uniformData;
        StorageData storageData;
    };

}
