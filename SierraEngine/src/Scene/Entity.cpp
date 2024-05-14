//
// Created by Nikolay Kanchevski on 6.05.24.
//

#include "Entity.h"

#include "Scene.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Entity::Entity(const std::string_view name, const entt::entity entity, entt::registry* registry)
        : registry(registry), entity(entity)
    {
        AddComponent<Tag>(name);
        AddComponent<UUID>();
    }

}