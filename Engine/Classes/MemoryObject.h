//
// Created by Nikolay Kanchevski on 17.05.23.
//

#pragma once

namespace Sierra::Engine
{

    class MemoryObject
    {
    public:
        /* --- CONSTRUCTORS --- */
        MemoryObject() = default;
        MemoryObject(uint64 elementSize, uint count = 1);

        template<typename T, uint C = 1>
        inline MemoryObject()
            : data(std::calloc(C, sizeof(T))), memorySize(sizeof(T) * C)
        {

        }

        /* --- GETTER METHODS --- */
        uint64 GetMemorySize() const { return memorySize; }

        /* --- SETTER METHODS --- */
        void SetDataByOffset(const void* newData, uint64 size = 0, uint64 offset = 0);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline void*& GetData() { return data; }

        template<typename T>
        [[nodiscard]] inline T& GetDataAs() const { return *reinterpret_cast<T*>(data); }

        /* --- DESTRUCTOR --- */
        void Destroy();

    private:
        bool managedExternally;
        void* data = nullptr;
        uint64 memorySize;

    };
}