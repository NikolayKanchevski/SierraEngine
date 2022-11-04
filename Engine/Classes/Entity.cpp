//
// Created by Nikolay Kanchevski on 17.10.22.
//

#include "Entity.h"

#include <iostream>

using namespace Sierra::Engine::Components;

namespace Sierra::Engine::Classes
{
    /* --- CONSTRUCTORS --- */

    Entity::Entity()
        : enttEntity(World::RegisterEntity())
    {
        AddComponent<Transform>();
        AddComponent<Tag>("Entity");
        AddComponent<Relationship>(enttEntity);
    }

    Entity::Entity(const std::string &givenName)
        : enttEntity(World::RegisterEntity())
    {
        AddComponent<Transform>();
        AddComponent<Tag>(givenName);
        AddComponent<Relationship>(enttEntity);
    }

    Entity::Entity(Entity &givenParent)
        : enttEntity(World::RegisterEntity())
    {
        AddComponent<Transform>();
        AddComponent<Tag>("Entity");
        AddComponent<Relationship>(enttEntity);

        SetParent(givenParent);
    }

    Entity::Entity(const std::string &givenName, Entity &givenParent)
        : enttEntity(World::RegisterEntity())
    {
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
        World::GetEnttRegistry().destroy(enttEntity);
    }

    /* --- DESTRUCTOR --- */

    Entity::~Entity()
    {

    }
}