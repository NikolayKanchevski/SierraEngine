//
// Created by Nikolay Kanchevski on 22.04.24.
//

#include "Scene.h"

#include "Components/Tag.h"
#include "Components/UUID.h"
#include "Components/Relationship.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Scene::Scene(const SceneCreateInfo &createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    Entity Scene::CreateEntity(const std::string_view tag)
    {
        Entity entity = Entity(&registry, registry.create());
        entity.AddComponent<UUID>();
        entity.AddComponent<Tag>(tag);
        entity.AddComponent<Relationship>();
        return entity;
    }

    void Scene::ForEachEntity(const EntityCallback Callback)
    {
        for (const entt::entity entity : registry.view<Relationship>()) Callback(Entity(&registry, entity));
    }

    void Scene::DestroyEntity(const Entity entity)
    {
        registry.destroy(entity.entity);
    }

}
