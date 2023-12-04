//
// Created by Nikolay Kanchevski on 11.09.23.
//

#include "VulkanResource.h"

namespace Sierra
{

    void VulkanResource::PushToPNextChain(void* mainStruct, void* newStruct)
    {
        // We just cast them to any Vulkan structure, as they all have their pNext stored exactly 4 bytes within the struct
        auto mainStructAsVkStruct = reinterpret_cast<VkBufferMemoryBarrier*>(mainStruct);
        auto newStructAsVkStruct = reinterpret_cast<VkBufferMemoryBarrier*>(newStruct);
        newStructAsVkStruct->pNext = mainStructAsVkStruct->pNext;
        mainStructAsVkStruct->pNext = newStruct;
    }

}