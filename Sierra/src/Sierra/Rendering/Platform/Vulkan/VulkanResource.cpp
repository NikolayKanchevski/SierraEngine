//
// Created by Nikolay Kanchevski on 11.09.23.
//

#include "VulkanResource.h"

namespace Sierra
{

    void VulkanResource::PushToPNextChain(void* mainStruct, void* newStruct)
    {
        // We just cast them to any Vulkan structure, as they all have their pNext stored exactly 4 bytes within the struct
        auto mainStructAsVkStructure = reinterpret_cast<VkBufferMemoryBarrier*>(mainStruct);
        auto newStructAsVkStructure = reinterpret_cast<VkBufferMemoryBarrier*>(newStruct);
        newStructAsVkStructure->pNext = mainStructAsVkStructure->pNext;
        mainStructAsVkStructure->pNext = newStruct;
    }

}