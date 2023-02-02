//
// Created by Nikolay Kanchevski on 17.10.22.
//

#include "Entity.h"


#include "../Components/Relationship.h"
#include "../Components/Transform.h"

using namespace Sierra::Engine::Components;

namespace Sierra::Engine::Classes
{
    const Entity Entity::Null = Entity((entt::entity)   entt::null);

    /* --- CONSTRUCTORS --- */

    Entity::Entity(const String &givenName)
        : enttEntity(World::RegisterEntity())
    {
        AddComponent<Components::UUID>();
        AddComponent<Transform>();
        AddComponent<Tag>(givenName);
        AddComponent<Relationship>(enttEntity);
    }

    Entity::Entity(Entity &givenParent)
        : enttEntity(World::RegisterEntity())
    {
        AddComponent<Components::UUID>();
        AddComponent<Transform>();
        AddComponent<Tag>("Entity");
        AddComponent<Relationship>(enttEntity);

        SetParent(givenParent);
    }

    Entity::Entity(const String &givenName, Entity &givenParent)
        : enttEntity(World::RegisterEntity())
    {
        AddComponent<Components::UUID>();
        AddComponent<Transform>();
        AddComponent<Tag>(givenName);
        AddComponent<Relationship>(enttEntity);

        SetParent(givenParent);
    }

    /* --- SETTER METHODS --- */

    void Entity::SetParent(Entity &givenParent)
    {
        GetComponent<Relationship>().SetParent(givenParent.enttEntity);
    }

    void Entity::Destroy()
    {
        World::DestroyEntity(enttEntity);
    }

    /* --- DESTRUCTOR --- */

    Entity::~Entity()
    {

    }

    bool Entity::operator==(Entity &right)
    {
        return enttEntity == right.enttEntity;
    }

    Entity::operator entt::entity() const noexcept
    {
        return enttEntity;
    }

    Entity::operator Components::UUID() const noexcept
    {
        return GetComponent<Components::UUID>();
    }
}