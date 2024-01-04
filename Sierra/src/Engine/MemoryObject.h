//
// Created by Nikolay Kanchevski on 5.12.23.
//

#pragma once


namespace Sierra
{

    class SIERRA_API MemoryObject
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MemoryObject(uint64 portionSize, uint32 portionCount = 1);
        MemoryObject(void* memoryPointer, uint64 memorySize);

        template<typename T, uint32 C = 1>
        inline MemoryObject() : MemoryObject(sizeof(T), C) { }

        /* --- POLLING METHODS --- */
        void SetMemory(char byte, uint64 memorySize = 0, uint64 offset = 0);
        void CopyFromMemory(const void* memoryPointer, uint64 memorySize = 0, uint64 sourceOffset = 0, uint64 destinationOffset = 0);
        void CopyFromMemory(const MemoryObject &source, uint64 memorySize = 0, uint64 sourceOffset = 0, uint64 destinationOffset = 0);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint64 GetMemorySize() const { return memorySize; }
        [[nodiscard]] inline void*& GetData() { return data; }

        template<typename T>
        [[nodiscard]] inline const T& GetDataAs() const { return *reinterpret_cast<const T*>(data); }

        /* --- OPERATORS --- */
        MemoryObject(const MemoryObject&) = delete;
        MemoryObject& operator=(const MemoryObject&) = delete;

        /* --- MOVE SEMANTICS --- */
        MemoryObject(MemoryObject &&other);
        MemoryObject& operator=(MemoryObject &&other);

        /* --- DESTRUCTOR --- */
        ~MemoryObject();

    private:
        uint64 memorySize = 0;
        void* data = nullptr;
        bool managedExternally = false;


    };

}