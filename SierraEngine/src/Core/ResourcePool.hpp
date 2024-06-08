//
// Created by Nikolay Kanchevski on 20.02.24.
//

#pragma once

namespace SierraEngine
{

    struct ResourcePoolCreateInfo
    {
        uint32 initialSize = 128;
    };

    template<typename K, typename T>
    class ResourcePool final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ResourcePool(const ResourcePoolCreateInfo &createInfo)
            : resources(createInfo.initialSize)
        {

        }

        /* --- POLLING METHODS --- */
        T& AddResource(const K ID, T&& resource)
        {
            auto iterator = resources.find(ID);

            if (iterator != resources.end()) iterator->second = std::move(resource);
            else iterator = resources.emplace(ID, std::move(resource)).first;

            return iterator->second;
        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] T* GetResource(const K ID) const
        {
            auto iterator = resources.find(ID);
            if (iterator != resources.end()) return iterator->second;
            return nullptr;
        }

        [[nodiscard]] bool ResourceExists(const K ID) const
        {
            return GetResource(ID).has_value();
        }

        /* --- OPERATORS --- */
        ResourcePool(const ResourcePool&) = delete;
        ResourcePool& operator=(const ResourcePool&) = delete;

        /* --- DESTRUCTOR --- */
        ~ResourcePool() = default;

    private:
        std::unordered_map<K, T> resources = { };

    };

}
