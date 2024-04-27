//
// Created by Nikolay Kanchevski on 24.04.24.
//

#include "SceneImporter.h"

#include "../../Core/Components/All.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    SceneImporter::SceneImporter(const SceneImporterCreateInfo &createInfo)
    {
        
    }

    /* --- POLLING METHODS --- */

    std::optional<ImportedScene> SceneImporter::Import(const std::span<const uint8> serializedSceneMemory)
    {
        const SerializedScene &serializedScene = *reinterpret_cast<const SerializedScene*>(serializedSceneMemory.data());
        if (!serializedScene.header.IsValid())
        {
            APP_WARNING("Cannot import scene, as its data is either in an invalid format, or it is corrupted!");
            return std::nullopt;
        }
        if (serializedScene.header.IsOutdated(VERSION))
        {
            APP_WARNING("Cannot import scene, as its data has been serialized with a newer version [{0}.{1}.{2}] than that of the importer - [{3}.{4}.{5}]!", serializedScene.header.version.GetMajor(), serializedScene.header.version.GetMinor(), serializedScene.header.version.GetPatch(), VERSION.GetMajor(), VERSION.GetMinor(), VERSION.GetPatch());
            return std::nullopt;
        }

        entt::registry registry = { };

        uint64 seekOffset = sizeof(SerializedScene);
        for (uint32 i = 0; i < serializedScene.index.entityCount; i++)
        {
            const SerializedEntity &serializedEntity = *reinterpret_cast<const SerializedEntity*>(serializedSceneMemory.data() + seekOffset);
            seekOffset += sizeof(SerializedEntity);

            // Create entity
            const entt::entity entity = registry.create(serializedEntity.entity);
            if (static_cast<uint32>(entity) != static_cast<uint32>(serializedEntity.entity))
            {
                APP_WARNING("Cannot import scene, due to an entity mismatch!");
                return std::nullopt;
            }

            // Load entity's components
            LoadEntityComponents(serializedSceneMemory.data(), seekOffset, registry, entity, AllComponents());
        }

        ImportedScene importedScene
        {
            .registry = std::move(registry)
        };

        return importedScene;
    }

    /* --- PRIVATE METHODS --- */

    template<typename... Component>
    void SceneImporter::LoadEntityComponents(const uint8* blob, uint64 &seekOffset, entt::registry &registry, const entt::entity entity, const ComponentGroup<Component...>)
    {
        ([&]() -> void
        {
            const SerializedComponent &serializedComponent = *reinterpret_cast<const SerializedComponent*>(blob + seekOffset);
            seekOffset += sizeof(SerializedComponent);

            if (strncmp(serializedComponent.signature.data(), GetComponentSignature<Component>().data(), serializedComponent.signature.size()) == 0)
            {
                constexpr uint64 COMPONENT_STARTING_OFFSET = sizeof(void*);
                constexpr uint64 COMPONENT_MEMORY_SIZE = sizeof(Component) - COMPONENT_STARTING_OFFSET; // This offset is to strip out v-table pointer

                // Add component & overwrite its memory
                Component &component = registry.emplace<Component>(entity);
                std::memcpy(reinterpret_cast<uint8*>(&component) + COMPONENT_STARTING_OFFSET, blob + seekOffset, COMPONENT_MEMORY_SIZE);
                seekOffset += COMPONENT_MEMORY_SIZE;

                return;
            }
        }(), ...);
    }

}