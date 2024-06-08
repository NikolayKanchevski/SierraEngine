//
// Created by Nikolay Kanchevski on 6.05.24.
//

#include "Entity.h"

#include "Components/UUID.h"
#include "Components/Tag.h"
#include "Components/Relationship.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Entity::Entity(entt::registry* registry, const entt::entity entity)
        : registry(registry), entity(entity)
    {

    }

    /* --- GETTER METHODS --- */

    std::string_view Entity::GetTag() const
    {
        return GetComponent<Tag>().GetTag();
    }

    uint64 Entity::GetHash() const
    {
        return GetComponent<UUID>().GetHash();
    }

    bool Entity::HasParent() const
    {
        return GetComponent<Relationship>().HasParent();
    }

    Entity Entity::GetParent() const
    {
        return Entity(registry, GetComponent<Relationship>().GetParent());
    }

    uint32 Entity::GetChildCount() const
    {
        return GetComponent<Relationship>().GetChildCount();
    }

    Entity Entity::GetChild(const uint32 index) const
    {
        return Entity(registry, GetComponent<Relationship>().GetChildren()[index]);
    }

    /* --- SETTER METHODS --- */

    void Entity::SetTag(const std::string_view tag) const
    {
        GetComponent<Tag>().SetTag(tag);
    }

    void Entity::SetParent(const Entity parent) const
    {
        Relationship &relationship = GetComponent<Relationship>();
        if (relationship.HasParent())
        {
            // Remove old parent's reference to this
            Entity(registry, relationship.GetParent()).GetComponent<Relationship>().RemoveChild(entity);
        }

        // Update this and parent's references
        relationship.SetParent(parent.entity);
        parent.GetComponent<Relationship>().AddChild(entity);
    }

    void Entity::RemoveParent() const
    {
        Relationship &relationship = GetComponent<Relationship>();
        if (relationship.HasParent())
        {
            // Remove old parent's reference to this
            Entity(registry, relationship.GetParent()).GetComponent<Relationship>().RemoveChild(entity);
        }
        relationship.RemoveParent();
    }

    void Entity::AddChild(const Entity child) const
    {
        Relationship &childRelationship = child.GetComponent<Relationship>();
        if (childRelationship.HasParent())
        {
            // Remove old parent's reference to child
            Entity(registry, childRelationship.GetParent()).GetComponent<Relationship>().RemoveChild(child.entity);
        }

        // Update this and child's references
        childRelationship.SetParent(entity);
        GetComponent<Relationship>().AddChild(entity);
    }

    void Entity::RemoveChild(const Entity child) const
    {
        child.GetComponent<Relationship>().RemoveParent();
        GetComponent<Relationship>().RemoveChild(child.entity);
    }

}
