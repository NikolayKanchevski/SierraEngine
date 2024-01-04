//
// Created by Nikolay Kanchevski on 5.12.23.
//

#include "MemoryObject.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MemoryObject::MemoryObject(const uint64 portionSize, const uint32 portionCount)
        : memorySize(std::max(portionSize * portionCount, portionSize)), data(malloc(memorySize)), managedExternally(portionCount == 0 || portionSize == 0)
    {

    }

    MemoryObject::MemoryObject(void* memoryPointer, const uint64 memorySize)
        : memorySize(memorySize), data(memoryPointer), managedExternally(memorySize == 0)
    {

    }

    /* --- POLLING METHODS --- */

    void MemoryObject::SetMemory(const char byte, const uint64 setMemorySize, const uint64 offset)
    {
        SR_ERROR_IF(setMemorySize + offset > memorySize, "Buffer overflow - setting external memory!");
        memset(reinterpret_cast<uchar*>(data) + offset, byte, setMemorySize != 0 ? setMemorySize : memorySize);
    }

    void MemoryObject::CopyFromMemory(const void* memoryPointer, const uint64 copyMemorySize, const uint64 sourceOffset, const uint64 destinationOffset)
    {
        SR_ERROR_IF(copyMemorySize + destinationOffset > memorySize, "Buffer overflow - writing copied data to external memory!");
        memcpy(reinterpret_cast<uchar*>(data) + destinationOffset, reinterpret_cast<const uchar*>(memoryPointer) + sourceOffset, copyMemorySize != 0 ? copyMemorySize : memorySize);
    }

    void MemoryObject::CopyFromMemory(const MemoryObject &source, const uint64 copyMemorySize, const uint64 sourceOffset, const uint64 destinationOffset)
    {
        CopyFromMemory(source.data, copyMemorySize, sourceOffset, destinationOffset);
    }

    /* --- DESTRUCTOR --- */

    MemoryObject::~MemoryObject()
    {
        if (!managedExternally) std::free(data);
    }

    /* --- MOVE SEMANTICS --- */

    MemoryObject::MemoryObject(MemoryObject &&other)
        : memorySize(other.memorySize), data(other.data), managedExternally(other.managedExternally)
    {
        other.managedExternally = true;
    }

    MemoryObject& MemoryObject::operator=(MemoryObject &&other)
    {
        if (this != &other)
        {
            memorySize = other.memorySize;
            data = other.data;
            managedExternally = other.managedExternally;
            other.managedExternally = true;
        }
        return *this;
    }

}
