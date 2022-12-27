//
// Created by Nikolay Kanchevski on 19.12.22.
//

#include "InternalComponents.h"

#include <imgui.h>

#include "../Classes/RNG.h"

using Sierra::Core::World;
using Sierra::Engine::Classes::RNG;

namespace Sierra::Engine::Components
{
    UUID::UUID()
        : value(RNG::GetRandomUInt64())
    {

    }

    UUID &UUID::operator=(uint32_t givenValue)
    {
        this->value = givenValue;
        return *this;
    }

    void Relationship::SetParent(entt::entity givenParent)
    {
        // Get the new parent's relationship
        Relationship &givenParentRelationship = World::GetEnttRegistry()->get<Relationship>(givenParent);

        // Check if the current parent is not null
        if (parent != entt::null)
        {
            // Get the current parent's relationship
            Relationship &currentParentRelationship = World::GetEnttRegistry()->get<Relationship>(parent);

            // Get the index of "self" in current parent's children
            uint32_t childIndex = 0;
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
    }

    void Transform::DrawUI()
    {

    }
}
