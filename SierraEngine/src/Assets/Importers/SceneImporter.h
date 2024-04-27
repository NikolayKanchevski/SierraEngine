//
// Created by Nikolay Kanchevski on 24.04.24.
//

#pragma once

#include <entt/entt.hpp>

#include "../SerializedScene.h"
#include "../../Core/Component.h"

namespace SierraEngine
{

    struct SceneImporterCreateInfo
    {

    };

    struct ImportedScene
    {
        entt::registry registry = { };
    };

    class SceneImporter
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit SceneImporter(const SceneImporterCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<ImportedScene> Import(std::span<const uint8> serializedSceneMemory);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Sierra::Version GetVersion() const { return VERSION; }

        /* --- OPERATORS --- */
        SceneImporter(const SceneImporter&) = delete;
        SceneImporter &operator=(const SceneImporter&) = delete;

        /* --- DESTRUCTOR --- */
        ~SceneImporter() = default;

    private:
        constexpr static Sierra::Version VERSION = Sierra::Version({ 1, 0, 0 });

        template<typename... Component>
        void LoadEntityComponents(const uint8* blob, uint64 &seekOffset, entt::registry &registry, entt::entity entity, ComponentGroup<Component...>);

    };

}
