//
// Created by Nikolay Kanchevski on 23.04.24.
//

#include "SceneSerializer.h"

#include "../../../../Core/Components/All.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    SceneSerializer::SceneSerializer(const SceneSerializerCreateInfo &createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    std::vector<uint8> SceneSerializer::Serialize(const SceneSerializeInfo &serializeInfo)
    {
        // Get all entities
        const entt::registry &registry = serializeInfo.scene.registry;
        const auto &entities = registry.view<UUID>();

        std::vector<uint8> blob(entities.size_hint() * 512);

        constexpr uint64 sceneDataOffset = 0;
        uint64 seekOffset = sizeof(SerializedScene);

        // Write entity data after reserved space for scene data
        uint32 entityCount = 0;
        for (const entt::entity entity : entities)
        {
            const uint64 entityDataOffset = seekOffset;
            seekOffset += sizeof(SerializedEntity);

            // Write component data after reserved space for entity data
            const uint32 componentCount = SerializeEntityComponents(entity, serializeInfo.scene.registry, AllComponents(), blob, seekOffset);

            // Write entity data back (before component data)
            const SerializedEntity serializedEntity
            {
                .entity = entity,
                .componentCount = componentCount
            };
            std::memcpy(blob.data() + entityDataOffset, &serializedEntity, sizeof(SerializedEntity));
            entityCount++;
        }

        // Write scene data back (before entity data)
        const SerializedScene serializedScene
        {
            .header {
                .version = GetVersion()
            },
            .index = {
                .entityCount = entityCount
            }
        };
        std::memcpy(blob.data() + sceneDataOffset, &serializedScene, sizeof(SerializedScene));

        // Strip out the unused pre-allocated memory
        blob.resize(seekOffset);

        return std::move(blob);
    }

    /* --- PRIVATE METHODS --- */

    template<typename... Component>
    uint32 SceneSerializer::SerializeEntityComponents(const entt::entity entity, const entt::registry &registry, const ComponentGroup<Component...>, std::vector<uint8> &blob, uint64 &seekOffset)
    {
        uint32 componentCount = 0;
        ([&]() -> void
        {
            if (registry.all_of<Component>(entity))
            {
                constexpr uint64 COMPONENT_STARTING_OFFSET = sizeof(void*);
                constexpr uint64 COMPONENT_MEMORY_SIZE = sizeof(Component) - COMPONENT_STARTING_OFFSET; // This offset is to strip out v-table pointer

                // If blob is not big enough, increase its size
                if (const uint64 neededMemorySize = seekOffset + sizeof(SerializedComponent) + COMPONENT_MEMORY_SIZE; blob.size() < neededMemorySize)
                {
                    blob.resize(glm::max(static_cast<uint64>(blob.size()) * 2, neededMemorySize));
                }

                // Copy component header
                constexpr SerializedComponent SERIALIZED_COMPONENT = { .signature = GetComponentSignature<Component>() };
                std::memcpy(blob.data() + seekOffset, &SERIALIZED_COMPONENT, sizeof(SerializedComponent));
                seekOffset += sizeof(SerializedComponent);

                // Copy component data
                const Component &component = registry.get<Component>(entity);
                std::memcpy(blob.data() + seekOffset, reinterpret_cast<const uint8*>(&component) + COMPONENT_STARTING_OFFSET, COMPONENT_MEMORY_SIZE);
                seekOffset += COMPONENT_MEMORY_SIZE;

                // Increase component count
                componentCount++;
            }
        }(), ...);

        return componentCount;
    }

}