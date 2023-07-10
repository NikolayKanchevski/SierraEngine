//
// Created by Nikolay Kanchevski on 17.10.22.
//

#include "Entity.h"


#include "../Components/Relationship.h"
#include "../Components/Transform.h"

namespace Sierra::Engine
{
    const Entity Entity::Null = Entity((entt::entity)   entt::null);

    /* --- CONSTRUCTORS --- */

    Entity::Entity(const String &givenName)
        : enttEntity(World::RegisterEntity())
    {
        AddComponent<UUID>();
        AddComponent<Transform>();
        AddComponent<Tag>(givenName);
        AddComponent<Relationship>(enttEntity);
    }

    Entity::Entity(Entity &givenParent)
        : enttEntity(World::RegisterEntity())
    {
        AddComponent<UUID>();
        AddComponent<Transform>();
        AddComponent<Tag>("Entity");
        AddComponent<Relationship>(enttEntity);

        SetParent(givenParent);
    }

    Entity::Entity(const String &givenName, Entity &givenParent)
        : enttEntity(World::RegisterEntity())
    {
        AddComponent<UUID>();
        AddComponent<Transform>();
        AddComponent<Tag>(givenName);
        AddComponent<Relationship>(enttEntity);

        SetParent(givenParent);
    }

    /* --- SETTER METHODS --- */

    void Entity::SetParent(Entity &givenParent) const
    {
        GetComponent<Relationship>().SetParent(givenParent.enttEntity);
    }

    void Entity::Destroy()
    {
        World::DestroyEntity(enttEntity);
    }

    /* --- DESTRUCTOR --- */

    bool Entity::operator==(Entity &right)
    {
        return enttEntity == right.enttEntity;
    }

    Entity::operator entt::entity() const
    {
        return enttEntity;
    }

    Entity::operator UUID() const
    {
        return GetComponent<UUID>();
    }
}