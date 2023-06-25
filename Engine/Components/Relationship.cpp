//
// Created by Nikolay Kanchevski on 18.01.23.
//

#include "Relationship.h"

#include "../Classes/Entity.h"

namespace Sierra::Engine::Components
{

    Relationship::Relationship(const entt::entity givenSelf) : self(givenSelf)
    {
        World::GetOriginEntitiesList()[static_cast<uint>(self)] = self;
    }

    void Relationship::SetParent(entt::entity givenParent)
    {
        // Get the new parent's relationship
        Relationship &givenParentRelationship = World::GetComponent<Relationship>(givenParent);

        // Check if the current parent is not null
        if (parent != entt::null)
        {
            // Get the current parent's relationship
            Relationship &currentParentRelationship = World::GetComponent<Relationship>(parent);

            // Get the index of "self" in current parent's children
            uint childIndex = 0;
            while (currentParentRelationship.children[childIndex] != self)
            {
                childIndex++;
            }

            // Erase "self" from current parent's children
            currentParentRelationship.children.erase(currentParentRelationship.children.begin() + childIndex);
        }

        // Change parent
        parent = givenParent;

        // Add "self" to new parent's children
        givenParentRelationship.children.push_back(self);

        using Classes::Entity;

        // Retrieve origin parent
        Entity currentEntity = Entity(self);
        while (true)
        {
            if (currentEntity.GetComponent<Relationship>().parent != entt::null) currentEntity = Entity(currentEntity.GetComponent<Relationship>().parent);
            else break;
        }

        originParent = currentEntity.GetEnttEntity();

        if (givenParent == entt::null) World::GetOriginEntitiesList()[static_cast<uint>(self)] = self;
        else { if (World::GetOriginEntitiesList().count(static_cast<uint>(self)) != 0) { World::GetOriginEntitiesList().erase(static_cast<uint>(self)); } }
    }

}
