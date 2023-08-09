//
// Created by Nikolay Kanchevski on 19.12.22.
//

#pragma once

#include "../Abstractions/Device.h"
#include "../Abstractions/Texture.h"
#include "../Abstractions/Queries.h"
#include "../Abstractions/Swapchain.h"
#include "../Abstractions/Descriptors.h"

#define VK_VERSION VK_API_VERSION_1_2

namespace Sierra::Rendering::VK
{

    /* --- CONSTRUCTORS --- */
    void Initialize();

    /* --- GETTER METHODS --- */
    [[nodiscard]] VkInstance                            GetInstance();
    [[nodiscard]] VmaAllocator&                         GetMemoryAllocator();
    [[nodiscard]] UniquePtr<Device>&                    GetDevice();
    [[nodiscard]] VkPhysicalDevice                      GetPhysicalDevice();
    [[nodiscard]] VkDevice                              GetLogicalDevice();
    [[nodiscard]] VkCommandPool                         GetCommandPool();
    [[nodiscard]] UniquePtr<QueryPool>&                 GetQueryPool();
    [[nodiscard]] VkDescriptorPool                      GetImGuiDescriptorPool();

    /* --- DESTRUCTOR --- */
    void Destroy();

    /* --- UTILITY METHODS --- */
    inline static void PushToPNextChain(void* mainStruct, void* newStruct)
    {
        // We just cast them to any Vulkan structure, as they all have their pNext stored exactly 4 bytes within the struct
        auto mainStructAsVkStructure = reinterpret_cast<VkBufferMemoryBarrier*>(mainStruct);
        auto newStructAsVkStructure = reinterpret_cast<VkBufferMemoryBarrier*>(newStruct);
        newStructAsVkStructure->pNext = mainStructAsVkStructure->pNext;
        mainStructAsVkStructure->pNext = newStruct;
    }

}