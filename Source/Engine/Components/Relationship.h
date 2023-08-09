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
        Relationship() = default;
        void OnAddComponent() override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Entity GetParent() const { return parent; }
        [[nodiscard]] inline std::vector<Entity> GetChildren() const { return children; }

        /* --- SETTER METHODS --- */
        void SetParent(const Entity &newParent);

    private:
        Entity parent = Entity::Null;
        std::vector<Entity> children;

    };
}
