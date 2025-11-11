//
// Created by Nikolay Kanchevski on 27.10.25.
//

#include "VulkanResource.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanResource::VulkanResource(const std::string_view name)
        : name(name)
    {

    }

    /* --- POLLING METHODS --- */

    void VulkanResource::AddToPNextChain(void* mainStruct, void* newStruct) const noexcept
    {
        // We just cast them to any Vulkan structure, as they all have their pNext stored exactly 4 bytes within the struct
        VkBufferMemoryBarrier* mainStructAsVkStruct = reinterpret_cast<VkBufferMemoryBarrier*>(mainStruct);
        VkBufferMemoryBarrier* newStructAsVkStruct = reinterpret_cast<VkBufferMemoryBarrier*>(newStruct);
        newStructAsVkStruct->pNext = mainStructAsVkStruct->pNext;
        mainStructAsVkStruct->pNext = newStruct;
    }

}