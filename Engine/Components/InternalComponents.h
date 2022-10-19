//
// Created by Nikolay Kanchevski on 18.10.22.
//

#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include "Component.h"

namespace Sierra::Engine::Components
{

    class Tag : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        Tag(const std::string& givenTag) : tag(givenTag) {}

        /* --- PROPERTIES --- */
        std::string tag;

        Tag() = default;
        Tag(const Tag&) = default;
    };

    class Transform : public Component
    {
    public:
        /* --- PROPERTIES --- */
        glm::vec3 position = {0, 0, 0 };
        glm::vec3 rotation = { 0, 0, 0 };
        glm::vec3 scale = { 1, 1, 1 };

        Transform() = default;
        Transform(const Transform&) = default;
    };

    class Relationship : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline Relationship(entt::entity givenSelf) : self(givenSelf) { }

        /* --- GETTER METHODS --- */
        inline entt::entity& GetEnttParentEntity() { return parent; }
        inline std::vector<entt::entity> GetEnttChildrenEntities() const { return children; }

        /* --- SETTER METHODS --- */
        inline void SetParent(entt::entity &givenParent)
        {
            // Get the new parent's relationship
            Relationship &givenParentRelationship = World::GetEnttRegistry().get<Relationship>(givenParent);

            // Check if the current parent is not null
            if (parent != entt::null)
            {
                // Get the current parent's relationship
                Relationship &currentParentRelationship = Core::World::GetEnttRegistry().get<Relationship>(parent);

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

        Relationship() = default;
        Relationship(const Relationship&) = default;

    private:
        entt::entity parent = entt::null;
        entt::entity self = entt::null;
        std::vector<entt::entity> children {};
    };
}