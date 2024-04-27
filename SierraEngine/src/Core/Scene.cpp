//
// Created by Nikolay Kanchevski on 22.04.24.
//

#include "Scene.h"

#include "Components/Tag.h"
#include "Components/UUID.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Scene::Scene(const SceneCreateInfo &createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    Entity Scene::CreateEntity(const std::string_view name)
    {
        return Entity(name, registry.create(), this);
    }

    void Scene::DestroyEntity(const Entity entity)
    {
        registry.destroy(entity.entity);
    }

    /* --- CONSTRUCTORS --- */

    Entity::Entity(const std::string_view name, const entt::entity entity, Scene* scene)
        : scene(scene), entity(entity)
    {
        AddComponent<UUID>();
        AddComponent<Tag>(name);
    }

    /* --- GETTER METHODS --- */

    std::string_view Entity::GetName() const
    {
        return GetComponent<Tag>().GetName();
    }

    uint64 Entity::GetHash() const
    {
        return GetComponent<UUID>().GetHash();
    }

}