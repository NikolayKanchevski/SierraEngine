//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

#include <entt/entt.hpp>

#include "Entity.h"
#include "Component.h"

#include "../Rendering/ArenaAllocator.h"

namespace SierraEngine
{

    struct SceneCreateInfo
    {
        const Sierra::RenderingContext &renderingContext;
    };

    class SIERRA_ENGINE_API Scene final
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using AllComponents = ComponentGroup<Tag, UUID>;

        /* --- CONSTRUCTORS --- */
        explicit Scene(const SceneCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Update(std::unique_ptr<Sierra::CommandBuffer> &commandBuffer);

        [[nodiscard]] Entity CreateEntity(std::string_view name = "Entity");
        void DestroyEntity(Entity entity);

        /* --- GETTER METHODS --- */
        [[nodiscard]] ArenaAllocator& GetArenaAllocator() { return arenaAllocator; }
        [[nodiscard]] std::unique_ptr<Sierra::ResourceTable>& GetResourceTable() { return resourceTable; }

        template<ComponentType Components>
        [[nodiscard]] inline auto GetAllComponents() { return registry.view<Components>(); }

        template<ComponentType... Components>
        [[nodiscard]] inline auto GetAllComponentPairs() { return registry.group<Components...>(); }

        /* --- OPERATORS --- */
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        /* --- DESTRUCTOR --- */
        ~Scene() = default;

    private:
        friend class Entity;
        friend class SceneSerializer;
        entt::registry registry = { };

        ArenaAllocator arenaAllocator;
        std::unique_ptr<Sierra::ResourceTable> resourceTable = nullptr;

    };

}
