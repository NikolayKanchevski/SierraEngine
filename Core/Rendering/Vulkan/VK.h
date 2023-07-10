//
// Created by Nikolay Kanchevski on 19.12.22.
//

#pragma once

#include "Abstractions/Device.h"
#include "Abstractions/Descriptors.h"
#include "Abstractions/Texture.h"
#include "Abstractions/Queries.h"

#define VK_VERSION VK_API_VERSION_1_2

namespace Sierra::Rendering::VK
{

    /* --- POLLING METHODS --- */
    void Initialize();
    void Destroy();

    /* --- GETTER METHODS --- */
    [[nodiscard]] VkInstance                 GetInstance();
    [[nodiscard]] VmaAllocator&              GetMemoryAllocator();
    [[nodiscard]] UniquePtr<Device>&         GetDevice();
    [[nodiscard]] VkPhysicalDevice           GetPhysicalDevice();
    [[nodiscard]] VkDevice                   GetLogicalDevice();
    [[nodiscard]] VkCommandPool              GetCommandPool();
    [[nodiscard]] UniquePtr<QueryPool>&      GetQueryPool();
    [[nodiscard]] VkDescriptorPool           GetImGuiDescriptorPool();

    /* --- UTILITY METHODS --- */
    template<typename MainType, typename NewType>
    inline static void PushToPNextChain(MainType &mainStruct, NewType &newStruct)
    {
        newStruct.pNext = mainStruct.pNext;
        mainStruct.pNext = &newStruct;
    }

}