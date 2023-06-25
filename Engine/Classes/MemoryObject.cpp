//
// Created by Nikolay Kanchevski on 19.05.23.
//

#include "MemoryObject.h"

namespace Sierra::Engine::Classes
{
    /* --- CONSTRUCTORS --- */

    MemoryObject::MemoryObject(const uint64 elementSize, const uint count)
        : data(count != 0 ? std::calloc(count, elementSize) : nullptr), memorySize(std::max(elementSize, elementSize * count)), managedExternally(count == 0)
    {
        // Count equal to 0 means that memory will be allocated from outside
    }

    /* --- SETTERS --- */

    void MemoryObject::SetDataByOffset(const void* newData, const uint64 offset, const uint64 range)
    {
        memcpy(reinterpret_cast<char*>(data) + offset, newData, range != 0 ? range : memorySize);
    }

    /* --- DESTRUCTORS --- */

    void MemoryObject::Destroy()
    {
        if (!managedExternally) std::free(data);
        data = nullptr;
    }

}