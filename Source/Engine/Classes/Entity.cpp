//
// Created by Nikolay Kanchevski on 17.10.22.
//

#include "Entity.h"

#include "../Components/Tag.h"
#include "../Components/UUID.h"
#include "../Components/Transform.h"
#include "../Components/Relationship.h"

namespace Sierra::Engine
{
    const Entity Entity::Null = Entity((entt::entity) entt::null);

    /* --- CONSTRUCTORS --- */

    Entity::Entity(const entt::entity givenEnttEntity)
        : enttEntity(givenEnttEntity)
    {

    }

    Entity::Entity(const String &givenName)
        : enttEntity(World::RegisterEntity())
    {
        AddComponent<UUID>();
        AddComponent<Transform>();
        AddComponent<Tag>(givenName);
        AddComponent<Relationship>();
    }

    /* --- SETTER METHODS --- */

    void Entity::SetParent(const Entity &parent) const
    {
        GetComponent<Relationship>().SetParent(parent);
    }

    void Entity::Destroy() const
    {
        World::DestroyEntity(enttEntity);
    }

    /* --- GETTER METHODS --- */

    String& Entity::GetTag() const
    {
        return GetComponent<Tag>().tag;
    }

}