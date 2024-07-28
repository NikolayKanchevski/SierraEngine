//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

#include "Entity.h"
#include "Component.h"

#include "../Rendering/ArenaAllocator.h"

namespace SierraEngine
{

    struct SceneCreateInfo
    {

    };

    class SIERRA_ENGINE_API Scene final
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using EntityCallback = std::function<void(Entity)>;

        template<ComponentType... Components>
        using ComponentCallback = std::function<void(Components&...)>;

        /* --- CONSTRUCTORS --- */
        explicit Scene(const SceneCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] Entity CreateEntity(std::string_view tag = "Entity");
        void ForEachEntity(EntityCallback Callback);
        void DestroyEntity(Entity entity);

        template<ComponentType Component>
        void ForEachComponent(ComponentCallback<Component> Callback) const { registry.view<Component>().each(Callback); }

        template<ComponentType... Components>
        void ForEachComponentPair(ComponentCallback<Components...> Callback) const { registry.view<Component>().each(Callback); }

        /* --- OPERATORS --- */
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        /* --- DESTRUCTOR --- */
        ~Scene() = default;

    private:
        entt::registry registry = { };

    };

}
