//
// Created by Nikolay Kanchevski on 18.01.23.
//

#pragma once

#include "Component.h"

namespace Sierra::Engine
{
    class Relationship : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        Relationship(entt::entity givenSelf);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline entt::entity& GetEnttParentEntity() { return parent; }
        [[nodiscard]] inline entt::entity& GetEnttOriginParentEntity() { return originParent; }
        [[nodiscard]] inline std::vector<entt::entity> GetEnttChildrenEntities() const { return children; }

        /* --- SETTER METHODS --- */
        void SetParent(entt::entity givenParent);

        /* --- OPERATORS --- */
        Relationship() = default;
        Relationship(const Relationship&) = default;

    private:
        entt::entity originParent = entt::null;
        entt::entity parent = entt::null;
        entt::entity self = entt::null;
        std::vector<entt::entity> children {};

    };
}
